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
 * @file reg_hsu.h
 * @brief Definitions of the HSU HW block registers and register access functions.
 *
 * @defgroup REG_HSU REG_HSU
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the HSU HW block registers and register access functions.
 */
#ifndef _REG_HSU_H_
#define _REG_HSU_H_

#include "co_int.h"
#include "_reg_hsu.h"
#include "compiler.h"
#include "arch.h"
#include "dbg_assert.h"
#include "reg_access.h"

/** @brief Number of registers in the REG_HSU peripheral.
 */
#define REG_HSU_COUNT 76

/** @brief Decoding mask of the REG_HSU peripheral registers from the CPU point of view.
 */
#define REG_HSU_DECODING_MASK 0x000001FF

/**
 * @name REVISION register definitions
 * <table>
 * <caption>REVISION bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 30    <td>            RSA_768 <td> W <td> R <td> 1
 * <tr><td> 29    <td>            RSA_512 <td> W <td> R <td> 1
 * <tr><td> 28    <td>            RSA_256 <td> W <td> R <td> 1
 * <tr><td> 27    <td>           RSA_4096 <td> W <td> R <td> 1
 * <tr><td> 26    <td>           RSA_2048 <td> W <td> R <td> 1
 * <tr><td> 25    <td>           RSA_1024 <td> W <td> R <td> 1
 * <tr><td> 24    <td> HMAC_SHA512_SHA384 <td> W <td> R <td> 1
 * <tr><td> 23    <td> HMAC_SHA256_SHA224 <td> W <td> R <td> 1
 * <tr><td> 22    <td>          HMAC_SHA1 <td> W <td> R <td> 1
 * <tr><td> 21    <td>        SHA_512_384 <td> W <td> R <td> 1
 * <tr><td> 20    <td>        SHA_256_224 <td> W <td> R <td> 1
 * <tr><td> 19    <td>              SHA_1 <td> W <td> R <td> 1
 * <tr><td> 18    <td>             IP_CHK <td> W <td> R <td> 1
 * <tr><td> 17    <td>       AES_128_CMAC <td> W <td> R <td> 1
 * <tr><td> 16    <td>           TKIP_MIC <td> W <td> R <td> 1
 * <tr><td> 15:00 <td>            VERSION <td> W <td> R <td> 0x8
 * </table>
 *
 * @{
 */

/// Address of the REVISION register
#define HSU_REVISION_ADDR   0x24930000
/// Offset of the REVISION register from the base address
#define HSU_REVISION_OFFSET 0x00000000
/// Index of the REVISION register
#define HSU_REVISION_INDEX  0x00000000
/// Reset value of the REVISION register
#define HSU_REVISION_RESET  0x7FFF0008

/**
 * @brief Returns the current value of the REVISION register.
 * The REVISION register will be read and its value returned.
 * @return The current value of the REVISION register.
 */
__INLINE uint32_t hsu_revision_get(void)
{
    return REG_PL_RD(HSU_REVISION_ADDR);
}

// field definitions
/// RSA_768 field bit
#define HSU_RSA_768_BIT               ((uint32_t)0x40000000)
/// RSA_768 field position
#define HSU_RSA_768_POS               30
/// RSA_512 field bit
#define HSU_RSA_512_BIT               ((uint32_t)0x20000000)
/// RSA_512 field position
#define HSU_RSA_512_POS               29
/// RSA_256 field bit
#define HSU_RSA_256_BIT               ((uint32_t)0x10000000)
/// RSA_256 field position
#define HSU_RSA_256_POS               28
/// RSA_4096 field bit
#define HSU_RSA_4096_BIT              ((uint32_t)0x08000000)
/// RSA_4096 field position
#define HSU_RSA_4096_POS              27
/// RSA_2048 field bit
#define HSU_RSA_2048_BIT              ((uint32_t)0x04000000)
/// RSA_2048 field position
#define HSU_RSA_2048_POS              26
/// RSA_1024 field bit
#define HSU_RSA_1024_BIT              ((uint32_t)0x02000000)
/// RSA_1024 field position
#define HSU_RSA_1024_POS              25
/// HMAC_SHA512_SHA384 field bit
#define HSU_HMAC_SHA512_SHA384_BIT    ((uint32_t)0x01000000)
/// HMAC_SHA512_SHA384 field position
#define HSU_HMAC_SHA512_SHA384_POS    24
/// HMAC_SHA256_SHA224 field bit
#define HSU_HMAC_SHA256_SHA224_BIT    ((uint32_t)0x00800000)
/// HMAC_SHA256_SHA224 field position
#define HSU_HMAC_SHA256_SHA224_POS    23
/// HMAC_SHA1 field bit
#define HSU_HMAC_SHA1_BIT             ((uint32_t)0x00400000)
/// HMAC_SHA1 field position
#define HSU_HMAC_SHA1_POS             22
/// SHA_512_384 field bit
#define HSU_SHA_512_384_BIT           ((uint32_t)0x00200000)
/// SHA_512_384 field position
#define HSU_SHA_512_384_POS           21
/// SHA_256_224 field bit
#define HSU_SHA_256_224_BIT           ((uint32_t)0x00100000)
/// SHA_256_224 field position
#define HSU_SHA_256_224_POS           20
/// SHA_1 field bit
#define HSU_SHA_1_BIT                 ((uint32_t)0x00080000)
/// SHA_1 field position
#define HSU_SHA_1_POS                 19
/// IP_CHK field bit
#define HSU_IP_CHK_BIT                ((uint32_t)0x00040000)
/// IP_CHK field position
#define HSU_IP_CHK_POS                18
/// AES_128_CMAC field bit
#define HSU_AES_128_CMAC_BIT          ((uint32_t)0x00020000)
/// AES_128_CMAC field position
#define HSU_AES_128_CMAC_POS          17
/// TKIP_MIC field bit
#define HSU_TKIP_MIC_BIT              ((uint32_t)0x00010000)
/// TKIP_MIC field position
#define HSU_TKIP_MIC_POS              16
/// VERSION field mask
#define HSU_VERSION_MASK              ((uint32_t)0x0000FFFF)
/// VERSION field LSB position
#define HSU_VERSION_LSB               0
/// VERSION field width
#define HSU_VERSION_WIDTH             ((uint32_t)0x00000010)

/// RSA_768 field reset value
#define HSU_RSA_768_RST               0x1
/// RSA_512 field reset value
#define HSU_RSA_512_RST               0x1
/// RSA_256 field reset value
#define HSU_RSA_256_RST               0x1
/// RSA_4096 field reset value
#define HSU_RSA_4096_RST              0x1
/// RSA_2048 field reset value
#define HSU_RSA_2048_RST              0x1
/// RSA_1024 field reset value
#define HSU_RSA_1024_RST              0x1
/// HMAC_SHA512_SHA384 field reset value
#define HSU_HMAC_SHA512_SHA384_RST    0x1
/// HMAC_SHA256_SHA224 field reset value
#define HSU_HMAC_SHA256_SHA224_RST    0x1
/// HMAC_SHA1 field reset value
#define HSU_HMAC_SHA1_RST             0x1
/// SHA_512_384 field reset value
#define HSU_SHA_512_384_RST           0x1
/// SHA_256_224 field reset value
#define HSU_SHA_256_224_RST           0x1
/// SHA_1 field reset value
#define HSU_SHA_1_RST                 0x1
/// IP_CHK field reset value
#define HSU_IP_CHK_RST                0x1
/// AES_128_CMAC field reset value
#define HSU_AES_128_CMAC_RST          0x1
/// TKIP_MIC field reset value
#define HSU_TKIP_MIC_RST              0x1
/// VERSION field reset value
#define HSU_VERSION_RST               0x8

/**
 * @brief Unpacks REVISION's fields from current value of the REVISION register.
 *
 * Reads the REVISION register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rsa768 - Will be populated with the current value of this field from the register.
 * @param[out] rsa512 - Will be populated with the current value of this field from the register.
 * @param[out] rsa256 - Will be populated with the current value of this field from the register.
 * @param[out] rsa4096 - Will be populated with the current value of this field from the register.
 * @param[out] rsa2048 - Will be populated with the current value of this field from the register.
 * @param[out] rsa1024 - Will be populated with the current value of this field from the register.
 * @param[out] hmacsha512sha384 - Will be populated with the current value of this field from the register.
 * @param[out] hmacsha256sha224 - Will be populated with the current value of this field from the register.
 * @param[out] hmacsha1 - Will be populated with the current value of this field from the register.
 * @param[out] sha512384 - Will be populated with the current value of this field from the register.
 * @param[out] sha256224 - Will be populated with the current value of this field from the register.
 * @param[out] sha1 - Will be populated with the current value of this field from the register.
 * @param[out] ipchk - Will be populated with the current value of this field from the register.
 * @param[out] aes128cmac - Will be populated with the current value of this field from the register.
 * @param[out] tkipmic - Will be populated with the current value of this field from the register.
 * @param[out] version - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_revision_unpack(uint8_t* rsa768, uint8_t* rsa512, uint8_t* rsa256, uint8_t* rsa4096, uint8_t* rsa2048, uint8_t* rsa1024, uint8_t* hmacsha512sha384, uint8_t* hmacsha256sha224, uint8_t* hmacsha1, uint8_t* sha512384, uint8_t* sha256224, uint8_t* sha1, uint8_t* ipchk, uint8_t* aes128cmac, uint8_t* tkipmic, uint16_t* version)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);

    *rsa768 = (localVal & ((uint32_t)0x40000000)) >> 30;
    *rsa512 = (localVal & ((uint32_t)0x20000000)) >> 29;
    *rsa256 = (localVal & ((uint32_t)0x10000000)) >> 28;
    *rsa4096 = (localVal & ((uint32_t)0x08000000)) >> 27;
    *rsa2048 = (localVal & ((uint32_t)0x04000000)) >> 26;
    *rsa1024 = (localVal & ((uint32_t)0x02000000)) >> 25;
    *hmacsha512sha384 = (localVal & ((uint32_t)0x01000000)) >> 24;
    *hmacsha256sha224 = (localVal & ((uint32_t)0x00800000)) >> 23;
    *hmacsha1 = (localVal & ((uint32_t)0x00400000)) >> 22;
    *sha512384 = (localVal & ((uint32_t)0x00200000)) >> 21;
    *sha256224 = (localVal & ((uint32_t)0x00100000)) >> 20;
    *sha1 = (localVal & ((uint32_t)0x00080000)) >> 19;
    *ipchk = (localVal & ((uint32_t)0x00040000)) >> 18;
    *aes128cmac = (localVal & ((uint32_t)0x00020000)) >> 17;
    *tkipmic = (localVal & ((uint32_t)0x00010000)) >> 16;
    *version = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the RSA_768 field in the REVISION register.
 *
 * The REVISION register will be read and the RSA_768 field's value will be returned.
 *
 * @return The current value of the RSA_768 field in the REVISION register.
 */
__INLINE uint8_t hsu_rsa_768_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Returns the current value of the RSA_512 field in the REVISION register.
 *
 * The REVISION register will be read and the RSA_512 field's value will be returned.
 *
 * @return The current value of the RSA_512 field in the REVISION register.
 */
__INLINE uint8_t hsu_rsa_512_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Returns the current value of the RSA_256 field in the REVISION register.
 *
 * The REVISION register will be read and the RSA_256 field's value will be returned.
 *
 * @return The current value of the RSA_256 field in the REVISION register.
 */
__INLINE uint8_t hsu_rsa_256_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Returns the current value of the RSA_4096 field in the REVISION register.
 *
 * The REVISION register will be read and the RSA_4096 field's value will be returned.
 *
 * @return The current value of the RSA_4096 field in the REVISION register.
 */
__INLINE uint8_t hsu_rsa_4096_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Returns the current value of the RSA_2048 field in the REVISION register.
 *
 * The REVISION register will be read and the RSA_2048 field's value will be returned.
 *
 * @return The current value of the RSA_2048 field in the REVISION register.
 */
__INLINE uint8_t hsu_rsa_2048_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Returns the current value of the RSA_1024 field in the REVISION register.
 *
 * The REVISION register will be read and the RSA_1024 field's value will be returned.
 *
 * @return The current value of the RSA_1024 field in the REVISION register.
 */
__INLINE uint8_t hsu_rsa_1024_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Returns the current value of the HMAC_SHA512_SHA384 field in the REVISION register.
 *
 * The REVISION register will be read and the HMAC_SHA512_SHA384 field's value will be returned.
 *
 * @return The current value of the HMAC_SHA512_SHA384 field in the REVISION register.
 */
__INLINE uint8_t hsu_hmac_sha512_sha384_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Returns the current value of the HMAC_SHA256_SHA224 field in the REVISION register.
 *
 * The REVISION register will be read and the HMAC_SHA256_SHA224 field's value will be returned.
 *
 * @return The current value of the HMAC_SHA256_SHA224 field in the REVISION register.
 */
__INLINE uint8_t hsu_hmac_sha256_sha224_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Returns the current value of the HMAC_SHA1 field in the REVISION register.
 *
 * The REVISION register will be read and the HMAC_SHA1 field's value will be returned.
 *
 * @return The current value of the HMAC_SHA1 field in the REVISION register.
 */
__INLINE uint8_t hsu_hmac_sha1_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Returns the current value of the SHA_512_384 field in the REVISION register.
 *
 * The REVISION register will be read and the SHA_512_384 field's value will be returned.
 *
 * @return The current value of the SHA_512_384 field in the REVISION register.
 */
__INLINE uint8_t hsu_sha_512_384_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Returns the current value of the SHA_256_224 field in the REVISION register.
 *
 * The REVISION register will be read and the SHA_256_224 field's value will be returned.
 *
 * @return The current value of the SHA_256_224 field in the REVISION register.
 */
__INLINE uint8_t hsu_sha_256_224_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Returns the current value of the SHA_1 field in the REVISION register.
 *
 * The REVISION register will be read and the SHA_1 field's value will be returned.
 *
 * @return The current value of the SHA_1 field in the REVISION register.
 */
__INLINE uint8_t hsu_sha_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Returns the current value of the IP_CHK field in the REVISION register.
 *
 * The REVISION register will be read and the IP_CHK field's value will be returned.
 *
 * @return The current value of the IP_CHK field in the REVISION register.
 */
__INLINE uint8_t hsu_ip_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

/**
 * @brief Returns the current value of the AES_128_CMAC field in the REVISION register.
 *
 * The REVISION register will be read and the AES_128_CMAC field's value will be returned.
 *
 * @return The current value of the AES_128_CMAC field in the REVISION register.
 */
__INLINE uint8_t hsu_aes_128_cmac_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Returns the current value of the TKIP_MIC field in the REVISION register.
 *
 * The REVISION register will be read and the TKIP_MIC field's value will be returned.
 *
 * @return The current value of the TKIP_MIC field in the REVISION register.
 */
__INLINE uint8_t hsu_tkip_mic_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Returns the current value of the VERSION field in the REVISION register.
 *
 * The REVISION register will be read and the VERSION field's value will be returned.
 *
 * @return The current value of the VERSION field in the REVISION register.
 */
__INLINE uint16_t hsu_version_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/// @}

/**
 * @name CONTROL register definitions
 * <table>
 * <caption>CONTROL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 12:08 <td>         MODE <td> R <td> R/W <td> 0x0
 * <tr><td> 05    <td>  LAST_BUFFER <td> R <td> R/W <td> 0
 * <tr><td> 04    <td> FIRST_BUFFER <td> R <td> R/W <td> 0
 * <tr><td> 00    <td>        START <td> R/W <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the CONTROL register
#define HSU_CONTROL_ADDR   0x24930004
/// Offset of the CONTROL register from the base address
#define HSU_CONTROL_OFFSET 0x00000004
/// Index of the CONTROL register
#define HSU_CONTROL_INDEX  0x00000001
/// Reset value of the CONTROL register
#define HSU_CONTROL_RESET  0x00000000

/**
 * @brief Returns the current value of the CONTROL register.
 * The CONTROL register will be read and its value returned.
 * @return The current value of the CONTROL register.
 */
__INLINE uint32_t hsu_control_get(void)
{
    return REG_PL_RD(HSU_CONTROL_ADDR);
}

/**
 * @brief Sets the CONTROL register to a value.
 * The CONTROL register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_control_set(uint32_t value)
{
    REG_PL_WR(HSU_CONTROL_ADDR, value);
}

// field definitions
/// MODE field mask
#define HSU_MODE_MASK           ((uint32_t)0x00001F00)
/// MODE field LSB position
#define HSU_MODE_LSB            8
/// MODE field width
#define HSU_MODE_WIDTH          ((uint32_t)0x00000005)
/// LAST_BUFFER field bit
#define HSU_LAST_BUFFER_BIT     ((uint32_t)0x00000020)
/// LAST_BUFFER field position
#define HSU_LAST_BUFFER_POS     5
/// FIRST_BUFFER field bit
#define HSU_FIRST_BUFFER_BIT    ((uint32_t)0x00000010)
/// FIRST_BUFFER field position
#define HSU_FIRST_BUFFER_POS    4
/// START field bit
#define HSU_START_BIT           ((uint32_t)0x00000001)
/// START field position
#define HSU_START_POS           0

/// MODE field reset value
#define HSU_MODE_RST            0x0
/// LAST_BUFFER field reset value
#define HSU_LAST_BUFFER_RST     0x0
/// FIRST_BUFFER field reset value
#define HSU_FIRST_BUFFER_RST    0x0
/// START field reset value
#define HSU_START_RST           0x0

/**
 * @brief Constructs a value for the CONTROL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] mode - The value to use for the MODE field.
 * @param[in] lastbuffer - The value to use for the LAST_BUFFER field.
 * @param[in] firstbuffer - The value to use for the FIRST_BUFFER field.
 * @param[in] start - The value to use for the START field.
 */
__INLINE void hsu_control_pack(uint8_t mode, uint8_t lastbuffer, uint8_t firstbuffer, uint8_t start)
{
    ASSERT_ERR((((uint32_t)mode << 8) & ~((uint32_t)0x00001F00)) == 0);
    ASSERT_ERR((((uint32_t)lastbuffer << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)firstbuffer << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)start << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_CONTROL_ADDR,  ((uint32_t)mode << 8) | ((uint32_t)lastbuffer << 5) | ((uint32_t)firstbuffer << 4) | ((uint32_t)start << 0));
}

/**
 * @brief Unpacks CONTROL's fields from current value of the CONTROL register.
 *
 * Reads the CONTROL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] mode - Will be populated with the current value of this field from the register.
 * @param[out] lastbuffer - Will be populated with the current value of this field from the register.
 * @param[out] firstbuffer - Will be populated with the current value of this field from the register.
 * @param[out] start - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_control_unpack(uint8_t* mode, uint8_t* lastbuffer, uint8_t* firstbuffer, uint8_t* start)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_ADDR);

    *mode = (localVal & ((uint32_t)0x00001F00)) >> 8;
    *lastbuffer = (localVal & ((uint32_t)0x00000020)) >> 5;
    *firstbuffer = (localVal & ((uint32_t)0x00000010)) >> 4;
    *start = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the MODE field in the CONTROL register.
 *
 * The CONTROL register will be read and the MODE field's value will be returned.
 *
 * @return The current value of the MODE field in the CONTROL register.
 */
__INLINE uint8_t hsu_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00001F00)) >> 8);
}

/**
 * @brief Sets the MODE field of the CONTROL register.
 *
 * The CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mode - The value to set the field to.
 */
__INLINE void hsu_mode_setf(uint8_t mode)
{
    ASSERT_ERR((((uint32_t)mode << 8) & ~((uint32_t)0x00001F00)) == 0);
    REG_PL_WR(HSU_CONTROL_ADDR, (REG_PL_RD(HSU_CONTROL_ADDR) & ~((uint32_t)0x00001F00)) | ((uint32_t)mode << 8));
}

/**
 * @brief Returns the current value of the LAST_BUFFER field in the CONTROL register.
 *
 * The CONTROL register will be read and the LAST_BUFFER field's value will be returned.
 *
 * @return The current value of the LAST_BUFFER field in the CONTROL register.
 */
__INLINE uint8_t hsu_last_buffer_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the LAST_BUFFER field of the CONTROL register.
 *
 * The CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lastbuffer - The value to set the field to.
 */
__INLINE void hsu_last_buffer_setf(uint8_t lastbuffer)
{
    ASSERT_ERR((((uint32_t)lastbuffer << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(HSU_CONTROL_ADDR, (REG_PL_RD(HSU_CONTROL_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)lastbuffer << 5));
}

/**
 * @brief Returns the current value of the FIRST_BUFFER field in the CONTROL register.
 *
 * The CONTROL register will be read and the FIRST_BUFFER field's value will be returned.
 *
 * @return The current value of the FIRST_BUFFER field in the CONTROL register.
 */
__INLINE uint8_t hsu_first_buffer_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the FIRST_BUFFER field of the CONTROL register.
 *
 * The CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] firstbuffer - The value to set the field to.
 */
__INLINE void hsu_first_buffer_setf(uint8_t firstbuffer)
{
    ASSERT_ERR((((uint32_t)firstbuffer << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(HSU_CONTROL_ADDR, (REG_PL_RD(HSU_CONTROL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)firstbuffer << 4));
}

/**
 * @brief Returns the current value of the START field in the CONTROL register.
 *
 * The CONTROL register will be read and the START field's value will be returned.
 *
 * @return The current value of the START field in the CONTROL register.
 */
__INLINE uint8_t hsu_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the START field of the CONTROL register.
 *
 * The CONTROL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] start - The value to set the field to.
 */
__INLINE void hsu_start_setf(uint8_t start)
{
    ASSERT_ERR((((uint32_t)start << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_CONTROL_ADDR, (REG_PL_RD(HSU_CONTROL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)start << 0));
}

/// @}

/**
 * @name STATUS_SET register definitions
 * <table>
 * <caption>STATUS_SET bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 24 <td>  DONE_SET_EPM <td> W <td> R <td> 0
 * <tr><td> 20 <td>  DONE_SET_RSA <td> W <td> R <td> 0
 * <tr><td> 16 <td> DONE_SET_HMAC <td> W <td> R <td> 0
 * <tr><td> 12 <td>  DONE_SET_SHA <td> W <td> R <td> 0
 * <tr><td> 08 <td> DONE_SET_CMAC <td> W <td> R <td> 0
 * <tr><td> 04 <td>  DONE_SET_CHK <td> W <td> R <td> 0
 * <tr><td> 00 <td> DONE_SET_TKIP <td> W <td> R <td> 0
 * </table>
 *
 * @{
 */

/// Address of the STATUS_SET register
#define HSU_STATUS_SET_ADDR   0x24930008
/// Offset of the STATUS_SET register from the base address
#define HSU_STATUS_SET_OFFSET 0x00000008
/// Index of the STATUS_SET register
#define HSU_STATUS_SET_INDEX  0x00000002
/// Reset value of the STATUS_SET register
#define HSU_STATUS_SET_RESET  0x00000000

/**
 * @brief Returns the current value of the STATUS_SET register.
 * The STATUS_SET register will be read and its value returned.
 * @return The current value of the STATUS_SET register.
 */
__INLINE uint32_t hsu_status_set_get(void)
{
    return REG_PL_RD(HSU_STATUS_SET_ADDR);
}

/**
 * @brief Sets the STATUS_SET register to a value.
 * The STATUS_SET register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_status_set_set(uint32_t value)
{
    REG_PL_WR(HSU_STATUS_SET_ADDR, value);
}

// field definitions
/// DONE_SET_EPM field bit
#define HSU_DONE_SET_EPM_BIT     ((uint32_t)0x01000000)
/// DONE_SET_EPM field position
#define HSU_DONE_SET_EPM_POS     24
/// DONE_SET_RSA field bit
#define HSU_DONE_SET_RSA_BIT     ((uint32_t)0x00100000)
/// DONE_SET_RSA field position
#define HSU_DONE_SET_RSA_POS     20
/// DONE_SET_HMAC field bit
#define HSU_DONE_SET_HMAC_BIT    ((uint32_t)0x00010000)
/// DONE_SET_HMAC field position
#define HSU_DONE_SET_HMAC_POS    16
/// DONE_SET_SHA field bit
#define HSU_DONE_SET_SHA_BIT     ((uint32_t)0x00001000)
/// DONE_SET_SHA field position
#define HSU_DONE_SET_SHA_POS     12
/// DONE_SET_CMAC field bit
#define HSU_DONE_SET_CMAC_BIT    ((uint32_t)0x00000100)
/// DONE_SET_CMAC field position
#define HSU_DONE_SET_CMAC_POS    8
/// DONE_SET_CHK field bit
#define HSU_DONE_SET_CHK_BIT     ((uint32_t)0x00000010)
/// DONE_SET_CHK field position
#define HSU_DONE_SET_CHK_POS     4
/// DONE_SET_TKIP field bit
#define HSU_DONE_SET_TKIP_BIT    ((uint32_t)0x00000001)
/// DONE_SET_TKIP field position
#define HSU_DONE_SET_TKIP_POS    0

/// DONE_SET_EPM field reset value
#define HSU_DONE_SET_EPM_RST     0x0
/// DONE_SET_RSA field reset value
#define HSU_DONE_SET_RSA_RST     0x0
/// DONE_SET_HMAC field reset value
#define HSU_DONE_SET_HMAC_RST    0x0
/// DONE_SET_SHA field reset value
#define HSU_DONE_SET_SHA_RST     0x0
/// DONE_SET_CMAC field reset value
#define HSU_DONE_SET_CMAC_RST    0x0
/// DONE_SET_CHK field reset value
#define HSU_DONE_SET_CHK_RST     0x0
/// DONE_SET_TKIP field reset value
#define HSU_DONE_SET_TKIP_RST    0x0

/**
 * @brief Unpacks STATUS_SET's fields from current value of the STATUS_SET register.
 *
 * Reads the STATUS_SET register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] donesetepm - Will be populated with the current value of this field from the register.
 * @param[out] donesetrsa - Will be populated with the current value of this field from the register.
 * @param[out] donesethmac - Will be populated with the current value of this field from the register.
 * @param[out] donesetsha - Will be populated with the current value of this field from the register.
 * @param[out] donesetcmac - Will be populated with the current value of this field from the register.
 * @param[out] donesetchk - Will be populated with the current value of this field from the register.
 * @param[out] donesettkip - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_status_set_unpack(uint8_t* donesetepm, uint8_t* donesetrsa, uint8_t* donesethmac, uint8_t* donesetsha, uint8_t* donesetcmac, uint8_t* donesetchk, uint8_t* donesettkip)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_SET_ADDR);

    *donesetepm = (localVal & ((uint32_t)0x01000000)) >> 24;
    *donesetrsa = (localVal & ((uint32_t)0x00100000)) >> 20;
    *donesethmac = (localVal & ((uint32_t)0x00010000)) >> 16;
    *donesetsha = (localVal & ((uint32_t)0x00001000)) >> 12;
    *donesetcmac = (localVal & ((uint32_t)0x00000100)) >> 8;
    *donesetchk = (localVal & ((uint32_t)0x00000010)) >> 4;
    *donesettkip = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the DONE_SET_EPM field in the STATUS_SET register.
 *
 * The STATUS_SET register will be read and the DONE_SET_EPM field's value will be returned.
 *
 * @return The current value of the DONE_SET_EPM field in the STATUS_SET register.
 */
__INLINE uint8_t hsu_done_set_epm_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_SET_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Returns the current value of the DONE_SET_RSA field in the STATUS_SET register.
 *
 * The STATUS_SET register will be read and the DONE_SET_RSA field's value will be returned.
 *
 * @return The current value of the DONE_SET_RSA field in the STATUS_SET register.
 */
__INLINE uint8_t hsu_done_set_rsa_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_SET_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Returns the current value of the DONE_SET_HMAC field in the STATUS_SET register.
 *
 * The STATUS_SET register will be read and the DONE_SET_HMAC field's value will be returned.
 *
 * @return The current value of the DONE_SET_HMAC field in the STATUS_SET register.
 */
__INLINE uint8_t hsu_done_set_hmac_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_SET_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Returns the current value of the DONE_SET_SHA field in the STATUS_SET register.
 *
 * The STATUS_SET register will be read and the DONE_SET_SHA field's value will be returned.
 *
 * @return The current value of the DONE_SET_SHA field in the STATUS_SET register.
 */
__INLINE uint8_t hsu_done_set_sha_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_SET_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Returns the current value of the DONE_SET_CMAC field in the STATUS_SET register.
 *
 * The STATUS_SET register will be read and the DONE_SET_CMAC field's value will be returned.
 *
 * @return The current value of the DONE_SET_CMAC field in the STATUS_SET register.
 */
__INLINE uint8_t hsu_done_set_cmac_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Returns the current value of the DONE_SET_CHK field in the STATUS_SET register.
 *
 * The STATUS_SET register will be read and the DONE_SET_CHK field's value will be returned.
 *
 * @return The current value of the DONE_SET_CHK field in the STATUS_SET register.
 */
__INLINE uint8_t hsu_done_set_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Returns the current value of the DONE_SET_TKIP field in the STATUS_SET register.
 *
 * The STATUS_SET register will be read and the DONE_SET_TKIP field's value will be returned.
 *
 * @return The current value of the DONE_SET_TKIP field in the STATUS_SET register.
 */
__INLINE uint8_t hsu_done_set_tkip_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_SET_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name STATUS_CLEAR register definitions
 * <table>
 * <caption>STATUS_CLEAR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> DONE_CLEAR <td> R/W <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the STATUS_CLEAR register
#define HSU_STATUS_CLEAR_ADDR   0x2493000C
/// Offset of the STATUS_CLEAR register from the base address
#define HSU_STATUS_CLEAR_OFFSET 0x0000000C
/// Index of the STATUS_CLEAR register
#define HSU_STATUS_CLEAR_INDEX  0x00000003
/// Reset value of the STATUS_CLEAR register
#define HSU_STATUS_CLEAR_RESET  0x00000000

/**
 * @brief Returns the current value of the STATUS_CLEAR register.
 * The STATUS_CLEAR register will be read and its value returned.
 * @return The current value of the STATUS_CLEAR register.
 */
__INLINE uint32_t hsu_status_clear_get(void)
{
    return REG_PL_RD(HSU_STATUS_CLEAR_ADDR);
}

/**
 * @brief Sets the STATUS_CLEAR register to a value.
 * The STATUS_CLEAR register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_status_clear_set(uint32_t value)
{
    REG_PL_WR(HSU_STATUS_CLEAR_ADDR, value);
}

// field definitions
/// DONE_CLEAR field bit
#define HSU_DONE_CLEAR_BIT    ((uint32_t)0x00000001)
/// DONE_CLEAR field position
#define HSU_DONE_CLEAR_POS    0

/// DONE_CLEAR field reset value
#define HSU_DONE_CLEAR_RST    0x0

/**
 * @brief Returns the current value of the DONE_CLEAR field in the STATUS_CLEAR register.
 *
 * The STATUS_CLEAR register will be read and the DONE_CLEAR field's value will be returned.
 *
 * @return The current value of the DONE_CLEAR field in the STATUS_CLEAR register.
 */
__INLINE uint8_t hsu_done_clear_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_CLEAR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the DONE_CLEAR field of the STATUS_CLEAR register.
 *
 * The STATUS_CLEAR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] doneclear - The value to set the field to.
 */
__INLINE void hsu_done_clear_setf(uint8_t doneclear)
{
    ASSERT_ERR((((uint32_t)doneclear << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_STATUS_CLEAR_ADDR, (uint32_t)doneclear << 0);
}

/// @}

/**
 * @name KEY_TAB register definitions
 * <table>
 * <caption>KEY_TAB bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> KEY <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the KEY_TAB register
#define HSU_KEY_TAB_ADDR   0x24930010
/// Offset of the KEY_TAB register from the base address
#define HSU_KEY_TAB_OFFSET 0x00000010
/// Index of the KEY_TAB register
#define HSU_KEY_TAB_INDEX  0x00000004
/// Reset value of the KEY_TAB register
#define HSU_KEY_TAB_RESET  0x00000000
/// Number of elements of the KEY_TAB register array
#define HSU_KEY_TAB_COUNT  4

/**
 * @brief Returns the current value of the KEY_TAB register.
 * The KEY_TAB register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the KEY_TAB register.
 */
__INLINE uint32_t hsu_key_tab_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 3);
    return REG_PL_RD(HSU_KEY_TAB_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the KEY_TAB register to a value.
 * The KEY_TAB register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void hsu_key_tab_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 3);
    REG_PL_WR(HSU_KEY_TAB_ADDR + reg_idx * 4, value);
}

// field definitions
/// KEY field mask
#define HSU_KEY_MASK   ((uint32_t)0xFFFFFFFF)
/// KEY field LSB position
#define HSU_KEY_LSB    0
/// KEY field width
#define HSU_KEY_WIDTH  ((uint32_t)0x00000020)

/// KEY field reset value
#define HSU_KEY_RST    0x0

/**
 * @brief Returns the current value of the KEY field in the KEY_TAB register.
 *
 * The KEY_TAB register will be read and the KEY field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the KEY field in the KEY_TAB register.
 */
__INLINE uint32_t hsu_key_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 3);
    uint32_t localVal = REG_PL_RD(HSU_KEY_TAB_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the KEY field of the KEY_TAB register.
 *
 * The KEY_TAB register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] key - The value to set the field to.
 */
__INLINE void hsu_key_setf(int reg_idx, uint32_t key)
{
    ASSERT_ERR(reg_idx <= 3);
    ASSERT_ERR((((uint32_t)key << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(HSU_KEY_TAB_ADDR + reg_idx * 4, (uint32_t)key << 0);
}

/// @}

/**
 * @name SOURCE_ADDR register definitions
 * <table>
 * <caption>SOURCE_ADDR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> SOURCE_ADDR <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SOURCE_ADDR register
#define HSU_SOURCE_ADDR_ADDR   0x24930020
/// Offset of the SOURCE_ADDR register from the base address
#define HSU_SOURCE_ADDR_OFFSET 0x00000020
/// Index of the SOURCE_ADDR register
#define HSU_SOURCE_ADDR_INDEX  0x00000008
/// Reset value of the SOURCE_ADDR register
#define HSU_SOURCE_ADDR_RESET  0x00000000

/**
 * @brief Returns the current value of the SOURCE_ADDR register.
 * The SOURCE_ADDR register will be read and its value returned.
 * @return The current value of the SOURCE_ADDR register.
 */
__INLINE uint32_t hsu_source_addr_get(void)
{
    return REG_PL_RD(HSU_SOURCE_ADDR_ADDR);
}

/**
 * @brief Sets the SOURCE_ADDR register to a value.
 * The SOURCE_ADDR register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_source_addr_set(uint32_t value)
{
    REG_PL_WR(HSU_SOURCE_ADDR_ADDR, value);
}

// field definitions
/// SOURCE_ADDR field mask
#define HSU_SOURCE_ADDR_MASK   ((uint32_t)0xFFFFFFFF)
/// SOURCE_ADDR field LSB position
#define HSU_SOURCE_ADDR_LSB    0
/// SOURCE_ADDR field width
#define HSU_SOURCE_ADDR_WIDTH  ((uint32_t)0x00000020)

/// SOURCE_ADDR field reset value
#define HSU_SOURCE_ADDR_RST    0x0

/**
 * @brief Returns the current value of the SOURCE_ADDR field in the SOURCE_ADDR register.
 *
 * The SOURCE_ADDR register will be read and the SOURCE_ADDR field's value will be returned.
 *
 * @return The current value of the SOURCE_ADDR field in the SOURCE_ADDR register.
 */
__INLINE uint32_t hsu_source_addr_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_SOURCE_ADDR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the SOURCE_ADDR field of the SOURCE_ADDR register.
 *
 * The SOURCE_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sourceaddr - The value to set the field to.
 */
__INLINE void hsu_source_addr_setf(uint32_t sourceaddr)
{
    ASSERT_ERR((((uint32_t)sourceaddr << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(HSU_SOURCE_ADDR_ADDR, (uint32_t)sourceaddr << 0);
}

/// @}

/**
 * @name LENGTH register definitions
 * <table>
 * <caption>LENGTH bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:00 <td> LENGTH <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the LENGTH register
#define HSU_LENGTH_ADDR   0x24930024
/// Offset of the LENGTH register from the base address
#define HSU_LENGTH_OFFSET 0x00000024
/// Index of the LENGTH register
#define HSU_LENGTH_INDEX  0x00000009
/// Reset value of the LENGTH register
#define HSU_LENGTH_RESET  0x00000000

/**
 * @brief Returns the current value of the LENGTH register.
 * The LENGTH register will be read and its value returned.
 * @return The current value of the LENGTH register.
 */
__INLINE uint32_t hsu_length_get(void)
{
    return REG_PL_RD(HSU_LENGTH_ADDR);
}

/**
 * @brief Sets the LENGTH register to a value.
 * The LENGTH register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_length_set(uint32_t value)
{
    REG_PL_WR(HSU_LENGTH_ADDR, value);
}

// field definitions
/// LENGTH field mask
#define HSU_LENGTH_MASK   ((uint32_t)0x0000FFFF)
/// LENGTH field LSB position
#define HSU_LENGTH_LSB    0
/// LENGTH field width
#define HSU_LENGTH_WIDTH  ((uint32_t)0x00000010)

/// LENGTH field reset value
#define HSU_LENGTH_RST    0x0

/**
 * @brief Returns the current value of the LENGTH field in the LENGTH register.
 *
 * The LENGTH register will be read and the LENGTH field's value will be returned.
 *
 * @return The current value of the LENGTH field in the LENGTH register.
 */
__INLINE uint16_t hsu_length_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_LENGTH_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the LENGTH field of the LENGTH register.
 *
 * The LENGTH register will be read, modified to contain the new field value, and written.
 *
 * @param[in] length - The value to set the field to.
 */
__INLINE void hsu_length_setf(uint16_t length)
{
    ASSERT_ERR((((uint32_t)length << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(HSU_LENGTH_ADDR, (uint32_t)length << 0);
}

/// @}

/**
 * @name MIC_TAB register definitions
 * <table>
 * <caption>MIC_TAB bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> MIC <td> R/W <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the MIC_TAB register
#define HSU_MIC_TAB_ADDR   0x24930028
/// Offset of the MIC_TAB register from the base address
#define HSU_MIC_TAB_OFFSET 0x00000028
/// Index of the MIC_TAB register
#define HSU_MIC_TAB_INDEX  0x0000000A
/// Reset value of the MIC_TAB register
#define HSU_MIC_TAB_RESET  0x00000000
/// Number of elements of the MIC_TAB register array
#define HSU_MIC_TAB_COUNT  2

/**
 * @brief Returns the current value of the MIC_TAB register.
 * The MIC_TAB register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the MIC_TAB register.
 */
__INLINE uint32_t hsu_mic_tab_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 1);
    return REG_PL_RD(HSU_MIC_TAB_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the MIC_TAB register to a value.
 * The MIC_TAB register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void hsu_mic_tab_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 1);
    REG_PL_WR(HSU_MIC_TAB_ADDR + reg_idx * 4, value);
}

// field definitions
/// MIC field mask
#define HSU_MIC_MASK   ((uint32_t)0xFFFFFFFF)
/// MIC field LSB position
#define HSU_MIC_LSB    0
/// MIC field width
#define HSU_MIC_WIDTH  ((uint32_t)0x00000020)

/// MIC field reset value
#define HSU_MIC_RST    0x0

/**
 * @brief Returns the current value of the MIC field in the MIC_TAB register.
 *
 * The MIC_TAB register will be read and the MIC field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the MIC field in the MIC_TAB register.
 */
__INLINE uint32_t hsu_mic_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 1);
    uint32_t localVal = REG_PL_RD(HSU_MIC_TAB_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the MIC field of the MIC_TAB register.
 *
 * The MIC_TAB register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] mic - The value to set the field to.
 */
__INLINE void hsu_mic_setf(int reg_idx, uint32_t mic)
{
    ASSERT_ERR(reg_idx <= 1);
    ASSERT_ERR((((uint32_t)mic << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(HSU_MIC_TAB_ADDR + reg_idx * 4, (uint32_t)mic << 0);
}

/// @}

/**
 * @name REMAINING register definitions
 * <table>
 * <caption>REMAINING bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:24 <td> REMAINING_LENGTH <td> R/W <td> R/W <td> 0x0
 * <tr><td> 23:16 <td>  REMAINING_BYTE2 <td> R/W <td> R/W <td> 0x0
 * <tr><td> 15:08 <td>  REMAINING_BYTE1 <td> R/W <td> R/W <td> 0x0
 * <tr><td> 07:00 <td>  REMAINING_BYTE0 <td> R/W <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the REMAINING register
#define HSU_REMAINING_ADDR   0x24930030
/// Offset of the REMAINING register from the base address
#define HSU_REMAINING_OFFSET 0x00000030
/// Index of the REMAINING register
#define HSU_REMAINING_INDEX  0x0000000C
/// Reset value of the REMAINING register
#define HSU_REMAINING_RESET  0x00000000

/**
 * @brief Returns the current value of the REMAINING register.
 * The REMAINING register will be read and its value returned.
 * @return The current value of the REMAINING register.
 */
__INLINE uint32_t hsu_remaining_get(void)
{
    return REG_PL_RD(HSU_REMAINING_ADDR);
}

/**
 * @brief Sets the REMAINING register to a value.
 * The REMAINING register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_remaining_set(uint32_t value)
{
    REG_PL_WR(HSU_REMAINING_ADDR, value);
}

// field definitions
/// REMAINING_LENGTH field mask
#define HSU_REMAINING_LENGTH_MASK   ((uint32_t)0x03000000)
/// REMAINING_LENGTH field LSB position
#define HSU_REMAINING_LENGTH_LSB    24
/// REMAINING_LENGTH field width
#define HSU_REMAINING_LENGTH_WIDTH  ((uint32_t)0x00000002)
/// REMAINING_BYTE2 field mask
#define HSU_REMAINING_BYTE2_MASK    ((uint32_t)0x00FF0000)
/// REMAINING_BYTE2 field LSB position
#define HSU_REMAINING_BYTE2_LSB     16
/// REMAINING_BYTE2 field width
#define HSU_REMAINING_BYTE2_WIDTH   ((uint32_t)0x00000008)
/// REMAINING_BYTE1 field mask
#define HSU_REMAINING_BYTE1_MASK    ((uint32_t)0x0000FF00)
/// REMAINING_BYTE1 field LSB position
#define HSU_REMAINING_BYTE1_LSB     8
/// REMAINING_BYTE1 field width
#define HSU_REMAINING_BYTE1_WIDTH   ((uint32_t)0x00000008)
/// REMAINING_BYTE0 field mask
#define HSU_REMAINING_BYTE0_MASK    ((uint32_t)0x000000FF)
/// REMAINING_BYTE0 field LSB position
#define HSU_REMAINING_BYTE0_LSB     0
/// REMAINING_BYTE0 field width
#define HSU_REMAINING_BYTE0_WIDTH   ((uint32_t)0x00000008)

/// REMAINING_LENGTH field reset value
#define HSU_REMAINING_LENGTH_RST    0x0
/// REMAINING_BYTE2 field reset value
#define HSU_REMAINING_BYTE2_RST     0x0
/// REMAINING_BYTE1 field reset value
#define HSU_REMAINING_BYTE1_RST     0x0
/// REMAINING_BYTE0 field reset value
#define HSU_REMAINING_BYTE0_RST     0x0

/**
 * @brief Constructs a value for the REMAINING register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] remaininglength - The value to use for the REMAINING_LENGTH field.
 * @param[in] remainingbyte2 - The value to use for the REMAINING_BYTE2 field.
 * @param[in] remainingbyte1 - The value to use for the REMAINING_BYTE1 field.
 * @param[in] remainingbyte0 - The value to use for the REMAINING_BYTE0 field.
 */
__INLINE void hsu_remaining_pack(uint8_t remaininglength, uint8_t remainingbyte2, uint8_t remainingbyte1, uint8_t remainingbyte0)
{
    ASSERT_ERR((((uint32_t)remaininglength << 24) & ~((uint32_t)0x03000000)) == 0);
    ASSERT_ERR((((uint32_t)remainingbyte2 << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)remainingbyte1 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)remainingbyte0 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(HSU_REMAINING_ADDR,  ((uint32_t)remaininglength << 24) | ((uint32_t)remainingbyte2 << 16) | ((uint32_t)remainingbyte1 << 8) | ((uint32_t)remainingbyte0 << 0));
}

/**
 * @brief Unpacks REMAINING's fields from current value of the REMAINING register.
 *
 * Reads the REMAINING register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] remaininglength - Will be populated with the current value of this field from the register.
 * @param[out] remainingbyte2 - Will be populated with the current value of this field from the register.
 * @param[out] remainingbyte1 - Will be populated with the current value of this field from the register.
 * @param[out] remainingbyte0 - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_remaining_unpack(uint8_t* remaininglength, uint8_t* remainingbyte2, uint8_t* remainingbyte1, uint8_t* remainingbyte0)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_ADDR);

    *remaininglength = (localVal & ((uint32_t)0x03000000)) >> 24;
    *remainingbyte2 = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *remainingbyte1 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *remainingbyte0 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the REMAINING_LENGTH field in the REMAINING register.
 *
 * The REMAINING register will be read and the REMAINING_LENGTH field's value will be returned.
 *
 * @return The current value of the REMAINING_LENGTH field in the REMAINING register.
 */
__INLINE uint8_t hsu_remaining_length_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

/**
 * @brief Sets the REMAINING_LENGTH field of the REMAINING register.
 *
 * The REMAINING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remaininglength - The value to set the field to.
 */
__INLINE void hsu_remaining_length_setf(uint8_t remaininglength)
{
    ASSERT_ERR((((uint32_t)remaininglength << 24) & ~((uint32_t)0x03000000)) == 0);
    REG_PL_WR(HSU_REMAINING_ADDR, (REG_PL_RD(HSU_REMAINING_ADDR) & ~((uint32_t)0x03000000)) | ((uint32_t)remaininglength << 24));
}

/**
 * @brief Returns the current value of the REMAINING_BYTE2 field in the REMAINING register.
 *
 * The REMAINING register will be read and the REMAINING_BYTE2 field's value will be returned.
 *
 * @return The current value of the REMAINING_BYTE2 field in the REMAINING register.
 */
__INLINE uint8_t hsu_remaining_byte2_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the REMAINING_BYTE2 field of the REMAINING register.
 *
 * The REMAINING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remainingbyte2 - The value to set the field to.
 */
__INLINE void hsu_remaining_byte2_setf(uint8_t remainingbyte2)
{
    ASSERT_ERR((((uint32_t)remainingbyte2 << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(HSU_REMAINING_ADDR, (REG_PL_RD(HSU_REMAINING_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)remainingbyte2 << 16));
}

/**
 * @brief Returns the current value of the REMAINING_BYTE1 field in the REMAINING register.
 *
 * The REMAINING register will be read and the REMAINING_BYTE1 field's value will be returned.
 *
 * @return The current value of the REMAINING_BYTE1 field in the REMAINING register.
 */
__INLINE uint8_t hsu_remaining_byte1_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the REMAINING_BYTE1 field of the REMAINING register.
 *
 * The REMAINING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remainingbyte1 - The value to set the field to.
 */
__INLINE void hsu_remaining_byte1_setf(uint8_t remainingbyte1)
{
    ASSERT_ERR((((uint32_t)remainingbyte1 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(HSU_REMAINING_ADDR, (REG_PL_RD(HSU_REMAINING_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)remainingbyte1 << 8));
}

/**
 * @brief Returns the current value of the REMAINING_BYTE0 field in the REMAINING register.
 *
 * The REMAINING register will be read and the REMAINING_BYTE0 field's value will be returned.
 *
 * @return The current value of the REMAINING_BYTE0 field in the REMAINING register.
 */
__INLINE uint8_t hsu_remaining_byte0_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the REMAINING_BYTE0 field of the REMAINING register.
 *
 * The REMAINING register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remainingbyte0 - The value to set the field to.
 */
__INLINE void hsu_remaining_byte0_setf(uint8_t remainingbyte0)
{
    ASSERT_ERR((((uint32_t)remainingbyte0 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(HSU_REMAINING_ADDR, (REG_PL_RD(HSU_REMAINING_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)remainingbyte0 << 0));
}

/// @}

#if RW_HSU_SHA_EN
/**
 * @name SHA_TAB register definitions
 * <table>
 * <caption>SHA_TAB bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> SHA <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SHA_TAB register
#define HSU_SHA_TAB_ADDR   0x24930034
/// Offset of the SHA_TAB register from the base address
#define HSU_SHA_TAB_OFFSET 0x00000034
/// Index of the SHA_TAB register
#define HSU_SHA_TAB_INDEX  0x0000000D
/// Reset value of the SHA_TAB register
#define HSU_SHA_TAB_RESET  0x00000000
/// Number of elements of the SHA_TAB register array
#define HSU_SHA_TAB_COUNT  16

/**
 * @brief Returns the current value of the SHA_TAB register.
 * The SHA_TAB register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the SHA_TAB register.
 */
__INLINE uint32_t hsu_sha_tab_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 15);
    return REG_PL_RD(HSU_SHA_TAB_ADDR + reg_idx * 4);
}

// field definitions
/// SHA field mask
#define HSU_SHA_MASK   ((uint32_t)0xFFFFFFFF)
/// SHA field LSB position
#define HSU_SHA_LSB    0
/// SHA field width
#define HSU_SHA_WIDTH  ((uint32_t)0x00000020)

/// SHA field reset value
#define HSU_SHA_RST    0x0

/**
 * @brief Returns the current value of the SHA field in the SHA_TAB register.
 *
 * The SHA_TAB register will be read and the SHA field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the SHA field in the SHA_TAB register.
 */
__INLINE uint32_t hsu_sha_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 15);
    uint32_t localVal = REG_PL_RD(HSU_SHA_TAB_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

#endif // RW_HSU_SHA_EN
/// @}

#if RW_HSU_RSA_EN
/**
 * @name DESTINATION_ADDR register definitions
 * <table>
 * <caption>DESTINATION_ADDR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> DEST_ADDR <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the DESTINATION_ADDR register
#define HSU_DESTINATION_ADDR_ADDR   0x24930074
/// Offset of the DESTINATION_ADDR register from the base address
#define HSU_DESTINATION_ADDR_OFFSET 0x00000074
/// Index of the DESTINATION_ADDR register
#define HSU_DESTINATION_ADDR_INDEX  0x0000001D
/// Reset value of the DESTINATION_ADDR register
#define HSU_DESTINATION_ADDR_RESET  0x00000000

/**
 * @brief Returns the current value of the DESTINATION_ADDR register.
 * The DESTINATION_ADDR register will be read and its value returned.
 * @return The current value of the DESTINATION_ADDR register.
 */
__INLINE uint32_t hsu_destination_addr_get(void)
{
    return REG_PL_RD(HSU_DESTINATION_ADDR_ADDR);
}

/**
 * @brief Sets the DESTINATION_ADDR register to a value.
 * The DESTINATION_ADDR register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_destination_addr_set(uint32_t value)
{
    REG_PL_WR(HSU_DESTINATION_ADDR_ADDR, value);
}

// field definitions
/// DEST_ADDR field mask
#define HSU_DEST_ADDR_MASK   ((uint32_t)0xFFFFFFFF)
/// DEST_ADDR field LSB position
#define HSU_DEST_ADDR_LSB    0
/// DEST_ADDR field width
#define HSU_DEST_ADDR_WIDTH  ((uint32_t)0x00000020)

/// DEST_ADDR field reset value
#define HSU_DEST_ADDR_RST    0x0

/**
 * @brief Returns the current value of the DEST_ADDR field in the DESTINATION_ADDR register.
 *
 * The DESTINATION_ADDR register will be read and the DEST_ADDR field's value will be returned.
 *
 * @return The current value of the DEST_ADDR field in the DESTINATION_ADDR register.
 */
__INLINE uint32_t hsu_dest_addr_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_DESTINATION_ADDR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the DEST_ADDR field of the DESTINATION_ADDR register.
 *
 * The DESTINATION_ADDR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] destaddr - The value to set the field to.
 */
__INLINE void hsu_dest_addr_setf(uint32_t destaddr)
{
    ASSERT_ERR((((uint32_t)destaddr << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(HSU_DESTINATION_ADDR_ADDR, (uint32_t)destaddr << 0);
}

#endif // RW_HSU_RSA_EN
/// @}

/**
 * @name CONTROL_CHK register definitions
 * <table>
 * <caption>CONTROL_CHK bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 05 <td>  LAST_BUFFER_CHK <td> R <td> R/W <td> 0
 * <tr><td> 04 <td> FIRST_BUFFER_CHK <td> R <td> R/W <td> 0
 * <tr><td> 00 <td>        START_CHK <td> R/W <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the CONTROL_CHK register
#define HSU_CONTROL_CHK_ADDR   0x24930078
/// Offset of the CONTROL_CHK register from the base address
#define HSU_CONTROL_CHK_OFFSET 0x00000078
/// Index of the CONTROL_CHK register
#define HSU_CONTROL_CHK_INDEX  0x0000001E
/// Reset value of the CONTROL_CHK register
#define HSU_CONTROL_CHK_RESET  0x00000000

/**
 * @brief Returns the current value of the CONTROL_CHK register.
 * The CONTROL_CHK register will be read and its value returned.
 * @return The current value of the CONTROL_CHK register.
 */
__INLINE uint32_t hsu_control_chk_get(void)
{
    return REG_PL_RD(HSU_CONTROL_CHK_ADDR);
}

/**
 * @brief Sets the CONTROL_CHK register to a value.
 * The CONTROL_CHK register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_control_chk_set(uint32_t value)
{
    REG_PL_WR(HSU_CONTROL_CHK_ADDR, value);
}

// field definitions
/// LAST_BUFFER_CHK field bit
#define HSU_LAST_BUFFER_CHK_BIT     ((uint32_t)0x00000020)
/// LAST_BUFFER_CHK field position
#define HSU_LAST_BUFFER_CHK_POS     5
/// FIRST_BUFFER_CHK field bit
#define HSU_FIRST_BUFFER_CHK_BIT    ((uint32_t)0x00000010)
/// FIRST_BUFFER_CHK field position
#define HSU_FIRST_BUFFER_CHK_POS    4
/// START_CHK field bit
#define HSU_START_CHK_BIT           ((uint32_t)0x00000001)
/// START_CHK field position
#define HSU_START_CHK_POS           0

/// LAST_BUFFER_CHK field reset value
#define HSU_LAST_BUFFER_CHK_RST     0x0
/// FIRST_BUFFER_CHK field reset value
#define HSU_FIRST_BUFFER_CHK_RST    0x0
/// START_CHK field reset value
#define HSU_START_CHK_RST           0x0

/**
 * @brief Constructs a value for the CONTROL_CHK register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] lastbufferchk - The value to use for the LAST_BUFFER_CHK field.
 * @param[in] firstbufferchk - The value to use for the FIRST_BUFFER_CHK field.
 * @param[in] startchk - The value to use for the START_CHK field.
 */
__INLINE void hsu_control_chk_pack(uint8_t lastbufferchk, uint8_t firstbufferchk, uint8_t startchk)
{
    ASSERT_ERR((((uint32_t)lastbufferchk << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)firstbufferchk << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)startchk << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_CONTROL_CHK_ADDR,  ((uint32_t)lastbufferchk << 5) | ((uint32_t)firstbufferchk << 4) | ((uint32_t)startchk << 0));
}

/**
 * @brief Unpacks CONTROL_CHK's fields from current value of the CONTROL_CHK register.
 *
 * Reads the CONTROL_CHK register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] lastbufferchk - Will be populated with the current value of this field from the register.
 * @param[out] firstbufferchk - Will be populated with the current value of this field from the register.
 * @param[out] startchk - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_control_chk_unpack(uint8_t* lastbufferchk, uint8_t* firstbufferchk, uint8_t* startchk)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_CHK_ADDR);

    *lastbufferchk = (localVal & ((uint32_t)0x00000020)) >> 5;
    *firstbufferchk = (localVal & ((uint32_t)0x00000010)) >> 4;
    *startchk = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the LAST_BUFFER_CHK field in the CONTROL_CHK register.
 *
 * The CONTROL_CHK register will be read and the LAST_BUFFER_CHK field's value will be returned.
 *
 * @return The current value of the LAST_BUFFER_CHK field in the CONTROL_CHK register.
 */
__INLINE uint8_t hsu_last_buffer_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_CHK_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the LAST_BUFFER_CHK field of the CONTROL_CHK register.
 *
 * The CONTROL_CHK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lastbufferchk - The value to set the field to.
 */
__INLINE void hsu_last_buffer_chk_setf(uint8_t lastbufferchk)
{
    ASSERT_ERR((((uint32_t)lastbufferchk << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(HSU_CONTROL_CHK_ADDR, (REG_PL_RD(HSU_CONTROL_CHK_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)lastbufferchk << 5));
}

/**
 * @brief Returns the current value of the FIRST_BUFFER_CHK field in the CONTROL_CHK register.
 *
 * The CONTROL_CHK register will be read and the FIRST_BUFFER_CHK field's value will be returned.
 *
 * @return The current value of the FIRST_BUFFER_CHK field in the CONTROL_CHK register.
 */
__INLINE uint8_t hsu_first_buffer_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_CHK_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the FIRST_BUFFER_CHK field of the CONTROL_CHK register.
 *
 * The CONTROL_CHK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] firstbufferchk - The value to set the field to.
 */
__INLINE void hsu_first_buffer_chk_setf(uint8_t firstbufferchk)
{
    ASSERT_ERR((((uint32_t)firstbufferchk << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(HSU_CONTROL_CHK_ADDR, (REG_PL_RD(HSU_CONTROL_CHK_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)firstbufferchk << 4));
}

/**
 * @brief Returns the current value of the START_CHK field in the CONTROL_CHK register.
 *
 * The CONTROL_CHK register will be read and the START_CHK field's value will be returned.
 *
 * @return The current value of the START_CHK field in the CONTROL_CHK register.
 */
__INLINE uint8_t hsu_start_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_CONTROL_CHK_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the START_CHK field of the CONTROL_CHK register.
 *
 * The CONTROL_CHK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] startchk - The value to set the field to.
 */
__INLINE void hsu_start_chk_setf(uint8_t startchk)
{
    ASSERT_ERR((((uint32_t)startchk << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_CONTROL_CHK_ADDR, (REG_PL_RD(HSU_CONTROL_CHK_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)startchk << 0));
}

/// @}

/**
 * @name STATUS_CLEAR_CHK register definitions
 * <table>
 * <caption>STATUS_CLEAR_CHK bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> DONE_CLEAR_CHK <td> R/W <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the STATUS_CLEAR_CHK register
#define HSU_STATUS_CLEAR_CHK_ADDR   0x2493007C
/// Offset of the STATUS_CLEAR_CHK register from the base address
#define HSU_STATUS_CLEAR_CHK_OFFSET 0x0000007C
/// Index of the STATUS_CLEAR_CHK register
#define HSU_STATUS_CLEAR_CHK_INDEX  0x0000001F
/// Reset value of the STATUS_CLEAR_CHK register
#define HSU_STATUS_CLEAR_CHK_RESET  0x00000000

/**
 * @brief Returns the current value of the STATUS_CLEAR_CHK register.
 * The STATUS_CLEAR_CHK register will be read and its value returned.
 * @return The current value of the STATUS_CLEAR_CHK register.
 */
__INLINE uint32_t hsu_status_clear_chk_get(void)
{
    return REG_PL_RD(HSU_STATUS_CLEAR_CHK_ADDR);
}

/**
 * @brief Sets the STATUS_CLEAR_CHK register to a value.
 * The STATUS_CLEAR_CHK register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_status_clear_chk_set(uint32_t value)
{
    REG_PL_WR(HSU_STATUS_CLEAR_CHK_ADDR, value);
}

// field definitions
/// DONE_CLEAR_CHK field bit
#define HSU_DONE_CLEAR_CHK_BIT    ((uint32_t)0x00000001)
/// DONE_CLEAR_CHK field position
#define HSU_DONE_CLEAR_CHK_POS    0

/// DONE_CLEAR_CHK field reset value
#define HSU_DONE_CLEAR_CHK_RST    0x0

/**
 * @brief Returns the current value of the DONE_CLEAR_CHK field in the STATUS_CLEAR_CHK register.
 *
 * The STATUS_CLEAR_CHK register will be read and the DONE_CLEAR_CHK field's value will be returned.
 *
 * @return The current value of the DONE_CLEAR_CHK field in the STATUS_CLEAR_CHK register.
 */
__INLINE uint8_t hsu_done_clear_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_STATUS_CLEAR_CHK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the DONE_CLEAR_CHK field of the STATUS_CLEAR_CHK register.
 *
 * The STATUS_CLEAR_CHK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] doneclearchk - The value to set the field to.
 */
__INLINE void hsu_done_clear_chk_setf(uint8_t doneclearchk)
{
    ASSERT_ERR((((uint32_t)doneclearchk << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_STATUS_CLEAR_CHK_ADDR, (uint32_t)doneclearchk << 0);
}

/// @}

#if RW_HSU_IP_CHK_EN
/**
 * @name SOURCE_ADDR_CHK register definitions
 * <table>
 * <caption>SOURCE_ADDR_CHK bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> SOURCE_ADDR_CHK <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SOURCE_ADDR_CHK register
#define HSU_SOURCE_ADDR_CHK_ADDR   0x24930080
/// Offset of the SOURCE_ADDR_CHK register from the base address
#define HSU_SOURCE_ADDR_CHK_OFFSET 0x00000080
/// Index of the SOURCE_ADDR_CHK register
#define HSU_SOURCE_ADDR_CHK_INDEX  0x00000020
/// Reset value of the SOURCE_ADDR_CHK register
#define HSU_SOURCE_ADDR_CHK_RESET  0x00000000

/**
 * @brief Returns the current value of the SOURCE_ADDR_CHK register.
 * The SOURCE_ADDR_CHK register will be read and its value returned.
 * @return The current value of the SOURCE_ADDR_CHK register.
 */
__INLINE uint32_t hsu_source_addr_chk_get(void)
{
    return REG_PL_RD(HSU_SOURCE_ADDR_CHK_ADDR);
}

/**
 * @brief Sets the SOURCE_ADDR_CHK register to a value.
 * The SOURCE_ADDR_CHK register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_source_addr_chk_set(uint32_t value)
{
    REG_PL_WR(HSU_SOURCE_ADDR_CHK_ADDR, value);
}

// field definitions
/// SOURCE_ADDR_CHK field mask
#define HSU_SOURCE_ADDR_CHK_MASK   ((uint32_t)0xFFFFFFFF)
/// SOURCE_ADDR_CHK field LSB position
#define HSU_SOURCE_ADDR_CHK_LSB    0
/// SOURCE_ADDR_CHK field width
#define HSU_SOURCE_ADDR_CHK_WIDTH  ((uint32_t)0x00000020)

/// SOURCE_ADDR_CHK field reset value
#define HSU_SOURCE_ADDR_CHK_RST    0x0

/**
 * @brief Returns the current value of the SOURCE_ADDR_CHK field in the SOURCE_ADDR_CHK register.
 *
 * The SOURCE_ADDR_CHK register will be read and the SOURCE_ADDR_CHK field's value will be returned.
 *
 * @return The current value of the SOURCE_ADDR_CHK field in the SOURCE_ADDR_CHK register.
 */
__INLINE uint32_t hsu_source_addr_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_SOURCE_ADDR_CHK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the SOURCE_ADDR_CHK field of the SOURCE_ADDR_CHK register.
 *
 * The SOURCE_ADDR_CHK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sourceaddrchk - The value to set the field to.
 */
__INLINE void hsu_source_addr_chk_setf(uint32_t sourceaddrchk)
{
    ASSERT_ERR((((uint32_t)sourceaddrchk << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(HSU_SOURCE_ADDR_CHK_ADDR, (uint32_t)sourceaddrchk << 0);
}

#endif // RW_HSU_IP_CHK_EN
/// @}

/**
 * @name LENGTH_CHK register definitions
 * <table>
 * <caption>LENGTH_CHK bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:00 <td> LENGTH_CHK <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the LENGTH_CHK register
#define HSU_LENGTH_CHK_ADDR   0x24930084
/// Offset of the LENGTH_CHK register from the base address
#define HSU_LENGTH_CHK_OFFSET 0x00000084
/// Index of the LENGTH_CHK register
#define HSU_LENGTH_CHK_INDEX  0x00000021
/// Reset value of the LENGTH_CHK register
#define HSU_LENGTH_CHK_RESET  0x00000000

/**
 * @brief Returns the current value of the LENGTH_CHK register.
 * The LENGTH_CHK register will be read and its value returned.
 * @return The current value of the LENGTH_CHK register.
 */
__INLINE uint32_t hsu_length_chk_get(void)
{
    return REG_PL_RD(HSU_LENGTH_CHK_ADDR);
}

/**
 * @brief Sets the LENGTH_CHK register to a value.
 * The LENGTH_CHK register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_length_chk_set(uint32_t value)
{
    REG_PL_WR(HSU_LENGTH_CHK_ADDR, value);
}

// field definitions
/// LENGTH_CHK field mask
#define HSU_LENGTH_CHK_MASK   ((uint32_t)0x0000FFFF)
/// LENGTH_CHK field LSB position
#define HSU_LENGTH_CHK_LSB    0
/// LENGTH_CHK field width
#define HSU_LENGTH_CHK_WIDTH  ((uint32_t)0x00000010)

/// LENGTH_CHK field reset value
#define HSU_LENGTH_CHK_RST    0x0

/**
 * @brief Returns the current value of the LENGTH_CHK field in the LENGTH_CHK register.
 *
 * The LENGTH_CHK register will be read and the LENGTH_CHK field's value will be returned.
 *
 * @return The current value of the LENGTH_CHK field in the LENGTH_CHK register.
 */
__INLINE uint16_t hsu_length_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_LENGTH_CHK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the LENGTH_CHK field of the LENGTH_CHK register.
 *
 * The LENGTH_CHK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] lengthchk - The value to set the field to.
 */
__INLINE void hsu_length_chk_setf(uint16_t lengthchk)
{
    ASSERT_ERR((((uint32_t)lengthchk << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(HSU_LENGTH_CHK_ADDR, (uint32_t)lengthchk << 0);
}

/// @}

#if RW_HSU_IP_CHK_EN
/**
 * @name MIC_TAB_CHK register definitions
 * <table>
 * <caption>MIC_TAB_CHK bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:00 <td> MIC_CHK <td> R/W <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the MIC_TAB_CHK register
#define HSU_MIC_TAB_CHK_ADDR   0x24930088
/// Offset of the MIC_TAB_CHK register from the base address
#define HSU_MIC_TAB_CHK_OFFSET 0x00000088
/// Index of the MIC_TAB_CHK register
#define HSU_MIC_TAB_CHK_INDEX  0x00000022
/// Reset value of the MIC_TAB_CHK register
#define HSU_MIC_TAB_CHK_RESET  0x00000000

/**
 * @brief Returns the current value of the MIC_TAB_CHK register.
 * The MIC_TAB_CHK register will be read and its value returned.
 * @return The current value of the MIC_TAB_CHK register.
 */
__INLINE uint32_t hsu_mic_tab_chk_get(void)
{
    return REG_PL_RD(HSU_MIC_TAB_CHK_ADDR);
}

/**
 * @brief Sets the MIC_TAB_CHK register to a value.
 * The MIC_TAB_CHK register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_mic_tab_chk_set(uint32_t value)
{
    REG_PL_WR(HSU_MIC_TAB_CHK_ADDR, value);
}

// field definitions
/// MIC_CHK field mask
#define HSU_MIC_CHK_MASK   ((uint32_t)0x0000FFFF)
/// MIC_CHK field LSB position
#define HSU_MIC_CHK_LSB    0
/// MIC_CHK field width
#define HSU_MIC_CHK_WIDTH  ((uint32_t)0x00000010)

/// MIC_CHK field reset value
#define HSU_MIC_CHK_RST    0x0

/**
 * @brief Returns the current value of the MIC_CHK field in the MIC_TAB_CHK register.
 *
 * The MIC_TAB_CHK register will be read and the MIC_CHK field's value will be returned.
 *
 * @return The current value of the MIC_CHK field in the MIC_TAB_CHK register.
 */
__INLINE uint16_t hsu_mic_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_MIC_TAB_CHK_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the MIC_CHK field of the MIC_TAB_CHK register.
 *
 * The MIC_TAB_CHK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] micchk - The value to set the field to.
 */
__INLINE void hsu_mic_chk_setf(uint16_t micchk)
{
    ASSERT_ERR((((uint32_t)micchk << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(HSU_MIC_TAB_CHK_ADDR, (uint32_t)micchk << 0);
}

#endif // RW_HSU_IP_CHK_EN
/// @}

#if RW_HSU_IP_CHK_EN
/**
 * @name REMAINING_CHK register definitions
 * <table>
 * <caption>REMAINING_CHK bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:24 <td> REMAINING_LENGTH_CHK <td> R/W <td> R/W <td> 0x0
 * <tr><td> 23:16 <td>  REMAINING_BYTE2_CHK <td> R/W <td> R/W <td> 0x0
 * <tr><td> 15:08 <td>  REMAINING_BYTE1_CHK <td> R/W <td> R/W <td> 0x0
 * <tr><td> 07:00 <td>  REMAINING_BYTE0_CHK <td> R/W <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the REMAINING_CHK register
#define HSU_REMAINING_CHK_ADDR   0x2493008C
/// Offset of the REMAINING_CHK register from the base address
#define HSU_REMAINING_CHK_OFFSET 0x0000008C
/// Index of the REMAINING_CHK register
#define HSU_REMAINING_CHK_INDEX  0x00000023
/// Reset value of the REMAINING_CHK register
#define HSU_REMAINING_CHK_RESET  0x00000000

/**
 * @brief Returns the current value of the REMAINING_CHK register.
 * The REMAINING_CHK register will be read and its value returned.
 * @return The current value of the REMAINING_CHK register.
 */
__INLINE uint32_t hsu_remaining_chk_get(void)
{
    return REG_PL_RD(HSU_REMAINING_CHK_ADDR);
}

/**
 * @brief Sets the REMAINING_CHK register to a value.
 * The REMAINING_CHK register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_remaining_chk_set(uint32_t value)
{
    REG_PL_WR(HSU_REMAINING_CHK_ADDR, value);
}

// field definitions
/// REMAINING_LENGTH_CHK field mask
#define HSU_REMAINING_LENGTH_CHK_MASK   ((uint32_t)0x03000000)
/// REMAINING_LENGTH_CHK field LSB position
#define HSU_REMAINING_LENGTH_CHK_LSB    24
/// REMAINING_LENGTH_CHK field width
#define HSU_REMAINING_LENGTH_CHK_WIDTH  ((uint32_t)0x00000002)
/// REMAINING_BYTE2_CHK field mask
#define HSU_REMAINING_BYTE2_CHK_MASK    ((uint32_t)0x00FF0000)
/// REMAINING_BYTE2_CHK field LSB position
#define HSU_REMAINING_BYTE2_CHK_LSB     16
/// REMAINING_BYTE2_CHK field width
#define HSU_REMAINING_BYTE2_CHK_WIDTH   ((uint32_t)0x00000008)
/// REMAINING_BYTE1_CHK field mask
#define HSU_REMAINING_BYTE1_CHK_MASK    ((uint32_t)0x0000FF00)
/// REMAINING_BYTE1_CHK field LSB position
#define HSU_REMAINING_BYTE1_CHK_LSB     8
/// REMAINING_BYTE1_CHK field width
#define HSU_REMAINING_BYTE1_CHK_WIDTH   ((uint32_t)0x00000008)
/// REMAINING_BYTE0_CHK field mask
#define HSU_REMAINING_BYTE0_CHK_MASK    ((uint32_t)0x000000FF)
/// REMAINING_BYTE0_CHK field LSB position
#define HSU_REMAINING_BYTE0_CHK_LSB     0
/// REMAINING_BYTE0_CHK field width
#define HSU_REMAINING_BYTE0_CHK_WIDTH   ((uint32_t)0x00000008)

/// REMAINING_LENGTH_CHK field reset value
#define HSU_REMAINING_LENGTH_CHK_RST    0x0
/// REMAINING_BYTE2_CHK field reset value
#define HSU_REMAINING_BYTE2_CHK_RST     0x0
/// REMAINING_BYTE1_CHK field reset value
#define HSU_REMAINING_BYTE1_CHK_RST     0x0
/// REMAINING_BYTE0_CHK field reset value
#define HSU_REMAINING_BYTE0_CHK_RST     0x0

/**
 * @brief Constructs a value for the REMAINING_CHK register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] remaininglengthchk - The value to use for the REMAINING_LENGTH_CHK field.
 * @param[in] remainingbyte2chk - The value to use for the REMAINING_BYTE2_CHK field.
 * @param[in] remainingbyte1chk - The value to use for the REMAINING_BYTE1_CHK field.
 * @param[in] remainingbyte0chk - The value to use for the REMAINING_BYTE0_CHK field.
 */
__INLINE void hsu_remaining_chk_pack(uint8_t remaininglengthchk, uint8_t remainingbyte2chk, uint8_t remainingbyte1chk, uint8_t remainingbyte0chk)
{
    ASSERT_ERR((((uint32_t)remaininglengthchk << 24) & ~((uint32_t)0x03000000)) == 0);
    ASSERT_ERR((((uint32_t)remainingbyte2chk << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)remainingbyte1chk << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)remainingbyte0chk << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(HSU_REMAINING_CHK_ADDR,  ((uint32_t)remaininglengthchk << 24) | ((uint32_t)remainingbyte2chk << 16) | ((uint32_t)remainingbyte1chk << 8) | ((uint32_t)remainingbyte0chk << 0));
}

/**
 * @brief Unpacks REMAINING_CHK's fields from current value of the REMAINING_CHK register.
 *
 * Reads the REMAINING_CHK register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] remaininglengthchk - Will be populated with the current value of this field from the register.
 * @param[out] remainingbyte2chk - Will be populated with the current value of this field from the register.
 * @param[out] remainingbyte1chk - Will be populated with the current value of this field from the register.
 * @param[out] remainingbyte0chk - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_remaining_chk_unpack(uint8_t* remaininglengthchk, uint8_t* remainingbyte2chk, uint8_t* remainingbyte1chk, uint8_t* remainingbyte0chk)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_CHK_ADDR);

    *remaininglengthchk = (localVal & ((uint32_t)0x03000000)) >> 24;
    *remainingbyte2chk = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *remainingbyte1chk = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *remainingbyte0chk = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the REMAINING_LENGTH_CHK field in the REMAINING_CHK register.
 *
 * The REMAINING_CHK register will be read and the REMAINING_LENGTH_CHK field's value will be returned.
 *
 * @return The current value of the REMAINING_LENGTH_CHK field in the REMAINING_CHK register.
 */
__INLINE uint8_t hsu_remaining_length_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_CHK_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

/**
 * @brief Sets the REMAINING_LENGTH_CHK field of the REMAINING_CHK register.
 *
 * The REMAINING_CHK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remaininglengthchk - The value to set the field to.
 */
__INLINE void hsu_remaining_length_chk_setf(uint8_t remaininglengthchk)
{
    ASSERT_ERR((((uint32_t)remaininglengthchk << 24) & ~((uint32_t)0x03000000)) == 0);
    REG_PL_WR(HSU_REMAINING_CHK_ADDR, (REG_PL_RD(HSU_REMAINING_CHK_ADDR) & ~((uint32_t)0x03000000)) | ((uint32_t)remaininglengthchk << 24));
}

/**
 * @brief Returns the current value of the REMAINING_BYTE2_CHK field in the REMAINING_CHK register.
 *
 * The REMAINING_CHK register will be read and the REMAINING_BYTE2_CHK field's value will be returned.
 *
 * @return The current value of the REMAINING_BYTE2_CHK field in the REMAINING_CHK register.
 */
__INLINE uint8_t hsu_remaining_byte2_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_CHK_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the REMAINING_BYTE2_CHK field of the REMAINING_CHK register.
 *
 * The REMAINING_CHK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remainingbyte2chk - The value to set the field to.
 */
__INLINE void hsu_remaining_byte2_chk_setf(uint8_t remainingbyte2chk)
{
    ASSERT_ERR((((uint32_t)remainingbyte2chk << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(HSU_REMAINING_CHK_ADDR, (REG_PL_RD(HSU_REMAINING_CHK_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)remainingbyte2chk << 16));
}

/**
 * @brief Returns the current value of the REMAINING_BYTE1_CHK field in the REMAINING_CHK register.
 *
 * The REMAINING_CHK register will be read and the REMAINING_BYTE1_CHK field's value will be returned.
 *
 * @return The current value of the REMAINING_BYTE1_CHK field in the REMAINING_CHK register.
 */
__INLINE uint8_t hsu_remaining_byte1_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_CHK_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the REMAINING_BYTE1_CHK field of the REMAINING_CHK register.
 *
 * The REMAINING_CHK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remainingbyte1chk - The value to set the field to.
 */
__INLINE void hsu_remaining_byte1_chk_setf(uint8_t remainingbyte1chk)
{
    ASSERT_ERR((((uint32_t)remainingbyte1chk << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(HSU_REMAINING_CHK_ADDR, (REG_PL_RD(HSU_REMAINING_CHK_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)remainingbyte1chk << 8));
}

/**
 * @brief Returns the current value of the REMAINING_BYTE0_CHK field in the REMAINING_CHK register.
 *
 * The REMAINING_CHK register will be read and the REMAINING_BYTE0_CHK field's value will be returned.
 *
 * @return The current value of the REMAINING_BYTE0_CHK field in the REMAINING_CHK register.
 */
__INLINE uint8_t hsu_remaining_byte0_chk_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REMAINING_CHK_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the REMAINING_BYTE0_CHK field of the REMAINING_CHK register.
 *
 * The REMAINING_CHK register will be read, modified to contain the new field value, and written.
 *
 * @param[in] remainingbyte0chk - The value to set the field to.
 */
__INLINE void hsu_remaining_byte0_chk_setf(uint8_t remainingbyte0chk)
{
    ASSERT_ERR((((uint32_t)remainingbyte0chk << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(HSU_REMAINING_CHK_ADDR, (REG_PL_RD(HSU_REMAINING_CHK_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)remainingbyte0chk << 0));
}

#endif // RW_HSU_IP_CHK_EN
/// @}

#if RW_HSU_IP_CHK_EN
/**
 * @name ALGORITM_PRIORITY register definitions
 * <table>
 * <caption>ALGORITM_PRIORITY bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 00 <td> PRIO_ALGO <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the ALGORITM_PRIORITY register
#define HSU_ALGORITM_PRIORITY_ADDR   0x24930090
/// Offset of the ALGORITM_PRIORITY register from the base address
#define HSU_ALGORITM_PRIORITY_OFFSET 0x00000090
/// Index of the ALGORITM_PRIORITY register
#define HSU_ALGORITM_PRIORITY_INDEX  0x00000024
/// Reset value of the ALGORITM_PRIORITY register
#define HSU_ALGORITM_PRIORITY_RESET  0x00000000

/**
 * @brief Returns the current value of the ALGORITM_PRIORITY register.
 * The ALGORITM_PRIORITY register will be read and its value returned.
 * @return The current value of the ALGORITM_PRIORITY register.
 */
__INLINE uint32_t hsu_algoritm_priority_get(void)
{
    return REG_PL_RD(HSU_ALGORITM_PRIORITY_ADDR);
}

/**
 * @brief Sets the ALGORITM_PRIORITY register to a value.
 * The ALGORITM_PRIORITY register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_algoritm_priority_set(uint32_t value)
{
    REG_PL_WR(HSU_ALGORITM_PRIORITY_ADDR, value);
}

// field definitions
/// PRIO_ALGO field bit
#define HSU_PRIO_ALGO_BIT    ((uint32_t)0x00000001)
/// PRIO_ALGO field position
#define HSU_PRIO_ALGO_POS    0

/// PRIO_ALGO field reset value
#define HSU_PRIO_ALGO_RST    0x0

/**
 * @brief Returns the current value of the PRIO_ALGO field in the ALGORITM_PRIORITY register.
 *
 * The ALGORITM_PRIORITY register will be read and the PRIO_ALGO field's value will be returned.
 *
 * @return The current value of the PRIO_ALGO field in the ALGORITM_PRIORITY register.
 */
__INLINE uint8_t hsu_prio_algo_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_ALGORITM_PRIORITY_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the PRIO_ALGO field of the ALGORITM_PRIORITY register.
 *
 * The ALGORITM_PRIORITY register will be read, modified to contain the new field value, and written.
 *
 * @param[in] prioalgo - The value to set the field to.
 */
__INLINE void hsu_prio_algo_setf(uint8_t prioalgo)
{
    ASSERT_ERR((((uint32_t)prioalgo << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_ALGORITM_PRIORITY_ADDR, (uint32_t)prioalgo << 0);
}

#endif // RW_HSU_IP_CHK_EN
/// @}

#if RW_HSU_IP_CHK_EN
/**
 * @name IRQ_CTRL_EN register definitions
 * <table>
 * <caption>IRQ_CTRL_EN bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 04 <td>    CHK_IRQ_EN <td> R <td> R/W <td> 0
 * <tr><td> 00 <td> CRYPTO_IRQ_EN <td> R <td> R/W <td> 1
 * </table>
 *
 * @{
 */

/// Address of the IRQ_CTRL_EN register
#define HSU_IRQ_CTRL_EN_ADDR   0x24930094
/// Offset of the IRQ_CTRL_EN register from the base address
#define HSU_IRQ_CTRL_EN_OFFSET 0x00000094
/// Index of the IRQ_CTRL_EN register
#define HSU_IRQ_CTRL_EN_INDEX  0x00000025
/// Reset value of the IRQ_CTRL_EN register
#define HSU_IRQ_CTRL_EN_RESET  0x00000001

/**
 * @brief Returns the current value of the IRQ_CTRL_EN register.
 * The IRQ_CTRL_EN register will be read and its value returned.
 * @return The current value of the IRQ_CTRL_EN register.
 */
__INLINE uint32_t hsu_irq_ctrl_en_get(void)
{
    return REG_PL_RD(HSU_IRQ_CTRL_EN_ADDR);
}

/**
 * @brief Sets the IRQ_CTRL_EN register to a value.
 * The IRQ_CTRL_EN register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_irq_ctrl_en_set(uint32_t value)
{
    REG_PL_WR(HSU_IRQ_CTRL_EN_ADDR, value);
}

// field definitions
/// CHK_IRQ_EN field bit
#define HSU_CHK_IRQ_EN_BIT       ((uint32_t)0x00000010)
/// CHK_IRQ_EN field position
#define HSU_CHK_IRQ_EN_POS       4
/// CRYPTO_IRQ_EN field bit
#define HSU_CRYPTO_IRQ_EN_BIT    ((uint32_t)0x00000001)
/// CRYPTO_IRQ_EN field position
#define HSU_CRYPTO_IRQ_EN_POS    0

/// CHK_IRQ_EN field reset value
#define HSU_CHK_IRQ_EN_RST       0x0
/// CRYPTO_IRQ_EN field reset value
#define HSU_CRYPTO_IRQ_EN_RST    0x1

/**
 * @brief Constructs a value for the IRQ_CTRL_EN register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] chkirqen - The value to use for the CHK_IRQ_EN field.
 * @param[in] cryptoirqen - The value to use for the CRYPTO_IRQ_EN field.
 */
__INLINE void hsu_irq_ctrl_en_pack(uint8_t chkirqen, uint8_t cryptoirqen)
{
    ASSERT_ERR((((uint32_t)chkirqen << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)cryptoirqen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_IRQ_CTRL_EN_ADDR,  ((uint32_t)chkirqen << 4) | ((uint32_t)cryptoirqen << 0));
}

/**
 * @brief Unpacks IRQ_CTRL_EN's fields from current value of the IRQ_CTRL_EN register.
 *
 * Reads the IRQ_CTRL_EN register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] chkirqen - Will be populated with the current value of this field from the register.
 * @param[out] cryptoirqen - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_irq_ctrl_en_unpack(uint8_t* chkirqen, uint8_t* cryptoirqen)
{
    uint32_t localVal = REG_PL_RD(HSU_IRQ_CTRL_EN_ADDR);

    *chkirqen = (localVal & ((uint32_t)0x00000010)) >> 4;
    *cryptoirqen = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the CHK_IRQ_EN field in the IRQ_CTRL_EN register.
 *
 * The IRQ_CTRL_EN register will be read and the CHK_IRQ_EN field's value will be returned.
 *
 * @return The current value of the CHK_IRQ_EN field in the IRQ_CTRL_EN register.
 */
__INLINE uint8_t hsu_chk_irq_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_IRQ_CTRL_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the CHK_IRQ_EN field of the IRQ_CTRL_EN register.
 *
 * The IRQ_CTRL_EN register will be read, modified to contain the new field value, and written.
 *
 * @param[in] chkirqen - The value to set the field to.
 */
__INLINE void hsu_chk_irq_en_setf(uint8_t chkirqen)
{
    ASSERT_ERR((((uint32_t)chkirqen << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(HSU_IRQ_CTRL_EN_ADDR, (REG_PL_RD(HSU_IRQ_CTRL_EN_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)chkirqen << 4));
}

/**
 * @brief Returns the current value of the CRYPTO_IRQ_EN field in the IRQ_CTRL_EN register.
 *
 * The IRQ_CTRL_EN register will be read and the CRYPTO_IRQ_EN field's value will be returned.
 *
 * @return The current value of the CRYPTO_IRQ_EN field in the IRQ_CTRL_EN register.
 */
__INLINE uint8_t hsu_crypto_irq_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_IRQ_CTRL_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the CRYPTO_IRQ_EN field of the IRQ_CTRL_EN register.
 *
 * The IRQ_CTRL_EN register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cryptoirqen - The value to set the field to.
 */
__INLINE void hsu_crypto_irq_en_setf(uint8_t cryptoirqen)
{
    ASSERT_ERR((((uint32_t)cryptoirqen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_IRQ_CTRL_EN_ADDR, (REG_PL_RD(HSU_IRQ_CTRL_EN_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)cryptoirqen << 0));
}

#endif // RW_HSU_IP_CHK_EN
/// @}

#if RW_HSU_RSA_EN
/**
 * @name RSA_CONFIG_ERROR register definitions
 * <table>
 * <caption>RSA_CONFIG_ERROR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31 <td> RSA_CFG_ERROR_CLEAR <td> R/W <td> R/W <td> 0
 * <tr><td> 00 <td>       RSA_CFG_ERROR <td> W <td> R <td> 0
 * </table>
 *
 * @{
 */

/// Address of the RSA_CONFIG_ERROR register
#define HSU_RSA_CONFIG_ERROR_ADDR   0x24930098
/// Offset of the RSA_CONFIG_ERROR register from the base address
#define HSU_RSA_CONFIG_ERROR_OFFSET 0x00000098
/// Index of the RSA_CONFIG_ERROR register
#define HSU_RSA_CONFIG_ERROR_INDEX  0x00000026
/// Reset value of the RSA_CONFIG_ERROR register
#define HSU_RSA_CONFIG_ERROR_RESET  0x00000000

/**
 * @brief Returns the current value of the RSA_CONFIG_ERROR register.
 * The RSA_CONFIG_ERROR register will be read and its value returned.
 * @return The current value of the RSA_CONFIG_ERROR register.
 */
__INLINE uint32_t hsu_rsa_config_error_get(void)
{
    return REG_PL_RD(HSU_RSA_CONFIG_ERROR_ADDR);
}

/**
 * @brief Sets the RSA_CONFIG_ERROR register to a value.
 * The RSA_CONFIG_ERROR register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_rsa_config_error_set(uint32_t value)
{
    REG_PL_WR(HSU_RSA_CONFIG_ERROR_ADDR, value);
}

// field definitions
/// RSA_CFG_ERROR_CLEAR field bit
#define HSU_RSA_CFG_ERROR_CLEAR_BIT    ((uint32_t)0x80000000)
/// RSA_CFG_ERROR_CLEAR field position
#define HSU_RSA_CFG_ERROR_CLEAR_POS    31
/// RSA_CFG_ERROR field bit
#define HSU_RSA_CFG_ERROR_BIT          ((uint32_t)0x00000001)
/// RSA_CFG_ERROR field position
#define HSU_RSA_CFG_ERROR_POS          0

/// RSA_CFG_ERROR_CLEAR field reset value
#define HSU_RSA_CFG_ERROR_CLEAR_RST    0x0
/// RSA_CFG_ERROR field reset value
#define HSU_RSA_CFG_ERROR_RST          0x0

/**
 * @brief Unpacks RSA_CONFIG_ERROR's fields from current value of the RSA_CONFIG_ERROR register.
 *
 * Reads the RSA_CONFIG_ERROR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rsacfgerrorclear - Will be populated with the current value of this field from the register.
 * @param[out] rsacfgerror - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_rsa_config_error_unpack(uint8_t* rsacfgerrorclear, uint8_t* rsacfgerror)
{
    uint32_t localVal = REG_PL_RD(HSU_RSA_CONFIG_ERROR_ADDR);

    *rsacfgerrorclear = (localVal & ((uint32_t)0x80000000)) >> 31;
    *rsacfgerror = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the RSA_CFG_ERROR_CLEAR field in the RSA_CONFIG_ERROR register.
 *
 * The RSA_CONFIG_ERROR register will be read and the RSA_CFG_ERROR_CLEAR field's value will be returned.
 *
 * @return The current value of the RSA_CFG_ERROR_CLEAR field in the RSA_CONFIG_ERROR register.
 */
__INLINE uint8_t hsu_rsa_cfg_error_clear_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_RSA_CONFIG_ERROR_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the RSA_CFG_ERROR_CLEAR field of the RSA_CONFIG_ERROR register.
 *
 * The RSA_CONFIG_ERROR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rsacfgerrorclear - The value to set the field to.
 */
__INLINE void hsu_rsa_cfg_error_clear_setf(uint8_t rsacfgerrorclear)
{
    ASSERT_ERR((((uint32_t)rsacfgerrorclear << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(HSU_RSA_CONFIG_ERROR_ADDR, (uint32_t)rsacfgerrorclear << 31);
}

/**
 * @brief Returns the current value of the RSA_CFG_ERROR field in the RSA_CONFIG_ERROR register.
 *
 * The RSA_CONFIG_ERROR register will be read and the RSA_CFG_ERROR field's value will be returned.
 *
 * @return The current value of the RSA_CFG_ERROR field in the RSA_CONFIG_ERROR register.
 */
__INLINE uint8_t hsu_rsa_cfg_error_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_RSA_CONFIG_ERROR_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

#endif // RW_HSU_RSA_EN
/// @}

/**
 * @name REVISION_1 register definitions
 * <table>
 * <caption>REVISION_1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 02 <td> EPM_521 <td> W <td> R <td> 1
 * <tr><td> 01 <td> EPM_384 <td> W <td> R <td> 1
 * <tr><td> 00 <td> EPM_256 <td> W <td> R <td> 1
 * </table>
 *
 * @{
 */

/// Address of the REVISION_1 register
#define HSU_REVISION_1_ADDR   0x2493009C
/// Offset of the REVISION_1 register from the base address
#define HSU_REVISION_1_OFFSET 0x0000009C
/// Index of the REVISION_1 register
#define HSU_REVISION_1_INDEX  0x00000027
/// Reset value of the REVISION_1 register
#define HSU_REVISION_1_RESET  0x00000007

/**
 * @brief Returns the current value of the REVISION_1 register.
 * The REVISION_1 register will be read and its value returned.
 * @return The current value of the REVISION_1 register.
 */
__INLINE uint32_t hsu_revision_1_get(void)
{
    return REG_PL_RD(HSU_REVISION_1_ADDR);
}

// field definitions
/// EPM_521 field bit
#define HSU_EPM_521_BIT    ((uint32_t)0x00000004)
/// EPM_521 field position
#define HSU_EPM_521_POS    2
/// EPM_384 field bit
#define HSU_EPM_384_BIT    ((uint32_t)0x00000002)
/// EPM_384 field position
#define HSU_EPM_384_POS    1
/// EPM_256 field bit
#define HSU_EPM_256_BIT    ((uint32_t)0x00000001)
/// EPM_256 field position
#define HSU_EPM_256_POS    0

/// EPM_521 field reset value
#define HSU_EPM_521_RST    0x1
/// EPM_384 field reset value
#define HSU_EPM_384_RST    0x1
/// EPM_256 field reset value
#define HSU_EPM_256_RST    0x1

/**
 * @brief Unpacks REVISION_1's fields from current value of the REVISION_1 register.
 *
 * Reads the REVISION_1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] epm521 - Will be populated with the current value of this field from the register.
 * @param[out] epm384 - Will be populated with the current value of this field from the register.
 * @param[out] epm256 - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_revision_1_unpack(uint8_t* epm521, uint8_t* epm384, uint8_t* epm256)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_1_ADDR);

    *epm521 = (localVal & ((uint32_t)0x00000004)) >> 2;
    *epm384 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *epm256 = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the EPM_521 field in the REVISION_1 register.
 *
 * The REVISION_1 register will be read and the EPM_521 field's value will be returned.
 *
 * @return The current value of the EPM_521 field in the REVISION_1 register.
 */
__INLINE uint8_t hsu_epm_521_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Returns the current value of the EPM_384 field in the REVISION_1 register.
 *
 * The REVISION_1 register will be read and the EPM_384 field's value will be returned.
 *
 * @return The current value of the EPM_384 field in the REVISION_1 register.
 */
__INLINE uint8_t hsu_epm_384_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the EPM_256 field in the REVISION_1 register.
 *
 * The REVISION_1 register will be read and the EPM_256 field's value will be returned.
 *
 * @return The current value of the EPM_256 field in the REVISION_1 register.
 */
__INLINE uint8_t hsu_epm_256_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_REVISION_1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

#if RW_HSU_EPM_EN
/**
 * @name EPM_CFG register definitions
 * <table>
 * <caption>EPM_CFG bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 09:08 <td>   EPM_MODE <td> R <td> R/W <td> 0x0
 * <tr><td> 04    <td>  EPM_READY <td> W <td> R <td> 0
 * <tr><td> 00    <td> EPM_ENABLE <td> R/W <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the EPM_CFG register
#define HSU_EPM_CFG_ADDR   0x249300A0
/// Offset of the EPM_CFG register from the base address
#define HSU_EPM_CFG_OFFSET 0x000000A0
/// Index of the EPM_CFG register
#define HSU_EPM_CFG_INDEX  0x00000028
/// Reset value of the EPM_CFG register
#define HSU_EPM_CFG_RESET  0x00000000

/**
 * @brief Returns the current value of the EPM_CFG register.
 * The EPM_CFG register will be read and its value returned.
 * @return The current value of the EPM_CFG register.
 */
__INLINE uint32_t hsu_epm_cfg_get(void)
{
    return REG_PL_RD(HSU_EPM_CFG_ADDR);
}

/**
 * @brief Sets the EPM_CFG register to a value.
 * The EPM_CFG register will be written.
 * @param value - The value to write.
 */
__INLINE void hsu_epm_cfg_set(uint32_t value)
{
    REG_PL_WR(HSU_EPM_CFG_ADDR, value);
}

// field definitions
/// EPM_MODE field mask
#define HSU_EPM_MODE_MASK     ((uint32_t)0x00000300)
/// EPM_MODE field LSB position
#define HSU_EPM_MODE_LSB      8
/// EPM_MODE field width
#define HSU_EPM_MODE_WIDTH    ((uint32_t)0x00000002)
/// EPM_READY field bit
#define HSU_EPM_READY_BIT     ((uint32_t)0x00000010)
/// EPM_READY field position
#define HSU_EPM_READY_POS     4
/// EPM_ENABLE field bit
#define HSU_EPM_ENABLE_BIT    ((uint32_t)0x00000001)
/// EPM_ENABLE field position
#define HSU_EPM_ENABLE_POS    0

/// EPM_MODE field reset value
#define HSU_EPM_MODE_RST      0x0
/// EPM_READY field reset value
#define HSU_EPM_READY_RST     0x0
/// EPM_ENABLE field reset value
#define HSU_EPM_ENABLE_RST    0x0

/**
 * @brief Constructs a value for the EPM_CFG register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] epmmode - The value to use for the EPM_MODE field.
 * @param[in] epmenable - The value to use for the EPM_ENABLE field.
 */
__INLINE void hsu_epm_cfg_pack(uint8_t epmmode, uint8_t epmenable)
{
    ASSERT_ERR((((uint32_t)epmmode << 8) & ~((uint32_t)0x00000300)) == 0);
    ASSERT_ERR((((uint32_t)epmenable << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_EPM_CFG_ADDR,  ((uint32_t)epmmode << 8) | ((uint32_t)epmenable << 0));
}

/**
 * @brief Unpacks EPM_CFG's fields from current value of the EPM_CFG register.
 *
 * Reads the EPM_CFG register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] epmmode - Will be populated with the current value of this field from the register.
 * @param[out] epmready - Will be populated with the current value of this field from the register.
 * @param[out] epmenable - Will be populated with the current value of this field from the register.
 */
__INLINE void hsu_epm_cfg_unpack(uint8_t* epmmode, uint8_t* epmready, uint8_t* epmenable)
{
    uint32_t localVal = REG_PL_RD(HSU_EPM_CFG_ADDR);

    *epmmode = (localVal & ((uint32_t)0x00000300)) >> 8;
    *epmready = (localVal & ((uint32_t)0x00000010)) >> 4;
    *epmenable = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the EPM_MODE field in the EPM_CFG register.
 *
 * The EPM_CFG register will be read and the EPM_MODE field's value will be returned.
 *
 * @return The current value of the EPM_MODE field in the EPM_CFG register.
 */
__INLINE uint8_t hsu_epm_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_EPM_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

/**
 * @brief Sets the EPM_MODE field of the EPM_CFG register.
 *
 * The EPM_CFG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] epmmode - The value to set the field to.
 */
__INLINE void hsu_epm_mode_setf(uint8_t epmmode)
{
    ASSERT_ERR((((uint32_t)epmmode << 8) & ~((uint32_t)0x00000300)) == 0);
    REG_PL_WR(HSU_EPM_CFG_ADDR, (REG_PL_RD(HSU_EPM_CFG_ADDR) & ~((uint32_t)0x00000300)) | ((uint32_t)epmmode << 8));
}

/**
 * @brief Returns the current value of the EPM_READY field in the EPM_CFG register.
 *
 * The EPM_CFG register will be read and the EPM_READY field's value will be returned.
 *
 * @return The current value of the EPM_READY field in the EPM_CFG register.
 */
__INLINE uint8_t hsu_epm_ready_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_EPM_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Returns the current value of the EPM_ENABLE field in the EPM_CFG register.
 *
 * The EPM_CFG register will be read and the EPM_ENABLE field's value will be returned.
 *
 * @return The current value of the EPM_ENABLE field in the EPM_CFG register.
 */
__INLINE uint8_t hsu_epm_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(HSU_EPM_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the EPM_ENABLE field of the EPM_CFG register.
 *
 * The EPM_CFG register will be read, modified to contain the new field value, and written.
 *
 * @param[in] epmenable - The value to set the field to.
 */
__INLINE void hsu_epm_enable_setf(uint8_t epmenable)
{
    ASSERT_ERR((((uint32_t)epmenable << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(HSU_EPM_CFG_ADDR, (REG_PL_RD(HSU_EPM_CFG_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)epmenable << 0));
}

#endif // RW_HSU_EPM_EN
/// @}

#if RW_HSU_EPM_EN
/**
 * @name EPM_K register definitions
 * <table>
 * <caption>EPM_K bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> K_EPM <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the EPM_K register
#define HSU_EPM_K_ADDR   0x249300A4
/// Offset of the EPM_K register from the base address
#define HSU_EPM_K_OFFSET 0x000000A4
/// Index of the EPM_K register
#define HSU_EPM_K_INDEX  0x00000029
/// Reset value of the EPM_K register
#define HSU_EPM_K_RESET  0x00000000
/// Number of elements of the EPM_K register array
#define HSU_EPM_K_COUNT  17

/**
 * @brief Returns the current value of the EPM_K register.
 * The EPM_K register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the EPM_K register.
 */
__INLINE uint32_t hsu_epm_k_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 16);
    return REG_PL_RD(HSU_EPM_K_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the EPM_K register to a value.
 * The EPM_K register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void hsu_epm_k_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 16);
    REG_PL_WR(HSU_EPM_K_ADDR + reg_idx * 4, value);
}

// field definitions
/// K_EPM field mask
#define HSU_K_EPM_MASK   ((uint32_t)0xFFFFFFFF)
/// K_EPM field LSB position
#define HSU_K_EPM_LSB    0
/// K_EPM field width
#define HSU_K_EPM_WIDTH  ((uint32_t)0x00000020)

/// K_EPM field reset value
#define HSU_K_EPM_RST    0x0

/**
 * @brief Returns the current value of the K_EPM field in the EPM_K register.
 *
 * The EPM_K register will be read and the K_EPM field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the K_EPM field in the EPM_K register.
 */
__INLINE uint32_t hsu_k_epm_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 16);
    uint32_t localVal = REG_PL_RD(HSU_EPM_K_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the K_EPM field of the EPM_K register.
 *
 * The EPM_K register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] kepm - The value to set the field to.
 */
__INLINE void hsu_k_epm_setf(int reg_idx, uint32_t kepm)
{
    ASSERT_ERR(reg_idx <= 16);
    ASSERT_ERR((((uint32_t)kepm << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(HSU_EPM_K_ADDR + reg_idx * 4, (uint32_t)kepm << 0);
}

#endif // RW_HSU_EPM_EN
/// @}

#if RW_HSU_EPM_EN
/**
 * @name EPM_X register definitions
 * <table>
 * <caption>EPM_X bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> X_EPM <td> R/W <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the EPM_X register
#define HSU_EPM_X_ADDR   0x249300E8
/// Offset of the EPM_X register from the base address
#define HSU_EPM_X_OFFSET 0x000000E8
/// Index of the EPM_X register
#define HSU_EPM_X_INDEX  0x0000003A
/// Reset value of the EPM_X register
#define HSU_EPM_X_RESET  0x00000000
/// Number of elements of the EPM_X register array
#define HSU_EPM_X_COUNT  17

/**
 * @brief Returns the current value of the EPM_X register.
 * The EPM_X register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the EPM_X register.
 */
__INLINE uint32_t hsu_epm_x_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 16);
    return REG_PL_RD(HSU_EPM_X_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the EPM_X register to a value.
 * The EPM_X register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void hsu_epm_x_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 16);
    REG_PL_WR(HSU_EPM_X_ADDR + reg_idx * 4, value);
}

// field definitions
/// X_EPM field mask
#define HSU_X_EPM_MASK   ((uint32_t)0xFFFFFFFF)
/// X_EPM field LSB position
#define HSU_X_EPM_LSB    0
/// X_EPM field width
#define HSU_X_EPM_WIDTH  ((uint32_t)0x00000020)

/// X_EPM field reset value
#define HSU_X_EPM_RST    0x0

/**
 * @brief Returns the current value of the X_EPM field in the EPM_X register.
 *
 * The EPM_X register will be read and the X_EPM field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the X_EPM field in the EPM_X register.
 */
__INLINE uint32_t hsu_x_epm_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 16);
    uint32_t localVal = REG_PL_RD(HSU_EPM_X_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the X_EPM field of the EPM_X register.
 *
 * The EPM_X register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] xepm - The value to set the field to.
 */
__INLINE void hsu_x_epm_setf(int reg_idx, uint32_t xepm)
{
    ASSERT_ERR(reg_idx <= 16);
    ASSERT_ERR((((uint32_t)xepm << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(HSU_EPM_X_ADDR + reg_idx * 4, (uint32_t)xepm << 0);
}

#endif // RW_HSU_EPM_EN
/// @}

#if RW_HSU_EPM_EN
/**
 * @name EPM_Y register definitions
 * <table>
 * <caption>EPM_Y bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> Y_EPM <td> R/W <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the EPM_Y register
#define HSU_EPM_Y_ADDR   0x2493012C
/// Offset of the EPM_Y register from the base address
#define HSU_EPM_Y_OFFSET 0x0000012C
/// Index of the EPM_Y register
#define HSU_EPM_Y_INDEX  0x0000004B
/// Reset value of the EPM_Y register
#define HSU_EPM_Y_RESET  0x00000000
/// Number of elements of the EPM_Y register array
#define HSU_EPM_Y_COUNT  17

/**
 * @brief Returns the current value of the EPM_Y register.
 * The EPM_Y register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the EPM_Y register.
 */
__INLINE uint32_t hsu_epm_y_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 16);
    return REG_PL_RD(HSU_EPM_Y_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the EPM_Y register to a value.
 * The EPM_Y register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void hsu_epm_y_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 16);
    REG_PL_WR(HSU_EPM_Y_ADDR + reg_idx * 4, value);
}

// field definitions
/// Y_EPM field mask
#define HSU_Y_EPM_MASK   ((uint32_t)0xFFFFFFFF)
/// Y_EPM field LSB position
#define HSU_Y_EPM_LSB    0
/// Y_EPM field width
#define HSU_Y_EPM_WIDTH  ((uint32_t)0x00000020)

/// Y_EPM field reset value
#define HSU_Y_EPM_RST    0x0

/**
 * @brief Returns the current value of the Y_EPM field in the EPM_Y register.
 *
 * The EPM_Y register will be read and the Y_EPM field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the Y_EPM field in the EPM_Y register.
 */
__INLINE uint32_t hsu_y_epm_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 16);
    uint32_t localVal = REG_PL_RD(HSU_EPM_Y_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the Y_EPM field of the EPM_Y register.
 *
 * The EPM_Y register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] yepm - The value to set the field to.
 */
__INLINE void hsu_y_epm_setf(int reg_idx, uint32_t yepm)
{
    ASSERT_ERR(reg_idx <= 16);
    ASSERT_ERR((((uint32_t)yepm << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(HSU_EPM_Y_ADDR + reg_idx * 4, (uint32_t)yepm << 0);
}

#endif // RW_HSU_EPM_EN
/// @}


#endif // _REG_HSU_H_

/// @}

