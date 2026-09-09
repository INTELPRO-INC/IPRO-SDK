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
 * @file reg_mdm_cfg.h
 * @brief Definitions of the MDM HW block registers and register access functions.
 *
 * @defgroup REG_MDM_CFG REG_MDM_CFG
 * @ingroup REG
 * @{
 *
 * @brief Definitions of the MDM HW block registers and register access functions.
 */
#ifndef _REG_MDM_CFG_H_
#define _REG_MDM_CFG_H_

#include "co_int.h"
#include "_reg_mdm_cfg.h"
#include "compiler.h"
#include "arch.h"
#include "dbg_assert.h"
#include "reg_access.h"


#ifndef BOUF_TXSCAL2
#define BOUF_TXSCAL2 0
#endif

#ifndef BOUF_TXSCAL1
#define BOUF_TXSCAL1 0
#endif

#ifndef BOUF_TXSCAL0
#define BOUF_TXSCAL0 0
#endif

#ifndef RW_NX_CSI
#define RW_NX_CSI 0
#endif

#ifndef RW_BFMER_EN
#define RW_BFMER_EN 0
#endif

/** @brief Number of registers in the REG_MDM_CFG peripheral.
 */
#define REG_MDM_CFG_COUNT 583

/** @brief Decoding mask of the REG_MDM_CFG peripheral registers from the CPU point of view.
 */
#define REG_MDM_CFG_DECODING_MASK 0x00000FFF

/**
 * @name HDMCONFIG register definitions
 * <table>
 * <caption>HDMCONFIG bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td> MUMIMOTX <td> W <td> R <td> 0
 * <tr><td> 30    <td> MUMIMORX <td> W <td> R <td> 0
 * <tr><td> 29    <td>    BFMER <td> W <td> R <td> 0
 * <tr><td> 28    <td>    BFMEE <td> W <td> R <td> 0
 * <tr><td> 27    <td>  LDPCDEC <td> W <td> R <td> 0
 * <tr><td> 26    <td>  LDPCENC <td> W <td> R <td> 0
 * <tr><td> 25:24 <td>     CHBW <td> W <td> R <td> 0x0
 * <tr><td> 23    <td>  DSSSCCK <td> W <td> R <td> 0
 * <tr><td> 22    <td>      VHT <td> W <td> R <td> 0
 * <tr><td> 21    <td>       HE <td> W <td> R <td> 1
 * <tr><td> 20    <td>      ESS <td> W <td> R <td> 0
 * <tr><td> 19:16 <td>   RFMODE <td> W <td> R <td> 0x2
 * <tr><td> 15:12 <td>     NSTS <td> W <td> R <td> 0x0
 * <tr><td> 11:08 <td>      NSS <td> W <td> R <td> 0x0
 * <tr><td> 07:04 <td>      NTX <td> W <td> R <td> 0x0
 * <tr><td> 03:00 <td>      NRX <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the HDMCONFIG register
#define MDM_HDMCONFIG_ADDR   0x24C00000
/// Offset of the HDMCONFIG register from the base address
#define MDM_HDMCONFIG_OFFSET 0x00000000
/// Index of the HDMCONFIG register
#define MDM_HDMCONFIG_INDEX  0x00000000
/// Reset value of the HDMCONFIG register
#define MDM_HDMCONFIG_RESET  0x00220000

/**
 * @brief Returns the current value of the HDMCONFIG register.
 * The HDMCONFIG register will be read and its value returned.
 * @return The current value of the HDMCONFIG register.
 */
__INLINE uint32_t mdm_hdmconfig_get(void)
{
    return REG_PL_RD(MDM_HDMCONFIG_ADDR);
}

// field definitions
/// MUMIMOTX field bit
#define MDM_MUMIMOTX_BIT    ((uint32_t)0x80000000)
/// MUMIMOTX field position
#define MDM_MUMIMOTX_POS    31
/// MUMIMORX field bit
#define MDM_MUMIMORX_BIT    ((uint32_t)0x40000000)
/// MUMIMORX field position
#define MDM_MUMIMORX_POS    30
/// BFMER field bit
#define MDM_BFMER_BIT       ((uint32_t)0x20000000)
/// BFMER field position
#define MDM_BFMER_POS       29
/// BFMEE field bit
#define MDM_BFMEE_BIT       ((uint32_t)0x10000000)
/// BFMEE field position
#define MDM_BFMEE_POS       28
/// LDPCDEC field bit
#define MDM_LDPCDEC_BIT     ((uint32_t)0x08000000)
/// LDPCDEC field position
#define MDM_LDPCDEC_POS     27
/// LDPCENC field bit
#define MDM_LDPCENC_BIT     ((uint32_t)0x04000000)
/// LDPCENC field position
#define MDM_LDPCENC_POS     26
/// CHBW field mask
#define MDM_CHBW_MASK       ((uint32_t)0x03000000)
/// CHBW field LSB position
#define MDM_CHBW_LSB        24
/// CHBW field width
#define MDM_CHBW_WIDTH      ((uint32_t)0x00000002)
/// DSSSCCK field bit
#define MDM_DSSSCCK_BIT     ((uint32_t)0x00800000)
/// DSSSCCK field position
#define MDM_DSSSCCK_POS     23
/// VHT field bit
#define MDM_VHT_BIT         ((uint32_t)0x00400000)
/// VHT field position
#define MDM_VHT_POS         22
/// HE field bit
#define MDM_HE_BIT          ((uint32_t)0x00200000)
/// HE field position
#define MDM_HE_POS          21
/// ESS field bit
#define MDM_ESS_BIT         ((uint32_t)0x00100000)
/// ESS field position
#define MDM_ESS_POS         20
/// RFMODE field mask
#define MDM_RFMODE_MASK     ((uint32_t)0x000F0000)
/// RFMODE field LSB position
#define MDM_RFMODE_LSB      16
/// RFMODE field width
#define MDM_RFMODE_WIDTH    ((uint32_t)0x00000004)
/// NSTS field mask
#define MDM_NSTS_MASK       ((uint32_t)0x0000F000)
/// NSTS field LSB position
#define MDM_NSTS_LSB        12
/// NSTS field width
#define MDM_NSTS_WIDTH      ((uint32_t)0x00000004)
/// NSS field mask
#define MDM_NSS_MASK        ((uint32_t)0x00000F00)
/// NSS field LSB position
#define MDM_NSS_LSB         8
/// NSS field width
#define MDM_NSS_WIDTH       ((uint32_t)0x00000004)
/// NTX field mask
#define MDM_NTX_MASK        ((uint32_t)0x000000F0)
/// NTX field LSB position
#define MDM_NTX_LSB         4
/// NTX field width
#define MDM_NTX_WIDTH       ((uint32_t)0x00000004)
/// NRX field mask
#define MDM_NRX_MASK        ((uint32_t)0x0000000F)
/// NRX field LSB position
#define MDM_NRX_LSB         0
/// NRX field width
#define MDM_NRX_WIDTH       ((uint32_t)0x00000004)

/// MUMIMOTX field reset value
#define MDM_MUMIMOTX_RST    0x0
/// MUMIMORX field reset value
#define MDM_MUMIMORX_RST    0x0
/// BFMER field reset value
#define MDM_BFMER_RST       0x0
/// BFMEE field reset value
#define MDM_BFMEE_RST       0x0
/// LDPCDEC field reset value
#define MDM_LDPCDEC_RST     0x0
/// LDPCENC field reset value
#define MDM_LDPCENC_RST     0x0
/// CHBW field reset value
#define MDM_CHBW_RST        0x0
/// DSSSCCK field reset value
#define MDM_DSSSCCK_RST     0x0
/// VHT field reset value
#define MDM_VHT_RST         0x0
/// HE field reset value
#define MDM_HE_RST          0x1
/// ESS field reset value
#define MDM_ESS_RST         0x0
/// RFMODE field reset value
#define MDM_RFMODE_RST      0x2
/// NSTS field reset value
#define MDM_NSTS_RST        0x0
/// NSS field reset value
#define MDM_NSS_RST         0x0
/// NTX field reset value
#define MDM_NTX_RST         0x0
/// NRX field reset value
#define MDM_NRX_RST         0x0

/**
 * @brief Unpacks HDMCONFIG's fields from current value of the HDMCONFIG register.
 *
 * Reads the HDMCONFIG register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] mumimotx - Will be populated with the current value of this field from the register.
 * @param[out] mumimorx - Will be populated with the current value of this field from the register.
 * @param[out] bfmer - Will be populated with the current value of this field from the register.
 * @param[out] bfmee - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdec - Will be populated with the current value of this field from the register.
 * @param[out] ldpcenc - Will be populated with the current value of this field from the register.
 * @param[out] chbw - Will be populated with the current value of this field from the register.
 * @param[out] dssscck - Will be populated with the current value of this field from the register.
 * @param[out] vht - Will be populated with the current value of this field from the register.
 * @param[out] he - Will be populated with the current value of this field from the register.
 * @param[out] ess - Will be populated with the current value of this field from the register.
 * @param[out] rfmode - Will be populated with the current value of this field from the register.
 * @param[out] nsts - Will be populated with the current value of this field from the register.
 * @param[out] nss - Will be populated with the current value of this field from the register.
 * @param[out] ntx - Will be populated with the current value of this field from the register.
 * @param[out] nrx - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_hdmconfig_unpack(uint8_t* mumimotx, uint8_t* mumimorx, uint8_t* bfmer, uint8_t* bfmee, uint8_t* ldpcdec, uint8_t* ldpcenc, uint8_t* chbw, uint8_t* dssscck, uint8_t* vht, uint8_t* he, uint8_t* ess, uint8_t* rfmode, uint8_t* nsts, uint8_t* nss, uint8_t* ntx, uint8_t* nrx)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);

    *mumimotx = (localVal & ((uint32_t)0x80000000)) >> 31;
    *mumimorx = (localVal & ((uint32_t)0x40000000)) >> 30;
    *bfmer = (localVal & ((uint32_t)0x20000000)) >> 29;
    *bfmee = (localVal & ((uint32_t)0x10000000)) >> 28;
    *ldpcdec = (localVal & ((uint32_t)0x08000000)) >> 27;
    *ldpcenc = (localVal & ((uint32_t)0x04000000)) >> 26;
    *chbw = (localVal & ((uint32_t)0x03000000)) >> 24;
    *dssscck = (localVal & ((uint32_t)0x00800000)) >> 23;
    *vht = (localVal & ((uint32_t)0x00400000)) >> 22;
    *he = (localVal & ((uint32_t)0x00200000)) >> 21;
    *ess = (localVal & ((uint32_t)0x00100000)) >> 20;
    *rfmode = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *nsts = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *nss = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *ntx = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *nrx = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the MUMIMOTX field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the MUMIMOTX field's value will be returned.
 *
 * @return The current value of the MUMIMOTX field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_mumimotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Returns the current value of the MUMIMORX field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the MUMIMORX field's value will be returned.
 *
 * @return The current value of the MUMIMORX field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_mumimorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Returns the current value of the BFMER field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the BFMER field's value will be returned.
 *
 * @return The current value of the BFMER field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_bfmer_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Returns the current value of the BFMEE field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the BFMEE field's value will be returned.
 *
 * @return The current value of the BFMEE field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_bfmee_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Returns the current value of the LDPCDEC field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the LDPCDEC field's value will be returned.
 *
 * @return The current value of the LDPCDEC field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_ldpcdec_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Returns the current value of the LDPCENC field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the LDPCENC field's value will be returned.
 *
 * @return The current value of the LDPCENC field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_ldpcenc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Returns the current value of the CHBW field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the CHBW field's value will be returned.
 *
 * @return The current value of the CHBW field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_chbw_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

/**
 * @brief Returns the current value of the DSSSCCK field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the DSSSCCK field's value will be returned.
 *
 * @return The current value of the DSSSCCK field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_dssscck_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Returns the current value of the VHT field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the VHT field's value will be returned.
 *
 * @return The current value of the VHT field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_vht_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Returns the current value of the HE field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the HE field's value will be returned.
 *
 * @return The current value of the HE field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_he_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Returns the current value of the ESS field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the ESS field's value will be returned.
 *
 * @return The current value of the ESS field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_ess_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Returns the current value of the RFMODE field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the RFMODE field's value will be returned.
 *
 * @return The current value of the RFMODE field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_rfmode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Returns the current value of the NSTS field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the NSTS field's value will be returned.
 *
 * @return The current value of the NSTS field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_nsts_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Returns the current value of the NSS field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the NSS field's value will be returned.
 *
 * @return The current value of the NSS field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_nss_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Returns the current value of the NTX field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the NTX field's value will be returned.
 *
 * @return The current value of the NTX field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_ntx_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Returns the current value of the NRX field in the HDMCONFIG register.
 *
 * The HDMCONFIG register will be read and the NRX field's value will be returned.
 *
 * @return The current value of the NRX field in the HDMCONFIG register.
 */
__INLINE uint8_t mdm_nrx_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/// @}

/**
 * @name TDOFFSETSTAT0 register definitions
 * <table>
 * <caption>TDOFFSETSTAT0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 20:00 <td> COARSEFOANGLE <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TDOFFSETSTAT0 register
#define MDM_TDOFFSETSTAT0_ADDR   0x24C00004
/// Offset of the TDOFFSETSTAT0 register from the base address
#define MDM_TDOFFSETSTAT0_OFFSET 0x00000004
/// Index of the TDOFFSETSTAT0 register
#define MDM_TDOFFSETSTAT0_INDEX  0x00000001
/// Reset value of the TDOFFSETSTAT0 register
#define MDM_TDOFFSETSTAT0_RESET  0x00000000

/**
 * @brief Returns the current value of the TDOFFSETSTAT0 register.
 * The TDOFFSETSTAT0 register will be read and its value returned.
 * @return The current value of the TDOFFSETSTAT0 register.
 */
__INLINE uint32_t mdm_tdoffsetstat0_get(void)
{
    return REG_PL_RD(MDM_TDOFFSETSTAT0_ADDR);
}

// field definitions
/// COARSEFOANGLE field mask
#define MDM_COARSEFOANGLE_MASK   ((uint32_t)0x001FFFFF)
/// COARSEFOANGLE field LSB position
#define MDM_COARSEFOANGLE_LSB    0
/// COARSEFOANGLE field width
#define MDM_COARSEFOANGLE_WIDTH  ((uint32_t)0x00000015)

/// COARSEFOANGLE field reset value
#define MDM_COARSEFOANGLE_RST    0x0

/**
 * @brief Returns the current value of the COARSEFOANGLE field in the TDOFFSETSTAT0 register.
 *
 * The TDOFFSETSTAT0 register will be read and the COARSEFOANGLE field's value will be returned.
 *
 * @return The current value of the COARSEFOANGLE field in the TDOFFSETSTAT0 register.
 */
__INLINE uint32_t mdm_coarsefoangle_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDOFFSETSTAT0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x001FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name TDOFFSETSTAT1 register definitions
 * <table>
 * <caption>TDOFFSETSTAT1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 20:00 <td> FINEFOANGLE <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TDOFFSETSTAT1 register
#define MDM_TDOFFSETSTAT1_ADDR   0x24C00008
/// Offset of the TDOFFSETSTAT1 register from the base address
#define MDM_TDOFFSETSTAT1_OFFSET 0x00000008
/// Index of the TDOFFSETSTAT1 register
#define MDM_TDOFFSETSTAT1_INDEX  0x00000002
/// Reset value of the TDOFFSETSTAT1 register
#define MDM_TDOFFSETSTAT1_RESET  0x00000000

/**
 * @brief Returns the current value of the TDOFFSETSTAT1 register.
 * The TDOFFSETSTAT1 register will be read and its value returned.
 * @return The current value of the TDOFFSETSTAT1 register.
 */
__INLINE uint32_t mdm_tdoffsetstat1_get(void)
{
    return REG_PL_RD(MDM_TDOFFSETSTAT1_ADDR);
}

// field definitions
/// FINEFOANGLE field mask
#define MDM_FINEFOANGLE_MASK   ((uint32_t)0x001FFFFF)
/// FINEFOANGLE field LSB position
#define MDM_FINEFOANGLE_LSB    0
/// FINEFOANGLE field width
#define MDM_FINEFOANGLE_WIDTH  ((uint32_t)0x00000015)

/// FINEFOANGLE field reset value
#define MDM_FINEFOANGLE_RST    0x0

/**
 * @brief Returns the current value of the FINEFOANGLE field in the TDOFFSETSTAT1 register.
 *
 * The TDOFFSETSTAT1 register will be read and the FINEFOANGLE field's value will be returned.
 *
 * @return The current value of the FINEFOANGLE field in the TDOFFSETSTAT1 register.
 */
__INLINE uint32_t mdm_finefoangle_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDOFFSETSTAT1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x001FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FDTOFFSETSTAT0 register definitions
 * <table>
 * <caption>FDTOFFSETSTAT0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 29:08 <td>   STOSLOPESTAT <td> W <td> R <td> 0x0
 * <tr><td> 05:00 <td> TDSYNCHOFFSTAT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FDTOFFSETSTAT0 register
#define MDM_FDTOFFSETSTAT0_ADDR   0x24C0000C
/// Offset of the FDTOFFSETSTAT0 register from the base address
#define MDM_FDTOFFSETSTAT0_OFFSET 0x0000000C
/// Index of the FDTOFFSETSTAT0 register
#define MDM_FDTOFFSETSTAT0_INDEX  0x00000003
/// Reset value of the FDTOFFSETSTAT0 register
#define MDM_FDTOFFSETSTAT0_RESET  0x00000000

/**
 * @brief Returns the current value of the FDTOFFSETSTAT0 register.
 * The FDTOFFSETSTAT0 register will be read and its value returned.
 * @return The current value of the FDTOFFSETSTAT0 register.
 */
__INLINE uint32_t mdm_fdtoffsetstat0_get(void)
{
    return REG_PL_RD(MDM_FDTOFFSETSTAT0_ADDR);
}

// field definitions
/// STOSLOPESTAT field mask
#define MDM_STOSLOPESTAT_MASK     ((uint32_t)0x3FFFFF00)
/// STOSLOPESTAT field LSB position
#define MDM_STOSLOPESTAT_LSB      8
/// STOSLOPESTAT field width
#define MDM_STOSLOPESTAT_WIDTH    ((uint32_t)0x00000016)
/// TDSYNCHOFFSTAT field mask
#define MDM_TDSYNCHOFFSTAT_MASK   ((uint32_t)0x0000003F)
/// TDSYNCHOFFSTAT field LSB position
#define MDM_TDSYNCHOFFSTAT_LSB    0
/// TDSYNCHOFFSTAT field width
#define MDM_TDSYNCHOFFSTAT_WIDTH  ((uint32_t)0x00000006)

/// STOSLOPESTAT field reset value
#define MDM_STOSLOPESTAT_RST      0x0
/// TDSYNCHOFFSTAT field reset value
#define MDM_TDSYNCHOFFSTAT_RST    0x0

/**
 * @brief Unpacks FDTOFFSETSTAT0's fields from current value of the FDTOFFSETSTAT0 register.
 *
 * Reads the FDTOFFSETSTAT0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] stoslopestat - Will be populated with the current value of this field from the register.
 * @param[out] tdsynchoffstat - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_fdtoffsetstat0_unpack(uint32_t* stoslopestat, uint8_t* tdsynchoffstat)
{
    uint32_t localVal = REG_PL_RD(MDM_FDTOFFSETSTAT0_ADDR);

    *stoslopestat = (localVal & ((uint32_t)0x3FFFFF00)) >> 8;
    *tdsynchoffstat = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the STOSLOPESTAT field in the FDTOFFSETSTAT0 register.
 *
 * The FDTOFFSETSTAT0 register will be read and the STOSLOPESTAT field's value will be returned.
 *
 * @return The current value of the STOSLOPESTAT field in the FDTOFFSETSTAT0 register.
 */
__INLINE uint32_t mdm_stoslopestat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDTOFFSETSTAT0_ADDR);
    return ((localVal & ((uint32_t)0x3FFFFF00)) >> 8);
}

/**
 * @brief Returns the current value of the TDSYNCHOFFSTAT field in the FDTOFFSETSTAT0 register.
 *
 * The FDTOFFSETSTAT0 register will be read and the TDSYNCHOFFSTAT field's value will be returned.
 *
 * @return The current value of the TDSYNCHOFFSTAT field in the FDTOFFSETSTAT0 register.
 */
__INLINE uint8_t mdm_tdsynchoffstat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDTOFFSETSTAT0_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/// @}

/**
 * @name FDTOFFSETSTAT1 register definitions
 * <table>
 * <caption>FDTOFFSETSTAT1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:00 <td> CPESLOPESTAT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FDTOFFSETSTAT1 register
#define MDM_FDTOFFSETSTAT1_ADDR   0x24C00010
/// Offset of the FDTOFFSETSTAT1 register from the base address
#define MDM_FDTOFFSETSTAT1_OFFSET 0x00000010
/// Index of the FDTOFFSETSTAT1 register
#define MDM_FDTOFFSETSTAT1_INDEX  0x00000004
/// Reset value of the FDTOFFSETSTAT1 register
#define MDM_FDTOFFSETSTAT1_RESET  0x00000000

/**
 * @brief Returns the current value of the FDTOFFSETSTAT1 register.
 * The FDTOFFSETSTAT1 register will be read and its value returned.
 * @return The current value of the FDTOFFSETSTAT1 register.
 */
__INLINE uint32_t mdm_fdtoffsetstat1_get(void)
{
    return REG_PL_RD(MDM_FDTOFFSETSTAT1_ADDR);
}

// field definitions
/// CPESLOPESTAT field mask
#define MDM_CPESLOPESTAT_MASK   ((uint32_t)0x003FFFFF)
/// CPESLOPESTAT field LSB position
#define MDM_CPESLOPESTAT_LSB    0
/// CPESLOPESTAT field width
#define MDM_CPESLOPESTAT_WIDTH  ((uint32_t)0x00000016)

/// CPESLOPESTAT field reset value
#define MDM_CPESLOPESTAT_RST    0x0

/**
 * @brief Returns the current value of the CPESLOPESTAT field in the FDTOFFSETSTAT1 register.
 *
 * The FDTOFFSETSTAT1 register will be read and the CPESLOPESTAT field's value will be returned.
 *
 * @return The current value of the CPESLOPESTAT field in the FDTOFFSETSTAT1 register.
 */
__INLINE uint32_t mdm_cpeslopestat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDTOFFSETSTAT1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x003FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name TBESTAT register definitions
 * <table>
 * <caption>TBESTAT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 20:08 <td>     TBEMaxCorrValSum <td> W <td> R <td> 0x0
 * <tr><td> 07:00 <td>             TBECOUNT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TBESTAT register
#define MDM_TBESTAT_ADDR   0x24C00014
/// Offset of the TBESTAT register from the base address
#define MDM_TBESTAT_OFFSET 0x00000014
/// Index of the TBESTAT register
#define MDM_TBESTAT_INDEX  0x00000005
/// Reset value of the TBESTAT register
#define MDM_TBESTAT_RESET  0x00000000

/**
 * @brief Returns the current value of the TBESTAT register.
 * The TBESTAT register will be read and its value returned.
 * @return The current value of the TBESTAT register.
 */
__INLINE uint32_t mdm_tbestat_get(void)
{
    return REG_PL_RD(MDM_TBESTAT_ADDR);
}

// field definitions
/// TBE_MAX_CORR_VAL_SUM field mask
#define MDM_TBE_MAX_CORR_VAL_SUM_MASK   ((uint32_t)0x001FFF00)
/// TBE_MAX_CORR_VAL_SUM field LSB position
#define MDM_TBE_MAX_CORR_VAL_SUM_LSB    8
/// TBE_MAX_CORR_VAL_SUM field width
#define MDM_TBE_MAX_CORR_VAL_SUM_WIDTH  ((uint32_t)0x0000000D)
/// TBECOUNT field mask
#define MDM_TBECOUNT_MASK               ((uint32_t)0x000000FF)
/// TBECOUNT field LSB position
#define MDM_TBECOUNT_LSB                0
/// TBECOUNT field width
#define MDM_TBECOUNT_WIDTH              ((uint32_t)0x00000008)

/// TBE_MAX_CORR_VAL_SUM field reset value
#define MDM_TBE_MAX_CORR_VAL_SUM_RST    0x0
/// TBECOUNT field reset value
#define MDM_TBECOUNT_RST                0x0

/**
 * @brief Unpacks TBESTAT's fields from current value of the TBESTAT register.
 *
 * Reads the TBESTAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tbemaxcorrvalsum - Will be populated with the current value of this field from the register.
 * @param[out] tbecount - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tbestat_unpack(uint16_t* tbemaxcorrvalsum, uint8_t* tbecount)
{
    uint32_t localVal = REG_PL_RD(MDM_TBESTAT_ADDR);

    *tbemaxcorrvalsum = (localVal & ((uint32_t)0x001FFF00)) >> 8;
    *tbecount = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the TBEMaxCorrValSum field in the TBESTAT register.
 *
 * The TBESTAT register will be read and the TBEMaxCorrValSum field's value will be returned.
 *
 * @return The current value of the TBEMaxCorrValSum field in the TBESTAT register.
 */
__INLINE uint16_t mdm_tbe_max_corr_val_sum_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBESTAT_ADDR);
    return ((localVal & ((uint32_t)0x001FFF00)) >> 8);
}

/**
 * @brief Returns the current value of the TBECOUNT field in the TBESTAT register.
 *
 * The TBESTAT register will be read and the TBECOUNT field's value will be returned.
 *
 * @return The current value of the TBECOUNT field in the TBESTAT register.
 */
__INLINE uint8_t mdm_tbecount_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBESTAT_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/// @}

#if RW_BFMER_EN
/**
 * @name BFMERSTAT0 register definitions
 * <table>
 * <caption>BFMERSTAT0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 04:00 <td>   BFMERMemSize <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the BFMERSTAT0 register
#define MDM_BFMERSTAT0_ADDR   0x24C00018
/// Offset of the BFMERSTAT0 register from the base address
#define MDM_BFMERSTAT0_OFFSET 0x00000018
/// Index of the BFMERSTAT0 register
#define MDM_BFMERSTAT0_INDEX  0x00000006
/// Reset value of the BFMERSTAT0 register
#define MDM_BFMERSTAT0_RESET  0x00000000

/**
 * @brief Returns the current value of the BFMERSTAT0 register.
 * The BFMERSTAT0 register will be read and its value returned.
 * @return The current value of the BFMERSTAT0 register.
 */
__INLINE uint32_t mdm_bfmerstat0_get(void)
{
    return REG_PL_RD(MDM_BFMERSTAT0_ADDR);
}

// field definitions
/// BFMER_MEM_SIZE field mask
#define MDM_BFMER_MEM_SIZE_MASK   ((uint32_t)0x0000001F)
/// BFMER_MEM_SIZE field LSB position
#define MDM_BFMER_MEM_SIZE_LSB    0
/// BFMER_MEM_SIZE field width
#define MDM_BFMER_MEM_SIZE_WIDTH  ((uint32_t)0x00000005)

/// BFMER_MEM_SIZE field reset value
#define MDM_BFMER_MEM_SIZE_RST    0x0

/**
 * @brief Returns the current value of the BFMERMemSize field in the BFMERSTAT0 register.
 *
 * The BFMERSTAT0 register will be read and the BFMERMemSize field's value will be returned.
 *
 * @return The current value of the BFMERMemSize field in the BFMERSTAT0 register.
 */
__INLINE uint8_t mdm_bfmer_mem_size_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BFMERSTAT0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000001F)) == 0);
    return (localVal >> 0);
}

#endif // RW_BFMER_EN
/// @}

/**
 * @name EQUALSTAT0 register definitions
 * <table>
 * <caption>EQUALSTAT0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 27:00 <td> MHSTAT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the EQUALSTAT0 register
#define MDM_EQUALSTAT0_ADDR   0x24C00034
/// Offset of the EQUALSTAT0 register from the base address
#define MDM_EQUALSTAT0_OFFSET 0x00000034
/// Index of the EQUALSTAT0 register
#define MDM_EQUALSTAT0_INDEX  0x0000000D
/// Reset value of the EQUALSTAT0 register
#define MDM_EQUALSTAT0_RESET  0x00000000

/**
 * @brief Returns the current value of the EQUALSTAT0 register.
 * The EQUALSTAT0 register will be read and its value returned.
 * @return The current value of the EQUALSTAT0 register.
 */
__INLINE uint32_t mdm_equalstat0_get(void)
{
    return REG_PL_RD(MDM_EQUALSTAT0_ADDR);
}

// field definitions
/// MHSTAT field mask
#define MDM_MHSTAT_MASK   ((uint32_t)0x0FFFFFFF)
/// MHSTAT field LSB position
#define MDM_MHSTAT_LSB    0
/// MHSTAT field width
#define MDM_MHSTAT_WIDTH  ((uint32_t)0x0000001C)

/// MHSTAT field reset value
#define MDM_MHSTAT_RST    0x0

/**
 * @brief Returns the current value of the MHSTAT field in the EQUALSTAT0 register.
 *
 * The EQUALSTAT0 register will be read and the MHSTAT field's value will be returned.
 *
 * @return The current value of the MHSTAT field in the EQUALSTAT0 register.
 */
__INLINE uint32_t mdm_mhstat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALSTAT0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0FFFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name EQUALSTAT1 register definitions
 * <table>
 * <caption>EQUALSTAT1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 19:00 <td> MHSCMAXSTAT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the EQUALSTAT1 register
#define MDM_EQUALSTAT1_ADDR   0x24C00038
/// Offset of the EQUALSTAT1 register from the base address
#define MDM_EQUALSTAT1_OFFSET 0x00000038
/// Index of the EQUALSTAT1 register
#define MDM_EQUALSTAT1_INDEX  0x0000000E
/// Reset value of the EQUALSTAT1 register
#define MDM_EQUALSTAT1_RESET  0x00000000

/**
 * @brief Returns the current value of the EQUALSTAT1 register.
 * The EQUALSTAT1 register will be read and its value returned.
 * @return The current value of the EQUALSTAT1 register.
 */
__INLINE uint32_t mdm_equalstat1_get(void)
{
    return REG_PL_RD(MDM_EQUALSTAT1_ADDR);
}

// field definitions
/// MHSCMAXSTAT field mask
#define MDM_MHSCMAXSTAT_MASK   ((uint32_t)0x000FFFFF)
/// MHSCMAXSTAT field LSB position
#define MDM_MHSCMAXSTAT_LSB    0
/// MHSCMAXSTAT field width
#define MDM_MHSCMAXSTAT_WIDTH  ((uint32_t)0x00000014)

/// MHSCMAXSTAT field reset value
#define MDM_MHSCMAXSTAT_RST    0x0

/**
 * @brief Returns the current value of the MHSCMAXSTAT field in the EQUALSTAT1 register.
 *
 * The EQUALSTAT1 register will be read and the MHSCMAXSTAT field's value will be returned.
 *
 * @return The current value of the MHSCMAXSTAT field in the EQUALSTAT1 register.
 */
__INLINE uint32_t mdm_mhscmaxstat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALSTAT1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name HDMVERSION register definitions
 * <table>
 * <caption>HDMVERSION bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:24 <td>  MajorVersion <td> W <td> R <td> 0x1
 * <tr><td> 23:16 <td>  MinorVersion <td> W <td> R <td> 0x2
 * <tr><td> 15:00 <td>           Rel <td> R <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the HDMVERSION register
#define MDM_HDMVERSION_ADDR   0x24C0003C
/// Offset of the HDMVERSION register from the base address
#define MDM_HDMVERSION_OFFSET 0x0000003C
/// Index of the HDMVERSION register
#define MDM_HDMVERSION_INDEX  0x0000000F
/// Reset value of the HDMVERSION register
#define MDM_HDMVERSION_RESET  0x01020000

/**
 * @brief Returns the current value of the HDMVERSION register.
 * The HDMVERSION register will be read and its value returned.
 * @return The current value of the HDMVERSION register.
 */
__INLINE uint32_t mdm_hdmversion_get(void)
{
    return REG_PL_RD(MDM_HDMVERSION_ADDR);
}

// field definitions
/// MAJOR_VERSION field mask
#define MDM_MAJOR_VERSION_MASK   ((uint32_t)0xFF000000)
/// MAJOR_VERSION field LSB position
#define MDM_MAJOR_VERSION_LSB    24
/// MAJOR_VERSION field width
#define MDM_MAJOR_VERSION_WIDTH  ((uint32_t)0x00000008)
/// MINOR_VERSION field mask
#define MDM_MINOR_VERSION_MASK   ((uint32_t)0x00FF0000)
/// MINOR_VERSION field LSB position
#define MDM_MINOR_VERSION_LSB    16
/// MINOR_VERSION field width
#define MDM_MINOR_VERSION_WIDTH  ((uint32_t)0x00000008)
/// REL field mask
#define MDM_REL_MASK             ((uint32_t)0x0000FFFF)
/// REL field LSB position
#define MDM_REL_LSB              0
/// REL field width
#define MDM_REL_WIDTH            ((uint32_t)0x00000010)

/// MAJOR_VERSION field reset value
#define MDM_MAJOR_VERSION_RST    0x1
/// MINOR_VERSION field reset value
#define MDM_MINOR_VERSION_RST    0x2
/// REL field reset value
#define MDM_REL_RST              0x0

/**
 * @brief Unpacks HDMVERSION's fields from current value of the HDMVERSION register.
 *
 * Reads the HDMVERSION register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] majorversion - Will be populated with the current value of this field from the register.
 * @param[out] minorversion - Will be populated with the current value of this field from the register.
 * @param[out] rel - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_hdmversion_unpack(uint8_t* majorversion, uint8_t* minorversion, uint16_t* rel)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMVERSION_ADDR);

    *majorversion = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *minorversion = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *rel = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the MajorVersion field in the HDMVERSION register.
 *
 * The HDMVERSION register will be read and the MajorVersion field's value will be returned.
 *
 * @return The current value of the MajorVersion field in the HDMVERSION register.
 */
__INLINE uint8_t mdm_major_version_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMVERSION_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the MinorVersion field in the HDMVERSION register.
 *
 * The HDMVERSION register will be read and the MinorVersion field's value will be returned.
 *
 * @return The current value of the MinorVersion field in the HDMVERSION register.
 */
__INLINE uint8_t mdm_minor_version_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMVERSION_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the Rel field in the HDMVERSION register.
 *
 * The HDMVERSION register will be read and the Rel field's value will be returned.
 *
 * @return The current value of the Rel field in the HDMVERSION register.
 */
__INLINE uint16_t mdm_rel_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMVERSION_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/// @}

/**
 * @name HDMCONFIG2 register definitions
 * <table>
 * <caption>HDMCONFIG2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 01 <td>  QAM256 <td> W <td> R <td> 0
 * <tr><td> 00 <td> QAM1024 <td> W <td> R <td> 0
 * </table>
 *
 * @{
 */

/// Address of the HDMCONFIG2 register
#define MDM_HDMCONFIG2_ADDR   0x24C00048
/// Offset of the HDMCONFIG2 register from the base address
#define MDM_HDMCONFIG2_OFFSET 0x00000048
/// Index of the HDMCONFIG2 register
#define MDM_HDMCONFIG2_INDEX  0x00000012
/// Reset value of the HDMCONFIG2 register
#define MDM_HDMCONFIG2_RESET  0x00000000

/**
 * @brief Returns the current value of the HDMCONFIG2 register.
 * The HDMCONFIG2 register will be read and its value returned.
 * @return The current value of the HDMCONFIG2 register.
 */
__INLINE uint32_t mdm_hdmconfig2_get(void)
{
    return REG_PL_RD(MDM_HDMCONFIG2_ADDR);
}

// field definitions
/// QAM256 field bit
#define MDM_QAM256_BIT     ((uint32_t)0x00000002)
/// QAM256 field position
#define MDM_QAM256_POS     1
/// QAM1024 field bit
#define MDM_QAM1024_BIT    ((uint32_t)0x00000001)
/// QAM1024 field position
#define MDM_QAM1024_POS    0

/// QAM256 field reset value
#define MDM_QAM256_RST     0x0
/// QAM1024 field reset value
#define MDM_QAM1024_RST    0x0

/**
 * @brief Unpacks HDMCONFIG2's fields from current value of the HDMCONFIG2 register.
 *
 * Reads the HDMCONFIG2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] qam256 - Will be populated with the current value of this field from the register.
 * @param[out] qam1024 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_hdmconfig2_unpack(uint8_t* qam256, uint8_t* qam1024)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG2_ADDR);

    *qam256 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *qam1024 = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the QAM256 field in the HDMCONFIG2 register.
 *
 * The HDMCONFIG2 register will be read and the QAM256 field's value will be returned.
 *
 * @return The current value of the QAM256 field in the HDMCONFIG2 register.
 */
__INLINE uint8_t mdm_qam256_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG2_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the QAM1024 field in the HDMCONFIG2 register.
 *
 * The HDMCONFIG2 register will be read and the QAM1024 field's value will be returned.
 *
 * @return The current value of the QAM1024 field in the HDMCONFIG2 register.
 */
__INLINE uint8_t mdm_qam1024_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_HDMCONFIG2_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/// @}

/**
 * @name FDTOFFSETSTAT2 register definitions
 * <table>
 * <caption>FDTOFFSETSTAT2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:00 <td> FDOPREAMBSTAT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FDTOFFSETSTAT2 register
#define MDM_FDTOFFSETSTAT2_ADDR   0x24C0004C
/// Offset of the FDTOFFSETSTAT2 register from the base address
#define MDM_FDTOFFSETSTAT2_OFFSET 0x0000004C
/// Index of the FDTOFFSETSTAT2 register
#define MDM_FDTOFFSETSTAT2_INDEX  0x00000013
/// Reset value of the FDTOFFSETSTAT2 register
#define MDM_FDTOFFSETSTAT2_RESET  0x00000000

/**
 * @brief Returns the current value of the FDTOFFSETSTAT2 register.
 * The FDTOFFSETSTAT2 register will be read and its value returned.
 * @return The current value of the FDTOFFSETSTAT2 register.
 */
__INLINE uint32_t mdm_fdtoffsetstat2_get(void)
{
    return REG_PL_RD(MDM_FDTOFFSETSTAT2_ADDR);
}

// field definitions
/// FDOPREAMBSTAT field mask
#define MDM_FDOPREAMBSTAT_MASK   ((uint32_t)0x003FFFFF)
/// FDOPREAMBSTAT field LSB position
#define MDM_FDOPREAMBSTAT_LSB    0
/// FDOPREAMBSTAT field width
#define MDM_FDOPREAMBSTAT_WIDTH  ((uint32_t)0x00000016)

/// FDOPREAMBSTAT field reset value
#define MDM_FDOPREAMBSTAT_RST    0x0

/**
 * @brief Returns the current value of the FDOPREAMBSTAT field in the FDTOFFSETSTAT2 register.
 *
 * The FDTOFFSETSTAT2 register will be read and the FDOPREAMBSTAT field's value will be returned.
 *
 * @return The current value of the FDOPREAMBSTAT field in the FDTOFFSETSTAT2 register.
 */
__INLINE uint32_t mdm_fdopreambstat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDTOFFSETSTAT2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x003FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name FDTOFFSETSTAT3 register definitions
 * <table>
 * <caption>FDTOFFSETSTAT3 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:00 <td> FDODATASTAT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FDTOFFSETSTAT3 register
#define MDM_FDTOFFSETSTAT3_ADDR   0x24C00050
/// Offset of the FDTOFFSETSTAT3 register from the base address
#define MDM_FDTOFFSETSTAT3_OFFSET 0x00000050
/// Index of the FDTOFFSETSTAT3 register
#define MDM_FDTOFFSETSTAT3_INDEX  0x00000014
/// Reset value of the FDTOFFSETSTAT3 register
#define MDM_FDTOFFSETSTAT3_RESET  0x00000000

/**
 * @brief Returns the current value of the FDTOFFSETSTAT3 register.
 * The FDTOFFSETSTAT3 register will be read and its value returned.
 * @return The current value of the FDTOFFSETSTAT3 register.
 */
__INLINE uint32_t mdm_fdtoffsetstat3_get(void)
{
    return REG_PL_RD(MDM_FDTOFFSETSTAT3_ADDR);
}

// field definitions
/// FDODATASTAT field mask
#define MDM_FDODATASTAT_MASK   ((uint32_t)0x003FFFFF)
/// FDODATASTAT field LSB position
#define MDM_FDODATASTAT_LSB    0
/// FDODATASTAT field width
#define MDM_FDODATASTAT_WIDTH  ((uint32_t)0x00000016)

/// FDODATASTAT field reset value
#define MDM_FDODATASTAT_RST    0x0

/**
 * @brief Returns the current value of the FDODATASTAT field in the FDTOFFSETSTAT3 register.
 *
 * The FDTOFFSETSTAT3 register will be read and the FDODATASTAT field's value will be returned.
 *
 * @return The current value of the FDODATASTAT field in the FDTOFFSETSTAT3 register.
 */
__INLINE uint32_t mdm_fdodatastat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDTOFFSETSTAT3_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x003FFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name RXCFOEST register definitions
 * <table>
 * <caption>RXCFOEST bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 24:00 <td> RXCFOPHASEEST <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the RXCFOEST register
#define MDM_RXCFOEST_ADDR   0x24C00054
/// Offset of the RXCFOEST register from the base address
#define MDM_RXCFOEST_OFFSET 0x00000054
/// Index of the RXCFOEST register
#define MDM_RXCFOEST_INDEX  0x00000015
/// Reset value of the RXCFOEST register
#define MDM_RXCFOEST_RESET  0x00000000

/**
 * @brief Returns the current value of the RXCFOEST register.
 * The RXCFOEST register will be read and its value returned.
 * @return The current value of the RXCFOEST register.
 */
__INLINE uint32_t mdm_rxcfoest_get(void)
{
    return REG_PL_RD(MDM_RXCFOEST_ADDR);
}

// field definitions
/// RXCFOPHASEEST field mask
#define MDM_RXCFOPHASEEST_MASK   ((uint32_t)0x01FFFFFF)
/// RXCFOPHASEEST field LSB position
#define MDM_RXCFOPHASEEST_LSB    0
/// RXCFOPHASEEST field width
#define MDM_RXCFOPHASEEST_WIDTH  ((uint32_t)0x00000019)

/// RXCFOPHASEEST field reset value
#define MDM_RXCFOPHASEEST_RST    0x0

/**
 * @brief Returns the current value of the RXCFOPHASEEST field in the RXCFOEST register.
 *
 * The RXCFOEST register will be read and the RXCFOPHASEEST field's value will be returned.
 *
 * @return The current value of the RXCFOPHASEEST field in the RXCFOEST register.
 */
__INLINE uint32_t mdm_rxcfophaseest_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCFOEST_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x01FFFFFF)) == 0);
    return (localVal >> 0);
}

/// @}

/**
 * @name RXSFOEST register definitions
 * <table>
 * <caption>RXSFOEST bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 28    <td>    RXFSRATIOOOR <td> W <td> R <td> 0
 * <tr><td> 26:00 <td> RXSFOFSRATIOEST <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the RXSFOEST register
#define MDM_RXSFOEST_ADDR   0x24C00058
/// Offset of the RXSFOEST register from the base address
#define MDM_RXSFOEST_OFFSET 0x00000058
/// Index of the RXSFOEST register
#define MDM_RXSFOEST_INDEX  0x00000016
/// Reset value of the RXSFOEST register
#define MDM_RXSFOEST_RESET  0x00000000

/**
 * @brief Returns the current value of the RXSFOEST register.
 * The RXSFOEST register will be read and its value returned.
 * @return The current value of the RXSFOEST register.
 */
__INLINE uint32_t mdm_rxsfoest_get(void)
{
    return REG_PL_RD(MDM_RXSFOEST_ADDR);
}

// field definitions
/// RXFSRATIOOOR field bit
#define MDM_RXFSRATIOOOR_BIT       ((uint32_t)0x10000000)
/// RXFSRATIOOOR field position
#define MDM_RXFSRATIOOOR_POS       28
/// RXSFOFSRATIOEST field mask
#define MDM_RXSFOFSRATIOEST_MASK   ((uint32_t)0x07FFFFFF)
/// RXSFOFSRATIOEST field LSB position
#define MDM_RXSFOFSRATIOEST_LSB    0
/// RXSFOFSRATIOEST field width
#define MDM_RXSFOFSRATIOEST_WIDTH  ((uint32_t)0x0000001B)

/// RXFSRATIOOOR field reset value
#define MDM_RXFSRATIOOOR_RST       0x0
/// RXSFOFSRATIOEST field reset value
#define MDM_RXSFOFSRATIOEST_RST    0x0

/**
 * @brief Unpacks RXSFOEST's fields from current value of the RXSFOEST register.
 *
 * Reads the RXSFOEST register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxfsratiooor - Will be populated with the current value of this field from the register.
 * @param[out] rxsfofsratioest - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxsfoest_unpack(uint8_t* rxfsratiooor, uint32_t* rxsfofsratioest)
{
    uint32_t localVal = REG_PL_RD(MDM_RXSFOEST_ADDR);

    *rxfsratiooor = (localVal & ((uint32_t)0x10000000)) >> 28;
    *rxsfofsratioest = (localVal & ((uint32_t)0x07FFFFFF)) >> 0;
}

/**
 * @brief Returns the current value of the RXFSRATIOOOR field in the RXSFOEST register.
 *
 * The RXSFOEST register will be read and the RXFSRATIOOOR field's value will be returned.
 *
 * @return The current value of the RXFSRATIOOOR field in the RXSFOEST register.
 */
__INLINE uint8_t mdm_rxfsratiooor_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXSFOEST_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Returns the current value of the RXSFOFSRATIOEST field in the RXSFOEST register.
 *
 * The RXSFOEST register will be read and the RXSFOFSRATIOEST field's value will be returned.
 *
 * @return The current value of the RXSFOFSRATIOEST field in the RXSFOEST register.
 */
__INLINE uint32_t mdm_rxsfofsratioest_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXSFOEST_ADDR);
    return ((localVal & ((uint32_t)0x07FFFFFF)) >> 0);
}

/// @}

/**
 * @name RXNSRSTAT register definitions
 * <table>
 * <caption>RXNSRSTAT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:16 <td>  NSREXP <td> W <td> R <td> 0x0
 * <tr><td> 11:00 <td> NSRMANT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the RXNSRSTAT register
#define MDM_RXNSRSTAT_ADDR   0x24C0005C
/// Offset of the RXNSRSTAT register from the base address
#define MDM_RXNSRSTAT_OFFSET 0x0000005C
/// Index of the RXNSRSTAT register
#define MDM_RXNSRSTAT_INDEX  0x00000017
/// Reset value of the RXNSRSTAT register
#define MDM_RXNSRSTAT_RESET  0x00000000

/**
 * @brief Returns the current value of the RXNSRSTAT register.
 * The RXNSRSTAT register will be read and its value returned.
 * @return The current value of the RXNSRSTAT register.
 */
__INLINE uint32_t mdm_rxnsrstat_get(void)
{
    return REG_PL_RD(MDM_RXNSRSTAT_ADDR);
}

// field definitions
/// NSREXP field mask
#define MDM_NSREXP_MASK    ((uint32_t)0x003F0000)
/// NSREXP field LSB position
#define MDM_NSREXP_LSB     16
/// NSREXP field width
#define MDM_NSREXP_WIDTH   ((uint32_t)0x00000006)
/// NSRMANT field mask
#define MDM_NSRMANT_MASK   ((uint32_t)0x00000FFF)
/// NSRMANT field LSB position
#define MDM_NSRMANT_LSB    0
/// NSRMANT field width
#define MDM_NSRMANT_WIDTH  ((uint32_t)0x0000000C)

/// NSREXP field reset value
#define MDM_NSREXP_RST     0x0
/// NSRMANT field reset value
#define MDM_NSRMANT_RST    0x0

/**
 * @brief Unpacks RXNSRSTAT's fields from current value of the RXNSRSTAT register.
 *
 * Reads the RXNSRSTAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] nsrexp - Will be populated with the current value of this field from the register.
 * @param[out] nsrmant - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxnsrstat_unpack(uint8_t* nsrexp, uint16_t* nsrmant)
{
    uint32_t localVal = REG_PL_RD(MDM_RXNSRSTAT_ADDR);

    *nsrexp = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *nsrmant = (localVal & ((uint32_t)0x00000FFF)) >> 0;
}

/**
 * @brief Returns the current value of the NSREXP field in the RXNSRSTAT register.
 *
 * The RXNSRSTAT register will be read and the NSREXP field's value will be returned.
 *
 * @return The current value of the NSREXP field in the RXNSRSTAT register.
 */
__INLINE uint8_t mdm_nsrexp_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXNSRSTAT_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Returns the current value of the NSRMANT field in the RXNSRSTAT register.
 *
 * The RXNSRSTAT register will be read and the NSRMANT field's value will be returned.
 *
 * @return The current value of the NSRMANT field in the RXNSRSTAT register.
 */
__INLINE uint16_t mdm_nsrmant_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXNSRSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000FFF)) >> 0);
}

/// @}

#if RW_NX_LDPC_DEC
/**
 * @name LDPCDECSTAT0 register definitions
 * <table>
 * <caption>LDPCDECSTAT0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:16 <td> LDPCFFTOVCOUNT <td> W <td> R <td> 0x0
 * <tr><td> 15:00 <td>      LDPCCOUNT <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the LDPCDECSTAT0 register
#define MDM_LDPCDECSTAT0_ADDR   0x24C00060
/// Offset of the LDPCDECSTAT0 register from the base address
#define MDM_LDPCDECSTAT0_OFFSET 0x00000060
/// Index of the LDPCDECSTAT0 register
#define MDM_LDPCDECSTAT0_INDEX  0x00000018
/// Reset value of the LDPCDECSTAT0 register
#define MDM_LDPCDECSTAT0_RESET  0x00000000

/**
 * @brief Returns the current value of the LDPCDECSTAT0 register.
 * The LDPCDECSTAT0 register will be read and its value returned.
 * @return The current value of the LDPCDECSTAT0 register.
 */
__INLINE uint32_t mdm_ldpcdecstat0_get(void)
{
    return REG_PL_RD(MDM_LDPCDECSTAT0_ADDR);
}

// field definitions
/// LDPCFFTOVCOUNT field mask
#define MDM_LDPCFFTOVCOUNT_MASK   ((uint32_t)0xFFFF0000)
/// LDPCFFTOVCOUNT field LSB position
#define MDM_LDPCFFTOVCOUNT_LSB    16
/// LDPCFFTOVCOUNT field width
#define MDM_LDPCFFTOVCOUNT_WIDTH  ((uint32_t)0x00000010)
/// LDPCCOUNT field mask
#define MDM_LDPCCOUNT_MASK        ((uint32_t)0x0000FFFF)
/// LDPCCOUNT field LSB position
#define MDM_LDPCCOUNT_LSB         0
/// LDPCCOUNT field width
#define MDM_LDPCCOUNT_WIDTH       ((uint32_t)0x00000010)

/// LDPCFFTOVCOUNT field reset value
#define MDM_LDPCFFTOVCOUNT_RST    0x0
/// LDPCCOUNT field reset value
#define MDM_LDPCCOUNT_RST         0x0

/**
 * @brief Unpacks LDPCDECSTAT0's fields from current value of the LDPCDECSTAT0 register.
 *
 * Reads the LDPCDECSTAT0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ldpcfftovcount - Will be populated with the current value of this field from the register.
 * @param[out] ldpccount - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_ldpcdecstat0_unpack(uint16_t* ldpcfftovcount, uint16_t* ldpccount)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECSTAT0_ADDR);

    *ldpcfftovcount = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *ldpccount = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the LDPCFFTOVCOUNT field in the LDPCDECSTAT0 register.
 *
 * The LDPCDECSTAT0 register will be read and the LDPCFFTOVCOUNT field's value will be returned.
 *
 * @return The current value of the LDPCFFTOVCOUNT field in the LDPCDECSTAT0 register.
 */
__INLINE uint16_t mdm_ldpcfftovcount_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECSTAT0_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the LDPCCOUNT field in the LDPCDECSTAT0 register.
 *
 * The LDPCDECSTAT0 register will be read and the LDPCCOUNT field's value will be returned.
 *
 * @return The current value of the LDPCCOUNT field in the LDPCDECSTAT0 register.
 */
__INLINE uint16_t mdm_ldpccount_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECSTAT0_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

#endif // RW_NX_LDPC_DEC
/// @}

#if RW_NX_CSI
/**
 * @name CSIRDSTAT register definitions
 * <table>
 * <caption>CSIRDSTAT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 10:08 <td>   CSI_BANDWIDTH <td> W <td> R <td> 0x0
 * <tr><td> 07:04 <td> CSI_FORMAT_MODE <td> W <td> R <td> 0x0
 * <tr><td> 01    <td>       CSI_ERROR <td> W <td> R <td> 0
 * <tr><td> 00    <td>       CSI_READY <td> W <td> R <td> 0
 * </table>
 *
 * @{
 */

/// Address of the CSIRDSTAT register
#define MDM_CSIRDSTAT_ADDR   0x24C00064
/// Offset of the CSIRDSTAT register from the base address
#define MDM_CSIRDSTAT_OFFSET 0x00000064
/// Index of the CSIRDSTAT register
#define MDM_CSIRDSTAT_INDEX  0x00000019
/// Reset value of the CSIRDSTAT register
#define MDM_CSIRDSTAT_RESET  0x00000000

/**
 * @brief Returns the current value of the CSIRDSTAT register.
 * The CSIRDSTAT register will be read and its value returned.
 * @return The current value of the CSIRDSTAT register.
 */
__INLINE uint32_t mdm_csirdstat_get(void)
{
    return REG_PL_RD(MDM_CSIRDSTAT_ADDR);
}

// field definitions
/// CSI_BANDWIDTH field mask
#define MDM_CSI_BANDWIDTH_MASK     ((uint32_t)0x00000700)
/// CSI_BANDWIDTH field LSB position
#define MDM_CSI_BANDWIDTH_LSB      8
/// CSI_BANDWIDTH field width
#define MDM_CSI_BANDWIDTH_WIDTH    ((uint32_t)0x00000003)
/// CSI_FORMAT_MODE field mask
#define MDM_CSI_FORMAT_MODE_MASK   ((uint32_t)0x000000F0)
/// CSI_FORMAT_MODE field LSB position
#define MDM_CSI_FORMAT_MODE_LSB    4
/// CSI_FORMAT_MODE field width
#define MDM_CSI_FORMAT_MODE_WIDTH  ((uint32_t)0x00000004)
/// CSI_ERROR field bit
#define MDM_CSI_ERROR_BIT          ((uint32_t)0x00000002)
/// CSI_ERROR field position
#define MDM_CSI_ERROR_POS          1
/// CSI_READY field bit
#define MDM_CSI_READY_BIT          ((uint32_t)0x00000001)
/// CSI_READY field position
#define MDM_CSI_READY_POS          0

/// CSI_BANDWIDTH field reset value
#define MDM_CSI_BANDWIDTH_RST      0x0
/// CSI_FORMAT_MODE field reset value
#define MDM_CSI_FORMAT_MODE_RST    0x0
/// CSI_ERROR field reset value
#define MDM_CSI_ERROR_RST          0x0
/// CSI_READY field reset value
#define MDM_CSI_READY_RST          0x0

/**
 * @brief Unpacks CSIRDSTAT's fields from current value of the CSIRDSTAT register.
 *
 * Reads the CSIRDSTAT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] csibandwidth - Will be populated with the current value of this field from the register.
 * @param[out] csiformatmode - Will be populated with the current value of this field from the register.
 * @param[out] csierror - Will be populated with the current value of this field from the register.
 * @param[out] csiready - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_csirdstat_unpack(uint8_t* csibandwidth, uint8_t* csiformatmode, uint8_t* csierror, uint8_t* csiready)
{
    uint32_t localVal = REG_PL_RD(MDM_CSIRDSTAT_ADDR);

    *csibandwidth = (localVal & ((uint32_t)0x00000700)) >> 8;
    *csiformatmode = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *csierror = (localVal & ((uint32_t)0x00000002)) >> 1;
    *csiready = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the CSI_BANDWIDTH field in the CSIRDSTAT register.
 *
 * The CSIRDSTAT register will be read and the CSI_BANDWIDTH field's value will be returned.
 *
 * @return The current value of the CSI_BANDWIDTH field in the CSIRDSTAT register.
 */
__INLINE uint8_t mdm_csi_bandwidth_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CSIRDSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

/**
 * @brief Returns the current value of the CSI_FORMAT_MODE field in the CSIRDSTAT register.
 *
 * The CSIRDSTAT register will be read and the CSI_FORMAT_MODE field's value will be returned.
 *
 * @return The current value of the CSI_FORMAT_MODE field in the CSIRDSTAT register.
 */
__INLINE uint8_t mdm_csi_format_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CSIRDSTAT_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Returns the current value of the CSI_ERROR field in the CSIRDSTAT register.
 *
 * The CSIRDSTAT register will be read and the CSI_ERROR field's value will be returned.
 *
 * @return The current value of the CSI_ERROR field in the CSIRDSTAT register.
 */
__INLINE uint8_t mdm_csi_error_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CSIRDSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Returns the current value of the CSI_READY field in the CSIRDSTAT register.
 *
 * The CSIRDSTAT register will be read and the CSI_READY field's value will be returned.
 *
 * @return The current value of the CSI_READY field in the CSIRDSTAT register.
 */
__INLINE uint8_t mdm_csi_ready_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CSIRDSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

#endif // RW_NX_CSI
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name FEBCNTL register definitions
 * <table>
 * <caption>FEBCNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:16 <td>   MAXSTAGE <td> R <td> R/W <td> 0x27
 * <tr><td> 03    <td>    FIRDISB <td> R <td> R/W <td> 0
 * <tr><td> 02    <td>   GAINDISB <td> R <td> R/W <td> 0
 * <tr><td> 01    <td> INTERPDISB <td> R <td> R/W <td> 0
 * <tr><td> 00    <td> FIRCOEFSEL <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the FEBCNTL register
#define MDM_FEBCNTL_ADDR   0x24C00300
/// Offset of the FEBCNTL register from the base address
#define MDM_FEBCNTL_OFFSET 0x00000300
/// Index of the FEBCNTL register
#define MDM_FEBCNTL_INDEX  0x000000C0
/// Reset value of the FEBCNTL register
#define MDM_FEBCNTL_RESET  0x00270000

/**
 * @brief Returns the current value of the FEBCNTL register.
 * The FEBCNTL register will be read and its value returned.
 * @return The current value of the FEBCNTL register.
 */
__INLINE uint32_t mdm_febcntl_get(void)
{
    return REG_PL_RD(MDM_FEBCNTL_ADDR);
}

/**
 * @brief Sets the FEBCNTL register to a value.
 * The FEBCNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_febcntl_set(uint32_t value)
{
    REG_PL_WR(MDM_FEBCNTL_ADDR, value);
}

// field definitions
/// MAXSTAGE field mask
#define MDM_MAXSTAGE_MASK     ((uint32_t)0x003F0000)
/// MAXSTAGE field LSB position
#define MDM_MAXSTAGE_LSB      16
/// MAXSTAGE field width
#define MDM_MAXSTAGE_WIDTH    ((uint32_t)0x00000006)
/// FIRDISB field bit
#define MDM_FIRDISB_BIT       ((uint32_t)0x00000008)
/// FIRDISB field position
#define MDM_FIRDISB_POS       3
/// GAINDISB field bit
#define MDM_GAINDISB_BIT      ((uint32_t)0x00000004)
/// GAINDISB field position
#define MDM_GAINDISB_POS      2
/// INTERPDISB field bit
#define MDM_INTERPDISB_BIT    ((uint32_t)0x00000002)
/// INTERPDISB field position
#define MDM_INTERPDISB_POS    1
/// FIRCOEFSEL field bit
#define MDM_FIRCOEFSEL_BIT    ((uint32_t)0x00000001)
/// FIRCOEFSEL field position
#define MDM_FIRCOEFSEL_POS    0

/// MAXSTAGE field reset value
#define MDM_MAXSTAGE_RST      0x27
/// FIRDISB field reset value
#define MDM_FIRDISB_RST       0x0
/// GAINDISB field reset value
#define MDM_GAINDISB_RST      0x0
/// INTERPDISB field reset value
#define MDM_INTERPDISB_RST    0x0
/// FIRCOEFSEL field reset value
#define MDM_FIRCOEFSEL_RST    0x0

/**
 * @brief Constructs a value for the FEBCNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] maxstage - The value to use for the MAXSTAGE field.
 * @param[in] firdisb - The value to use for the FIRDISB field.
 * @param[in] gaindisb - The value to use for the GAINDISB field.
 * @param[in] interpdisb - The value to use for the INTERPDISB field.
 * @param[in] fircoefsel - The value to use for the FIRCOEFSEL field.
 */
__INLINE void mdm_febcntl_pack(uint8_t maxstage, uint8_t firdisb, uint8_t gaindisb, uint8_t interpdisb, uint8_t fircoefsel)
{
    ASSERT_ERR((((uint32_t)maxstage << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)firdisb << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)gaindisb << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)interpdisb << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)fircoefsel << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_FEBCNTL_ADDR,  ((uint32_t)maxstage << 16) | ((uint32_t)firdisb << 3) | ((uint32_t)gaindisb << 2) | ((uint32_t)interpdisb << 1) | ((uint32_t)fircoefsel << 0));
}

/**
 * @brief Unpacks FEBCNTL's fields from current value of the FEBCNTL register.
 *
 * Reads the FEBCNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] maxstage - Will be populated with the current value of this field from the register.
 * @param[out] firdisb - Will be populated with the current value of this field from the register.
 * @param[out] gaindisb - Will be populated with the current value of this field from the register.
 * @param[out] interpdisb - Will be populated with the current value of this field from the register.
 * @param[out] fircoefsel - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_febcntl_unpack(uint8_t* maxstage, uint8_t* firdisb, uint8_t* gaindisb, uint8_t* interpdisb, uint8_t* fircoefsel)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBCNTL_ADDR);

    *maxstage = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *firdisb = (localVal & ((uint32_t)0x00000008)) >> 3;
    *gaindisb = (localVal & ((uint32_t)0x00000004)) >> 2;
    *interpdisb = (localVal & ((uint32_t)0x00000002)) >> 1;
    *fircoefsel = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the MAXSTAGE field in the FEBCNTL register.
 *
 * The FEBCNTL register will be read and the MAXSTAGE field's value will be returned.
 *
 * @return The current value of the MAXSTAGE field in the FEBCNTL register.
 */
__INLINE uint8_t mdm_maxstage_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the MAXSTAGE field of the FEBCNTL register.
 *
 * The FEBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maxstage - The value to set the field to.
 */
__INLINE void mdm_maxstage_setf(uint8_t maxstage)
{
    ASSERT_ERR((((uint32_t)maxstage << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(MDM_FEBCNTL_ADDR, (REG_PL_RD(MDM_FEBCNTL_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)maxstage << 16));
}

/**
 * @brief Returns the current value of the FIRDISB field in the FEBCNTL register.
 *
 * The FEBCNTL register will be read and the FIRDISB field's value will be returned.
 *
 * @return The current value of the FIRDISB field in the FEBCNTL register.
 */
__INLINE uint8_t mdm_firdisb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the FIRDISB field of the FEBCNTL register.
 *
 * The FEBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] firdisb - The value to set the field to.
 */
__INLINE void mdm_firdisb_setf(uint8_t firdisb)
{
    ASSERT_ERR((((uint32_t)firdisb << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_FEBCNTL_ADDR, (REG_PL_RD(MDM_FEBCNTL_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)firdisb << 3));
}

/**
 * @brief Returns the current value of the GAINDISB field in the FEBCNTL register.
 *
 * The FEBCNTL register will be read and the GAINDISB field's value will be returned.
 *
 * @return The current value of the GAINDISB field in the FEBCNTL register.
 */
__INLINE uint8_t mdm_gaindisb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the GAINDISB field of the FEBCNTL register.
 *
 * The FEBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] gaindisb - The value to set the field to.
 */
__INLINE void mdm_gaindisb_setf(uint8_t gaindisb)
{
    ASSERT_ERR((((uint32_t)gaindisb << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_FEBCNTL_ADDR, (REG_PL_RD(MDM_FEBCNTL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)gaindisb << 2));
}

/**
 * @brief Returns the current value of the INTERPDISB field in the FEBCNTL register.
 *
 * The FEBCNTL register will be read and the INTERPDISB field's value will be returned.
 *
 * @return The current value of the INTERPDISB field in the FEBCNTL register.
 */
__INLINE uint8_t mdm_interpdisb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the INTERPDISB field of the FEBCNTL register.
 *
 * The FEBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] interpdisb - The value to set the field to.
 */
__INLINE void mdm_interpdisb_setf(uint8_t interpdisb)
{
    ASSERT_ERR((((uint32_t)interpdisb << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_FEBCNTL_ADDR, (REG_PL_RD(MDM_FEBCNTL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)interpdisb << 1));
}

/**
 * @brief Returns the current value of the FIRCOEFSEL field in the FEBCNTL register.
 *
 * The FEBCNTL register will be read and the FIRCOEFSEL field's value will be returned.
 *
 * @return The current value of the FIRCOEFSEL field in the FEBCNTL register.
 */
__INLINE uint8_t mdm_fircoefsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the FIRCOEFSEL field of the FEBCNTL register.
 *
 * The FEBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoefsel - The value to set the field to.
 */
__INLINE void mdm_fircoefsel_setf(uint8_t fircoefsel)
{
    ASSERT_ERR((((uint32_t)fircoefsel << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_FEBCNTL_ADDR, (REG_PL_RD(MDM_FEBCNTL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)fircoefsel << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name MDMBCNTL register definitions
 * <table>
 * <caption>MDMBCNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 28    <td>    IQMMDISB <td> R <td> R/W <td> 0
 * <tr><td> 26    <td> PRECOMPDISB <td> R <td> R/W <td> 0
 * <tr><td> 25    <td>   DCOFFDISB <td> R <td> R/W <td> 0
 * <tr><td> 24    <td>    COMPDISB <td> R <td> R/W <td> 0
 * <tr><td> 23    <td>      EQDISB <td> R <td> R/W <td> 0
 * <tr><td> 21    <td>  SPREADDISB <td> R <td> R/W <td> 0
 * <tr><td> 20    <td>  SCRAMBDISB <td> R <td> R/W <td> 0
 * <tr><td> 14:12 <td>      SFDERR <td> R <td> R/W <td> 0x0
 * <tr><td> 10:08 <td>      SFDLEN <td> R <td> R/W <td> 0x0
 * <tr><td> 05:00 <td>      PREPRE <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the MDMBCNTL register
#define MDM_MDMBCNTL_ADDR   0x24C00304
/// Offset of the MDMBCNTL register from the base address
#define MDM_MDMBCNTL_OFFSET 0x00000304
/// Index of the MDMBCNTL register
#define MDM_MDMBCNTL_INDEX  0x000000C1
/// Reset value of the MDMBCNTL register
#define MDM_MDMBCNTL_RESET  0x00000000

/**
 * @brief Returns the current value of the MDMBCNTL register.
 * The MDMBCNTL register will be read and its value returned.
 * @return The current value of the MDMBCNTL register.
 */
__INLINE uint32_t mdm_mdmbcntl_get(void)
{
    return REG_PL_RD(MDM_MDMBCNTL_ADDR);
}

/**
 * @brief Sets the MDMBCNTL register to a value.
 * The MDMBCNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_mdmbcntl_set(uint32_t value)
{
    REG_PL_WR(MDM_MDMBCNTL_ADDR, value);
}

// field definitions
/// IQMMDISB field bit
#define MDM_IQMMDISB_BIT       ((uint32_t)0x10000000)
/// IQMMDISB field position
#define MDM_IQMMDISB_POS       28
/// PRECOMPDISB field bit
#define MDM_PRECOMPDISB_BIT    ((uint32_t)0x04000000)
/// PRECOMPDISB field position
#define MDM_PRECOMPDISB_POS    26
/// DCOFFDISB field bit
#define MDM_DCOFFDISB_BIT      ((uint32_t)0x02000000)
/// DCOFFDISB field position
#define MDM_DCOFFDISB_POS      25
/// COMPDISB field bit
#define MDM_COMPDISB_BIT       ((uint32_t)0x01000000)
/// COMPDISB field position
#define MDM_COMPDISB_POS       24
/// EQDISB field bit
#define MDM_EQDISB_BIT         ((uint32_t)0x00800000)
/// EQDISB field position
#define MDM_EQDISB_POS         23
/// SPREADDISB field bit
#define MDM_SPREADDISB_BIT     ((uint32_t)0x00200000)
/// SPREADDISB field position
#define MDM_SPREADDISB_POS     21
/// SCRAMBDISB field bit
#define MDM_SCRAMBDISB_BIT     ((uint32_t)0x00100000)
/// SCRAMBDISB field position
#define MDM_SCRAMBDISB_POS     20
/// SFDERR field mask
#define MDM_SFDERR_MASK        ((uint32_t)0x00007000)
/// SFDERR field LSB position
#define MDM_SFDERR_LSB         12
/// SFDERR field width
#define MDM_SFDERR_WIDTH       ((uint32_t)0x00000003)
/// SFDLEN field mask
#define MDM_SFDLEN_MASK        ((uint32_t)0x00000700)
/// SFDLEN field LSB position
#define MDM_SFDLEN_LSB         8
/// SFDLEN field width
#define MDM_SFDLEN_WIDTH       ((uint32_t)0x00000003)
/// PREPRE field mask
#define MDM_PREPRE_MASK        ((uint32_t)0x0000003F)
/// PREPRE field LSB position
#define MDM_PREPRE_LSB         0
/// PREPRE field width
#define MDM_PREPRE_WIDTH       ((uint32_t)0x00000006)

/// IQMMDISB field reset value
#define MDM_IQMMDISB_RST       0x0
/// PRECOMPDISB field reset value
#define MDM_PRECOMPDISB_RST    0x0
/// DCOFFDISB field reset value
#define MDM_DCOFFDISB_RST      0x0
/// COMPDISB field reset value
#define MDM_COMPDISB_RST       0x0
/// EQDISB field reset value
#define MDM_EQDISB_RST         0x0
/// SPREADDISB field reset value
#define MDM_SPREADDISB_RST     0x0
/// SCRAMBDISB field reset value
#define MDM_SCRAMBDISB_RST     0x0
/// SFDERR field reset value
#define MDM_SFDERR_RST         0x0
/// SFDLEN field reset value
#define MDM_SFDLEN_RST         0x0
/// PREPRE field reset value
#define MDM_PREPRE_RST         0x0

/**
 * @brief Constructs a value for the MDMBCNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] iqmmdisb - The value to use for the IQMMDISB field.
 * @param[in] precompdisb - The value to use for the PRECOMPDISB field.
 * @param[in] dcoffdisb - The value to use for the DCOFFDISB field.
 * @param[in] compdisb - The value to use for the COMPDISB field.
 * @param[in] eqdisb - The value to use for the EQDISB field.
 * @param[in] spreaddisb - The value to use for the SPREADDISB field.
 * @param[in] scrambdisb - The value to use for the SCRAMBDISB field.
 * @param[in] sfderr - The value to use for the SFDERR field.
 * @param[in] sfdlen - The value to use for the SFDLEN field.
 * @param[in] prepre - The value to use for the PREPRE field.
 */
__INLINE void mdm_mdmbcntl_pack(uint8_t iqmmdisb, uint8_t precompdisb, uint8_t dcoffdisb, uint8_t compdisb, uint8_t eqdisb, uint8_t spreaddisb, uint8_t scrambdisb, uint8_t sfderr, uint8_t sfdlen, uint8_t prepre)
{
    ASSERT_ERR((((uint32_t)iqmmdisb << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)precompdisb << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)dcoffdisb << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)compdisb << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)eqdisb << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)spreaddisb << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)scrambdisb << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)sfderr << 12) & ~((uint32_t)0x00007000)) == 0);
    ASSERT_ERR((((uint32_t)sfdlen << 8) & ~((uint32_t)0x00000700)) == 0);
    ASSERT_ERR((((uint32_t)prepre << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(MDM_MDMBCNTL_ADDR,  ((uint32_t)iqmmdisb << 28) | ((uint32_t)precompdisb << 26) | ((uint32_t)dcoffdisb << 25) | ((uint32_t)compdisb << 24) | ((uint32_t)eqdisb << 23) | ((uint32_t)spreaddisb << 21) | ((uint32_t)scrambdisb << 20) | ((uint32_t)sfderr << 12) | ((uint32_t)sfdlen << 8) | ((uint32_t)prepre << 0));
}

/**
 * @brief Unpacks MDMBCNTL's fields from current value of the MDMBCNTL register.
 *
 * Reads the MDMBCNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] iqmmdisb - Will be populated with the current value of this field from the register.
 * @param[out] precompdisb - Will be populated with the current value of this field from the register.
 * @param[out] dcoffdisb - Will be populated with the current value of this field from the register.
 * @param[out] compdisb - Will be populated with the current value of this field from the register.
 * @param[out] eqdisb - Will be populated with the current value of this field from the register.
 * @param[out] spreaddisb - Will be populated with the current value of this field from the register.
 * @param[out] scrambdisb - Will be populated with the current value of this field from the register.
 * @param[out] sfderr - Will be populated with the current value of this field from the register.
 * @param[out] sfdlen - Will be populated with the current value of this field from the register.
 * @param[out] prepre - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mdmbcntl_unpack(uint8_t* iqmmdisb, uint8_t* precompdisb, uint8_t* dcoffdisb, uint8_t* compdisb, uint8_t* eqdisb, uint8_t* spreaddisb, uint8_t* scrambdisb, uint8_t* sfderr, uint8_t* sfdlen, uint8_t* prepre)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL_ADDR);

    *iqmmdisb = (localVal & ((uint32_t)0x10000000)) >> 28;
    *precompdisb = (localVal & ((uint32_t)0x04000000)) >> 26;
    *dcoffdisb = (localVal & ((uint32_t)0x02000000)) >> 25;
    *compdisb = (localVal & ((uint32_t)0x01000000)) >> 24;
    *eqdisb = (localVal & ((uint32_t)0x00800000)) >> 23;
    *spreaddisb = (localVal & ((uint32_t)0x00200000)) >> 21;
    *scrambdisb = (localVal & ((uint32_t)0x00100000)) >> 20;
    *sfderr = (localVal & ((uint32_t)0x00007000)) >> 12;
    *sfdlen = (localVal & ((uint32_t)0x00000700)) >> 8;
    *prepre = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the IQMMDISB field in the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read and the IQMMDISB field's value will be returned.
 *
 * @return The current value of the IQMMDISB field in the MDMBCNTL register.
 */
__INLINE uint8_t mdm_iqmmdisb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the IQMMDISB field of the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] iqmmdisb - The value to set the field to.
 */
__INLINE void mdm_iqmmdisb_setf(uint8_t iqmmdisb)
{
    ASSERT_ERR((((uint32_t)iqmmdisb << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(MDM_MDMBCNTL_ADDR, (REG_PL_RD(MDM_MDMBCNTL_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)iqmmdisb << 28));
}

/**
 * @brief Returns the current value of the PRECOMPDISB field in the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read and the PRECOMPDISB field's value will be returned.
 *
 * @return The current value of the PRECOMPDISB field in the MDMBCNTL register.
 */
__INLINE uint8_t mdm_precompdisb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the PRECOMPDISB field of the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] precompdisb - The value to set the field to.
 */
__INLINE void mdm_precompdisb_setf(uint8_t precompdisb)
{
    ASSERT_ERR((((uint32_t)precompdisb << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(MDM_MDMBCNTL_ADDR, (REG_PL_RD(MDM_MDMBCNTL_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)precompdisb << 26));
}

/**
 * @brief Returns the current value of the DCOFFDISB field in the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read and the DCOFFDISB field's value will be returned.
 *
 * @return The current value of the DCOFFDISB field in the MDMBCNTL register.
 */
__INLINE uint8_t mdm_dcoffdisb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the DCOFFDISB field of the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] dcoffdisb - The value to set the field to.
 */
__INLINE void mdm_dcoffdisb_setf(uint8_t dcoffdisb)
{
    ASSERT_ERR((((uint32_t)dcoffdisb << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(MDM_MDMBCNTL_ADDR, (REG_PL_RD(MDM_MDMBCNTL_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)dcoffdisb << 25));
}

/**
 * @brief Returns the current value of the COMPDISB field in the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read and the COMPDISB field's value will be returned.
 *
 * @return The current value of the COMPDISB field in the MDMBCNTL register.
 */
__INLINE uint8_t mdm_compdisb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the COMPDISB field of the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] compdisb - The value to set the field to.
 */
__INLINE void mdm_compdisb_setf(uint8_t compdisb)
{
    ASSERT_ERR((((uint32_t)compdisb << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(MDM_MDMBCNTL_ADDR, (REG_PL_RD(MDM_MDMBCNTL_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)compdisb << 24));
}

/**
 * @brief Returns the current value of the EQDISB field in the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read and the EQDISB field's value will be returned.
 *
 * @return The current value of the EQDISB field in the MDMBCNTL register.
 */
__INLINE uint8_t mdm_eqdisb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the EQDISB field of the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] eqdisb - The value to set the field to.
 */
__INLINE void mdm_eqdisb_setf(uint8_t eqdisb)
{
    ASSERT_ERR((((uint32_t)eqdisb << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(MDM_MDMBCNTL_ADDR, (REG_PL_RD(MDM_MDMBCNTL_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)eqdisb << 23));
}

/**
 * @brief Returns the current value of the SPREADDISB field in the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read and the SPREADDISB field's value will be returned.
 *
 * @return The current value of the SPREADDISB field in the MDMBCNTL register.
 */
__INLINE uint8_t mdm_spreaddisb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the SPREADDISB field of the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] spreaddisb - The value to set the field to.
 */
__INLINE void mdm_spreaddisb_setf(uint8_t spreaddisb)
{
    ASSERT_ERR((((uint32_t)spreaddisb << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(MDM_MDMBCNTL_ADDR, (REG_PL_RD(MDM_MDMBCNTL_ADDR) & ~((uint32_t)0x00200000)) | ((uint32_t)spreaddisb << 21));
}

/**
 * @brief Returns the current value of the SCRAMBDISB field in the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read and the SCRAMBDISB field's value will be returned.
 *
 * @return The current value of the SCRAMBDISB field in the MDMBCNTL register.
 */
__INLINE uint8_t mdm_scrambdisb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the SCRAMBDISB field of the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] scrambdisb - The value to set the field to.
 */
__INLINE void mdm_scrambdisb_setf(uint8_t scrambdisb)
{
    ASSERT_ERR((((uint32_t)scrambdisb << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(MDM_MDMBCNTL_ADDR, (REG_PL_RD(MDM_MDMBCNTL_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)scrambdisb << 20));
}

/**
 * @brief Returns the current value of the SFDERR field in the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read and the SFDERR field's value will be returned.
 *
 * @return The current value of the SFDERR field in the MDMBCNTL register.
 */
__INLINE uint8_t mdm_sfderr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

/**
 * @brief Sets the SFDERR field of the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sfderr - The value to set the field to.
 */
__INLINE void mdm_sfderr_setf(uint8_t sfderr)
{
    ASSERT_ERR((((uint32_t)sfderr << 12) & ~((uint32_t)0x00007000)) == 0);
    REG_PL_WR(MDM_MDMBCNTL_ADDR, (REG_PL_RD(MDM_MDMBCNTL_ADDR) & ~((uint32_t)0x00007000)) | ((uint32_t)sfderr << 12));
}

/**
 * @brief Returns the current value of the SFDLEN field in the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read and the SFDLEN field's value will be returned.
 *
 * @return The current value of the SFDLEN field in the MDMBCNTL register.
 */
__INLINE uint8_t mdm_sfdlen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

/**
 * @brief Sets the SFDLEN field of the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] sfdlen - The value to set the field to.
 */
__INLINE void mdm_sfdlen_setf(uint8_t sfdlen)
{
    ASSERT_ERR((((uint32_t)sfdlen << 8) & ~((uint32_t)0x00000700)) == 0);
    REG_PL_WR(MDM_MDMBCNTL_ADDR, (REG_PL_RD(MDM_MDMBCNTL_ADDR) & ~((uint32_t)0x00000700)) | ((uint32_t)sfdlen << 8));
}

/**
 * @brief Returns the current value of the PREPRE field in the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read and the PREPRE field's value will be returned.
 *
 * @return The current value of the PREPRE field in the MDMBCNTL register.
 */
__INLINE uint8_t mdm_prepre_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the PREPRE field of the MDMBCNTL register.
 *
 * The MDMBCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] prepre - The value to set the field to.
 */
__INLINE void mdm_prepre_setf(uint8_t prepre)
{
    ASSERT_ERR((((uint32_t)prepre << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(MDM_MDMBCNTL_ADDR, (REG_PL_RD(MDM_MDMBCNTL_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)prepre << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name MDMBPRMINIT register definitions
 * <table>
 * <caption>MDMBPRMINIT bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:20 <td>   RHO <td> R <td> R/W <td> 0x0
 * <tr><td> 17:16 <td>    MU <td> R <td> R/W <td> 0x1
 * <tr><td> 05:04 <td>  BETA <td> R <td> R/W <td> 0x2
 * <tr><td> 01:00 <td> ALPHA <td> R <td> R/W <td> 0x2
 * </table>
 *
 * @{
 */

/// Address of the MDMBPRMINIT register
#define MDM_MDMBPRMINIT_ADDR   0x24C00308
/// Offset of the MDMBPRMINIT register from the base address
#define MDM_MDMBPRMINIT_OFFSET 0x00000308
/// Index of the MDMBPRMINIT register
#define MDM_MDMBPRMINIT_INDEX  0x000000C2
/// Reset value of the MDMBPRMINIT register
#define MDM_MDMBPRMINIT_RESET  0x00010022

/**
 * @brief Returns the current value of the MDMBPRMINIT register.
 * The MDMBPRMINIT register will be read and its value returned.
 * @return The current value of the MDMBPRMINIT register.
 */
__INLINE uint32_t mdm_mdmbprminit_get(void)
{
    return REG_PL_RD(MDM_MDMBPRMINIT_ADDR);
}

/**
 * @brief Sets the MDMBPRMINIT register to a value.
 * The MDMBPRMINIT register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_mdmbprminit_set(uint32_t value)
{
    REG_PL_WR(MDM_MDMBPRMINIT_ADDR, value);
}

// field definitions
/// RHO field mask
#define MDM_RHO_MASK     ((uint32_t)0x00300000)
/// RHO field LSB position
#define MDM_RHO_LSB      20
/// RHO field width
#define MDM_RHO_WIDTH    ((uint32_t)0x00000002)
/// MU field mask
#define MDM_MU_MASK      ((uint32_t)0x00030000)
/// MU field LSB position
#define MDM_MU_LSB       16
/// MU field width
#define MDM_MU_WIDTH     ((uint32_t)0x00000002)
/// BETA field mask
#define MDM_BETA_MASK    ((uint32_t)0x00000030)
/// BETA field LSB position
#define MDM_BETA_LSB     4
/// BETA field width
#define MDM_BETA_WIDTH   ((uint32_t)0x00000002)
/// ALPHA field mask
#define MDM_ALPHA_MASK   ((uint32_t)0x00000003)
/// ALPHA field LSB position
#define MDM_ALPHA_LSB    0
/// ALPHA field width
#define MDM_ALPHA_WIDTH  ((uint32_t)0x00000002)

/// RHO field reset value
#define MDM_RHO_RST      0x0
/// MU field reset value
#define MDM_MU_RST       0x1
/// BETA field reset value
#define MDM_BETA_RST     0x2
/// ALPHA field reset value
#define MDM_ALPHA_RST    0x2

/**
 * @brief Constructs a value for the MDMBPRMINIT register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rho - The value to use for the RHO field.
 * @param[in] mu - The value to use for the MU field.
 * @param[in] beta - The value to use for the BETA field.
 * @param[in] alpha - The value to use for the ALPHA field.
 */
__INLINE void mdm_mdmbprminit_pack(uint8_t rho, uint8_t mu, uint8_t beta, uint8_t alpha)
{
    ASSERT_ERR((((uint32_t)rho << 20) & ~((uint32_t)0x00300000)) == 0);
    ASSERT_ERR((((uint32_t)mu << 16) & ~((uint32_t)0x00030000)) == 0);
    ASSERT_ERR((((uint32_t)beta << 4) & ~((uint32_t)0x00000030)) == 0);
    ASSERT_ERR((((uint32_t)alpha << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(MDM_MDMBPRMINIT_ADDR,  ((uint32_t)rho << 20) | ((uint32_t)mu << 16) | ((uint32_t)beta << 4) | ((uint32_t)alpha << 0));
}

/**
 * @brief Unpacks MDMBPRMINIT's fields from current value of the MDMBPRMINIT register.
 *
 * Reads the MDMBPRMINIT register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rho - Will be populated with the current value of this field from the register.
 * @param[out] mu - Will be populated with the current value of this field from the register.
 * @param[out] beta - Will be populated with the current value of this field from the register.
 * @param[out] alpha - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mdmbprminit_unpack(uint8_t* rho, uint8_t* mu, uint8_t* beta, uint8_t* alpha)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBPRMINIT_ADDR);

    *rho = (localVal & ((uint32_t)0x00300000)) >> 20;
    *mu = (localVal & ((uint32_t)0x00030000)) >> 16;
    *beta = (localVal & ((uint32_t)0x00000030)) >> 4;
    *alpha = (localVal & ((uint32_t)0x00000003)) >> 0;
}

/**
 * @brief Returns the current value of the RHO field in the MDMBPRMINIT register.
 *
 * The MDMBPRMINIT register will be read and the RHO field's value will be returned.
 *
 * @return The current value of the RHO field in the MDMBPRMINIT register.
 */
__INLINE uint8_t mdm_rho_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBPRMINIT_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

/**
 * @brief Sets the RHO field of the MDMBPRMINIT register.
 *
 * The MDMBPRMINIT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rho - The value to set the field to.
 */
__INLINE void mdm_rho_setf(uint8_t rho)
{
    ASSERT_ERR((((uint32_t)rho << 20) & ~((uint32_t)0x00300000)) == 0);
    REG_PL_WR(MDM_MDMBPRMINIT_ADDR, (REG_PL_RD(MDM_MDMBPRMINIT_ADDR) & ~((uint32_t)0x00300000)) | ((uint32_t)rho << 20));
}

/**
 * @brief Returns the current value of the MU field in the MDMBPRMINIT register.
 *
 * The MDMBPRMINIT register will be read and the MU field's value will be returned.
 *
 * @return The current value of the MU field in the MDMBPRMINIT register.
 */
__INLINE uint8_t mdm_mu_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBPRMINIT_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

/**
 * @brief Sets the MU field of the MDMBPRMINIT register.
 *
 * The MDMBPRMINIT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mu - The value to set the field to.
 */
__INLINE void mdm_mu_setf(uint8_t mu)
{
    ASSERT_ERR((((uint32_t)mu << 16) & ~((uint32_t)0x00030000)) == 0);
    REG_PL_WR(MDM_MDMBPRMINIT_ADDR, (REG_PL_RD(MDM_MDMBPRMINIT_ADDR) & ~((uint32_t)0x00030000)) | ((uint32_t)mu << 16));
}

/**
 * @brief Returns the current value of the BETA field in the MDMBPRMINIT register.
 *
 * The MDMBPRMINIT register will be read and the BETA field's value will be returned.
 *
 * @return The current value of the BETA field in the MDMBPRMINIT register.
 */
__INLINE uint8_t mdm_beta_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBPRMINIT_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

/**
 * @brief Sets the BETA field of the MDMBPRMINIT register.
 *
 * The MDMBPRMINIT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] beta - The value to set the field to.
 */
__INLINE void mdm_beta_setf(uint8_t beta)
{
    ASSERT_ERR((((uint32_t)beta << 4) & ~((uint32_t)0x00000030)) == 0);
    REG_PL_WR(MDM_MDMBPRMINIT_ADDR, (REG_PL_RD(MDM_MDMBPRMINIT_ADDR) & ~((uint32_t)0x00000030)) | ((uint32_t)beta << 4));
}

/**
 * @brief Returns the current value of the ALPHA field in the MDMBPRMINIT register.
 *
 * The MDMBPRMINIT register will be read and the ALPHA field's value will be returned.
 *
 * @return The current value of the ALPHA field in the MDMBPRMINIT register.
 */
__INLINE uint8_t mdm_alpha_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBPRMINIT_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

/**
 * @brief Sets the ALPHA field of the MDMBPRMINIT register.
 *
 * The MDMBPRMINIT register will be read, modified to contain the new field value, and written.
 *
 * @param[in] alpha - The value to set the field to.
 */
__INLINE void mdm_alpha_setf(uint8_t alpha)
{
    ASSERT_ERR((((uint32_t)alpha << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(MDM_MDMBPRMINIT_ADDR, (REG_PL_RD(MDM_MDMBPRMINIT_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)alpha << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name MDMBTALPHA register definitions
 * <table>
 * <caption>MDMBTALPHA bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:12 <td> TALPHA3 <td> R <td> R/W <td> 0x6
 * <tr><td> 11:08 <td> TALPHA2 <td> R <td> R/W <td> 0x2
 * <tr><td> 07:04 <td> TALPHA1 <td> R <td> R/W <td> 0x3
 * <tr><td> 03:00 <td> TALPHA0 <td> R <td> R/W <td> 0x6
 * </table>
 *
 * @{
 */

/// Address of the MDMBTALPHA register
#define MDM_MDMBTALPHA_ADDR   0x24C0030C
/// Offset of the MDMBTALPHA register from the base address
#define MDM_MDMBTALPHA_OFFSET 0x0000030C
/// Index of the MDMBTALPHA register
#define MDM_MDMBTALPHA_INDEX  0x000000C3
/// Reset value of the MDMBTALPHA register
#define MDM_MDMBTALPHA_RESET  0x00006236

/**
 * @brief Returns the current value of the MDMBTALPHA register.
 * The MDMBTALPHA register will be read and its value returned.
 * @return The current value of the MDMBTALPHA register.
 */
__INLINE uint32_t mdm_mdmbtalpha_get(void)
{
    return REG_PL_RD(MDM_MDMBTALPHA_ADDR);
}

/**
 * @brief Sets the MDMBTALPHA register to a value.
 * The MDMBTALPHA register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_mdmbtalpha_set(uint32_t value)
{
    REG_PL_WR(MDM_MDMBTALPHA_ADDR, value);
}

// field definitions
/// TALPHA3 field mask
#define MDM_TALPHA3_MASK   ((uint32_t)0x0000F000)
/// TALPHA3 field LSB position
#define MDM_TALPHA3_LSB    12
/// TALPHA3 field width
#define MDM_TALPHA3_WIDTH  ((uint32_t)0x00000004)
/// TALPHA2 field mask
#define MDM_TALPHA2_MASK   ((uint32_t)0x00000F00)
/// TALPHA2 field LSB position
#define MDM_TALPHA2_LSB    8
/// TALPHA2 field width
#define MDM_TALPHA2_WIDTH  ((uint32_t)0x00000004)
/// TALPHA1 field mask
#define MDM_TALPHA1_MASK   ((uint32_t)0x000000F0)
/// TALPHA1 field LSB position
#define MDM_TALPHA1_LSB    4
/// TALPHA1 field width
#define MDM_TALPHA1_WIDTH  ((uint32_t)0x00000004)
/// TALPHA0 field mask
#define MDM_TALPHA0_MASK   ((uint32_t)0x0000000F)
/// TALPHA0 field LSB position
#define MDM_TALPHA0_LSB    0
/// TALPHA0 field width
#define MDM_TALPHA0_WIDTH  ((uint32_t)0x00000004)

/// TALPHA3 field reset value
#define MDM_TALPHA3_RST    0x6
/// TALPHA2 field reset value
#define MDM_TALPHA2_RST    0x2
/// TALPHA1 field reset value
#define MDM_TALPHA1_RST    0x3
/// TALPHA0 field reset value
#define MDM_TALPHA0_RST    0x6

/**
 * @brief Constructs a value for the MDMBTALPHA register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] talpha3 - The value to use for the TALPHA3 field.
 * @param[in] talpha2 - The value to use for the TALPHA2 field.
 * @param[in] talpha1 - The value to use for the TALPHA1 field.
 * @param[in] talpha0 - The value to use for the TALPHA0 field.
 */
__INLINE void mdm_mdmbtalpha_pack(uint8_t talpha3, uint8_t talpha2, uint8_t talpha1, uint8_t talpha0)
{
    ASSERT_ERR((((uint32_t)talpha3 << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)talpha2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)talpha1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)talpha0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMBTALPHA_ADDR,  ((uint32_t)talpha3 << 12) | ((uint32_t)talpha2 << 8) | ((uint32_t)talpha1 << 4) | ((uint32_t)talpha0 << 0));
}

/**
 * @brief Unpacks MDMBTALPHA's fields from current value of the MDMBTALPHA register.
 *
 * Reads the MDMBTALPHA register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] talpha3 - Will be populated with the current value of this field from the register.
 * @param[out] talpha2 - Will be populated with the current value of this field from the register.
 * @param[out] talpha1 - Will be populated with the current value of this field from the register.
 * @param[out] talpha0 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mdmbtalpha_unpack(uint8_t* talpha3, uint8_t* talpha2, uint8_t* talpha1, uint8_t* talpha0)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTALPHA_ADDR);

    *talpha3 = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *talpha2 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *talpha1 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *talpha0 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the TALPHA3 field in the MDMBTALPHA register.
 *
 * The MDMBTALPHA register will be read and the TALPHA3 field's value will be returned.
 *
 * @return The current value of the TALPHA3 field in the MDMBTALPHA register.
 */
__INLINE uint8_t mdm_talpha3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTALPHA_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the TALPHA3 field of the MDMBTALPHA register.
 *
 * The MDMBTALPHA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] talpha3 - The value to set the field to.
 */
__INLINE void mdm_talpha3_setf(uint8_t talpha3)
{
    ASSERT_ERR((((uint32_t)talpha3 << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(MDM_MDMBTALPHA_ADDR, (REG_PL_RD(MDM_MDMBTALPHA_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)talpha3 << 12));
}

/**
 * @brief Returns the current value of the TALPHA2 field in the MDMBTALPHA register.
 *
 * The MDMBTALPHA register will be read and the TALPHA2 field's value will be returned.
 *
 * @return The current value of the TALPHA2 field in the MDMBTALPHA register.
 */
__INLINE uint8_t mdm_talpha2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTALPHA_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the TALPHA2 field of the MDMBTALPHA register.
 *
 * The MDMBTALPHA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] talpha2 - The value to set the field to.
 */
__INLINE void mdm_talpha2_setf(uint8_t talpha2)
{
    ASSERT_ERR((((uint32_t)talpha2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(MDM_MDMBTALPHA_ADDR, (REG_PL_RD(MDM_MDMBTALPHA_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)talpha2 << 8));
}

/**
 * @brief Returns the current value of the TALPHA1 field in the MDMBTALPHA register.
 *
 * The MDMBTALPHA register will be read and the TALPHA1 field's value will be returned.
 *
 * @return The current value of the TALPHA1 field in the MDMBTALPHA register.
 */
__INLINE uint8_t mdm_talpha1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTALPHA_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the TALPHA1 field of the MDMBTALPHA register.
 *
 * The MDMBTALPHA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] talpha1 - The value to set the field to.
 */
__INLINE void mdm_talpha1_setf(uint8_t talpha1)
{
    ASSERT_ERR((((uint32_t)talpha1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_MDMBTALPHA_ADDR, (REG_PL_RD(MDM_MDMBTALPHA_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)talpha1 << 4));
}

/**
 * @brief Returns the current value of the TALPHA0 field in the MDMBTALPHA register.
 *
 * The MDMBTALPHA register will be read and the TALPHA0 field's value will be returned.
 *
 * @return The current value of the TALPHA0 field in the MDMBTALPHA register.
 */
__INLINE uint8_t mdm_talpha0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTALPHA_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the TALPHA0 field of the MDMBTALPHA register.
 *
 * The MDMBTALPHA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] talpha0 - The value to set the field to.
 */
__INLINE void mdm_talpha0_setf(uint8_t talpha0)
{
    ASSERT_ERR((((uint32_t)talpha0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMBTALPHA_ADDR, (REG_PL_RD(MDM_MDMBTALPHA_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)talpha0 << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name MDMBTBETA register definitions
 * <table>
 * <caption>MDMBTBETA bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:12 <td> TBETA3 <td> R <td> R/W <td> 0x6
 * <tr><td> 11:08 <td> TBETA2 <td> R <td> R/W <td> 0x2
 * <tr><td> 07:04 <td> TBETA1 <td> R <td> R/W <td> 0x3
 * <tr><td> 03:00 <td> TBETA0 <td> R <td> R/W <td> 0x6
 * </table>
 *
 * @{
 */

/// Address of the MDMBTBETA register
#define MDM_MDMBTBETA_ADDR   0x24C00310
/// Offset of the MDMBTBETA register from the base address
#define MDM_MDMBTBETA_OFFSET 0x00000310
/// Index of the MDMBTBETA register
#define MDM_MDMBTBETA_INDEX  0x000000C4
/// Reset value of the MDMBTBETA register
#define MDM_MDMBTBETA_RESET  0x00006236

/**
 * @brief Returns the current value of the MDMBTBETA register.
 * The MDMBTBETA register will be read and its value returned.
 * @return The current value of the MDMBTBETA register.
 */
__INLINE uint32_t mdm_mdmbtbeta_get(void)
{
    return REG_PL_RD(MDM_MDMBTBETA_ADDR);
}

/**
 * @brief Sets the MDMBTBETA register to a value.
 * The MDMBTBETA register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_mdmbtbeta_set(uint32_t value)
{
    REG_PL_WR(MDM_MDMBTBETA_ADDR, value);
}

// field definitions
/// TBETA3 field mask
#define MDM_TBETA3_MASK   ((uint32_t)0x0000F000)
/// TBETA3 field LSB position
#define MDM_TBETA3_LSB    12
/// TBETA3 field width
#define MDM_TBETA3_WIDTH  ((uint32_t)0x00000004)
/// TBETA2 field mask
#define MDM_TBETA2_MASK   ((uint32_t)0x00000F00)
/// TBETA2 field LSB position
#define MDM_TBETA2_LSB    8
/// TBETA2 field width
#define MDM_TBETA2_WIDTH  ((uint32_t)0x00000004)
/// TBETA1 field mask
#define MDM_TBETA1_MASK   ((uint32_t)0x000000F0)
/// TBETA1 field LSB position
#define MDM_TBETA1_LSB    4
/// TBETA1 field width
#define MDM_TBETA1_WIDTH  ((uint32_t)0x00000004)
/// TBETA0 field mask
#define MDM_TBETA0_MASK   ((uint32_t)0x0000000F)
/// TBETA0 field LSB position
#define MDM_TBETA0_LSB    0
/// TBETA0 field width
#define MDM_TBETA0_WIDTH  ((uint32_t)0x00000004)

/// TBETA3 field reset value
#define MDM_TBETA3_RST    0x6
/// TBETA2 field reset value
#define MDM_TBETA2_RST    0x2
/// TBETA1 field reset value
#define MDM_TBETA1_RST    0x3
/// TBETA0 field reset value
#define MDM_TBETA0_RST    0x6

/**
 * @brief Constructs a value for the MDMBTBETA register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tbeta3 - The value to use for the TBETA3 field.
 * @param[in] tbeta2 - The value to use for the TBETA2 field.
 * @param[in] tbeta1 - The value to use for the TBETA1 field.
 * @param[in] tbeta0 - The value to use for the TBETA0 field.
 */
__INLINE void mdm_mdmbtbeta_pack(uint8_t tbeta3, uint8_t tbeta2, uint8_t tbeta1, uint8_t tbeta0)
{
    ASSERT_ERR((((uint32_t)tbeta3 << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)tbeta2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)tbeta1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)tbeta0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMBTBETA_ADDR,  ((uint32_t)tbeta3 << 12) | ((uint32_t)tbeta2 << 8) | ((uint32_t)tbeta1 << 4) | ((uint32_t)tbeta0 << 0));
}

/**
 * @brief Unpacks MDMBTBETA's fields from current value of the MDMBTBETA register.
 *
 * Reads the MDMBTBETA register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tbeta3 - Will be populated with the current value of this field from the register.
 * @param[out] tbeta2 - Will be populated with the current value of this field from the register.
 * @param[out] tbeta1 - Will be populated with the current value of this field from the register.
 * @param[out] tbeta0 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mdmbtbeta_unpack(uint8_t* tbeta3, uint8_t* tbeta2, uint8_t* tbeta1, uint8_t* tbeta0)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTBETA_ADDR);

    *tbeta3 = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *tbeta2 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *tbeta1 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *tbeta0 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the TBETA3 field in the MDMBTBETA register.
 *
 * The MDMBTBETA register will be read and the TBETA3 field's value will be returned.
 *
 * @return The current value of the TBETA3 field in the MDMBTBETA register.
 */
__INLINE uint8_t mdm_tbeta3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTBETA_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the TBETA3 field of the MDMBTBETA register.
 *
 * The MDMBTBETA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbeta3 - The value to set the field to.
 */
__INLINE void mdm_tbeta3_setf(uint8_t tbeta3)
{
    ASSERT_ERR((((uint32_t)tbeta3 << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(MDM_MDMBTBETA_ADDR, (REG_PL_RD(MDM_MDMBTBETA_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)tbeta3 << 12));
}

/**
 * @brief Returns the current value of the TBETA2 field in the MDMBTBETA register.
 *
 * The MDMBTBETA register will be read and the TBETA2 field's value will be returned.
 *
 * @return The current value of the TBETA2 field in the MDMBTBETA register.
 */
__INLINE uint8_t mdm_tbeta2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTBETA_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the TBETA2 field of the MDMBTBETA register.
 *
 * The MDMBTBETA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbeta2 - The value to set the field to.
 */
__INLINE void mdm_tbeta2_setf(uint8_t tbeta2)
{
    ASSERT_ERR((((uint32_t)tbeta2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(MDM_MDMBTBETA_ADDR, (REG_PL_RD(MDM_MDMBTBETA_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)tbeta2 << 8));
}

/**
 * @brief Returns the current value of the TBETA1 field in the MDMBTBETA register.
 *
 * The MDMBTBETA register will be read and the TBETA1 field's value will be returned.
 *
 * @return The current value of the TBETA1 field in the MDMBTBETA register.
 */
__INLINE uint8_t mdm_tbeta1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTBETA_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the TBETA1 field of the MDMBTBETA register.
 *
 * The MDMBTBETA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbeta1 - The value to set the field to.
 */
__INLINE void mdm_tbeta1_setf(uint8_t tbeta1)
{
    ASSERT_ERR((((uint32_t)tbeta1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_MDMBTBETA_ADDR, (REG_PL_RD(MDM_MDMBTBETA_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)tbeta1 << 4));
}

/**
 * @brief Returns the current value of the TBETA0 field in the MDMBTBETA register.
 *
 * The MDMBTBETA register will be read and the TBETA0 field's value will be returned.
 *
 * @return The current value of the TBETA0 field in the MDMBTBETA register.
 */
__INLINE uint8_t mdm_tbeta0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTBETA_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the TBETA0 field of the MDMBTBETA register.
 *
 * The MDMBTBETA register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbeta0 - The value to set the field to.
 */
__INLINE void mdm_tbeta0_setf(uint8_t tbeta0)
{
    ASSERT_ERR((((uint32_t)tbeta0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMBTBETA_ADDR, (REG_PL_RD(MDM_MDMBTBETA_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)tbeta0 << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name MDMBTMU register definitions
 * <table>
 * <caption>MDMBTMU bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:12 <td> TMU3 <td> R <td> R/W <td> 0x5
 * <tr><td> 11:08 <td> TMU2 <td> R <td> R/W <td> 0x5
 * <tr><td> 07:04 <td> TMU1 <td> R <td> R/W <td> 0x5
 * <tr><td> 03:00 <td> TMU0 <td> R <td> R/W <td> 0x5
 * </table>
 *
 * @{
 */

/// Address of the MDMBTMU register
#define MDM_MDMBTMU_ADDR   0x24C00314
/// Offset of the MDMBTMU register from the base address
#define MDM_MDMBTMU_OFFSET 0x00000314
/// Index of the MDMBTMU register
#define MDM_MDMBTMU_INDEX  0x000000C5
/// Reset value of the MDMBTMU register
#define MDM_MDMBTMU_RESET  0x00005555

/**
 * @brief Returns the current value of the MDMBTMU register.
 * The MDMBTMU register will be read and its value returned.
 * @return The current value of the MDMBTMU register.
 */
__INLINE uint32_t mdm_mdmbtmu_get(void)
{
    return REG_PL_RD(MDM_MDMBTMU_ADDR);
}

/**
 * @brief Sets the MDMBTMU register to a value.
 * The MDMBTMU register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_mdmbtmu_set(uint32_t value)
{
    REG_PL_WR(MDM_MDMBTMU_ADDR, value);
}

// field definitions
/// TMU3 field mask
#define MDM_TMU3_MASK   ((uint32_t)0x0000F000)
/// TMU3 field LSB position
#define MDM_TMU3_LSB    12
/// TMU3 field width
#define MDM_TMU3_WIDTH  ((uint32_t)0x00000004)
/// TMU2 field mask
#define MDM_TMU2_MASK   ((uint32_t)0x00000F00)
/// TMU2 field LSB position
#define MDM_TMU2_LSB    8
/// TMU2 field width
#define MDM_TMU2_WIDTH  ((uint32_t)0x00000004)
/// TMU1 field mask
#define MDM_TMU1_MASK   ((uint32_t)0x000000F0)
/// TMU1 field LSB position
#define MDM_TMU1_LSB    4
/// TMU1 field width
#define MDM_TMU1_WIDTH  ((uint32_t)0x00000004)
/// TMU0 field mask
#define MDM_TMU0_MASK   ((uint32_t)0x0000000F)
/// TMU0 field LSB position
#define MDM_TMU0_LSB    0
/// TMU0 field width
#define MDM_TMU0_WIDTH  ((uint32_t)0x00000004)

/// TMU3 field reset value
#define MDM_TMU3_RST    0x5
/// TMU2 field reset value
#define MDM_TMU2_RST    0x5
/// TMU1 field reset value
#define MDM_TMU1_RST    0x5
/// TMU0 field reset value
#define MDM_TMU0_RST    0x5

/**
 * @brief Constructs a value for the MDMBTMU register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tmu3 - The value to use for the TMU3 field.
 * @param[in] tmu2 - The value to use for the TMU2 field.
 * @param[in] tmu1 - The value to use for the TMU1 field.
 * @param[in] tmu0 - The value to use for the TMU0 field.
 */
__INLINE void mdm_mdmbtmu_pack(uint8_t tmu3, uint8_t tmu2, uint8_t tmu1, uint8_t tmu0)
{
    ASSERT_ERR((((uint32_t)tmu3 << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)tmu2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)tmu1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)tmu0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMBTMU_ADDR,  ((uint32_t)tmu3 << 12) | ((uint32_t)tmu2 << 8) | ((uint32_t)tmu1 << 4) | ((uint32_t)tmu0 << 0));
}

/**
 * @brief Unpacks MDMBTMU's fields from current value of the MDMBTMU register.
 *
 * Reads the MDMBTMU register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tmu3 - Will be populated with the current value of this field from the register.
 * @param[out] tmu2 - Will be populated with the current value of this field from the register.
 * @param[out] tmu1 - Will be populated with the current value of this field from the register.
 * @param[out] tmu0 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mdmbtmu_unpack(uint8_t* tmu3, uint8_t* tmu2, uint8_t* tmu1, uint8_t* tmu0)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTMU_ADDR);

    *tmu3 = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *tmu2 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *tmu1 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *tmu0 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the TMU3 field in the MDMBTMU register.
 *
 * The MDMBTMU register will be read and the TMU3 field's value will be returned.
 *
 * @return The current value of the TMU3 field in the MDMBTMU register.
 */
__INLINE uint8_t mdm_tmu3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTMU_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the TMU3 field of the MDMBTMU register.
 *
 * The MDMBTMU register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tmu3 - The value to set the field to.
 */
__INLINE void mdm_tmu3_setf(uint8_t tmu3)
{
    ASSERT_ERR((((uint32_t)tmu3 << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(MDM_MDMBTMU_ADDR, (REG_PL_RD(MDM_MDMBTMU_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)tmu3 << 12));
}

/**
 * @brief Returns the current value of the TMU2 field in the MDMBTMU register.
 *
 * The MDMBTMU register will be read and the TMU2 field's value will be returned.
 *
 * @return The current value of the TMU2 field in the MDMBTMU register.
 */
__INLINE uint8_t mdm_tmu2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTMU_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the TMU2 field of the MDMBTMU register.
 *
 * The MDMBTMU register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tmu2 - The value to set the field to.
 */
__INLINE void mdm_tmu2_setf(uint8_t tmu2)
{
    ASSERT_ERR((((uint32_t)tmu2 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(MDM_MDMBTMU_ADDR, (REG_PL_RD(MDM_MDMBTMU_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)tmu2 << 8));
}

/**
 * @brief Returns the current value of the TMU1 field in the MDMBTMU register.
 *
 * The MDMBTMU register will be read and the TMU1 field's value will be returned.
 *
 * @return The current value of the TMU1 field in the MDMBTMU register.
 */
__INLINE uint8_t mdm_tmu1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTMU_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the TMU1 field of the MDMBTMU register.
 *
 * The MDMBTMU register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tmu1 - The value to set the field to.
 */
__INLINE void mdm_tmu1_setf(uint8_t tmu1)
{
    ASSERT_ERR((((uint32_t)tmu1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_MDMBTMU_ADDR, (REG_PL_RD(MDM_MDMBTMU_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)tmu1 << 4));
}

/**
 * @brief Returns the current value of the TMU0 field in the MDMBTMU register.
 *
 * The MDMBTMU register will be read and the TMU0 field's value will be returned.
 *
 * @return The current value of the TMU0 field in the MDMBTMU register.
 */
__INLINE uint8_t mdm_tmu0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBTMU_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the TMU0 field of the MDMBTMU register.
 *
 * The MDMBTMU register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tmu0 - The value to set the field to.
 */
__INLINE void mdm_tmu0_setf(uint8_t tmu0)
{
    ASSERT_ERR((((uint32_t)tmu0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMBTMU_ADDR, (REG_PL_RD(MDM_MDMBTMU_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)tmu0 << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name MDMBCNTL1 register definitions
 * <table>
 * <caption>MDMBCNTL1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 12    <td>  RXLENCHKEN <td> R <td> R/W <td> 1
 * <tr><td> 11:00 <td> RXMAXLENGTH <td> R <td> R/W <td> 0x92A
 * </table>
 *
 * @{
 */

/// Address of the MDMBCNTL1 register
#define MDM_MDMBCNTL1_ADDR   0x24C00318
/// Offset of the MDMBCNTL1 register from the base address
#define MDM_MDMBCNTL1_OFFSET 0x00000318
/// Index of the MDMBCNTL1 register
#define MDM_MDMBCNTL1_INDEX  0x000000C6
/// Reset value of the MDMBCNTL1 register
#define MDM_MDMBCNTL1_RESET  0x0000192A

/**
 * @brief Returns the current value of the MDMBCNTL1 register.
 * The MDMBCNTL1 register will be read and its value returned.
 * @return The current value of the MDMBCNTL1 register.
 */
__INLINE uint32_t mdm_mdmbcntl1_get(void)
{
    return REG_PL_RD(MDM_MDMBCNTL1_ADDR);
}

/**
 * @brief Sets the MDMBCNTL1 register to a value.
 * The MDMBCNTL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_mdmbcntl1_set(uint32_t value)
{
    REG_PL_WR(MDM_MDMBCNTL1_ADDR, value);
}

// field definitions
/// RXLENCHKEN field bit
#define MDM_RXLENCHKEN_BIT     ((uint32_t)0x00001000)
/// RXLENCHKEN field position
#define MDM_RXLENCHKEN_POS     12
/// RXMAXLENGTH field mask
#define MDM_RXMAXLENGTH_MASK   ((uint32_t)0x00000FFF)
/// RXMAXLENGTH field LSB position
#define MDM_RXMAXLENGTH_LSB    0
/// RXMAXLENGTH field width
#define MDM_RXMAXLENGTH_WIDTH  ((uint32_t)0x0000000C)

/// RXLENCHKEN field reset value
#define MDM_RXLENCHKEN_RST     0x1
/// RXMAXLENGTH field reset value
#define MDM_RXMAXLENGTH_RST    0x92A

/**
 * @brief Constructs a value for the MDMBCNTL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxlenchken - The value to use for the RXLENCHKEN field.
 * @param[in] rxmaxlength - The value to use for the RXMAXLENGTH field.
 */
__INLINE void mdm_mdmbcntl1_pack(uint8_t rxlenchken, uint16_t rxmaxlength)
{
    ASSERT_ERR((((uint32_t)rxlenchken << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)rxmaxlength << 0) & ~((uint32_t)0x00000FFF)) == 0);
    REG_PL_WR(MDM_MDMBCNTL1_ADDR,  ((uint32_t)rxlenchken << 12) | ((uint32_t)rxmaxlength << 0));
}

/**
 * @brief Unpacks MDMBCNTL1's fields from current value of the MDMBCNTL1 register.
 *
 * Reads the MDMBCNTL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxlenchken - Will be populated with the current value of this field from the register.
 * @param[out] rxmaxlength - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mdmbcntl1_unpack(uint8_t* rxlenchken, uint16_t* rxmaxlength)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL1_ADDR);

    *rxlenchken = (localVal & ((uint32_t)0x00001000)) >> 12;
    *rxmaxlength = (localVal & ((uint32_t)0x00000FFF)) >> 0;
}

/**
 * @brief Returns the current value of the RXLENCHKEN field in the MDMBCNTL1 register.
 *
 * The MDMBCNTL1 register will be read and the RXLENCHKEN field's value will be returned.
 *
 * @return The current value of the RXLENCHKEN field in the MDMBCNTL1 register.
 */
__INLINE uint8_t mdm_rxlenchken_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

/**
 * @brief Sets the RXLENCHKEN field of the MDMBCNTL1 register.
 *
 * The MDMBCNTL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxlenchken - The value to set the field to.
 */
__INLINE void mdm_rxlenchken_setf(uint8_t rxlenchken)
{
    ASSERT_ERR((((uint32_t)rxlenchken << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_PL_WR(MDM_MDMBCNTL1_ADDR, (REG_PL_RD(MDM_MDMBCNTL1_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)rxlenchken << 12));
}

/**
 * @brief Returns the current value of the RXMAXLENGTH field in the MDMBCNTL1 register.
 *
 * The MDMBCNTL1 register will be read and the RXMAXLENGTH field's value will be returned.
 *
 * @return The current value of the RXMAXLENGTH field in the MDMBCNTL1 register.
 */
__INLINE uint16_t mdm_rxmaxlength_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00000FFF)) >> 0);
}

/**
 * @brief Sets the RXMAXLENGTH field of the MDMBCNTL1 register.
 *
 * The MDMBCNTL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxmaxlength - The value to set the field to.
 */
__INLINE void mdm_rxmaxlength_setf(uint16_t rxmaxlength)
{
    ASSERT_ERR((((uint32_t)rxmaxlength << 0) & ~((uint32_t)0x00000FFF)) == 0);
    REG_PL_WR(MDM_MDMBCNTL1_ADDR, (REG_PL_RD(MDM_MDMBCNTL1_ADDR) & ~((uint32_t)0x00000FFF)) | ((uint32_t)rxmaxlength << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name MDMBRFCNTL register definitions
 * <table>
 * <caption>MDMBRFCNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 07:00 <td> TXENDDEL <td> R <td> R/W <td> 0x30
 * </table>
 *
 * @{
 */

/// Address of the MDMBRFCNTL register
#define MDM_MDMBRFCNTL_ADDR   0x24C0031C
/// Offset of the MDMBRFCNTL register from the base address
#define MDM_MDMBRFCNTL_OFFSET 0x0000031C
/// Index of the MDMBRFCNTL register
#define MDM_MDMBRFCNTL_INDEX  0x000000C7
/// Reset value of the MDMBRFCNTL register
#define MDM_MDMBRFCNTL_RESET  0x00000030

/**
 * @brief Returns the current value of the MDMBRFCNTL register.
 * The MDMBRFCNTL register will be read and its value returned.
 * @return The current value of the MDMBRFCNTL register.
 */
__INLINE uint32_t mdm_mdmbrfcntl_get(void)
{
    return REG_PL_RD(MDM_MDMBRFCNTL_ADDR);
}

/**
 * @brief Sets the MDMBRFCNTL register to a value.
 * The MDMBRFCNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_mdmbrfcntl_set(uint32_t value)
{
    REG_PL_WR(MDM_MDMBRFCNTL_ADDR, value);
}

// field definitions
/// TXENDDEL field mask
#define MDM_TXENDDEL_MASK   ((uint32_t)0x000000FF)
/// TXENDDEL field LSB position
#define MDM_TXENDDEL_LSB    0
/// TXENDDEL field width
#define MDM_TXENDDEL_WIDTH  ((uint32_t)0x00000008)

/// TXENDDEL field reset value
#define MDM_TXENDDEL_RST    0x30

/**
 * @brief Returns the current value of the TXENDDEL field in the MDMBRFCNTL register.
 *
 * The MDMBRFCNTL register will be read and the TXENDDEL field's value will be returned.
 *
 * @return The current value of the TXENDDEL field in the MDMBRFCNTL register.
 */
__INLINE uint8_t mdm_txenddel_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBRFCNTL_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the TXENDDEL field of the MDMBRFCNTL register.
 *
 * The MDMBRFCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txenddel - The value to set the field to.
 */
__INLINE void mdm_txenddel_setf(uint8_t txenddel)
{
    ASSERT_ERR((((uint32_t)txenddel << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_MDMBRFCNTL_ADDR, (uint32_t)txenddel << 0);
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name MDMBEQCNTL register definitions
 * <table>
 * <caption>MDMBEQCNTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 27:16 <td>   EQHOLD <td> R <td> R/W <td> 0xFFF
 * <tr><td> 14:10 <td> COMPTIME <td> R <td> R/W <td> 0x0
 * <tr><td> 09:05 <td>  ESTTIME <td> R <td> R/W <td> 0x0
 * <tr><td> 03:00 <td>   EQTIME <td> R <td> R/W <td> 0x1
 * </table>
 *
 * @{
 */

/// Address of the MDMBEQCNTL register
#define MDM_MDMBEQCNTL_ADDR   0x24C00320
/// Offset of the MDMBEQCNTL register from the base address
#define MDM_MDMBEQCNTL_OFFSET 0x00000320
/// Index of the MDMBEQCNTL register
#define MDM_MDMBEQCNTL_INDEX  0x000000C8
/// Reset value of the MDMBEQCNTL register
#define MDM_MDMBEQCNTL_RESET  0x0FFF0001

/**
 * @brief Returns the current value of the MDMBEQCNTL register.
 * The MDMBEQCNTL register will be read and its value returned.
 * @return The current value of the MDMBEQCNTL register.
 */
__INLINE uint32_t mdm_mdmbeqcntl_get(void)
{
    return REG_PL_RD(MDM_MDMBEQCNTL_ADDR);
}

/**
 * @brief Sets the MDMBEQCNTL register to a value.
 * The MDMBEQCNTL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_mdmbeqcntl_set(uint32_t value)
{
    REG_PL_WR(MDM_MDMBEQCNTL_ADDR, value);
}

// field definitions
/// EQHOLD field mask
#define MDM_EQHOLD_MASK     ((uint32_t)0x0FFF0000)
/// EQHOLD field LSB position
#define MDM_EQHOLD_LSB      16
/// EQHOLD field width
#define MDM_EQHOLD_WIDTH    ((uint32_t)0x0000000C)
/// COMPTIME field mask
#define MDM_COMPTIME_MASK   ((uint32_t)0x00007C00)
/// COMPTIME field LSB position
#define MDM_COMPTIME_LSB    10
/// COMPTIME field width
#define MDM_COMPTIME_WIDTH  ((uint32_t)0x00000005)
/// ESTTIME field mask
#define MDM_ESTTIME_MASK    ((uint32_t)0x000003E0)
/// ESTTIME field LSB position
#define MDM_ESTTIME_LSB     5
/// ESTTIME field width
#define MDM_ESTTIME_WIDTH   ((uint32_t)0x00000005)
/// EQTIME field mask
#define MDM_EQTIME_MASK     ((uint32_t)0x0000000F)
/// EQTIME field LSB position
#define MDM_EQTIME_LSB      0
/// EQTIME field width
#define MDM_EQTIME_WIDTH    ((uint32_t)0x00000004)

/// EQHOLD field reset value
#define MDM_EQHOLD_RST      0xFFF
/// COMPTIME field reset value
#define MDM_COMPTIME_RST    0x0
/// ESTTIME field reset value
#define MDM_ESTTIME_RST     0x0
/// EQTIME field reset value
#define MDM_EQTIME_RST      0x1

/**
 * @brief Constructs a value for the MDMBEQCNTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] eqhold - The value to use for the EQHOLD field.
 * @param[in] comptime - The value to use for the COMPTIME field.
 * @param[in] esttime - The value to use for the ESTTIME field.
 * @param[in] eqtime - The value to use for the EQTIME field.
 */
__INLINE void mdm_mdmbeqcntl_pack(uint16_t eqhold, uint8_t comptime, uint8_t esttime, uint8_t eqtime)
{
    ASSERT_ERR((((uint32_t)eqhold << 16) & ~((uint32_t)0x0FFF0000)) == 0);
    ASSERT_ERR((((uint32_t)comptime << 10) & ~((uint32_t)0x00007C00)) == 0);
    ASSERT_ERR((((uint32_t)esttime << 5) & ~((uint32_t)0x000003E0)) == 0);
    ASSERT_ERR((((uint32_t)eqtime << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMBEQCNTL_ADDR,  ((uint32_t)eqhold << 16) | ((uint32_t)comptime << 10) | ((uint32_t)esttime << 5) | ((uint32_t)eqtime << 0));
}

/**
 * @brief Unpacks MDMBEQCNTL's fields from current value of the MDMBEQCNTL register.
 *
 * Reads the MDMBEQCNTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] eqhold - Will be populated with the current value of this field from the register.
 * @param[out] comptime - Will be populated with the current value of this field from the register.
 * @param[out] esttime - Will be populated with the current value of this field from the register.
 * @param[out] eqtime - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mdmbeqcntl_unpack(uint16_t* eqhold, uint8_t* comptime, uint8_t* esttime, uint8_t* eqtime)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBEQCNTL_ADDR);

    *eqhold = (localVal & ((uint32_t)0x0FFF0000)) >> 16;
    *comptime = (localVal & ((uint32_t)0x00007C00)) >> 10;
    *esttime = (localVal & ((uint32_t)0x000003E0)) >> 5;
    *eqtime = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the EQHOLD field in the MDMBEQCNTL register.
 *
 * The MDMBEQCNTL register will be read and the EQHOLD field's value will be returned.
 *
 * @return The current value of the EQHOLD field in the MDMBEQCNTL register.
 */
__INLINE uint16_t mdm_eqhold_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBEQCNTL_ADDR);
    return ((localVal & ((uint32_t)0x0FFF0000)) >> 16);
}

/**
 * @brief Sets the EQHOLD field of the MDMBEQCNTL register.
 *
 * The MDMBEQCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] eqhold - The value to set the field to.
 */
__INLINE void mdm_eqhold_setf(uint16_t eqhold)
{
    ASSERT_ERR((((uint32_t)eqhold << 16) & ~((uint32_t)0x0FFF0000)) == 0);
    REG_PL_WR(MDM_MDMBEQCNTL_ADDR, (REG_PL_RD(MDM_MDMBEQCNTL_ADDR) & ~((uint32_t)0x0FFF0000)) | ((uint32_t)eqhold << 16));
}

/**
 * @brief Returns the current value of the COMPTIME field in the MDMBEQCNTL register.
 *
 * The MDMBEQCNTL register will be read and the COMPTIME field's value will be returned.
 *
 * @return The current value of the COMPTIME field in the MDMBEQCNTL register.
 */
__INLINE uint8_t mdm_comptime_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBEQCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00007C00)) >> 10);
}

/**
 * @brief Sets the COMPTIME field of the MDMBEQCNTL register.
 *
 * The MDMBEQCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] comptime - The value to set the field to.
 */
__INLINE void mdm_comptime_setf(uint8_t comptime)
{
    ASSERT_ERR((((uint32_t)comptime << 10) & ~((uint32_t)0x00007C00)) == 0);
    REG_PL_WR(MDM_MDMBEQCNTL_ADDR, (REG_PL_RD(MDM_MDMBEQCNTL_ADDR) & ~((uint32_t)0x00007C00)) | ((uint32_t)comptime << 10));
}

/**
 * @brief Returns the current value of the ESTTIME field in the MDMBEQCNTL register.
 *
 * The MDMBEQCNTL register will be read and the ESTTIME field's value will be returned.
 *
 * @return The current value of the ESTTIME field in the MDMBEQCNTL register.
 */
__INLINE uint8_t mdm_esttime_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBEQCNTL_ADDR);
    return ((localVal & ((uint32_t)0x000003E0)) >> 5);
}

/**
 * @brief Sets the ESTTIME field of the MDMBEQCNTL register.
 *
 * The MDMBEQCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] esttime - The value to set the field to.
 */
__INLINE void mdm_esttime_setf(uint8_t esttime)
{
    ASSERT_ERR((((uint32_t)esttime << 5) & ~((uint32_t)0x000003E0)) == 0);
    REG_PL_WR(MDM_MDMBEQCNTL_ADDR, (REG_PL_RD(MDM_MDMBEQCNTL_ADDR) & ~((uint32_t)0x000003E0)) | ((uint32_t)esttime << 5));
}

/**
 * @brief Returns the current value of the EQTIME field in the MDMBEQCNTL register.
 *
 * The MDMBEQCNTL register will be read and the EQTIME field's value will be returned.
 *
 * @return The current value of the EQTIME field in the MDMBEQCNTL register.
 */
__INLINE uint8_t mdm_eqtime_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBEQCNTL_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the EQTIME field of the MDMBEQCNTL register.
 *
 * The MDMBEQCNTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] eqtime - The value to set the field to.
 */
__INLINE void mdm_eqtime_setf(uint8_t eqtime)
{
    ASSERT_ERR((((uint32_t)eqtime << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMBEQCNTL_ADDR, (REG_PL_RD(MDM_MDMBEQCNTL_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)eqtime << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name MDMBCNTL2 register definitions
 * <table>
 * <caption>MDMBCNTL2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:16 <td>  PRECOMP <td> R <td> R/W <td> 0x38
 * <tr><td> 13:08 <td> SYNCTIME <td> R <td> R/W <td> 0x12
 * <tr><td> 03:00 <td> LOOPTIME <td> R <td> R/W <td> 0x5
 * </table>
 *
 * @{
 */

/// Address of the MDMBCNTL2 register
#define MDM_MDMBCNTL2_ADDR   0x24C00324
/// Offset of the MDMBCNTL2 register from the base address
#define MDM_MDMBCNTL2_OFFSET 0x00000324
/// Index of the MDMBCNTL2 register
#define MDM_MDMBCNTL2_INDEX  0x000000C9
/// Reset value of the MDMBCNTL2 register
#define MDM_MDMBCNTL2_RESET  0x00381205

/**
 * @brief Returns the current value of the MDMBCNTL2 register.
 * The MDMBCNTL2 register will be read and its value returned.
 * @return The current value of the MDMBCNTL2 register.
 */
__INLINE uint32_t mdm_mdmbcntl2_get(void)
{
    return REG_PL_RD(MDM_MDMBCNTL2_ADDR);
}

/**
 * @brief Sets the MDMBCNTL2 register to a value.
 * The MDMBCNTL2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_mdmbcntl2_set(uint32_t value)
{
    REG_PL_WR(MDM_MDMBCNTL2_ADDR, value);
}

// field definitions
/// PRECOMP field mask
#define MDM_PRECOMP_MASK    ((uint32_t)0x003F0000)
/// PRECOMP field LSB position
#define MDM_PRECOMP_LSB     16
/// PRECOMP field width
#define MDM_PRECOMP_WIDTH   ((uint32_t)0x00000006)
/// SYNCTIME field mask
#define MDM_SYNCTIME_MASK   ((uint32_t)0x00003F00)
/// SYNCTIME field LSB position
#define MDM_SYNCTIME_LSB    8
/// SYNCTIME field width
#define MDM_SYNCTIME_WIDTH  ((uint32_t)0x00000006)
/// LOOPTIME field mask
#define MDM_LOOPTIME_MASK   ((uint32_t)0x0000000F)
/// LOOPTIME field LSB position
#define MDM_LOOPTIME_LSB    0
/// LOOPTIME field width
#define MDM_LOOPTIME_WIDTH  ((uint32_t)0x00000004)

/// PRECOMP field reset value
#define MDM_PRECOMP_RST     0x38
/// SYNCTIME field reset value
#define MDM_SYNCTIME_RST    0x12
/// LOOPTIME field reset value
#define MDM_LOOPTIME_RST    0x5

/**
 * @brief Constructs a value for the MDMBCNTL2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] precomp - The value to use for the PRECOMP field.
 * @param[in] synctime - The value to use for the SYNCTIME field.
 * @param[in] looptime - The value to use for the LOOPTIME field.
 */
__INLINE void mdm_mdmbcntl2_pack(uint8_t precomp, uint8_t synctime, uint8_t looptime)
{
    ASSERT_ERR((((uint32_t)precomp << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)synctime << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)looptime << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMBCNTL2_ADDR,  ((uint32_t)precomp << 16) | ((uint32_t)synctime << 8) | ((uint32_t)looptime << 0));
}

/**
 * @brief Unpacks MDMBCNTL2's fields from current value of the MDMBCNTL2 register.
 *
 * Reads the MDMBCNTL2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] precomp - Will be populated with the current value of this field from the register.
 * @param[out] synctime - Will be populated with the current value of this field from the register.
 * @param[out] looptime - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mdmbcntl2_unpack(uint8_t* precomp, uint8_t* synctime, uint8_t* looptime)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL2_ADDR);

    *precomp = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *synctime = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *looptime = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the PRECOMP field in the MDMBCNTL2 register.
 *
 * The MDMBCNTL2 register will be read and the PRECOMP field's value will be returned.
 *
 * @return The current value of the PRECOMP field in the MDMBCNTL2 register.
 */
__INLINE uint8_t mdm_precomp_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL2_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the PRECOMP field of the MDMBCNTL2 register.
 *
 * The MDMBCNTL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] precomp - The value to set the field to.
 */
__INLINE void mdm_precomp_setf(uint8_t precomp)
{
    ASSERT_ERR((((uint32_t)precomp << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(MDM_MDMBCNTL2_ADDR, (REG_PL_RD(MDM_MDMBCNTL2_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)precomp << 16));
}

/**
 * @brief Returns the current value of the SYNCTIME field in the MDMBCNTL2 register.
 *
 * The MDMBCNTL2 register will be read and the SYNCTIME field's value will be returned.
 *
 * @return The current value of the SYNCTIME field in the MDMBCNTL2 register.
 */
__INLINE uint8_t mdm_synctime_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL2_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the SYNCTIME field of the MDMBCNTL2 register.
 *
 * The MDMBCNTL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] synctime - The value to set the field to.
 */
__INLINE void mdm_synctime_setf(uint8_t synctime)
{
    ASSERT_ERR((((uint32_t)synctime << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(MDM_MDMBCNTL2_ADDR, (REG_PL_RD(MDM_MDMBCNTL2_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)synctime << 8));
}

/**
 * @brief Returns the current value of the LOOPTIME field in the MDMBCNTL2 register.
 *
 * The MDMBCNTL2 register will be read and the LOOPTIME field's value will be returned.
 *
 * @return The current value of the LOOPTIME field in the MDMBCNTL2 register.
 */
__INLINE uint8_t mdm_looptime_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBCNTL2_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the LOOPTIME field of the MDMBCNTL2 register.
 *
 * The MDMBCNTL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] looptime - The value to set the field to.
 */
__INLINE void mdm_looptime_setf(uint8_t looptime)
{
    ASSERT_ERR((((uint32_t)looptime << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMBCNTL2_ADDR, (REG_PL_RD(MDM_MDMBCNTL2_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)looptime << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name MDMBSTAT0 register definitions
 * <table>
 * <caption>MDMBSTAT0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:24 <td>    EQSUMQ <td> W <td> R <td> 0x0
 * <tr><td> 23:16 <td>    EQSUMI <td> W <td> R <td> 0x0
 * <tr><td> 13:08 <td> DCOFFSETQ <td> W <td> R <td> 0x0
 * <tr><td> 05:00 <td> DCOFFSETI <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the MDMBSTAT0 register
#define MDM_MDMBSTAT0_ADDR   0x24C00328
/// Offset of the MDMBSTAT0 register from the base address
#define MDM_MDMBSTAT0_OFFSET 0x00000328
/// Index of the MDMBSTAT0 register
#define MDM_MDMBSTAT0_INDEX  0x000000CA
/// Reset value of the MDMBSTAT0 register
#define MDM_MDMBSTAT0_RESET  0x00000000

/**
 * @brief Returns the current value of the MDMBSTAT0 register.
 * The MDMBSTAT0 register will be read and its value returned.
 * @return The current value of the MDMBSTAT0 register.
 */
__INLINE uint32_t mdm_mdmbstat0_get(void)
{
    return REG_PL_RD(MDM_MDMBSTAT0_ADDR);
}

// field definitions
/// EQSUMQ field mask
#define MDM_EQSUMQ_MASK      ((uint32_t)0xFF000000)
/// EQSUMQ field LSB position
#define MDM_EQSUMQ_LSB       24
/// EQSUMQ field width
#define MDM_EQSUMQ_WIDTH     ((uint32_t)0x00000008)
/// EQSUMI field mask
#define MDM_EQSUMI_MASK      ((uint32_t)0x00FF0000)
/// EQSUMI field LSB position
#define MDM_EQSUMI_LSB       16
/// EQSUMI field width
#define MDM_EQSUMI_WIDTH     ((uint32_t)0x00000008)
/// DCOFFSETQ field mask
#define MDM_DCOFFSETQ_MASK   ((uint32_t)0x00003F00)
/// DCOFFSETQ field LSB position
#define MDM_DCOFFSETQ_LSB    8
/// DCOFFSETQ field width
#define MDM_DCOFFSETQ_WIDTH  ((uint32_t)0x00000006)
/// DCOFFSETI field mask
#define MDM_DCOFFSETI_MASK   ((uint32_t)0x0000003F)
/// DCOFFSETI field LSB position
#define MDM_DCOFFSETI_LSB    0
/// DCOFFSETI field width
#define MDM_DCOFFSETI_WIDTH  ((uint32_t)0x00000006)

/// EQSUMQ field reset value
#define MDM_EQSUMQ_RST       0x0
/// EQSUMI field reset value
#define MDM_EQSUMI_RST       0x0
/// DCOFFSETQ field reset value
#define MDM_DCOFFSETQ_RST    0x0
/// DCOFFSETI field reset value
#define MDM_DCOFFSETI_RST    0x0

/**
 * @brief Unpacks MDMBSTAT0's fields from current value of the MDMBSTAT0 register.
 *
 * Reads the MDMBSTAT0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] eqsumq - Will be populated with the current value of this field from the register.
 * @param[out] eqsumi - Will be populated with the current value of this field from the register.
 * @param[out] dcoffsetq - Will be populated with the current value of this field from the register.
 * @param[out] dcoffseti - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mdmbstat0_unpack(uint8_t* eqsumq, uint8_t* eqsumi, uint8_t* dcoffsetq, uint8_t* dcoffseti)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBSTAT0_ADDR);

    *eqsumq = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *eqsumi = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *dcoffsetq = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *dcoffseti = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the EQSUMQ field in the MDMBSTAT0 register.
 *
 * The MDMBSTAT0 register will be read and the EQSUMQ field's value will be returned.
 *
 * @return The current value of the EQSUMQ field in the MDMBSTAT0 register.
 */
__INLINE uint8_t mdm_eqsumq_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBSTAT0_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Returns the current value of the EQSUMI field in the MDMBSTAT0 register.
 *
 * The MDMBSTAT0 register will be read and the EQSUMI field's value will be returned.
 *
 * @return The current value of the EQSUMI field in the MDMBSTAT0 register.
 */
__INLINE uint8_t mdm_eqsumi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBSTAT0_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Returns the current value of the DCOFFSETQ field in the MDMBSTAT0 register.
 *
 * The MDMBSTAT0 register will be read and the DCOFFSETQ field's value will be returned.
 *
 * @return The current value of the DCOFFSETQ field in the MDMBSTAT0 register.
 */
__INLINE uint8_t mdm_dcoffsetq_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBSTAT0_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Returns the current value of the DCOFFSETI field in the MDMBSTAT0 register.
 *
 * The MDMBSTAT0 register will be read and the DCOFFSETI field's value will be returned.
 *
 * @return The current value of the DCOFFSETI field in the MDMBSTAT0 register.
 */
__INLINE uint8_t mdm_dcoffseti_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBSTAT0_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name MDMBSTAT1 register definitions
 * <table>
 * <caption>MDMBSTAT1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:30 <td>  RXERRORSTAT <td> W <td> R <td> 0x0
 * <tr><td> 28:16 <td>          EVM <td> W <td> R <td> 0x0
 * <tr><td> 14:08 <td>  IQGAINESTIM <td> W <td> R <td> 0x0
 * <tr><td> 07:00 <td> FREQOFFESTIM <td> W <td> R <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the MDMBSTAT1 register
#define MDM_MDMBSTAT1_ADDR   0x24C0032C
/// Offset of the MDMBSTAT1 register from the base address
#define MDM_MDMBSTAT1_OFFSET 0x0000032C
/// Index of the MDMBSTAT1 register
#define MDM_MDMBSTAT1_INDEX  0x000000CB
/// Reset value of the MDMBSTAT1 register
#define MDM_MDMBSTAT1_RESET  0x00000000

/**
 * @brief Returns the current value of the MDMBSTAT1 register.
 * The MDMBSTAT1 register will be read and its value returned.
 * @return The current value of the MDMBSTAT1 register.
 */
__INLINE uint32_t mdm_mdmbstat1_get(void)
{
    return REG_PL_RD(MDM_MDMBSTAT1_ADDR);
}

// field definitions
/// RXERRORSTAT field mask
#define MDM_RXERRORSTAT_MASK    ((uint32_t)0xC0000000)
/// RXERRORSTAT field LSB position
#define MDM_RXERRORSTAT_LSB     30
/// RXERRORSTAT field width
#define MDM_RXERRORSTAT_WIDTH   ((uint32_t)0x00000002)
/// EVM field mask
#define MDM_EVM_MASK            ((uint32_t)0x1FFF0000)
/// EVM field LSB position
#define MDM_EVM_LSB             16
/// EVM field width
#define MDM_EVM_WIDTH           ((uint32_t)0x0000000D)
/// IQGAINESTIM field mask
#define MDM_IQGAINESTIM_MASK    ((uint32_t)0x00007F00)
/// IQGAINESTIM field LSB position
#define MDM_IQGAINESTIM_LSB     8
/// IQGAINESTIM field width
#define MDM_IQGAINESTIM_WIDTH   ((uint32_t)0x00000007)
/// FREQOFFESTIM field mask
#define MDM_FREQOFFESTIM_MASK   ((uint32_t)0x000000FF)
/// FREQOFFESTIM field LSB position
#define MDM_FREQOFFESTIM_LSB    0
/// FREQOFFESTIM field width
#define MDM_FREQOFFESTIM_WIDTH  ((uint32_t)0x00000008)

/// RXERRORSTAT field reset value
#define MDM_RXERRORSTAT_RST     0x0
/// EVM field reset value
#define MDM_EVM_RST             0x0
/// IQGAINESTIM field reset value
#define MDM_IQGAINESTIM_RST     0x0
/// FREQOFFESTIM field reset value
#define MDM_FREQOFFESTIM_RST    0x0

/**
 * @brief Unpacks MDMBSTAT1's fields from current value of the MDMBSTAT1 register.
 *
 * Reads the MDMBSTAT1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxerrorstat - Will be populated with the current value of this field from the register.
 * @param[out] evm - Will be populated with the current value of this field from the register.
 * @param[out] iqgainestim - Will be populated with the current value of this field from the register.
 * @param[out] freqoffestim - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mdmbstat1_unpack(uint8_t* rxerrorstat, uint16_t* evm, uint8_t* iqgainestim, uint8_t* freqoffestim)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBSTAT1_ADDR);

    *rxerrorstat = (localVal & ((uint32_t)0xC0000000)) >> 30;
    *evm = (localVal & ((uint32_t)0x1FFF0000)) >> 16;
    *iqgainestim = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *freqoffestim = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the RXERRORSTAT field in the MDMBSTAT1 register.
 *
 * The MDMBSTAT1 register will be read and the RXERRORSTAT field's value will be returned.
 *
 * @return The current value of the RXERRORSTAT field in the MDMBSTAT1 register.
 */
__INLINE uint8_t mdm_rxerrorstat_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBSTAT1_ADDR);
    return ((localVal & ((uint32_t)0xC0000000)) >> 30);
}

/**
 * @brief Returns the current value of the EVM field in the MDMBSTAT1 register.
 *
 * The MDMBSTAT1 register will be read and the EVM field's value will be returned.
 *
 * @return The current value of the EVM field in the MDMBSTAT1 register.
 */
__INLINE uint16_t mdm_evm_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x1FFF0000)) >> 16);
}

/**
 * @brief Returns the current value of the IQGAINESTIM field in the MDMBSTAT1 register.
 *
 * The MDMBSTAT1 register will be read and the IQGAINESTIM field's value will be returned.
 *
 * @return The current value of the IQGAINESTIM field in the MDMBSTAT1 register.
 */
__INLINE uint8_t mdm_iqgainestim_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Returns the current value of the FREQOFFESTIM field in the MDMBSTAT1 register.
 *
 * The MDMBSTAT1 register will be read and the FREQOFFESTIM field's value will be returned.
 *
 * @return The current value of the FREQOFFESTIM field in the MDMBSTAT1 register.
 */
__INLINE uint8_t mdm_freqoffestim_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMBSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name FEBFIRCOEF03DOWNTO00 register definitions
 * <table>
 * <caption>FEBFIRCOEF03DOWNTO00 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 30:24 <td> FIRCOEF3 <td> R <td> R/W <td> 0x0
 * <tr><td> 22:16 <td> FIRCOEF2 <td> R <td> R/W <td> 0x0
 * <tr><td> 14:08 <td> FIRCOEF1 <td> R <td> R/W <td> 0x0
 * <tr><td> 06:00 <td> FIRCOEF0 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FEBFIRCOEF03DOWNTO00 register
#define MDM_FEBFIRCOEF03DOWNTO00_ADDR   0x24C00330
/// Offset of the FEBFIRCOEF03DOWNTO00 register from the base address
#define MDM_FEBFIRCOEF03DOWNTO00_OFFSET 0x00000330
/// Index of the FEBFIRCOEF03DOWNTO00 register
#define MDM_FEBFIRCOEF03DOWNTO00_INDEX  0x000000CC
/// Reset value of the FEBFIRCOEF03DOWNTO00 register
#define MDM_FEBFIRCOEF03DOWNTO00_RESET  0x00000000

/**
 * @brief Returns the current value of the FEBFIRCOEF03DOWNTO00 register.
 * The FEBFIRCOEF03DOWNTO00 register will be read and its value returned.
 * @return The current value of the FEBFIRCOEF03DOWNTO00 register.
 */
__INLINE uint32_t mdm_febfircoef03downto00_get(void)
{
    return REG_PL_RD(MDM_FEBFIRCOEF03DOWNTO00_ADDR);
}

/**
 * @brief Sets the FEBFIRCOEF03DOWNTO00 register to a value.
 * The FEBFIRCOEF03DOWNTO00 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_febfircoef03downto00_set(uint32_t value)
{
    REG_PL_WR(MDM_FEBFIRCOEF03DOWNTO00_ADDR, value);
}

// field definitions
/// FIRCOEF3 field mask
#define MDM_FIRCOEF3_MASK   ((uint32_t)0x7F000000)
/// FIRCOEF3 field LSB position
#define MDM_FIRCOEF3_LSB    24
/// FIRCOEF3 field width
#define MDM_FIRCOEF3_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF2 field mask
#define MDM_FIRCOEF2_MASK   ((uint32_t)0x007F0000)
/// FIRCOEF2 field LSB position
#define MDM_FIRCOEF2_LSB    16
/// FIRCOEF2 field width
#define MDM_FIRCOEF2_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF1 field mask
#define MDM_FIRCOEF1_MASK   ((uint32_t)0x00007F00)
/// FIRCOEF1 field LSB position
#define MDM_FIRCOEF1_LSB    8
/// FIRCOEF1 field width
#define MDM_FIRCOEF1_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF0 field mask
#define MDM_FIRCOEF0_MASK   ((uint32_t)0x0000007F)
/// FIRCOEF0 field LSB position
#define MDM_FIRCOEF0_LSB    0
/// FIRCOEF0 field width
#define MDM_FIRCOEF0_WIDTH  ((uint32_t)0x00000007)

/// FIRCOEF3 field reset value
#define MDM_FIRCOEF3_RST    0x0
/// FIRCOEF2 field reset value
#define MDM_FIRCOEF2_RST    0x0
/// FIRCOEF1 field reset value
#define MDM_FIRCOEF1_RST    0x0
/// FIRCOEF0 field reset value
#define MDM_FIRCOEF0_RST    0x0

/**
 * @brief Constructs a value for the FEBFIRCOEF03DOWNTO00 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] fircoef3 - The value to use for the FIRCOEF3 field.
 * @param[in] fircoef2 - The value to use for the FIRCOEF2 field.
 * @param[in] fircoef1 - The value to use for the FIRCOEF1 field.
 * @param[in] fircoef0 - The value to use for the FIRCOEF0 field.
 */
__INLINE void mdm_febfircoef03downto00_pack(uint8_t fircoef3, uint8_t fircoef2, uint8_t fircoef1, uint8_t fircoef0)
{
    ASSERT_ERR((((uint32_t)fircoef3 << 24) & ~((uint32_t)0x7F000000)) == 0);
    ASSERT_ERR((((uint32_t)fircoef2 << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)fircoef1 << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)fircoef0 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF03DOWNTO00_ADDR,  ((uint32_t)fircoef3 << 24) | ((uint32_t)fircoef2 << 16) | ((uint32_t)fircoef1 << 8) | ((uint32_t)fircoef0 << 0));
}

/**
 * @brief Unpacks FEBFIRCOEF03DOWNTO00's fields from current value of the FEBFIRCOEF03DOWNTO00 register.
 *
 * Reads the FEBFIRCOEF03DOWNTO00 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] fircoef3 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef2 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef1 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef0 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_febfircoef03downto00_unpack(uint8_t* fircoef3, uint8_t* fircoef2, uint8_t* fircoef1, uint8_t* fircoef0)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF03DOWNTO00_ADDR);

    *fircoef3 = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *fircoef2 = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *fircoef1 = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *fircoef0 = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the FIRCOEF3 field in the FEBFIRCOEF03DOWNTO00 register.
 *
 * The FEBFIRCOEF03DOWNTO00 register will be read and the FIRCOEF3 field's value will be returned.
 *
 * @return The current value of the FIRCOEF3 field in the FEBFIRCOEF03DOWNTO00 register.
 */
__INLINE uint8_t mdm_fircoef3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF03DOWNTO00_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

/**
 * @brief Sets the FIRCOEF3 field of the FEBFIRCOEF03DOWNTO00 register.
 *
 * The FEBFIRCOEF03DOWNTO00 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef3 - The value to set the field to.
 */
__INLINE void mdm_fircoef3_setf(uint8_t fircoef3)
{
    ASSERT_ERR((((uint32_t)fircoef3 << 24) & ~((uint32_t)0x7F000000)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF03DOWNTO00_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF03DOWNTO00_ADDR) & ~((uint32_t)0x7F000000)) | ((uint32_t)fircoef3 << 24));
}

/**
 * @brief Returns the current value of the FIRCOEF2 field in the FEBFIRCOEF03DOWNTO00 register.
 *
 * The FEBFIRCOEF03DOWNTO00 register will be read and the FIRCOEF2 field's value will be returned.
 *
 * @return The current value of the FIRCOEF2 field in the FEBFIRCOEF03DOWNTO00 register.
 */
__INLINE uint8_t mdm_fircoef2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF03DOWNTO00_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the FIRCOEF2 field of the FEBFIRCOEF03DOWNTO00 register.
 *
 * The FEBFIRCOEF03DOWNTO00 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef2 - The value to set the field to.
 */
__INLINE void mdm_fircoef2_setf(uint8_t fircoef2)
{
    ASSERT_ERR((((uint32_t)fircoef2 << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF03DOWNTO00_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF03DOWNTO00_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)fircoef2 << 16));
}

/**
 * @brief Returns the current value of the FIRCOEF1 field in the FEBFIRCOEF03DOWNTO00 register.
 *
 * The FEBFIRCOEF03DOWNTO00 register will be read and the FIRCOEF1 field's value will be returned.
 *
 * @return The current value of the FIRCOEF1 field in the FEBFIRCOEF03DOWNTO00 register.
 */
__INLINE uint8_t mdm_fircoef1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF03DOWNTO00_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the FIRCOEF1 field of the FEBFIRCOEF03DOWNTO00 register.
 *
 * The FEBFIRCOEF03DOWNTO00 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef1 - The value to set the field to.
 */
__INLINE void mdm_fircoef1_setf(uint8_t fircoef1)
{
    ASSERT_ERR((((uint32_t)fircoef1 << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF03DOWNTO00_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF03DOWNTO00_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)fircoef1 << 8));
}

/**
 * @brief Returns the current value of the FIRCOEF0 field in the FEBFIRCOEF03DOWNTO00 register.
 *
 * The FEBFIRCOEF03DOWNTO00 register will be read and the FIRCOEF0 field's value will be returned.
 *
 * @return The current value of the FIRCOEF0 field in the FEBFIRCOEF03DOWNTO00 register.
 */
__INLINE uint8_t mdm_fircoef0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF03DOWNTO00_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the FIRCOEF0 field of the FEBFIRCOEF03DOWNTO00 register.
 *
 * The FEBFIRCOEF03DOWNTO00 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef0 - The value to set the field to.
 */
__INLINE void mdm_fircoef0_setf(uint8_t fircoef0)
{
    ASSERT_ERR((((uint32_t)fircoef0 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF03DOWNTO00_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF03DOWNTO00_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)fircoef0 << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name FEBFIRCOEF07DOWNTO04 register definitions
 * <table>
 * <caption>FEBFIRCOEF07DOWNTO04 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 30:24 <td> FIRCOEF7 <td> R <td> R/W <td> 0xA
 * <tr><td> 22:16 <td> FIRCOEF6 <td> R <td> R/W <td> 0x2
 * <tr><td> 14:08 <td> FIRCOEF5 <td> R <td> R/W <td> 0x7F
 * <tr><td> 06:00 <td> FIRCOEF4 <td> R <td> R/W <td> 0x7F
 * </table>
 *
 * @{
 */

/// Address of the FEBFIRCOEF07DOWNTO04 register
#define MDM_FEBFIRCOEF07DOWNTO04_ADDR   0x24C00334
/// Offset of the FEBFIRCOEF07DOWNTO04 register from the base address
#define MDM_FEBFIRCOEF07DOWNTO04_OFFSET 0x00000334
/// Index of the FEBFIRCOEF07DOWNTO04 register
#define MDM_FEBFIRCOEF07DOWNTO04_INDEX  0x000000CD
/// Reset value of the FEBFIRCOEF07DOWNTO04 register
#define MDM_FEBFIRCOEF07DOWNTO04_RESET  0x0A027F7F

/**
 * @brief Returns the current value of the FEBFIRCOEF07DOWNTO04 register.
 * The FEBFIRCOEF07DOWNTO04 register will be read and its value returned.
 * @return The current value of the FEBFIRCOEF07DOWNTO04 register.
 */
__INLINE uint32_t mdm_febfircoef07downto04_get(void)
{
    return REG_PL_RD(MDM_FEBFIRCOEF07DOWNTO04_ADDR);
}

/**
 * @brief Sets the FEBFIRCOEF07DOWNTO04 register to a value.
 * The FEBFIRCOEF07DOWNTO04 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_febfircoef07downto04_set(uint32_t value)
{
    REG_PL_WR(MDM_FEBFIRCOEF07DOWNTO04_ADDR, value);
}

// field definitions
/// FIRCOEF7 field mask
#define MDM_FIRCOEF7_MASK   ((uint32_t)0x7F000000)
/// FIRCOEF7 field LSB position
#define MDM_FIRCOEF7_LSB    24
/// FIRCOEF7 field width
#define MDM_FIRCOEF7_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF6 field mask
#define MDM_FIRCOEF6_MASK   ((uint32_t)0x007F0000)
/// FIRCOEF6 field LSB position
#define MDM_FIRCOEF6_LSB    16
/// FIRCOEF6 field width
#define MDM_FIRCOEF6_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF5 field mask
#define MDM_FIRCOEF5_MASK   ((uint32_t)0x00007F00)
/// FIRCOEF5 field LSB position
#define MDM_FIRCOEF5_LSB    8
/// FIRCOEF5 field width
#define MDM_FIRCOEF5_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF4 field mask
#define MDM_FIRCOEF4_MASK   ((uint32_t)0x0000007F)
/// FIRCOEF4 field LSB position
#define MDM_FIRCOEF4_LSB    0
/// FIRCOEF4 field width
#define MDM_FIRCOEF4_WIDTH  ((uint32_t)0x00000007)

/// FIRCOEF7 field reset value
#define MDM_FIRCOEF7_RST    0xA
/// FIRCOEF6 field reset value
#define MDM_FIRCOEF6_RST    0x2
/// FIRCOEF5 field reset value
#define MDM_FIRCOEF5_RST    0x7F
/// FIRCOEF4 field reset value
#define MDM_FIRCOEF4_RST    0x7F

/**
 * @brief Constructs a value for the FEBFIRCOEF07DOWNTO04 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] fircoef7 - The value to use for the FIRCOEF7 field.
 * @param[in] fircoef6 - The value to use for the FIRCOEF6 field.
 * @param[in] fircoef5 - The value to use for the FIRCOEF5 field.
 * @param[in] fircoef4 - The value to use for the FIRCOEF4 field.
 */
__INLINE void mdm_febfircoef07downto04_pack(uint8_t fircoef7, uint8_t fircoef6, uint8_t fircoef5, uint8_t fircoef4)
{
    ASSERT_ERR((((uint32_t)fircoef7 << 24) & ~((uint32_t)0x7F000000)) == 0);
    ASSERT_ERR((((uint32_t)fircoef6 << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)fircoef5 << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)fircoef4 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF07DOWNTO04_ADDR,  ((uint32_t)fircoef7 << 24) | ((uint32_t)fircoef6 << 16) | ((uint32_t)fircoef5 << 8) | ((uint32_t)fircoef4 << 0));
}

/**
 * @brief Unpacks FEBFIRCOEF07DOWNTO04's fields from current value of the FEBFIRCOEF07DOWNTO04 register.
 *
 * Reads the FEBFIRCOEF07DOWNTO04 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] fircoef7 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef6 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef5 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef4 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_febfircoef07downto04_unpack(uint8_t* fircoef7, uint8_t* fircoef6, uint8_t* fircoef5, uint8_t* fircoef4)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF07DOWNTO04_ADDR);

    *fircoef7 = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *fircoef6 = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *fircoef5 = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *fircoef4 = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the FIRCOEF7 field in the FEBFIRCOEF07DOWNTO04 register.
 *
 * The FEBFIRCOEF07DOWNTO04 register will be read and the FIRCOEF7 field's value will be returned.
 *
 * @return The current value of the FIRCOEF7 field in the FEBFIRCOEF07DOWNTO04 register.
 */
__INLINE uint8_t mdm_fircoef7_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF07DOWNTO04_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

/**
 * @brief Sets the FIRCOEF7 field of the FEBFIRCOEF07DOWNTO04 register.
 *
 * The FEBFIRCOEF07DOWNTO04 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef7 - The value to set the field to.
 */
__INLINE void mdm_fircoef7_setf(uint8_t fircoef7)
{
    ASSERT_ERR((((uint32_t)fircoef7 << 24) & ~((uint32_t)0x7F000000)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF07DOWNTO04_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF07DOWNTO04_ADDR) & ~((uint32_t)0x7F000000)) | ((uint32_t)fircoef7 << 24));
}

/**
 * @brief Returns the current value of the FIRCOEF6 field in the FEBFIRCOEF07DOWNTO04 register.
 *
 * The FEBFIRCOEF07DOWNTO04 register will be read and the FIRCOEF6 field's value will be returned.
 *
 * @return The current value of the FIRCOEF6 field in the FEBFIRCOEF07DOWNTO04 register.
 */
__INLINE uint8_t mdm_fircoef6_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF07DOWNTO04_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the FIRCOEF6 field of the FEBFIRCOEF07DOWNTO04 register.
 *
 * The FEBFIRCOEF07DOWNTO04 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef6 - The value to set the field to.
 */
__INLINE void mdm_fircoef6_setf(uint8_t fircoef6)
{
    ASSERT_ERR((((uint32_t)fircoef6 << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF07DOWNTO04_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF07DOWNTO04_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)fircoef6 << 16));
}

/**
 * @brief Returns the current value of the FIRCOEF5 field in the FEBFIRCOEF07DOWNTO04 register.
 *
 * The FEBFIRCOEF07DOWNTO04 register will be read and the FIRCOEF5 field's value will be returned.
 *
 * @return The current value of the FIRCOEF5 field in the FEBFIRCOEF07DOWNTO04 register.
 */
__INLINE uint8_t mdm_fircoef5_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF07DOWNTO04_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the FIRCOEF5 field of the FEBFIRCOEF07DOWNTO04 register.
 *
 * The FEBFIRCOEF07DOWNTO04 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef5 - The value to set the field to.
 */
__INLINE void mdm_fircoef5_setf(uint8_t fircoef5)
{
    ASSERT_ERR((((uint32_t)fircoef5 << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF07DOWNTO04_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF07DOWNTO04_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)fircoef5 << 8));
}

/**
 * @brief Returns the current value of the FIRCOEF4 field in the FEBFIRCOEF07DOWNTO04 register.
 *
 * The FEBFIRCOEF07DOWNTO04 register will be read and the FIRCOEF4 field's value will be returned.
 *
 * @return The current value of the FIRCOEF4 field in the FEBFIRCOEF07DOWNTO04 register.
 */
__INLINE uint8_t mdm_fircoef4_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF07DOWNTO04_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the FIRCOEF4 field of the FEBFIRCOEF07DOWNTO04 register.
 *
 * The FEBFIRCOEF07DOWNTO04 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef4 - The value to set the field to.
 */
__INLINE void mdm_fircoef4_setf(uint8_t fircoef4)
{
    ASSERT_ERR((((uint32_t)fircoef4 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF07DOWNTO04_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF07DOWNTO04_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)fircoef4 << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name FEBFIRCOEF11DOWNTO08 register definitions
 * <table>
 * <caption>FEBFIRCOEF11DOWNTO08 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 30:24 <td> FIRCOEF11 <td> R <td> R/W <td> 0x23
 * <tr><td> 22:16 <td> FIRCOEF10 <td> R <td> R/W <td> 0x28
 * <tr><td> 14:08 <td>  FIRCOEF9 <td> R <td> R/W <td> 0x23
 * <tr><td> 06:00 <td>  FIRCOEF8 <td> R <td> R/W <td> 0x17
 * </table>
 *
 * @{
 */

/// Address of the FEBFIRCOEF11DOWNTO08 register
#define MDM_FEBFIRCOEF11DOWNTO08_ADDR   0x24C00338
/// Offset of the FEBFIRCOEF11DOWNTO08 register from the base address
#define MDM_FEBFIRCOEF11DOWNTO08_OFFSET 0x00000338
/// Index of the FEBFIRCOEF11DOWNTO08 register
#define MDM_FEBFIRCOEF11DOWNTO08_INDEX  0x000000CE
/// Reset value of the FEBFIRCOEF11DOWNTO08 register
#define MDM_FEBFIRCOEF11DOWNTO08_RESET  0x23282317

/**
 * @brief Returns the current value of the FEBFIRCOEF11DOWNTO08 register.
 * The FEBFIRCOEF11DOWNTO08 register will be read and its value returned.
 * @return The current value of the FEBFIRCOEF11DOWNTO08 register.
 */
__INLINE uint32_t mdm_febfircoef11downto08_get(void)
{
    return REG_PL_RD(MDM_FEBFIRCOEF11DOWNTO08_ADDR);
}

/**
 * @brief Sets the FEBFIRCOEF11DOWNTO08 register to a value.
 * The FEBFIRCOEF11DOWNTO08 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_febfircoef11downto08_set(uint32_t value)
{
    REG_PL_WR(MDM_FEBFIRCOEF11DOWNTO08_ADDR, value);
}

// field definitions
/// FIRCOEF11 field mask
#define MDM_FIRCOEF11_MASK   ((uint32_t)0x7F000000)
/// FIRCOEF11 field LSB position
#define MDM_FIRCOEF11_LSB    24
/// FIRCOEF11 field width
#define MDM_FIRCOEF11_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF10 field mask
#define MDM_FIRCOEF10_MASK   ((uint32_t)0x007F0000)
/// FIRCOEF10 field LSB position
#define MDM_FIRCOEF10_LSB    16
/// FIRCOEF10 field width
#define MDM_FIRCOEF10_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF9 field mask
#define MDM_FIRCOEF9_MASK    ((uint32_t)0x00007F00)
/// FIRCOEF9 field LSB position
#define MDM_FIRCOEF9_LSB     8
/// FIRCOEF9 field width
#define MDM_FIRCOEF9_WIDTH   ((uint32_t)0x00000007)
/// FIRCOEF8 field mask
#define MDM_FIRCOEF8_MASK    ((uint32_t)0x0000007F)
/// FIRCOEF8 field LSB position
#define MDM_FIRCOEF8_LSB     0
/// FIRCOEF8 field width
#define MDM_FIRCOEF8_WIDTH   ((uint32_t)0x00000007)

/// FIRCOEF11 field reset value
#define MDM_FIRCOEF11_RST    0x23
/// FIRCOEF10 field reset value
#define MDM_FIRCOEF10_RST    0x28
/// FIRCOEF9 field reset value
#define MDM_FIRCOEF9_RST     0x23
/// FIRCOEF8 field reset value
#define MDM_FIRCOEF8_RST     0x17

/**
 * @brief Constructs a value for the FEBFIRCOEF11DOWNTO08 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] fircoef11 - The value to use for the FIRCOEF11 field.
 * @param[in] fircoef10 - The value to use for the FIRCOEF10 field.
 * @param[in] fircoef9 - The value to use for the FIRCOEF9 field.
 * @param[in] fircoef8 - The value to use for the FIRCOEF8 field.
 */
__INLINE void mdm_febfircoef11downto08_pack(uint8_t fircoef11, uint8_t fircoef10, uint8_t fircoef9, uint8_t fircoef8)
{
    ASSERT_ERR((((uint32_t)fircoef11 << 24) & ~((uint32_t)0x7F000000)) == 0);
    ASSERT_ERR((((uint32_t)fircoef10 << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)fircoef9 << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)fircoef8 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF11DOWNTO08_ADDR,  ((uint32_t)fircoef11 << 24) | ((uint32_t)fircoef10 << 16) | ((uint32_t)fircoef9 << 8) | ((uint32_t)fircoef8 << 0));
}

/**
 * @brief Unpacks FEBFIRCOEF11DOWNTO08's fields from current value of the FEBFIRCOEF11DOWNTO08 register.
 *
 * Reads the FEBFIRCOEF11DOWNTO08 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] fircoef11 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef10 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef9 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef8 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_febfircoef11downto08_unpack(uint8_t* fircoef11, uint8_t* fircoef10, uint8_t* fircoef9, uint8_t* fircoef8)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF11DOWNTO08_ADDR);

    *fircoef11 = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *fircoef10 = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *fircoef9 = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *fircoef8 = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the FIRCOEF11 field in the FEBFIRCOEF11DOWNTO08 register.
 *
 * The FEBFIRCOEF11DOWNTO08 register will be read and the FIRCOEF11 field's value will be returned.
 *
 * @return The current value of the FIRCOEF11 field in the FEBFIRCOEF11DOWNTO08 register.
 */
__INLINE uint8_t mdm_fircoef11_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF11DOWNTO08_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

/**
 * @brief Sets the FIRCOEF11 field of the FEBFIRCOEF11DOWNTO08 register.
 *
 * The FEBFIRCOEF11DOWNTO08 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef11 - The value to set the field to.
 */
__INLINE void mdm_fircoef11_setf(uint8_t fircoef11)
{
    ASSERT_ERR((((uint32_t)fircoef11 << 24) & ~((uint32_t)0x7F000000)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF11DOWNTO08_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF11DOWNTO08_ADDR) & ~((uint32_t)0x7F000000)) | ((uint32_t)fircoef11 << 24));
}

/**
 * @brief Returns the current value of the FIRCOEF10 field in the FEBFIRCOEF11DOWNTO08 register.
 *
 * The FEBFIRCOEF11DOWNTO08 register will be read and the FIRCOEF10 field's value will be returned.
 *
 * @return The current value of the FIRCOEF10 field in the FEBFIRCOEF11DOWNTO08 register.
 */
__INLINE uint8_t mdm_fircoef10_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF11DOWNTO08_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the FIRCOEF10 field of the FEBFIRCOEF11DOWNTO08 register.
 *
 * The FEBFIRCOEF11DOWNTO08 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef10 - The value to set the field to.
 */
__INLINE void mdm_fircoef10_setf(uint8_t fircoef10)
{
    ASSERT_ERR((((uint32_t)fircoef10 << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF11DOWNTO08_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF11DOWNTO08_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)fircoef10 << 16));
}

/**
 * @brief Returns the current value of the FIRCOEF9 field in the FEBFIRCOEF11DOWNTO08 register.
 *
 * The FEBFIRCOEF11DOWNTO08 register will be read and the FIRCOEF9 field's value will be returned.
 *
 * @return The current value of the FIRCOEF9 field in the FEBFIRCOEF11DOWNTO08 register.
 */
__INLINE uint8_t mdm_fircoef9_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF11DOWNTO08_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the FIRCOEF9 field of the FEBFIRCOEF11DOWNTO08 register.
 *
 * The FEBFIRCOEF11DOWNTO08 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef9 - The value to set the field to.
 */
__INLINE void mdm_fircoef9_setf(uint8_t fircoef9)
{
    ASSERT_ERR((((uint32_t)fircoef9 << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF11DOWNTO08_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF11DOWNTO08_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)fircoef9 << 8));
}

/**
 * @brief Returns the current value of the FIRCOEF8 field in the FEBFIRCOEF11DOWNTO08 register.
 *
 * The FEBFIRCOEF11DOWNTO08 register will be read and the FIRCOEF8 field's value will be returned.
 *
 * @return The current value of the FIRCOEF8 field in the FEBFIRCOEF11DOWNTO08 register.
 */
__INLINE uint8_t mdm_fircoef8_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF11DOWNTO08_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the FIRCOEF8 field of the FEBFIRCOEF11DOWNTO08 register.
 *
 * The FEBFIRCOEF11DOWNTO08 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef8 - The value to set the field to.
 */
__INLINE void mdm_fircoef8_setf(uint8_t fircoef8)
{
    ASSERT_ERR((((uint32_t)fircoef8 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF11DOWNTO08_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF11DOWNTO08_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)fircoef8 << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name FEBFIRCOEF15DOWNTO12 register definitions
 * <table>
 * <caption>FEBFIRCOEF15DOWNTO12 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 30:24 <td> FIRCOEF15 <td> R <td> R/W <td> 0x7F
 * <tr><td> 22:16 <td> FIRCOEF14 <td> R <td> R/W <td> 0x2
 * <tr><td> 14:08 <td> FIRCOEF13 <td> R <td> R/W <td> 0xA
 * <tr><td> 06:00 <td> FIRCOEF12 <td> R <td> R/W <td> 0x17
 * </table>
 *
 * @{
 */

/// Address of the FEBFIRCOEF15DOWNTO12 register
#define MDM_FEBFIRCOEF15DOWNTO12_ADDR   0x24C0033C
/// Offset of the FEBFIRCOEF15DOWNTO12 register from the base address
#define MDM_FEBFIRCOEF15DOWNTO12_OFFSET 0x0000033C
/// Index of the FEBFIRCOEF15DOWNTO12 register
#define MDM_FEBFIRCOEF15DOWNTO12_INDEX  0x000000CF
/// Reset value of the FEBFIRCOEF15DOWNTO12 register
#define MDM_FEBFIRCOEF15DOWNTO12_RESET  0x7F020A17

/**
 * @brief Returns the current value of the FEBFIRCOEF15DOWNTO12 register.
 * The FEBFIRCOEF15DOWNTO12 register will be read and its value returned.
 * @return The current value of the FEBFIRCOEF15DOWNTO12 register.
 */
__INLINE uint32_t mdm_febfircoef15downto12_get(void)
{
    return REG_PL_RD(MDM_FEBFIRCOEF15DOWNTO12_ADDR);
}

/**
 * @brief Sets the FEBFIRCOEF15DOWNTO12 register to a value.
 * The FEBFIRCOEF15DOWNTO12 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_febfircoef15downto12_set(uint32_t value)
{
    REG_PL_WR(MDM_FEBFIRCOEF15DOWNTO12_ADDR, value);
}

// field definitions
/// FIRCOEF15 field mask
#define MDM_FIRCOEF15_MASK   ((uint32_t)0x7F000000)
/// FIRCOEF15 field LSB position
#define MDM_FIRCOEF15_LSB    24
/// FIRCOEF15 field width
#define MDM_FIRCOEF15_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF14 field mask
#define MDM_FIRCOEF14_MASK   ((uint32_t)0x007F0000)
/// FIRCOEF14 field LSB position
#define MDM_FIRCOEF14_LSB    16
/// FIRCOEF14 field width
#define MDM_FIRCOEF14_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF13 field mask
#define MDM_FIRCOEF13_MASK   ((uint32_t)0x00007F00)
/// FIRCOEF13 field LSB position
#define MDM_FIRCOEF13_LSB    8
/// FIRCOEF13 field width
#define MDM_FIRCOEF13_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF12 field mask
#define MDM_FIRCOEF12_MASK   ((uint32_t)0x0000007F)
/// FIRCOEF12 field LSB position
#define MDM_FIRCOEF12_LSB    0
/// FIRCOEF12 field width
#define MDM_FIRCOEF12_WIDTH  ((uint32_t)0x00000007)

/// FIRCOEF15 field reset value
#define MDM_FIRCOEF15_RST    0x7F
/// FIRCOEF14 field reset value
#define MDM_FIRCOEF14_RST    0x2
/// FIRCOEF13 field reset value
#define MDM_FIRCOEF13_RST    0xA
/// FIRCOEF12 field reset value
#define MDM_FIRCOEF12_RST    0x17

/**
 * @brief Constructs a value for the FEBFIRCOEF15DOWNTO12 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] fircoef15 - The value to use for the FIRCOEF15 field.
 * @param[in] fircoef14 - The value to use for the FIRCOEF14 field.
 * @param[in] fircoef13 - The value to use for the FIRCOEF13 field.
 * @param[in] fircoef12 - The value to use for the FIRCOEF12 field.
 */
__INLINE void mdm_febfircoef15downto12_pack(uint8_t fircoef15, uint8_t fircoef14, uint8_t fircoef13, uint8_t fircoef12)
{
    ASSERT_ERR((((uint32_t)fircoef15 << 24) & ~((uint32_t)0x7F000000)) == 0);
    ASSERT_ERR((((uint32_t)fircoef14 << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)fircoef13 << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)fircoef12 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF15DOWNTO12_ADDR,  ((uint32_t)fircoef15 << 24) | ((uint32_t)fircoef14 << 16) | ((uint32_t)fircoef13 << 8) | ((uint32_t)fircoef12 << 0));
}

/**
 * @brief Unpacks FEBFIRCOEF15DOWNTO12's fields from current value of the FEBFIRCOEF15DOWNTO12 register.
 *
 * Reads the FEBFIRCOEF15DOWNTO12 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] fircoef15 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef14 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef13 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef12 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_febfircoef15downto12_unpack(uint8_t* fircoef15, uint8_t* fircoef14, uint8_t* fircoef13, uint8_t* fircoef12)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF15DOWNTO12_ADDR);

    *fircoef15 = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *fircoef14 = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *fircoef13 = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *fircoef12 = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the FIRCOEF15 field in the FEBFIRCOEF15DOWNTO12 register.
 *
 * The FEBFIRCOEF15DOWNTO12 register will be read and the FIRCOEF15 field's value will be returned.
 *
 * @return The current value of the FIRCOEF15 field in the FEBFIRCOEF15DOWNTO12 register.
 */
__INLINE uint8_t mdm_fircoef15_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF15DOWNTO12_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

/**
 * @brief Sets the FIRCOEF15 field of the FEBFIRCOEF15DOWNTO12 register.
 *
 * The FEBFIRCOEF15DOWNTO12 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef15 - The value to set the field to.
 */
__INLINE void mdm_fircoef15_setf(uint8_t fircoef15)
{
    ASSERT_ERR((((uint32_t)fircoef15 << 24) & ~((uint32_t)0x7F000000)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF15DOWNTO12_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF15DOWNTO12_ADDR) & ~((uint32_t)0x7F000000)) | ((uint32_t)fircoef15 << 24));
}

/**
 * @brief Returns the current value of the FIRCOEF14 field in the FEBFIRCOEF15DOWNTO12 register.
 *
 * The FEBFIRCOEF15DOWNTO12 register will be read and the FIRCOEF14 field's value will be returned.
 *
 * @return The current value of the FIRCOEF14 field in the FEBFIRCOEF15DOWNTO12 register.
 */
__INLINE uint8_t mdm_fircoef14_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF15DOWNTO12_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the FIRCOEF14 field of the FEBFIRCOEF15DOWNTO12 register.
 *
 * The FEBFIRCOEF15DOWNTO12 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef14 - The value to set the field to.
 */
__INLINE void mdm_fircoef14_setf(uint8_t fircoef14)
{
    ASSERT_ERR((((uint32_t)fircoef14 << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF15DOWNTO12_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF15DOWNTO12_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)fircoef14 << 16));
}

/**
 * @brief Returns the current value of the FIRCOEF13 field in the FEBFIRCOEF15DOWNTO12 register.
 *
 * The FEBFIRCOEF15DOWNTO12 register will be read and the FIRCOEF13 field's value will be returned.
 *
 * @return The current value of the FIRCOEF13 field in the FEBFIRCOEF15DOWNTO12 register.
 */
__INLINE uint8_t mdm_fircoef13_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF15DOWNTO12_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the FIRCOEF13 field of the FEBFIRCOEF15DOWNTO12 register.
 *
 * The FEBFIRCOEF15DOWNTO12 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef13 - The value to set the field to.
 */
__INLINE void mdm_fircoef13_setf(uint8_t fircoef13)
{
    ASSERT_ERR((((uint32_t)fircoef13 << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF15DOWNTO12_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF15DOWNTO12_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)fircoef13 << 8));
}

/**
 * @brief Returns the current value of the FIRCOEF12 field in the FEBFIRCOEF15DOWNTO12 register.
 *
 * The FEBFIRCOEF15DOWNTO12 register will be read and the FIRCOEF12 field's value will be returned.
 *
 * @return The current value of the FIRCOEF12 field in the FEBFIRCOEF15DOWNTO12 register.
 */
__INLINE uint8_t mdm_fircoef12_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF15DOWNTO12_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the FIRCOEF12 field of the FEBFIRCOEF15DOWNTO12 register.
 *
 * The FEBFIRCOEF15DOWNTO12 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef12 - The value to set the field to.
 */
__INLINE void mdm_fircoef12_setf(uint8_t fircoef12)
{
    ASSERT_ERR((((uint32_t)fircoef12 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF15DOWNTO12_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF15DOWNTO12_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)fircoef12 << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name FEBFIRCOEF19DOWNTO16 register definitions
 * <table>
 * <caption>FEBFIRCOEF19DOWNTO16 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 30:24 <td> FIRCOEF19 <td> R <td> R/W <td> 0x0
 * <tr><td> 22:16 <td> FIRCOEF18 <td> R <td> R/W <td> 0x0
 * <tr><td> 14:08 <td> FIRCOEF17 <td> R <td> R/W <td> 0x0
 * <tr><td> 06:00 <td> FIRCOEF16 <td> R <td> R/W <td> 0x7F
 * </table>
 *
 * @{
 */

/// Address of the FEBFIRCOEF19DOWNTO16 register
#define MDM_FEBFIRCOEF19DOWNTO16_ADDR   0x24C00340
/// Offset of the FEBFIRCOEF19DOWNTO16 register from the base address
#define MDM_FEBFIRCOEF19DOWNTO16_OFFSET 0x00000340
/// Index of the FEBFIRCOEF19DOWNTO16 register
#define MDM_FEBFIRCOEF19DOWNTO16_INDEX  0x000000D0
/// Reset value of the FEBFIRCOEF19DOWNTO16 register
#define MDM_FEBFIRCOEF19DOWNTO16_RESET  0x0000007F

/**
 * @brief Returns the current value of the FEBFIRCOEF19DOWNTO16 register.
 * The FEBFIRCOEF19DOWNTO16 register will be read and its value returned.
 * @return The current value of the FEBFIRCOEF19DOWNTO16 register.
 */
__INLINE uint32_t mdm_febfircoef19downto16_get(void)
{
    return REG_PL_RD(MDM_FEBFIRCOEF19DOWNTO16_ADDR);
}

/**
 * @brief Sets the FEBFIRCOEF19DOWNTO16 register to a value.
 * The FEBFIRCOEF19DOWNTO16 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_febfircoef19downto16_set(uint32_t value)
{
    REG_PL_WR(MDM_FEBFIRCOEF19DOWNTO16_ADDR, value);
}

// field definitions
/// FIRCOEF19 field mask
#define MDM_FIRCOEF19_MASK   ((uint32_t)0x7F000000)
/// FIRCOEF19 field LSB position
#define MDM_FIRCOEF19_LSB    24
/// FIRCOEF19 field width
#define MDM_FIRCOEF19_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF18 field mask
#define MDM_FIRCOEF18_MASK   ((uint32_t)0x007F0000)
/// FIRCOEF18 field LSB position
#define MDM_FIRCOEF18_LSB    16
/// FIRCOEF18 field width
#define MDM_FIRCOEF18_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF17 field mask
#define MDM_FIRCOEF17_MASK   ((uint32_t)0x00007F00)
/// FIRCOEF17 field LSB position
#define MDM_FIRCOEF17_LSB    8
/// FIRCOEF17 field width
#define MDM_FIRCOEF17_WIDTH  ((uint32_t)0x00000007)
/// FIRCOEF16 field mask
#define MDM_FIRCOEF16_MASK   ((uint32_t)0x0000007F)
/// FIRCOEF16 field LSB position
#define MDM_FIRCOEF16_LSB    0
/// FIRCOEF16 field width
#define MDM_FIRCOEF16_WIDTH  ((uint32_t)0x00000007)

/// FIRCOEF19 field reset value
#define MDM_FIRCOEF19_RST    0x0
/// FIRCOEF18 field reset value
#define MDM_FIRCOEF18_RST    0x0
/// FIRCOEF17 field reset value
#define MDM_FIRCOEF17_RST    0x0
/// FIRCOEF16 field reset value
#define MDM_FIRCOEF16_RST    0x7F

/**
 * @brief Constructs a value for the FEBFIRCOEF19DOWNTO16 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] fircoef19 - The value to use for the FIRCOEF19 field.
 * @param[in] fircoef18 - The value to use for the FIRCOEF18 field.
 * @param[in] fircoef17 - The value to use for the FIRCOEF17 field.
 * @param[in] fircoef16 - The value to use for the FIRCOEF16 field.
 */
__INLINE void mdm_febfircoef19downto16_pack(uint8_t fircoef19, uint8_t fircoef18, uint8_t fircoef17, uint8_t fircoef16)
{
    ASSERT_ERR((((uint32_t)fircoef19 << 24) & ~((uint32_t)0x7F000000)) == 0);
    ASSERT_ERR((((uint32_t)fircoef18 << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)fircoef17 << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)fircoef16 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF19DOWNTO16_ADDR,  ((uint32_t)fircoef19 << 24) | ((uint32_t)fircoef18 << 16) | ((uint32_t)fircoef17 << 8) | ((uint32_t)fircoef16 << 0));
}

/**
 * @brief Unpacks FEBFIRCOEF19DOWNTO16's fields from current value of the FEBFIRCOEF19DOWNTO16 register.
 *
 * Reads the FEBFIRCOEF19DOWNTO16 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] fircoef19 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef18 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef17 - Will be populated with the current value of this field from the register.
 * @param[out] fircoef16 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_febfircoef19downto16_unpack(uint8_t* fircoef19, uint8_t* fircoef18, uint8_t* fircoef17, uint8_t* fircoef16)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF19DOWNTO16_ADDR);

    *fircoef19 = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *fircoef18 = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *fircoef17 = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *fircoef16 = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the FIRCOEF19 field in the FEBFIRCOEF19DOWNTO16 register.
 *
 * The FEBFIRCOEF19DOWNTO16 register will be read and the FIRCOEF19 field's value will be returned.
 *
 * @return The current value of the FIRCOEF19 field in the FEBFIRCOEF19DOWNTO16 register.
 */
__INLINE uint8_t mdm_fircoef19_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF19DOWNTO16_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

/**
 * @brief Sets the FIRCOEF19 field of the FEBFIRCOEF19DOWNTO16 register.
 *
 * The FEBFIRCOEF19DOWNTO16 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef19 - The value to set the field to.
 */
__INLINE void mdm_fircoef19_setf(uint8_t fircoef19)
{
    ASSERT_ERR((((uint32_t)fircoef19 << 24) & ~((uint32_t)0x7F000000)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF19DOWNTO16_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF19DOWNTO16_ADDR) & ~((uint32_t)0x7F000000)) | ((uint32_t)fircoef19 << 24));
}

/**
 * @brief Returns the current value of the FIRCOEF18 field in the FEBFIRCOEF19DOWNTO16 register.
 *
 * The FEBFIRCOEF19DOWNTO16 register will be read and the FIRCOEF18 field's value will be returned.
 *
 * @return The current value of the FIRCOEF18 field in the FEBFIRCOEF19DOWNTO16 register.
 */
__INLINE uint8_t mdm_fircoef18_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF19DOWNTO16_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

/**
 * @brief Sets the FIRCOEF18 field of the FEBFIRCOEF19DOWNTO16 register.
 *
 * The FEBFIRCOEF19DOWNTO16 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef18 - The value to set the field to.
 */
__INLINE void mdm_fircoef18_setf(uint8_t fircoef18)
{
    ASSERT_ERR((((uint32_t)fircoef18 << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF19DOWNTO16_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF19DOWNTO16_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)fircoef18 << 16));
}

/**
 * @brief Returns the current value of the FIRCOEF17 field in the FEBFIRCOEF19DOWNTO16 register.
 *
 * The FEBFIRCOEF19DOWNTO16 register will be read and the FIRCOEF17 field's value will be returned.
 *
 * @return The current value of the FIRCOEF17 field in the FEBFIRCOEF19DOWNTO16 register.
 */
__INLINE uint8_t mdm_fircoef17_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF19DOWNTO16_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the FIRCOEF17 field of the FEBFIRCOEF19DOWNTO16 register.
 *
 * The FEBFIRCOEF19DOWNTO16 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef17 - The value to set the field to.
 */
__INLINE void mdm_fircoef17_setf(uint8_t fircoef17)
{
    ASSERT_ERR((((uint32_t)fircoef17 << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF19DOWNTO16_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF19DOWNTO16_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)fircoef17 << 8));
}

/**
 * @brief Returns the current value of the FIRCOEF16 field in the FEBFIRCOEF19DOWNTO16 register.
 *
 * The FEBFIRCOEF19DOWNTO16 register will be read and the FIRCOEF16 field's value will be returned.
 *
 * @return The current value of the FIRCOEF16 field in the FEBFIRCOEF19DOWNTO16 register.
 */
__INLINE uint8_t mdm_fircoef16_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIRCOEF19DOWNTO16_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the FIRCOEF16 field of the FEBFIRCOEF19DOWNTO16 register.
 *
 * The FEBFIRCOEF19DOWNTO16 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fircoef16 - The value to set the field to.
 */
__INLINE void mdm_fircoef16_setf(uint8_t fircoef16)
{
    ASSERT_ERR((((uint32_t)fircoef16 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIRCOEF19DOWNTO16_ADDR, (REG_PL_RD(MDM_FEBFIRCOEF19DOWNTO16_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)fircoef16 << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

#if RW_NX_DERIV_80211B
/**
 * @name FEBFIROFFSETCOMP register definitions
 * <table>
 * <caption>FEBFIROFFSETCOMP bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 16    <td> FIROFFSETCOMPEN <td> R <td> R/W <td> 0
 * <tr><td> 14:08 <td>  FIROFFSETCOMPI <td> R <td> R/W <td> 0x0
 * <tr><td> 06:00 <td>  FIROFFSETCOMPQ <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FEBFIROFFSETCOMP register
#define MDM_FEBFIROFFSETCOMP_ADDR   0x24C00344
/// Offset of the FEBFIROFFSETCOMP register from the base address
#define MDM_FEBFIROFFSETCOMP_OFFSET 0x00000344
/// Index of the FEBFIROFFSETCOMP register
#define MDM_FEBFIROFFSETCOMP_INDEX  0x000000D1
/// Reset value of the FEBFIROFFSETCOMP register
#define MDM_FEBFIROFFSETCOMP_RESET  0x00000000

/**
 * @brief Returns the current value of the FEBFIROFFSETCOMP register.
 * The FEBFIROFFSETCOMP register will be read and its value returned.
 * @return The current value of the FEBFIROFFSETCOMP register.
 */
__INLINE uint32_t mdm_febfiroffsetcomp_get(void)
{
    return REG_PL_RD(MDM_FEBFIROFFSETCOMP_ADDR);
}

/**
 * @brief Sets the FEBFIROFFSETCOMP register to a value.
 * The FEBFIROFFSETCOMP register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_febfiroffsetcomp_set(uint32_t value)
{
    REG_PL_WR(MDM_FEBFIROFFSETCOMP_ADDR, value);
}

// field definitions
/// FIROFFSETCOMPEN field bit
#define MDM_FIROFFSETCOMPEN_BIT    ((uint32_t)0x00010000)
/// FIROFFSETCOMPEN field position
#define MDM_FIROFFSETCOMPEN_POS    16
/// FIROFFSETCOMPI field mask
#define MDM_FIROFFSETCOMPI_MASK    ((uint32_t)0x00007F00)
/// FIROFFSETCOMPI field LSB position
#define MDM_FIROFFSETCOMPI_LSB     8
/// FIROFFSETCOMPI field width
#define MDM_FIROFFSETCOMPI_WIDTH   ((uint32_t)0x00000007)
/// FIROFFSETCOMPQ field mask
#define MDM_FIROFFSETCOMPQ_MASK    ((uint32_t)0x0000007F)
/// FIROFFSETCOMPQ field LSB position
#define MDM_FIROFFSETCOMPQ_LSB     0
/// FIROFFSETCOMPQ field width
#define MDM_FIROFFSETCOMPQ_WIDTH   ((uint32_t)0x00000007)

/// FIROFFSETCOMPEN field reset value
#define MDM_FIROFFSETCOMPEN_RST    0x0
/// FIROFFSETCOMPI field reset value
#define MDM_FIROFFSETCOMPI_RST     0x0
/// FIROFFSETCOMPQ field reset value
#define MDM_FIROFFSETCOMPQ_RST     0x0

/**
 * @brief Constructs a value for the FEBFIROFFSETCOMP register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] firoffsetcompen - The value to use for the FIROFFSETCOMPEN field.
 * @param[in] firoffsetcompi - The value to use for the FIROFFSETCOMPI field.
 * @param[in] firoffsetcompq - The value to use for the FIROFFSETCOMPQ field.
 */
__INLINE void mdm_febfiroffsetcomp_pack(uint8_t firoffsetcompen, uint8_t firoffsetcompi, uint8_t firoffsetcompq)
{
    ASSERT_ERR((((uint32_t)firoffsetcompen << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)firoffsetcompi << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)firoffsetcompq << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIROFFSETCOMP_ADDR,  ((uint32_t)firoffsetcompen << 16) | ((uint32_t)firoffsetcompi << 8) | ((uint32_t)firoffsetcompq << 0));
}

/**
 * @brief Unpacks FEBFIROFFSETCOMP's fields from current value of the FEBFIROFFSETCOMP register.
 *
 * Reads the FEBFIROFFSETCOMP register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] firoffsetcompen - Will be populated with the current value of this field from the register.
 * @param[out] firoffsetcompi - Will be populated with the current value of this field from the register.
 * @param[out] firoffsetcompq - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_febfiroffsetcomp_unpack(uint8_t* firoffsetcompen, uint8_t* firoffsetcompi, uint8_t* firoffsetcompq)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIROFFSETCOMP_ADDR);

    *firoffsetcompen = (localVal & ((uint32_t)0x00010000)) >> 16;
    *firoffsetcompi = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *firoffsetcompq = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the FIROFFSETCOMPEN field in the FEBFIROFFSETCOMP register.
 *
 * The FEBFIROFFSETCOMP register will be read and the FIROFFSETCOMPEN field's value will be returned.
 *
 * @return The current value of the FIROFFSETCOMPEN field in the FEBFIROFFSETCOMP register.
 */
__INLINE uint8_t mdm_firoffsetcompen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIROFFSETCOMP_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the FIROFFSETCOMPEN field of the FEBFIROFFSETCOMP register.
 *
 * The FEBFIROFFSETCOMP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] firoffsetcompen - The value to set the field to.
 */
__INLINE void mdm_firoffsetcompen_setf(uint8_t firoffsetcompen)
{
    ASSERT_ERR((((uint32_t)firoffsetcompen << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(MDM_FEBFIROFFSETCOMP_ADDR, (REG_PL_RD(MDM_FEBFIROFFSETCOMP_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)firoffsetcompen << 16));
}

/**
 * @brief Returns the current value of the FIROFFSETCOMPI field in the FEBFIROFFSETCOMP register.
 *
 * The FEBFIROFFSETCOMP register will be read and the FIROFFSETCOMPI field's value will be returned.
 *
 * @return The current value of the FIROFFSETCOMPI field in the FEBFIROFFSETCOMP register.
 */
__INLINE uint8_t mdm_firoffsetcompi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIROFFSETCOMP_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the FIROFFSETCOMPI field of the FEBFIROFFSETCOMP register.
 *
 * The FEBFIROFFSETCOMP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] firoffsetcompi - The value to set the field to.
 */
__INLINE void mdm_firoffsetcompi_setf(uint8_t firoffsetcompi)
{
    ASSERT_ERR((((uint32_t)firoffsetcompi << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(MDM_FEBFIROFFSETCOMP_ADDR, (REG_PL_RD(MDM_FEBFIROFFSETCOMP_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)firoffsetcompi << 8));
}

/**
 * @brief Returns the current value of the FIROFFSETCOMPQ field in the FEBFIROFFSETCOMP register.
 *
 * The FEBFIROFFSETCOMP register will be read and the FIROFFSETCOMPQ field's value will be returned.
 *
 * @return The current value of the FIROFFSETCOMPQ field in the FEBFIROFFSETCOMP register.
 */
__INLINE uint8_t mdm_firoffsetcompq_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FEBFIROFFSETCOMP_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the FIROFFSETCOMPQ field of the FEBFIROFFSETCOMP register.
 *
 * The FEBFIROFFSETCOMP register will be read, modified to contain the new field value, and written.
 *
 * @param[in] firoffsetcompq - The value to set the field to.
 */
__INLINE void mdm_firoffsetcompq_setf(uint8_t firoffsetcompq)
{
    ASSERT_ERR((((uint32_t)firoffsetcompq << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_FEBFIROFFSETCOMP_ADDR, (REG_PL_RD(MDM_FEBFIROFFSETCOMP_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)firoffsetcompq << 0));
}

#endif // RW_NX_DERIV_80211B
/// @}

/**
 * @name MDMCONF register definitions
 * <table>
 * <caption>MDMCONF bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 11:08 <td> CONF_NRX <td> R <td> R/W <td> 0x0
 * <tr><td> 07:04 <td> CONF_NTX <td> R <td> R/W <td> 0x0
 * <tr><td> 03:00 <td>  CONF_BW <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the MDMCONF register
#define MDM_MDMCONF_ADDR   0x24C00800
/// Offset of the MDMCONF register from the base address
#define MDM_MDMCONF_OFFSET 0x00000800
/// Index of the MDMCONF register
#define MDM_MDMCONF_INDEX  0x00000200
/// Reset value of the MDMCONF register
#define MDM_MDMCONF_RESET  0x00000000

/**
 * @brief Returns the current value of the MDMCONF register.
 * The MDMCONF register will be read and its value returned.
 * @return The current value of the MDMCONF register.
 */
__INLINE uint32_t mdm_mdmconf_get(void)
{
    return REG_PL_RD(MDM_MDMCONF_ADDR);
}

/**
 * @brief Sets the MDMCONF register to a value.
 * The MDMCONF register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_mdmconf_set(uint32_t value)
{
    REG_PL_WR(MDM_MDMCONF_ADDR, value);
}

// field definitions
/// CONF_NRX field mask
#define MDM_CONF_NRX_MASK   ((uint32_t)0x00000F00)
/// CONF_NRX field LSB position
#define MDM_CONF_NRX_LSB    8
/// CONF_NRX field width
#define MDM_CONF_NRX_WIDTH  ((uint32_t)0x00000004)
/// CONF_NTX field mask
#define MDM_CONF_NTX_MASK   ((uint32_t)0x000000F0)
/// CONF_NTX field LSB position
#define MDM_CONF_NTX_LSB    4
/// CONF_NTX field width
#define MDM_CONF_NTX_WIDTH  ((uint32_t)0x00000004)
/// CONF_BW field mask
#define MDM_CONF_BW_MASK    ((uint32_t)0x0000000F)
/// CONF_BW field LSB position
#define MDM_CONF_BW_LSB     0
/// CONF_BW field width
#define MDM_CONF_BW_WIDTH   ((uint32_t)0x00000004)

/// CONF_NRX field reset value
#define MDM_CONF_NRX_RST    0x0
/// CONF_NTX field reset value
#define MDM_CONF_NTX_RST    0x0
/// CONF_BW field reset value
#define MDM_CONF_BW_RST     0x0

/**
 * @brief Constructs a value for the MDMCONF register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] confnrx - The value to use for the CONF_NRX field.
 * @param[in] confntx - The value to use for the CONF_NTX field.
 * @param[in] confbw - The value to use for the CONF_BW field.
 */
__INLINE void mdm_mdmconf_pack(uint8_t confnrx, uint8_t confntx, uint8_t confbw)
{
    ASSERT_ERR((((uint32_t)confnrx << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)confntx << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)confbw << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMCONF_ADDR,  ((uint32_t)confnrx << 8) | ((uint32_t)confntx << 4) | ((uint32_t)confbw << 0));
}

/**
 * @brief Unpacks MDMCONF's fields from current value of the MDMCONF register.
 *
 * Reads the MDMCONF register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] confnrx - Will be populated with the current value of this field from the register.
 * @param[out] confntx - Will be populated with the current value of this field from the register.
 * @param[out] confbw - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_mdmconf_unpack(uint8_t* confnrx, uint8_t* confntx, uint8_t* confbw)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMCONF_ADDR);

    *confnrx = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *confntx = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *confbw = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the CONF_NRX field in the MDMCONF register.
 *
 * The MDMCONF register will be read and the CONF_NRX field's value will be returned.
 *
 * @return The current value of the CONF_NRX field in the MDMCONF register.
 */
__INLINE uint8_t mdm_conf_nrx_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMCONF_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the CONF_NRX field of the MDMCONF register.
 *
 * The MDMCONF register will be read, modified to contain the new field value, and written.
 *
 * @param[in] confnrx - The value to set the field to.
 */
__INLINE void mdm_conf_nrx_setf(uint8_t confnrx)
{
    ASSERT_ERR((((uint32_t)confnrx << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(MDM_MDMCONF_ADDR, (REG_PL_RD(MDM_MDMCONF_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)confnrx << 8));
}

/**
 * @brief Returns the current value of the CONF_NTX field in the MDMCONF register.
 *
 * The MDMCONF register will be read and the CONF_NTX field's value will be returned.
 *
 * @return The current value of the CONF_NTX field in the MDMCONF register.
 */
__INLINE uint8_t mdm_conf_ntx_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMCONF_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the CONF_NTX field of the MDMCONF register.
 *
 * The MDMCONF register will be read, modified to contain the new field value, and written.
 *
 * @param[in] confntx - The value to set the field to.
 */
__INLINE void mdm_conf_ntx_setf(uint8_t confntx)
{
    ASSERT_ERR((((uint32_t)confntx << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_MDMCONF_ADDR, (REG_PL_RD(MDM_MDMCONF_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)confntx << 4));
}

/**
 * @brief Returns the current value of the CONF_BW field in the MDMCONF register.
 *
 * The MDMCONF register will be read and the CONF_BW field's value will be returned.
 *
 * @return The current value of the CONF_BW field in the MDMCONF register.
 */
__INLINE uint8_t mdm_conf_bw_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_MDMCONF_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the CONF_BW field of the MDMCONF register.
 *
 * The MDMCONF register will be read, modified to contain the new field value, and written.
 *
 * @param[in] confbw - The value to set the field to.
 */
__INLINE void mdm_conf_bw_setf(uint8_t confbw)
{
    ASSERT_ERR((((uint32_t)confbw << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_MDMCONF_ADDR, (REG_PL_RD(MDM_MDMCONF_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)confbw << 0));
}

/// @}

/**
 * @name RXTDCTRL0 register definitions
 * <table>
 * <caption>RXTDCTRL0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:24 <td>   TDSYNCOFF20 <td> R <td> R/W <td> 0x19
 * <tr><td> 15:08 <td> TDSYNCOFF2080 <td> R <td> R/W <td> 0x38
 * <tr><td> 07:00 <td> TDSYNCOFF2040 <td> R <td> R/W <td> 0xD
 * </table>
 *
 * @{
 */

/// Address of the RXTDCTRL0 register
#define MDM_RXTDCTRL0_ADDR   0x24C00804
/// Offset of the RXTDCTRL0 register from the base address
#define MDM_RXTDCTRL0_OFFSET 0x00000804
/// Index of the RXTDCTRL0 register
#define MDM_RXTDCTRL0_INDEX  0x00000201
/// Reset value of the RXTDCTRL0 register
#define MDM_RXTDCTRL0_RESET  0x1900380D

/**
 * @brief Returns the current value of the RXTDCTRL0 register.
 * The RXTDCTRL0 register will be read and its value returned.
 * @return The current value of the RXTDCTRL0 register.
 */
__INLINE uint32_t mdm_rxtdctrl0_get(void)
{
    return REG_PL_RD(MDM_RXTDCTRL0_ADDR);
}

/**
 * @brief Sets the RXTDCTRL0 register to a value.
 * The RXTDCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxtdctrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_RXTDCTRL0_ADDR, value);
}

// field definitions
/// TDSYNCOFF20 field mask
#define MDM_TDSYNCOFF20_MASK     ((uint32_t)0xFF000000)
/// TDSYNCOFF20 field LSB position
#define MDM_TDSYNCOFF20_LSB      24
/// TDSYNCOFF20 field width
#define MDM_TDSYNCOFF20_WIDTH    ((uint32_t)0x00000008)
/// TDSYNCOFF2080 field mask
#define MDM_TDSYNCOFF2080_MASK   ((uint32_t)0x0000FF00)
/// TDSYNCOFF2080 field LSB position
#define MDM_TDSYNCOFF2080_LSB    8
/// TDSYNCOFF2080 field width
#define MDM_TDSYNCOFF2080_WIDTH  ((uint32_t)0x00000008)
/// TDSYNCOFF2040 field mask
#define MDM_TDSYNCOFF2040_MASK   ((uint32_t)0x000000FF)
/// TDSYNCOFF2040 field LSB position
#define MDM_TDSYNCOFF2040_LSB    0
/// TDSYNCOFF2040 field width
#define MDM_TDSYNCOFF2040_WIDTH  ((uint32_t)0x00000008)

/// TDSYNCOFF20 field reset value
#define MDM_TDSYNCOFF20_RST      0x19
/// TDSYNCOFF2080 field reset value
#define MDM_TDSYNCOFF2080_RST    0x38
/// TDSYNCOFF2040 field reset value
#define MDM_TDSYNCOFF2040_RST    0xD

/**
 * @brief Constructs a value for the RXTDCTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tdsyncoff20 - The value to use for the TDSYNCOFF20 field.
 * @param[in] tdsyncoff2080 - The value to use for the TDSYNCOFF2080 field.
 * @param[in] tdsyncoff2040 - The value to use for the TDSYNCOFF2040 field.
 */
__INLINE void mdm_rxtdctrl0_pack(uint8_t tdsyncoff20, uint8_t tdsyncoff2080, uint8_t tdsyncoff2040)
{
    ASSERT_ERR((((uint32_t)tdsyncoff20 << 24) & ~((uint32_t)0xFF000000)) == 0);
    ASSERT_ERR((((uint32_t)tdsyncoff2080 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)tdsyncoff2040 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_RXTDCTRL0_ADDR,  ((uint32_t)tdsyncoff20 << 24) | ((uint32_t)tdsyncoff2080 << 8) | ((uint32_t)tdsyncoff2040 << 0));
}

/**
 * @brief Unpacks RXTDCTRL0's fields from current value of the RXTDCTRL0 register.
 *
 * Reads the RXTDCTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tdsyncoff20 - Will be populated with the current value of this field from the register.
 * @param[out] tdsyncoff2080 - Will be populated with the current value of this field from the register.
 * @param[out] tdsyncoff2040 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxtdctrl0_unpack(uint8_t* tdsyncoff20, uint8_t* tdsyncoff2080, uint8_t* tdsyncoff2040)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL0_ADDR);

    *tdsyncoff20 = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *tdsyncoff2080 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *tdsyncoff2040 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the TDSYNCOFF20 field in the RXTDCTRL0 register.
 *
 * The RXTDCTRL0 register will be read and the TDSYNCOFF20 field's value will be returned.
 *
 * @return The current value of the TDSYNCOFF20 field in the RXTDCTRL0 register.
 */
__INLINE uint8_t mdm_tdsyncoff20_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL0_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Sets the TDSYNCOFF20 field of the RXTDCTRL0 register.
 *
 * The RXTDCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdsyncoff20 - The value to set the field to.
 */
__INLINE void mdm_tdsyncoff20_setf(uint8_t tdsyncoff20)
{
    ASSERT_ERR((((uint32_t)tdsyncoff20 << 24) & ~((uint32_t)0xFF000000)) == 0);
    REG_PL_WR(MDM_RXTDCTRL0_ADDR, (REG_PL_RD(MDM_RXTDCTRL0_ADDR) & ~((uint32_t)0xFF000000)) | ((uint32_t)tdsyncoff20 << 24));
}

/**
 * @brief Returns the current value of the TDSYNCOFF2080 field in the RXTDCTRL0 register.
 *
 * The RXTDCTRL0 register will be read and the TDSYNCOFF2080 field's value will be returned.
 *
 * @return The current value of the TDSYNCOFF2080 field in the RXTDCTRL0 register.
 */
__INLINE uint8_t mdm_tdsyncoff2080_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the TDSYNCOFF2080 field of the RXTDCTRL0 register.
 *
 * The RXTDCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdsyncoff2080 - The value to set the field to.
 */
__INLINE void mdm_tdsyncoff2080_setf(uint8_t tdsyncoff2080)
{
    ASSERT_ERR((((uint32_t)tdsyncoff2080 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(MDM_RXTDCTRL0_ADDR, (REG_PL_RD(MDM_RXTDCTRL0_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)tdsyncoff2080 << 8));
}

/**
 * @brief Returns the current value of the TDSYNCOFF2040 field in the RXTDCTRL0 register.
 *
 * The RXTDCTRL0 register will be read and the TDSYNCOFF2040 field's value will be returned.
 *
 * @return The current value of the TDSYNCOFF2040 field in the RXTDCTRL0 register.
 */
__INLINE uint8_t mdm_tdsyncoff2040_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the TDSYNCOFF2040 field of the RXTDCTRL0 register.
 *
 * The RXTDCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdsyncoff2040 - The value to set the field to.
 */
__INLINE void mdm_tdsyncoff2040_setf(uint8_t tdsyncoff2040)
{
    ASSERT_ERR((((uint32_t)tdsyncoff2040 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_RXTDCTRL0_ADDR, (REG_PL_RD(MDM_RXTDCTRL0_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)tdsyncoff2040 << 0));
}

/// @}

/**
 * @name FDOCTRL0 register definitions
 * <table>
 * <caption>FDOCTRL0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>  CFGSTO4TDCOMP <td> R <td> R/W <td> 1
 * <tr><td> 30    <td>     CFGCPEWALK <td> R <td> R/W <td> 1
 * <tr><td> 29    <td>      CFGSTOREF <td> R <td> R/W <td> 1
 * <tr><td> 28    <td>      CFGCPEREF <td> R <td> R/W <td> 1
 * <tr><td> 26    <td>       ESTSECEN <td> R <td> R/W <td> 1
 * <tr><td> 25    <td>   CPESLOPELGHT <td> R <td> R/W <td> 1
 * <tr><td> 24    <td>   STOSLOPELGHT <td> R <td> R/W <td> 1
 * <tr><td> 23    <td>        CPEMODE <td> R <td> R/W <td> 1
 * <tr><td> 22    <td>        STOMODE <td> R <td> R/W <td> 1
 * <tr><td> 21:17 <td>    STOCOMPTIME <td> R <td> R/W <td> 0x0
 * <tr><td> 16:11 <td> MAXNTDSYNCHOFF <td> R <td> R/W <td> 0x1F
 * <tr><td> 10:05 <td> MAXPTDSYNCHOFF <td> R <td> R/W <td> 0x1F
 * <tr><td> 04    <td>    STOFDCOMPEN <td> R <td> R/W <td> 1
 * <tr><td> 03    <td>  CPESLOPEESTEN <td> R <td> R/W <td> 1
 * <tr><td> 02    <td>    CPEFDCOMPEN <td> R <td> R/W <td> 1
 * <tr><td> 01    <td>  STOSLOPEESTEN <td> R <td> R/W <td> 1
 * <tr><td> 00    <td>    STOTDCOMPEN <td> R <td> R/W <td> 1
 * </table>
 *
 * @{
 */

/// Address of the FDOCTRL0 register
#define MDM_FDOCTRL0_ADDR   0x24C00808
/// Offset of the FDOCTRL0 register from the base address
#define MDM_FDOCTRL0_OFFSET 0x00000808
/// Index of the FDOCTRL0 register
#define MDM_FDOCTRL0_INDEX  0x00000202
/// Reset value of the FDOCTRL0 register
#define MDM_FDOCTRL0_RESET  0xF7C0FBFF

/**
 * @brief Returns the current value of the FDOCTRL0 register.
 * The FDOCTRL0 register will be read and its value returned.
 * @return The current value of the FDOCTRL0 register.
 */
__INLINE uint32_t mdm_fdoctrl0_get(void)
{
    return REG_PL_RD(MDM_FDOCTRL0_ADDR);
}

/**
 * @brief Sets the FDOCTRL0 register to a value.
 * The FDOCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_fdoctrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_FDOCTRL0_ADDR, value);
}

// field definitions
/// CFGSTO4TDCOMP field bit
#define MDM_CFGSTO4TDCOMP_BIT     ((uint32_t)0x80000000)
/// CFGSTO4TDCOMP field position
#define MDM_CFGSTO4TDCOMP_POS     31
/// CFGCPEWALK field bit
#define MDM_CFGCPEWALK_BIT        ((uint32_t)0x40000000)
/// CFGCPEWALK field position
#define MDM_CFGCPEWALK_POS        30
/// CFGSTOREF field bit
#define MDM_CFGSTOREF_BIT         ((uint32_t)0x20000000)
/// CFGSTOREF field position
#define MDM_CFGSTOREF_POS         29
/// CFGCPEREF field bit
#define MDM_CFGCPEREF_BIT         ((uint32_t)0x10000000)
/// CFGCPEREF field position
#define MDM_CFGCPEREF_POS         28
/// ESTSECEN field bit
#define MDM_ESTSECEN_BIT          ((uint32_t)0x04000000)
/// ESTSECEN field position
#define MDM_ESTSECEN_POS          26
/// CPESLOPELGHT field bit
#define MDM_CPESLOPELGHT_BIT      ((uint32_t)0x02000000)
/// CPESLOPELGHT field position
#define MDM_CPESLOPELGHT_POS      25
/// STOSLOPELGHT field bit
#define MDM_STOSLOPELGHT_BIT      ((uint32_t)0x01000000)
/// STOSLOPELGHT field position
#define MDM_STOSLOPELGHT_POS      24
/// CPEMODE field bit
#define MDM_CPEMODE_BIT           ((uint32_t)0x00800000)
/// CPEMODE field position
#define MDM_CPEMODE_POS           23
/// STOMODE field bit
#define MDM_STOMODE_BIT           ((uint32_t)0x00400000)
/// STOMODE field position
#define MDM_STOMODE_POS           22
/// STOCOMPTIME field mask
#define MDM_STOCOMPTIME_MASK      ((uint32_t)0x003E0000)
/// STOCOMPTIME field LSB position
#define MDM_STOCOMPTIME_LSB       17
/// STOCOMPTIME field width
#define MDM_STOCOMPTIME_WIDTH     ((uint32_t)0x00000005)
/// MAXNTDSYNCHOFF field mask
#define MDM_MAXNTDSYNCHOFF_MASK   ((uint32_t)0x0001F800)
/// MAXNTDSYNCHOFF field LSB position
#define MDM_MAXNTDSYNCHOFF_LSB    11
/// MAXNTDSYNCHOFF field width
#define MDM_MAXNTDSYNCHOFF_WIDTH  ((uint32_t)0x00000006)
/// MAXPTDSYNCHOFF field mask
#define MDM_MAXPTDSYNCHOFF_MASK   ((uint32_t)0x000007E0)
/// MAXPTDSYNCHOFF field LSB position
#define MDM_MAXPTDSYNCHOFF_LSB    5
/// MAXPTDSYNCHOFF field width
#define MDM_MAXPTDSYNCHOFF_WIDTH  ((uint32_t)0x00000006)
/// STOFDCOMPEN field bit
#define MDM_STOFDCOMPEN_BIT       ((uint32_t)0x00000010)
/// STOFDCOMPEN field position
#define MDM_STOFDCOMPEN_POS       4
/// CPESLOPEESTEN field bit
#define MDM_CPESLOPEESTEN_BIT     ((uint32_t)0x00000008)
/// CPESLOPEESTEN field position
#define MDM_CPESLOPEESTEN_POS     3
/// CPEFDCOMPEN field bit
#define MDM_CPEFDCOMPEN_BIT       ((uint32_t)0x00000004)
/// CPEFDCOMPEN field position
#define MDM_CPEFDCOMPEN_POS       2
/// STOSLOPEESTEN field bit
#define MDM_STOSLOPEESTEN_BIT     ((uint32_t)0x00000002)
/// STOSLOPEESTEN field position
#define MDM_STOSLOPEESTEN_POS     1
/// STOTDCOMPEN field bit
#define MDM_STOTDCOMPEN_BIT       ((uint32_t)0x00000001)
/// STOTDCOMPEN field position
#define MDM_STOTDCOMPEN_POS       0

/// CFGSTO4TDCOMP field reset value
#define MDM_CFGSTO4TDCOMP_RST     0x1
/// CFGCPEWALK field reset value
#define MDM_CFGCPEWALK_RST        0x1
/// CFGSTOREF field reset value
#define MDM_CFGSTOREF_RST         0x1
/// CFGCPEREF field reset value
#define MDM_CFGCPEREF_RST         0x1
/// ESTSECEN field reset value
#define MDM_ESTSECEN_RST          0x1
/// CPESLOPELGHT field reset value
#define MDM_CPESLOPELGHT_RST      0x1
/// STOSLOPELGHT field reset value
#define MDM_STOSLOPELGHT_RST      0x1
/// CPEMODE field reset value
#define MDM_CPEMODE_RST           0x1
/// STOMODE field reset value
#define MDM_STOMODE_RST           0x1
/// STOCOMPTIME field reset value
#define MDM_STOCOMPTIME_RST       0x0
/// MAXNTDSYNCHOFF field reset value
#define MDM_MAXNTDSYNCHOFF_RST    0x1F
/// MAXPTDSYNCHOFF field reset value
#define MDM_MAXPTDSYNCHOFF_RST    0x1F
/// STOFDCOMPEN field reset value
#define MDM_STOFDCOMPEN_RST       0x1
/// CPESLOPEESTEN field reset value
#define MDM_CPESLOPEESTEN_RST     0x1
/// CPEFDCOMPEN field reset value
#define MDM_CPEFDCOMPEN_RST       0x1
/// STOSLOPEESTEN field reset value
#define MDM_STOSLOPEESTEN_RST     0x1
/// STOTDCOMPEN field reset value
#define MDM_STOTDCOMPEN_RST       0x1

/**
 * @brief Constructs a value for the FDOCTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] cfgsto4tdcomp - The value to use for the CFGSTO4TDCOMP field.
 * @param[in] cfgcpewalk - The value to use for the CFGCPEWALK field.
 * @param[in] cfgstoref - The value to use for the CFGSTOREF field.
 * @param[in] cfgcperef - The value to use for the CFGCPEREF field.
 * @param[in] estsecen - The value to use for the ESTSECEN field.
 * @param[in] cpeslopelght - The value to use for the CPESLOPELGHT field.
 * @param[in] stoslopelght - The value to use for the STOSLOPELGHT field.
 * @param[in] cpemode - The value to use for the CPEMODE field.
 * @param[in] stomode - The value to use for the STOMODE field.
 * @param[in] stocomptime - The value to use for the STOCOMPTIME field.
 * @param[in] maxntdsynchoff - The value to use for the MAXNTDSYNCHOFF field.
 * @param[in] maxptdsynchoff - The value to use for the MAXPTDSYNCHOFF field.
 * @param[in] stofdcompen - The value to use for the STOFDCOMPEN field.
 * @param[in] cpeslopeesten - The value to use for the CPESLOPEESTEN field.
 * @param[in] cpefdcompen - The value to use for the CPEFDCOMPEN field.
 * @param[in] stoslopeesten - The value to use for the STOSLOPEESTEN field.
 * @param[in] stotdcompen - The value to use for the STOTDCOMPEN field.
 */
__INLINE void mdm_fdoctrl0_pack(uint8_t cfgsto4tdcomp, uint8_t cfgcpewalk, uint8_t cfgstoref, uint8_t cfgcperef, uint8_t estsecen, uint8_t cpeslopelght, uint8_t stoslopelght, uint8_t cpemode, uint8_t stomode, uint8_t stocomptime, uint8_t maxntdsynchoff, uint8_t maxptdsynchoff, uint8_t stofdcompen, uint8_t cpeslopeesten, uint8_t cpefdcompen, uint8_t stoslopeesten, uint8_t stotdcompen)
{
    ASSERT_ERR((((uint32_t)cfgsto4tdcomp << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)cfgcpewalk << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)cfgstoref << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)cfgcperef << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)estsecen << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)cpeslopelght << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)stoslopelght << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)cpemode << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)stomode << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)stocomptime << 17) & ~((uint32_t)0x003E0000)) == 0);
    ASSERT_ERR((((uint32_t)maxntdsynchoff << 11) & ~((uint32_t)0x0001F800)) == 0);
    ASSERT_ERR((((uint32_t)maxptdsynchoff << 5) & ~((uint32_t)0x000007E0)) == 0);
    ASSERT_ERR((((uint32_t)stofdcompen << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)cpeslopeesten << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)cpefdcompen << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)stoslopeesten << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)stotdcompen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR,  ((uint32_t)cfgsto4tdcomp << 31) | ((uint32_t)cfgcpewalk << 30) | ((uint32_t)cfgstoref << 29) | ((uint32_t)cfgcperef << 28) | ((uint32_t)estsecen << 26) | ((uint32_t)cpeslopelght << 25) | ((uint32_t)stoslopelght << 24) | ((uint32_t)cpemode << 23) | ((uint32_t)stomode << 22) | ((uint32_t)stocomptime << 17) | ((uint32_t)maxntdsynchoff << 11) | ((uint32_t)maxptdsynchoff << 5) | ((uint32_t)stofdcompen << 4) | ((uint32_t)cpeslopeesten << 3) | ((uint32_t)cpefdcompen << 2) | ((uint32_t)stoslopeesten << 1) | ((uint32_t)stotdcompen << 0));
}

/**
 * @brief Unpacks FDOCTRL0's fields from current value of the FDOCTRL0 register.
 *
 * Reads the FDOCTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] cfgsto4tdcomp - Will be populated with the current value of this field from the register.
 * @param[out] cfgcpewalk - Will be populated with the current value of this field from the register.
 * @param[out] cfgstoref - Will be populated with the current value of this field from the register.
 * @param[out] cfgcperef - Will be populated with the current value of this field from the register.
 * @param[out] estsecen - Will be populated with the current value of this field from the register.
 * @param[out] cpeslopelght - Will be populated with the current value of this field from the register.
 * @param[out] stoslopelght - Will be populated with the current value of this field from the register.
 * @param[out] cpemode - Will be populated with the current value of this field from the register.
 * @param[out] stomode - Will be populated with the current value of this field from the register.
 * @param[out] stocomptime - Will be populated with the current value of this field from the register.
 * @param[out] maxntdsynchoff - Will be populated with the current value of this field from the register.
 * @param[out] maxptdsynchoff - Will be populated with the current value of this field from the register.
 * @param[out] stofdcompen - Will be populated with the current value of this field from the register.
 * @param[out] cpeslopeesten - Will be populated with the current value of this field from the register.
 * @param[out] cpefdcompen - Will be populated with the current value of this field from the register.
 * @param[out] stoslopeesten - Will be populated with the current value of this field from the register.
 * @param[out] stotdcompen - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_fdoctrl0_unpack(uint8_t* cfgsto4tdcomp, uint8_t* cfgcpewalk, uint8_t* cfgstoref, uint8_t* cfgcperef, uint8_t* estsecen, uint8_t* cpeslopelght, uint8_t* stoslopelght, uint8_t* cpemode, uint8_t* stomode, uint8_t* stocomptime, uint8_t* maxntdsynchoff, uint8_t* maxptdsynchoff, uint8_t* stofdcompen, uint8_t* cpeslopeesten, uint8_t* cpefdcompen, uint8_t* stoslopeesten, uint8_t* stotdcompen)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);

    *cfgsto4tdcomp = (localVal & ((uint32_t)0x80000000)) >> 31;
    *cfgcpewalk = (localVal & ((uint32_t)0x40000000)) >> 30;
    *cfgstoref = (localVal & ((uint32_t)0x20000000)) >> 29;
    *cfgcperef = (localVal & ((uint32_t)0x10000000)) >> 28;
    *estsecen = (localVal & ((uint32_t)0x04000000)) >> 26;
    *cpeslopelght = (localVal & ((uint32_t)0x02000000)) >> 25;
    *stoslopelght = (localVal & ((uint32_t)0x01000000)) >> 24;
    *cpemode = (localVal & ((uint32_t)0x00800000)) >> 23;
    *stomode = (localVal & ((uint32_t)0x00400000)) >> 22;
    *stocomptime = (localVal & ((uint32_t)0x003E0000)) >> 17;
    *maxntdsynchoff = (localVal & ((uint32_t)0x0001F800)) >> 11;
    *maxptdsynchoff = (localVal & ((uint32_t)0x000007E0)) >> 5;
    *stofdcompen = (localVal & ((uint32_t)0x00000010)) >> 4;
    *cpeslopeesten = (localVal & ((uint32_t)0x00000008)) >> 3;
    *cpefdcompen = (localVal & ((uint32_t)0x00000004)) >> 2;
    *stoslopeesten = (localVal & ((uint32_t)0x00000002)) >> 1;
    *stotdcompen = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the CFGSTO4TDCOMP field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the CFGSTO4TDCOMP field's value will be returned.
 *
 * @return The current value of the CFGSTO4TDCOMP field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_cfgsto4tdcomp_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the CFGSTO4TDCOMP field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgsto4tdcomp - The value to set the field to.
 */
__INLINE void mdm_cfgsto4tdcomp_setf(uint8_t cfgsto4tdcomp)
{
    ASSERT_ERR((((uint32_t)cfgsto4tdcomp << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)cfgsto4tdcomp << 31));
}

/**
 * @brief Returns the current value of the CFGCPEWALK field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the CFGCPEWALK field's value will be returned.
 *
 * @return The current value of the CFGCPEWALK field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_cfgcpewalk_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the CFGCPEWALK field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgcpewalk - The value to set the field to.
 */
__INLINE void mdm_cfgcpewalk_setf(uint8_t cfgcpewalk)
{
    ASSERT_ERR((((uint32_t)cfgcpewalk << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)cfgcpewalk << 30));
}

/**
 * @brief Returns the current value of the CFGSTOREF field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the CFGSTOREF field's value will be returned.
 *
 * @return The current value of the CFGSTOREF field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_cfgstoref_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the CFGSTOREF field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgstoref - The value to set the field to.
 */
__INLINE void mdm_cfgstoref_setf(uint8_t cfgstoref)
{
    ASSERT_ERR((((uint32_t)cfgstoref << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)cfgstoref << 29));
}

/**
 * @brief Returns the current value of the CFGCPEREF field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the CFGCPEREF field's value will be returned.
 *
 * @return The current value of the CFGCPEREF field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_cfgcperef_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the CFGCPEREF field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgcperef - The value to set the field to.
 */
__INLINE void mdm_cfgcperef_setf(uint8_t cfgcperef)
{
    ASSERT_ERR((((uint32_t)cfgcperef << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)cfgcperef << 28));
}

/**
 * @brief Returns the current value of the ESTSECEN field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the ESTSECEN field's value will be returned.
 *
 * @return The current value of the ESTSECEN field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_estsecen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the ESTSECEN field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] estsecen - The value to set the field to.
 */
__INLINE void mdm_estsecen_setf(uint8_t estsecen)
{
    ASSERT_ERR((((uint32_t)estsecen << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)estsecen << 26));
}

/**
 * @brief Returns the current value of the CPESLOPELGHT field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the CPESLOPELGHT field's value will be returned.
 *
 * @return The current value of the CPESLOPELGHT field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_cpeslopelght_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the CPESLOPELGHT field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cpeslopelght - The value to set the field to.
 */
__INLINE void mdm_cpeslopelght_setf(uint8_t cpeslopelght)
{
    ASSERT_ERR((((uint32_t)cpeslopelght << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)cpeslopelght << 25));
}

/**
 * @brief Returns the current value of the STOSLOPELGHT field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the STOSLOPELGHT field's value will be returned.
 *
 * @return The current value of the STOSLOPELGHT field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_stoslopelght_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the STOSLOPELGHT field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stoslopelght - The value to set the field to.
 */
__INLINE void mdm_stoslopelght_setf(uint8_t stoslopelght)
{
    ASSERT_ERR((((uint32_t)stoslopelght << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)stoslopelght << 24));
}

/**
 * @brief Returns the current value of the CPEMODE field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the CPEMODE field's value will be returned.
 *
 * @return The current value of the CPEMODE field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_cpemode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

/**
 * @brief Sets the CPEMODE field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cpemode - The value to set the field to.
 */
__INLINE void mdm_cpemode_setf(uint8_t cpemode)
{
    ASSERT_ERR((((uint32_t)cpemode << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)cpemode << 23));
}

/**
 * @brief Returns the current value of the STOMODE field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the STOMODE field's value will be returned.
 *
 * @return The current value of the STOMODE field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_stomode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

/**
 * @brief Sets the STOMODE field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stomode - The value to set the field to.
 */
__INLINE void mdm_stomode_setf(uint8_t stomode)
{
    ASSERT_ERR((((uint32_t)stomode << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)stomode << 22));
}

/**
 * @brief Returns the current value of the STOCOMPTIME field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the STOCOMPTIME field's value will be returned.
 *
 * @return The current value of the STOCOMPTIME field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_stocomptime_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x003E0000)) >> 17);
}

/**
 * @brief Sets the STOCOMPTIME field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stocomptime - The value to set the field to.
 */
__INLINE void mdm_stocomptime_setf(uint8_t stocomptime)
{
    ASSERT_ERR((((uint32_t)stocomptime << 17) & ~((uint32_t)0x003E0000)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x003E0000)) | ((uint32_t)stocomptime << 17));
}

/**
 * @brief Returns the current value of the MAXNTDSYNCHOFF field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the MAXNTDSYNCHOFF field's value will be returned.
 *
 * @return The current value of the MAXNTDSYNCHOFF field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_maxntdsynchoff_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0001F800)) >> 11);
}

/**
 * @brief Sets the MAXNTDSYNCHOFF field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maxntdsynchoff - The value to set the field to.
 */
__INLINE void mdm_maxntdsynchoff_setf(uint8_t maxntdsynchoff)
{
    ASSERT_ERR((((uint32_t)maxntdsynchoff << 11) & ~((uint32_t)0x0001F800)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x0001F800)) | ((uint32_t)maxntdsynchoff << 11));
}

/**
 * @brief Returns the current value of the MAXPTDSYNCHOFF field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the MAXPTDSYNCHOFF field's value will be returned.
 *
 * @return The current value of the MAXPTDSYNCHOFF field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_maxptdsynchoff_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000007E0)) >> 5);
}

/**
 * @brief Sets the MAXPTDSYNCHOFF field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] maxptdsynchoff - The value to set the field to.
 */
__INLINE void mdm_maxptdsynchoff_setf(uint8_t maxptdsynchoff)
{
    ASSERT_ERR((((uint32_t)maxptdsynchoff << 5) & ~((uint32_t)0x000007E0)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x000007E0)) | ((uint32_t)maxptdsynchoff << 5));
}

/**
 * @brief Returns the current value of the STOFDCOMPEN field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the STOFDCOMPEN field's value will be returned.
 *
 * @return The current value of the STOFDCOMPEN field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_stofdcompen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the STOFDCOMPEN field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stofdcompen - The value to set the field to.
 */
__INLINE void mdm_stofdcompen_setf(uint8_t stofdcompen)
{
    ASSERT_ERR((((uint32_t)stofdcompen << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)stofdcompen << 4));
}

/**
 * @brief Returns the current value of the CPESLOPEESTEN field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the CPESLOPEESTEN field's value will be returned.
 *
 * @return The current value of the CPESLOPEESTEN field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_cpeslopeesten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the CPESLOPEESTEN field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cpeslopeesten - The value to set the field to.
 */
__INLINE void mdm_cpeslopeesten_setf(uint8_t cpeslopeesten)
{
    ASSERT_ERR((((uint32_t)cpeslopeesten << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)cpeslopeesten << 3));
}

/**
 * @brief Returns the current value of the CPEFDCOMPEN field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the CPEFDCOMPEN field's value will be returned.
 *
 * @return The current value of the CPEFDCOMPEN field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_cpefdcompen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the CPEFDCOMPEN field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cpefdcompen - The value to set the field to.
 */
__INLINE void mdm_cpefdcompen_setf(uint8_t cpefdcompen)
{
    ASSERT_ERR((((uint32_t)cpefdcompen << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)cpefdcompen << 2));
}

/**
 * @brief Returns the current value of the STOSLOPEESTEN field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the STOSLOPEESTEN field's value will be returned.
 *
 * @return The current value of the STOSLOPEESTEN field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_stoslopeesten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the STOSLOPEESTEN field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stoslopeesten - The value to set the field to.
 */
__INLINE void mdm_stoslopeesten_setf(uint8_t stoslopeesten)
{
    ASSERT_ERR((((uint32_t)stoslopeesten << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)stoslopeesten << 1));
}

/**
 * @brief Returns the current value of the STOTDCOMPEN field in the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read and the STOTDCOMPEN field's value will be returned.
 *
 * @return The current value of the STOTDCOMPEN field in the FDOCTRL0 register.
 */
__INLINE uint8_t mdm_stotdcompen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the STOTDCOMPEN field of the FDOCTRL0 register.
 *
 * The FDOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stotdcompen - The value to set the field to.
 */
__INLINE void mdm_stotdcompen_setf(uint8_t stotdcompen)
{
    ASSERT_ERR((((uint32_t)stotdcompen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_FDOCTRL0_ADDR, (REG_PL_RD(MDM_FDOCTRL0_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)stotdcompen << 0));
}

/// @}

/**
 * @name FDOCTRL1 register definitions
 * <table>
 * <caption>FDOCTRL1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:00 <td> STOSLOPEFORCED <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FDOCTRL1 register
#define MDM_FDOCTRL1_ADDR   0x24C0080C
/// Offset of the FDOCTRL1 register from the base address
#define MDM_FDOCTRL1_OFFSET 0x0000080C
/// Index of the FDOCTRL1 register
#define MDM_FDOCTRL1_INDEX  0x00000203
/// Reset value of the FDOCTRL1 register
#define MDM_FDOCTRL1_RESET  0x00000000

/**
 * @brief Returns the current value of the FDOCTRL1 register.
 * The FDOCTRL1 register will be read and its value returned.
 * @return The current value of the FDOCTRL1 register.
 */
__INLINE uint32_t mdm_fdoctrl1_get(void)
{
    return REG_PL_RD(MDM_FDOCTRL1_ADDR);
}

/**
 * @brief Sets the FDOCTRL1 register to a value.
 * The FDOCTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_fdoctrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_FDOCTRL1_ADDR, value);
}

// field definitions
/// STOSLOPEFORCED field mask
#define MDM_STOSLOPEFORCED_MASK   ((uint32_t)0x003FFFFF)
/// STOSLOPEFORCED field LSB position
#define MDM_STOSLOPEFORCED_LSB    0
/// STOSLOPEFORCED field width
#define MDM_STOSLOPEFORCED_WIDTH  ((uint32_t)0x00000016)

/// STOSLOPEFORCED field reset value
#define MDM_STOSLOPEFORCED_RST    0x0

/**
 * @brief Returns the current value of the STOSLOPEFORCED field in the FDOCTRL1 register.
 *
 * The FDOCTRL1 register will be read and the STOSLOPEFORCED field's value will be returned.
 *
 * @return The current value of the STOSLOPEFORCED field in the FDOCTRL1 register.
 */
__INLINE uint32_t mdm_stoslopeforced_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x003FFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the STOSLOPEFORCED field of the FDOCTRL1 register.
 *
 * The FDOCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] stoslopeforced - The value to set the field to.
 */
__INLINE void mdm_stoslopeforced_setf(uint32_t stoslopeforced)
{
    ASSERT_ERR((((uint32_t)stoslopeforced << 0) & ~((uint32_t)0x003FFFFF)) == 0);
    REG_PL_WR(MDM_FDOCTRL1_ADDR, (uint32_t)stoslopeforced << 0);
}

/// @}

/**
 * @name FDOCTRL2 register definitions
 * <table>
 * <caption>FDOCTRL2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:00 <td> CPESLOPEFORCED <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the FDOCTRL2 register
#define MDM_FDOCTRL2_ADDR   0x24C00810
/// Offset of the FDOCTRL2 register from the base address
#define MDM_FDOCTRL2_OFFSET 0x00000810
/// Index of the FDOCTRL2 register
#define MDM_FDOCTRL2_INDEX  0x00000204
/// Reset value of the FDOCTRL2 register
#define MDM_FDOCTRL2_RESET  0x00000000

/**
 * @brief Returns the current value of the FDOCTRL2 register.
 * The FDOCTRL2 register will be read and its value returned.
 * @return The current value of the FDOCTRL2 register.
 */
__INLINE uint32_t mdm_fdoctrl2_get(void)
{
    return REG_PL_RD(MDM_FDOCTRL2_ADDR);
}

/**
 * @brief Sets the FDOCTRL2 register to a value.
 * The FDOCTRL2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_fdoctrl2_set(uint32_t value)
{
    REG_PL_WR(MDM_FDOCTRL2_ADDR, value);
}

// field definitions
/// CPESLOPEFORCED field mask
#define MDM_CPESLOPEFORCED_MASK   ((uint32_t)0x003FFFFF)
/// CPESLOPEFORCED field LSB position
#define MDM_CPESLOPEFORCED_LSB    0
/// CPESLOPEFORCED field width
#define MDM_CPESLOPEFORCED_WIDTH  ((uint32_t)0x00000016)

/// CPESLOPEFORCED field reset value
#define MDM_CPESLOPEFORCED_RST    0x0

/**
 * @brief Returns the current value of the CPESLOPEFORCED field in the FDOCTRL2 register.
 *
 * The FDOCTRL2 register will be read and the CPESLOPEFORCED field's value will be returned.
 *
 * @return The current value of the CPESLOPEFORCED field in the FDOCTRL2 register.
 */
__INLINE uint32_t mdm_cpeslopeforced_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_FDOCTRL2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x003FFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the CPESLOPEFORCED field of the FDOCTRL2 register.
 *
 * The FDOCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cpeslopeforced - The value to set the field to.
 */
__INLINE void mdm_cpeslopeforced_setf(uint32_t cpeslopeforced)
{
    ASSERT_ERR((((uint32_t)cpeslopeforced << 0) & ~((uint32_t)0x003FFFFF)) == 0);
    REG_PL_WR(MDM_FDOCTRL2_ADDR, (uint32_t)cpeslopeforced << 0);
}

/// @}

/**
 * @name EQUALCTRL0 register definitions
 * <table>
 * <caption>EQUALCTRL0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:29 <td>     FDDCWEIGHTP2 <td> R <td> R/W <td> 0x2
 * <tr><td> 28:26 <td>     FDDCWEIGHTP1 <td> R <td> R/W <td> 0x3
 * <tr><td> 25:23 <td>     FDDCWEIGHTM1 <td> R <td> R/W <td> 0x3
 * <tr><td> 22:20 <td>     FDDCWEIGHTM2 <td> R <td> R/W <td> 0x2
 * <tr><td> 19:12 <td> NOISEVARADJUSTDB <td> R <td> R/W <td> 0x0
 * <tr><td> 11:04 <td> FDDCSNRTHRESHOLD <td> R <td> R/W <td> 0x1E
 * <tr><td> 03    <td>       FDDCCOMPEN <td> R <td> R/W <td> 1
 * </table>
 *
 * @{
 */

/// Address of the EQUALCTRL0 register
#define MDM_EQUALCTRL0_ADDR   0x24C00814
/// Offset of the EQUALCTRL0 register from the base address
#define MDM_EQUALCTRL0_OFFSET 0x00000814
/// Index of the EQUALCTRL0 register
#define MDM_EQUALCTRL0_INDEX  0x00000205
/// Reset value of the EQUALCTRL0 register
#define MDM_EQUALCTRL0_RESET  0x4DA001E8

/**
 * @brief Returns the current value of the EQUALCTRL0 register.
 * The EQUALCTRL0 register will be read and its value returned.
 * @return The current value of the EQUALCTRL0 register.
 */
__INLINE uint32_t mdm_equalctrl0_get(void)
{
    return REG_PL_RD(MDM_EQUALCTRL0_ADDR);
}

/**
 * @brief Sets the EQUALCTRL0 register to a value.
 * The EQUALCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_equalctrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_EQUALCTRL0_ADDR, value);
}

// field definitions
/// FDDCWEIGHTP2 field mask
#define MDM_FDDCWEIGHTP2_MASK       ((uint32_t)0xE0000000)
/// FDDCWEIGHTP2 field LSB position
#define MDM_FDDCWEIGHTP2_LSB        29
/// FDDCWEIGHTP2 field width
#define MDM_FDDCWEIGHTP2_WIDTH      ((uint32_t)0x00000003)
/// FDDCWEIGHTP1 field mask
#define MDM_FDDCWEIGHTP1_MASK       ((uint32_t)0x1C000000)
/// FDDCWEIGHTP1 field LSB position
#define MDM_FDDCWEIGHTP1_LSB        26
/// FDDCWEIGHTP1 field width
#define MDM_FDDCWEIGHTP1_WIDTH      ((uint32_t)0x00000003)
/// FDDCWEIGHTM1 field mask
#define MDM_FDDCWEIGHTM1_MASK       ((uint32_t)0x03800000)
/// FDDCWEIGHTM1 field LSB position
#define MDM_FDDCWEIGHTM1_LSB        23
/// FDDCWEIGHTM1 field width
#define MDM_FDDCWEIGHTM1_WIDTH      ((uint32_t)0x00000003)
/// FDDCWEIGHTM2 field mask
#define MDM_FDDCWEIGHTM2_MASK       ((uint32_t)0x00700000)
/// FDDCWEIGHTM2 field LSB position
#define MDM_FDDCWEIGHTM2_LSB        20
/// FDDCWEIGHTM2 field width
#define MDM_FDDCWEIGHTM2_WIDTH      ((uint32_t)0x00000003)
/// NOISEVARADJUSTDB field mask
#define MDM_NOISEVARADJUSTDB_MASK   ((uint32_t)0x000FF000)
/// NOISEVARADJUSTDB field LSB position
#define MDM_NOISEVARADJUSTDB_LSB    12
/// NOISEVARADJUSTDB field width
#define MDM_NOISEVARADJUSTDB_WIDTH  ((uint32_t)0x00000008)
/// FDDCSNRTHRESHOLD field mask
#define MDM_FDDCSNRTHRESHOLD_MASK   ((uint32_t)0x00000FF0)
/// FDDCSNRTHRESHOLD field LSB position
#define MDM_FDDCSNRTHRESHOLD_LSB    4
/// FDDCSNRTHRESHOLD field width
#define MDM_FDDCSNRTHRESHOLD_WIDTH  ((uint32_t)0x00000008)
/// FDDCCOMPEN field bit
#define MDM_FDDCCOMPEN_BIT          ((uint32_t)0x00000008)
/// FDDCCOMPEN field position
#define MDM_FDDCCOMPEN_POS          3

/// FDDCWEIGHTP2 field reset value
#define MDM_FDDCWEIGHTP2_RST        0x2
/// FDDCWEIGHTP1 field reset value
#define MDM_FDDCWEIGHTP1_RST        0x3
/// FDDCWEIGHTM1 field reset value
#define MDM_FDDCWEIGHTM1_RST        0x3
/// FDDCWEIGHTM2 field reset value
#define MDM_FDDCWEIGHTM2_RST        0x2
/// NOISEVARADJUSTDB field reset value
#define MDM_NOISEVARADJUSTDB_RST    0x0
/// FDDCSNRTHRESHOLD field reset value
#define MDM_FDDCSNRTHRESHOLD_RST    0x1E
/// FDDCCOMPEN field reset value
#define MDM_FDDCCOMPEN_RST          0x1

/**
 * @brief Constructs a value for the EQUALCTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] fddcweightp2 - The value to use for the FDDCWEIGHTP2 field.
 * @param[in] fddcweightp1 - The value to use for the FDDCWEIGHTP1 field.
 * @param[in] fddcweightm1 - The value to use for the FDDCWEIGHTM1 field.
 * @param[in] fddcweightm2 - The value to use for the FDDCWEIGHTM2 field.
 * @param[in] noisevaradjustdb - The value to use for the NOISEVARADJUSTDB field.
 * @param[in] fddcsnrthreshold - The value to use for the FDDCSNRTHRESHOLD field.
 * @param[in] fddccompen - The value to use for the FDDCCOMPEN field.
 */
__INLINE void mdm_equalctrl0_pack(uint8_t fddcweightp2, uint8_t fddcweightp1, uint8_t fddcweightm1, uint8_t fddcweightm2, uint8_t noisevaradjustdb, uint8_t fddcsnrthreshold, uint8_t fddccompen)
{
    ASSERT_ERR((((uint32_t)fddcweightp2 << 29) & ~((uint32_t)0xE0000000)) == 0);
    ASSERT_ERR((((uint32_t)fddcweightp1 << 26) & ~((uint32_t)0x1C000000)) == 0);
    ASSERT_ERR((((uint32_t)fddcweightm1 << 23) & ~((uint32_t)0x03800000)) == 0);
    ASSERT_ERR((((uint32_t)fddcweightm2 << 20) & ~((uint32_t)0x00700000)) == 0);
    ASSERT_ERR((((uint32_t)noisevaradjustdb << 12) & ~((uint32_t)0x000FF000)) == 0);
    ASSERT_ERR((((uint32_t)fddcsnrthreshold << 4) & ~((uint32_t)0x00000FF0)) == 0);
    ASSERT_ERR((((uint32_t)fddccompen << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_EQUALCTRL0_ADDR,  ((uint32_t)fddcweightp2 << 29) | ((uint32_t)fddcweightp1 << 26) | ((uint32_t)fddcweightm1 << 23) | ((uint32_t)fddcweightm2 << 20) | ((uint32_t)noisevaradjustdb << 12) | ((uint32_t)fddcsnrthreshold << 4) | ((uint32_t)fddccompen << 3));
}

/**
 * @brief Unpacks EQUALCTRL0's fields from current value of the EQUALCTRL0 register.
 *
 * Reads the EQUALCTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] fddcweightp2 - Will be populated with the current value of this field from the register.
 * @param[out] fddcweightp1 - Will be populated with the current value of this field from the register.
 * @param[out] fddcweightm1 - Will be populated with the current value of this field from the register.
 * @param[out] fddcweightm2 - Will be populated with the current value of this field from the register.
 * @param[out] noisevaradjustdb - Will be populated with the current value of this field from the register.
 * @param[out] fddcsnrthreshold - Will be populated with the current value of this field from the register.
 * @param[out] fddccompen - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_equalctrl0_unpack(uint8_t* fddcweightp2, uint8_t* fddcweightp1, uint8_t* fddcweightm1, uint8_t* fddcweightm2, uint8_t* noisevaradjustdb, uint8_t* fddcsnrthreshold, uint8_t* fddccompen)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL0_ADDR);

    *fddcweightp2 = (localVal & ((uint32_t)0xE0000000)) >> 29;
    *fddcweightp1 = (localVal & ((uint32_t)0x1C000000)) >> 26;
    *fddcweightm1 = (localVal & ((uint32_t)0x03800000)) >> 23;
    *fddcweightm2 = (localVal & ((uint32_t)0x00700000)) >> 20;
    *noisevaradjustdb = (localVal & ((uint32_t)0x000FF000)) >> 12;
    *fddcsnrthreshold = (localVal & ((uint32_t)0x00000FF0)) >> 4;
    *fddccompen = (localVal & ((uint32_t)0x00000008)) >> 3;
}

/**
 * @brief Returns the current value of the FDDCWEIGHTP2 field in the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read and the FDDCWEIGHTP2 field's value will be returned.
 *
 * @return The current value of the FDDCWEIGHTP2 field in the EQUALCTRL0 register.
 */
__INLINE uint8_t mdm_fddcweightp2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL0_ADDR);
    return ((localVal & ((uint32_t)0xE0000000)) >> 29);
}

/**
 * @brief Sets the FDDCWEIGHTP2 field of the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddcweightp2 - The value to set the field to.
 */
__INLINE void mdm_fddcweightp2_setf(uint8_t fddcweightp2)
{
    ASSERT_ERR((((uint32_t)fddcweightp2 << 29) & ~((uint32_t)0xE0000000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL0_ADDR, (REG_PL_RD(MDM_EQUALCTRL0_ADDR) & ~((uint32_t)0xE0000000)) | ((uint32_t)fddcweightp2 << 29));
}

/**
 * @brief Returns the current value of the FDDCWEIGHTP1 field in the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read and the FDDCWEIGHTP1 field's value will be returned.
 *
 * @return The current value of the FDDCWEIGHTP1 field in the EQUALCTRL0 register.
 */
__INLINE uint8_t mdm_fddcweightp1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x1C000000)) >> 26);
}

/**
 * @brief Sets the FDDCWEIGHTP1 field of the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddcweightp1 - The value to set the field to.
 */
__INLINE void mdm_fddcweightp1_setf(uint8_t fddcweightp1)
{
    ASSERT_ERR((((uint32_t)fddcweightp1 << 26) & ~((uint32_t)0x1C000000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL0_ADDR, (REG_PL_RD(MDM_EQUALCTRL0_ADDR) & ~((uint32_t)0x1C000000)) | ((uint32_t)fddcweightp1 << 26));
}

/**
 * @brief Returns the current value of the FDDCWEIGHTM1 field in the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read and the FDDCWEIGHTM1 field's value will be returned.
 *
 * @return The current value of the FDDCWEIGHTM1 field in the EQUALCTRL0 register.
 */
__INLINE uint8_t mdm_fddcweightm1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x03800000)) >> 23);
}

/**
 * @brief Sets the FDDCWEIGHTM1 field of the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddcweightm1 - The value to set the field to.
 */
__INLINE void mdm_fddcweightm1_setf(uint8_t fddcweightm1)
{
    ASSERT_ERR((((uint32_t)fddcweightm1 << 23) & ~((uint32_t)0x03800000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL0_ADDR, (REG_PL_RD(MDM_EQUALCTRL0_ADDR) & ~((uint32_t)0x03800000)) | ((uint32_t)fddcweightm1 << 23));
}

/**
 * @brief Returns the current value of the FDDCWEIGHTM2 field in the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read and the FDDCWEIGHTM2 field's value will be returned.
 *
 * @return The current value of the FDDCWEIGHTM2 field in the EQUALCTRL0 register.
 */
__INLINE uint8_t mdm_fddcweightm2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

/**
 * @brief Sets the FDDCWEIGHTM2 field of the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddcweightm2 - The value to set the field to.
 */
__INLINE void mdm_fddcweightm2_setf(uint8_t fddcweightm2)
{
    ASSERT_ERR((((uint32_t)fddcweightm2 << 20) & ~((uint32_t)0x00700000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL0_ADDR, (REG_PL_RD(MDM_EQUALCTRL0_ADDR) & ~((uint32_t)0x00700000)) | ((uint32_t)fddcweightm2 << 20));
}

/**
 * @brief Returns the current value of the NOISEVARADJUSTDB field in the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read and the NOISEVARADJUSTDB field's value will be returned.
 *
 * @return The current value of the NOISEVARADJUSTDB field in the EQUALCTRL0 register.
 */
__INLINE uint8_t mdm_noisevaradjustdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000FF000)) >> 12);
}

/**
 * @brief Sets the NOISEVARADJUSTDB field of the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] noisevaradjustdb - The value to set the field to.
 */
__INLINE void mdm_noisevaradjustdb_setf(uint8_t noisevaradjustdb)
{
    ASSERT_ERR((((uint32_t)noisevaradjustdb << 12) & ~((uint32_t)0x000FF000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL0_ADDR, (REG_PL_RD(MDM_EQUALCTRL0_ADDR) & ~((uint32_t)0x000FF000)) | ((uint32_t)noisevaradjustdb << 12));
}

/**
 * @brief Returns the current value of the FDDCSNRTHRESHOLD field in the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read and the FDDCSNRTHRESHOLD field's value will be returned.
 *
 * @return The current value of the FDDCSNRTHRESHOLD field in the EQUALCTRL0 register.
 */
__INLINE uint8_t mdm_fddcsnrthreshold_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000FF0)) >> 4);
}

/**
 * @brief Sets the FDDCSNRTHRESHOLD field of the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddcsnrthreshold - The value to set the field to.
 */
__INLINE void mdm_fddcsnrthreshold_setf(uint8_t fddcsnrthreshold)
{
    ASSERT_ERR((((uint32_t)fddcsnrthreshold << 4) & ~((uint32_t)0x00000FF0)) == 0);
    REG_PL_WR(MDM_EQUALCTRL0_ADDR, (REG_PL_RD(MDM_EQUALCTRL0_ADDR) & ~((uint32_t)0x00000FF0)) | ((uint32_t)fddcsnrthreshold << 4));
}

/**
 * @brief Returns the current value of the FDDCCOMPEN field in the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read and the FDDCCOMPEN field's value will be returned.
 *
 * @return The current value of the FDDCCOMPEN field in the EQUALCTRL0 register.
 */
__INLINE uint8_t mdm_fddccompen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the FDDCCOMPEN field of the EQUALCTRL0 register.
 *
 * The EQUALCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] fddccompen - The value to set the field to.
 */
__INLINE void mdm_fddccompen_setf(uint8_t fddccompen)
{
    ASSERT_ERR((((uint32_t)fddccompen << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_EQUALCTRL0_ADDR, (REG_PL_RD(MDM_EQUALCTRL0_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)fddccompen << 3));
}

/// @}

/**
 * @name SMOOTHCTRL register definitions
 * <table>
 * <caption>SMOOTHCTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 27:20 <td> TDCYCROTVAL80 <td> R <td> R/W <td> 0x18
 * <tr><td> 19    <td>      SMOOTHEN <td> R <td> R/W <td> 1
 * <tr><td> 15:08 <td> TDCYCROTVAL40 <td> R <td> R/W <td> 0xC
 * <tr><td> 07:00 <td> TDCYCROTVAL20 <td> R <td> R/W <td> 0x6
 * </table>
 *
 * @{
 */

/// Address of the SMOOTHCTRL register
#define MDM_SMOOTHCTRL_ADDR   0x24C00818
/// Offset of the SMOOTHCTRL register from the base address
#define MDM_SMOOTHCTRL_OFFSET 0x00000818
/// Index of the SMOOTHCTRL register
#define MDM_SMOOTHCTRL_INDEX  0x00000206
/// Reset value of the SMOOTHCTRL register
#define MDM_SMOOTHCTRL_RESET  0x01880C06

/**
 * @brief Returns the current value of the SMOOTHCTRL register.
 * The SMOOTHCTRL register will be read and its value returned.
 * @return The current value of the SMOOTHCTRL register.
 */
__INLINE uint32_t mdm_smoothctrl_get(void)
{
    return REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
}

/**
 * @brief Sets the SMOOTHCTRL register to a value.
 * The SMOOTHCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_smoothctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, value);
}

// field definitions
/// TDCYCROTVAL80 field mask
#define MDM_TDCYCROTVAL80_MASK   ((uint32_t)0x0FF00000)
/// TDCYCROTVAL80 field LSB position
#define MDM_TDCYCROTVAL80_LSB    20
/// TDCYCROTVAL80 field width
#define MDM_TDCYCROTVAL80_WIDTH  ((uint32_t)0x00000008)
/// SMOOTHEN field bit
#define MDM_SMOOTHEN_BIT         ((uint32_t)0x00080000)
/// SMOOTHEN field position
#define MDM_SMOOTHEN_POS         19
/// TDCYCROTVAL40 field mask
#define MDM_TDCYCROTVAL40_MASK   ((uint32_t)0x0000FF00)
/// TDCYCROTVAL40 field LSB position
#define MDM_TDCYCROTVAL40_LSB    8
/// TDCYCROTVAL40 field width
#define MDM_TDCYCROTVAL40_WIDTH  ((uint32_t)0x00000008)
/// TDCYCROTVAL20 field mask
#define MDM_TDCYCROTVAL20_MASK   ((uint32_t)0x000000FF)
/// TDCYCROTVAL20 field LSB position
#define MDM_TDCYCROTVAL20_LSB    0
/// TDCYCROTVAL20 field width
#define MDM_TDCYCROTVAL20_WIDTH  ((uint32_t)0x00000008)

/// TDCYCROTVAL80 field reset value
#define MDM_TDCYCROTVAL80_RST    0x18
/// SMOOTHEN field reset value
#define MDM_SMOOTHEN_RST         0x1
/// TDCYCROTVAL40 field reset value
#define MDM_TDCYCROTVAL40_RST    0xC
/// TDCYCROTVAL20 field reset value
#define MDM_TDCYCROTVAL20_RST    0x6

/**
 * @brief Constructs a value for the SMOOTHCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tdcycrotval80 - The value to use for the TDCYCROTVAL80 field.
 * @param[in] smoothen - The value to use for the SMOOTHEN field.
 * @param[in] tdcycrotval40 - The value to use for the TDCYCROTVAL40 field.
 * @param[in] tdcycrotval20 - The value to use for the TDCYCROTVAL20 field.
 */
__INLINE void mdm_smoothctrl_pack(uint8_t tdcycrotval80, uint8_t smoothen, uint8_t tdcycrotval40, uint8_t tdcycrotval20)
{
    ASSERT_ERR((((uint32_t)tdcycrotval80 << 20) & ~((uint32_t)0x0FF00000)) == 0);
    ASSERT_ERR((((uint32_t)smoothen << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)tdcycrotval40 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)tdcycrotval20 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR,  ((uint32_t)tdcycrotval80 << 20) | ((uint32_t)smoothen << 19) | ((uint32_t)tdcycrotval40 << 8) | ((uint32_t)tdcycrotval20 << 0));
}

/**
 * @brief Unpacks SMOOTHCTRL's fields from current value of the SMOOTHCTRL register.
 *
 * Reads the SMOOTHCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tdcycrotval80 - Will be populated with the current value of this field from the register.
 * @param[out] smoothen - Will be populated with the current value of this field from the register.
 * @param[out] tdcycrotval40 - Will be populated with the current value of this field from the register.
 * @param[out] tdcycrotval20 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_smoothctrl_unpack(uint8_t* tdcycrotval80, uint8_t* smoothen, uint8_t* tdcycrotval40, uint8_t* tdcycrotval20)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);

    *tdcycrotval80 = (localVal & ((uint32_t)0x0FF00000)) >> 20;
    *smoothen = (localVal & ((uint32_t)0x00080000)) >> 19;
    *tdcycrotval40 = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *tdcycrotval20 = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the TDCYCROTVAL80 field in the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read and the TDCYCROTVAL80 field's value will be returned.
 *
 * @return The current value of the TDCYCROTVAL80 field in the SMOOTHCTRL register.
 */
__INLINE uint8_t mdm_tdcycrotval80_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
    return ((localVal & ((uint32_t)0x0FF00000)) >> 20);
}

/**
 * @brief Sets the TDCYCROTVAL80 field of the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcycrotval80 - The value to set the field to.
 */
__INLINE void mdm_tdcycrotval80_setf(uint8_t tdcycrotval80)
{
    ASSERT_ERR((((uint32_t)tdcycrotval80 << 20) & ~((uint32_t)0x0FF00000)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, (REG_PL_RD(MDM_SMOOTHCTRL_ADDR) & ~((uint32_t)0x0FF00000)) | ((uint32_t)tdcycrotval80 << 20));
}

/**
 * @brief Returns the current value of the SMOOTHEN field in the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read and the SMOOTHEN field's value will be returned.
 *
 * @return The current value of the SMOOTHEN field in the SMOOTHCTRL register.
 */
__INLINE uint8_t mdm_smoothen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

/**
 * @brief Sets the SMOOTHEN field of the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] smoothen - The value to set the field to.
 */
__INLINE void mdm_smoothen_setf(uint8_t smoothen)
{
    ASSERT_ERR((((uint32_t)smoothen << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, (REG_PL_RD(MDM_SMOOTHCTRL_ADDR) & ~((uint32_t)0x00080000)) | ((uint32_t)smoothen << 19));
}

/**
 * @brief Returns the current value of the TDCYCROTVAL40 field in the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read and the TDCYCROTVAL40 field's value will be returned.
 *
 * @return The current value of the TDCYCROTVAL40 field in the SMOOTHCTRL register.
 */
__INLINE uint8_t mdm_tdcycrotval40_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the TDCYCROTVAL40 field of the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcycrotval40 - The value to set the field to.
 */
__INLINE void mdm_tdcycrotval40_setf(uint8_t tdcycrotval40)
{
    ASSERT_ERR((((uint32_t)tdcycrotval40 << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, (REG_PL_RD(MDM_SMOOTHCTRL_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)tdcycrotval40 << 8));
}

/**
 * @brief Returns the current value of the TDCYCROTVAL20 field in the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read and the TDCYCROTVAL20 field's value will be returned.
 *
 * @return The current value of the TDCYCROTVAL20 field in the SMOOTHCTRL register.
 */
__INLINE uint8_t mdm_tdcycrotval20_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHCTRL_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the TDCYCROTVAL20 field of the SMOOTHCTRL register.
 *
 * The SMOOTHCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdcycrotval20 - The value to set the field to.
 */
__INLINE void mdm_tdcycrotval20_setf(uint8_t tdcycrotval20)
{
    ASSERT_ERR((((uint32_t)tdcycrotval20 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_SMOOTHCTRL_ADDR, (REG_PL_RD(MDM_SMOOTHCTRL_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)tdcycrotval20 << 0));
}

/// @}

/**
 * @name DCESTIMCTRL register definitions
 * <table>
 * <caption>DCESTIMCTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:28 <td> STARTHTDC <td> R <td> R/W <td> 0x0
 * <tr><td> 27:24 <td>   STARTDC <td> R <td> R/W <td> 0x2
 * <tr><td> 14:08 <td> DELAYSYNC <td> R <td> R/W <td> 0xA
 * <tr><td> 06:00 <td> WAITHTSTF <td> R <td> R/W <td> 0xF
 * </table>
 *
 * @{
 */

/// Address of the DCESTIMCTRL register
#define MDM_DCESTIMCTRL_ADDR   0x24C0081C
/// Offset of the DCESTIMCTRL register from the base address
#define MDM_DCESTIMCTRL_OFFSET 0x0000081C
/// Index of the DCESTIMCTRL register
#define MDM_DCESTIMCTRL_INDEX  0x00000207
/// Reset value of the DCESTIMCTRL register
#define MDM_DCESTIMCTRL_RESET  0x02000A0F

/**
 * @brief Returns the current value of the DCESTIMCTRL register.
 * The DCESTIMCTRL register will be read and its value returned.
 * @return The current value of the DCESTIMCTRL register.
 */
__INLINE uint32_t mdm_dcestimctrl_get(void)
{
    return REG_PL_RD(MDM_DCESTIMCTRL_ADDR);
}

/**
 * @brief Sets the DCESTIMCTRL register to a value.
 * The DCESTIMCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_dcestimctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR, value);
}

// field definitions
/// STARTHTDC field mask
#define MDM_STARTHTDC_MASK   ((uint32_t)0xF0000000)
/// STARTHTDC field LSB position
#define MDM_STARTHTDC_LSB    28
/// STARTHTDC field width
#define MDM_STARTHTDC_WIDTH  ((uint32_t)0x00000004)
/// STARTDC field mask
#define MDM_STARTDC_MASK     ((uint32_t)0x0F000000)
/// STARTDC field LSB position
#define MDM_STARTDC_LSB      24
/// STARTDC field width
#define MDM_STARTDC_WIDTH    ((uint32_t)0x00000004)
/// DELAYSYNC field mask
#define MDM_DELAYSYNC_MASK   ((uint32_t)0x00007F00)
/// DELAYSYNC field LSB position
#define MDM_DELAYSYNC_LSB    8
/// DELAYSYNC field width
#define MDM_DELAYSYNC_WIDTH  ((uint32_t)0x00000007)
/// WAITHTSTF field mask
#define MDM_WAITHTSTF_MASK   ((uint32_t)0x0000007F)
/// WAITHTSTF field LSB position
#define MDM_WAITHTSTF_LSB    0
/// WAITHTSTF field width
#define MDM_WAITHTSTF_WIDTH  ((uint32_t)0x00000007)

/// STARTHTDC field reset value
#define MDM_STARTHTDC_RST    0x0
/// STARTDC field reset value
#define MDM_STARTDC_RST      0x2
/// DELAYSYNC field reset value
#define MDM_DELAYSYNC_RST    0xA
/// WAITHTSTF field reset value
#define MDM_WAITHTSTF_RST    0xF

/**
 * @brief Constructs a value for the DCESTIMCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] starthtdc - The value to use for the STARTHTDC field.
 * @param[in] startdc - The value to use for the STARTDC field.
 * @param[in] delaysync - The value to use for the DELAYSYNC field.
 * @param[in] waithtstf - The value to use for the WAITHTSTF field.
 */
__INLINE void mdm_dcestimctrl_pack(uint8_t starthtdc, uint8_t startdc, uint8_t delaysync, uint8_t waithtstf)
{
    ASSERT_ERR((((uint32_t)starthtdc << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)startdc << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)delaysync << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)waithtstf << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR,  ((uint32_t)starthtdc << 28) | ((uint32_t)startdc << 24) | ((uint32_t)delaysync << 8) | ((uint32_t)waithtstf << 0));
}

/**
 * @brief Unpacks DCESTIMCTRL's fields from current value of the DCESTIMCTRL register.
 *
 * Reads the DCESTIMCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] starthtdc - Will be populated with the current value of this field from the register.
 * @param[out] startdc - Will be populated with the current value of this field from the register.
 * @param[out] delaysync - Will be populated with the current value of this field from the register.
 * @param[out] waithtstf - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_dcestimctrl_unpack(uint8_t* starthtdc, uint8_t* startdc, uint8_t* delaysync, uint8_t* waithtstf)
{
    uint32_t localVal = REG_PL_RD(MDM_DCESTIMCTRL_ADDR);

    *starthtdc = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *startdc = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *delaysync = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *waithtstf = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the STARTHTDC field in the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read and the STARTHTDC field's value will be returned.
 *
 * @return The current value of the STARTHTDC field in the DCESTIMCTRL register.
 */
__INLINE uint8_t mdm_starthtdc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_DCESTIMCTRL_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the STARTHTDC field of the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] starthtdc - The value to set the field to.
 */
__INLINE void mdm_starthtdc_setf(uint8_t starthtdc)
{
    ASSERT_ERR((((uint32_t)starthtdc << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR, (REG_PL_RD(MDM_DCESTIMCTRL_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)starthtdc << 28));
}

/**
 * @brief Returns the current value of the STARTDC field in the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read and the STARTDC field's value will be returned.
 *
 * @return The current value of the STARTDC field in the DCESTIMCTRL register.
 */
__INLINE uint8_t mdm_startdc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_DCESTIMCTRL_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the STARTDC field of the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] startdc - The value to set the field to.
 */
__INLINE void mdm_startdc_setf(uint8_t startdc)
{
    ASSERT_ERR((((uint32_t)startdc << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR, (REG_PL_RD(MDM_DCESTIMCTRL_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)startdc << 24));
}

/**
 * @brief Returns the current value of the DELAYSYNC field in the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read and the DELAYSYNC field's value will be returned.
 *
 * @return The current value of the DELAYSYNC field in the DCESTIMCTRL register.
 */
__INLINE uint8_t mdm_delaysync_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_DCESTIMCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the DELAYSYNC field of the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] delaysync - The value to set the field to.
 */
__INLINE void mdm_delaysync_setf(uint8_t delaysync)
{
    ASSERT_ERR((((uint32_t)delaysync << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR, (REG_PL_RD(MDM_DCESTIMCTRL_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)delaysync << 8));
}

/**
 * @brief Returns the current value of the WAITHTSTF field in the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read and the WAITHTSTF field's value will be returned.
 *
 * @return The current value of the WAITHTSTF field in the DCESTIMCTRL register.
 */
__INLINE uint8_t mdm_waithtstf_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_DCESTIMCTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the WAITHTSTF field of the DCESTIMCTRL register.
 *
 * The DCESTIMCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] waithtstf - The value to set the field to.
 */
__INLINE void mdm_waithtstf_setf(uint8_t waithtstf)
{
    ASSERT_ERR((((uint32_t)waithtstf << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_DCESTIMCTRL_ADDR, (REG_PL_RD(MDM_DCESTIMCTRL_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)waithtstf << 0));
}

/// @}

/**
 * @name RXMODE register definitions
 * <table>
 * <caption>RXMODE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 28    <td>  RXHEMUMIMOEN <td> R <td> R/W <td> 0
 * <tr><td> 25:24 <td>      RXCBWMAX <td> R <td> R/W <td> 0x0
 * <tr><td> 21    <td>       RXDCMEN <td> R <td> R/W <td> 1
 * <tr><td> 20    <td>        RXHEEN <td> R <td> R/W <td> 1
 * <tr><td> 16    <td> RXVHTMUMIMOEN <td> R <td> R/W <td> 0
 * <tr><td> 14:12 <td>  RXNDPNSTSMAX <td> R <td> R/W <td> 0x3
 * <tr><td> 09    <td>      RXSTBCEN <td> R <td> R/W <td> 1
 * <tr><td> 08    <td>      RXLDPCEN <td> R <td> R/W <td> 0
 * <tr><td> 06:04 <td>      RXNSSMAX <td> R <td> R/W <td> 0x0
 * <tr><td> 03    <td>        RXGFEN <td> R <td> R/W <td> 0
 * <tr><td> 02    <td>        RXMMEN <td> R <td> R/W <td> 1
 * <tr><td> 01    <td>       RXVHTEN <td> R <td> R/W <td> 1
 * <tr><td> 00    <td>      RXDSSSEN <td> R <td> R/W <td> 1
 * </table>
 *
 * @{
 */

/// Address of the RXMODE register
#define MDM_RXMODE_ADDR   0x24C00820
/// Offset of the RXMODE register from the base address
#define MDM_RXMODE_OFFSET 0x00000820
/// Index of the RXMODE register
#define MDM_RXMODE_INDEX  0x00000208
/// Reset value of the RXMODE register
#define MDM_RXMODE_RESET  0x00303207

/**
 * @brief Returns the current value of the RXMODE register.
 * The RXMODE register will be read and its value returned.
 * @return The current value of the RXMODE register.
 */
__INLINE uint32_t mdm_rxmode_get(void)
{
    return REG_PL_RD(MDM_RXMODE_ADDR);
}

/**
 * @brief Sets the RXMODE register to a value.
 * The RXMODE register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxmode_set(uint32_t value)
{
    REG_PL_WR(MDM_RXMODE_ADDR, value);
}

// field definitions
/// RXHEMUMIMOEN field bit
#define MDM_RXHEMUMIMOEN_BIT     ((uint32_t)0x10000000)
/// RXHEMUMIMOEN field position
#define MDM_RXHEMUMIMOEN_POS     28
/// RXCBWMAX field mask
#define MDM_RXCBWMAX_MASK        ((uint32_t)0x03000000)
/// RXCBWMAX field LSB position
#define MDM_RXCBWMAX_LSB         24
/// RXCBWMAX field width
#define MDM_RXCBWMAX_WIDTH       ((uint32_t)0x00000002)
/// RXDCMEN field bit
#define MDM_RXDCMEN_BIT          ((uint32_t)0x00200000)
/// RXDCMEN field position
#define MDM_RXDCMEN_POS          21
/// RXHEEN field bit
#define MDM_RXHEEN_BIT           ((uint32_t)0x00100000)
/// RXHEEN field position
#define MDM_RXHEEN_POS           20
/// RXVHTMUMIMOEN field bit
#define MDM_RXVHTMUMIMOEN_BIT    ((uint32_t)0x00010000)
/// RXVHTMUMIMOEN field position
#define MDM_RXVHTMUMIMOEN_POS    16
/// RXNDPNSTSMAX field mask
#define MDM_RXNDPNSTSMAX_MASK    ((uint32_t)0x00007000)
/// RXNDPNSTSMAX field LSB position
#define MDM_RXNDPNSTSMAX_LSB     12
/// RXNDPNSTSMAX field width
#define MDM_RXNDPNSTSMAX_WIDTH   ((uint32_t)0x00000003)
/// RXSTBCEN field bit
#define MDM_RXSTBCEN_BIT         ((uint32_t)0x00000200)
/// RXSTBCEN field position
#define MDM_RXSTBCEN_POS         9
/// RXLDPCEN field bit
#define MDM_RXLDPCEN_BIT         ((uint32_t)0x00000100)
/// RXLDPCEN field position
#define MDM_RXLDPCEN_POS         8
/// RXNSSMAX field mask
#define MDM_RXNSSMAX_MASK        ((uint32_t)0x00000070)
/// RXNSSMAX field LSB position
#define MDM_RXNSSMAX_LSB         4
/// RXNSSMAX field width
#define MDM_RXNSSMAX_WIDTH       ((uint32_t)0x00000003)
/// RXGFEN field bit
#define MDM_RXGFEN_BIT           ((uint32_t)0x00000008)
/// RXGFEN field position
#define MDM_RXGFEN_POS           3
/// RXMMEN field bit
#define MDM_RXMMEN_BIT           ((uint32_t)0x00000004)
/// RXMMEN field position
#define MDM_RXMMEN_POS           2
/// RXVHTEN field bit
#define MDM_RXVHTEN_BIT          ((uint32_t)0x00000002)
/// RXVHTEN field position
#define MDM_RXVHTEN_POS          1
/// RXDSSSEN field bit
#define MDM_RXDSSSEN_BIT         ((uint32_t)0x00000001)
/// RXDSSSEN field position
#define MDM_RXDSSSEN_POS         0

/// RXHEMUMIMOEN field reset value
#define MDM_RXHEMUMIMOEN_RST     0x0
/// RXCBWMAX field reset value
#define MDM_RXCBWMAX_RST         0x0
/// RXDCMEN field reset value
#define MDM_RXDCMEN_RST          0x1
/// RXHEEN field reset value
#define MDM_RXHEEN_RST           0x1
/// RXVHTMUMIMOEN field reset value
#define MDM_RXVHTMUMIMOEN_RST    0x0
/// RXNDPNSTSMAX field reset value
#define MDM_RXNDPNSTSMAX_RST     0x3
/// RXSTBCEN field reset value
#define MDM_RXSTBCEN_RST         0x1
/// RXLDPCEN field reset value
#define MDM_RXLDPCEN_RST         0x0
/// RXNSSMAX field reset value
#define MDM_RXNSSMAX_RST         0x0
/// RXGFEN field reset value
#define MDM_RXGFEN_RST           0x0
/// RXMMEN field reset value
#define MDM_RXMMEN_RST           0x1
/// RXVHTEN field reset value
#define MDM_RXVHTEN_RST          0x1
/// RXDSSSEN field reset value
#define MDM_RXDSSSEN_RST         0x1

/**
 * @brief Constructs a value for the RXMODE register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxhemumimoen - The value to use for the RXHEMUMIMOEN field.
 * @param[in] rxcbwmax - The value to use for the RXCBWMAX field.
 * @param[in] rxdcmen - The value to use for the RXDCMEN field.
 * @param[in] rxheen - The value to use for the RXHEEN field.
 * @param[in] rxvhtmumimoen - The value to use for the RXVHTMUMIMOEN field.
 * @param[in] rxndpnstsmax - The value to use for the RXNDPNSTSMAX field.
 * @param[in] rxstbcen - The value to use for the RXSTBCEN field.
 * @param[in] rxldpcen - The value to use for the RXLDPCEN field.
 * @param[in] rxnssmax - The value to use for the RXNSSMAX field.
 * @param[in] rxgfen - The value to use for the RXGFEN field.
 * @param[in] rxmmen - The value to use for the RXMMEN field.
 * @param[in] rxvhten - The value to use for the RXVHTEN field.
 * @param[in] rxdsssen - The value to use for the RXDSSSEN field.
 */
__INLINE void mdm_rxmode_pack(uint8_t rxhemumimoen, uint8_t rxcbwmax, uint8_t rxdcmen, uint8_t rxheen, uint8_t rxvhtmumimoen, uint8_t rxndpnstsmax, uint8_t rxstbcen, uint8_t rxldpcen, uint8_t rxnssmax, uint8_t rxgfen, uint8_t rxmmen, uint8_t rxvhten, uint8_t rxdsssen)
{
    ASSERT_ERR((((uint32_t)rxhemumimoen << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)rxcbwmax << 24) & ~((uint32_t)0x03000000)) == 0);
    ASSERT_ERR((((uint32_t)rxdcmen << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)rxheen << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)rxvhtmumimoen << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)rxndpnstsmax << 12) & ~((uint32_t)0x00007000)) == 0);
    ASSERT_ERR((((uint32_t)rxstbcen << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)rxldpcen << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)rxnssmax << 4) & ~((uint32_t)0x00000070)) == 0);
    ASSERT_ERR((((uint32_t)rxgfen << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)rxmmen << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)rxvhten << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)rxdsssen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR,  ((uint32_t)rxhemumimoen << 28) | ((uint32_t)rxcbwmax << 24) | ((uint32_t)rxdcmen << 21) | ((uint32_t)rxheen << 20) | ((uint32_t)rxvhtmumimoen << 16) | ((uint32_t)rxndpnstsmax << 12) | ((uint32_t)rxstbcen << 9) | ((uint32_t)rxldpcen << 8) | ((uint32_t)rxnssmax << 4) | ((uint32_t)rxgfen << 3) | ((uint32_t)rxmmen << 2) | ((uint32_t)rxvhten << 1) | ((uint32_t)rxdsssen << 0));
}

/**
 * @brief Unpacks RXMODE's fields from current value of the RXMODE register.
 *
 * Reads the RXMODE register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxhemumimoen - Will be populated with the current value of this field from the register.
 * @param[out] rxcbwmax - Will be populated with the current value of this field from the register.
 * @param[out] rxdcmen - Will be populated with the current value of this field from the register.
 * @param[out] rxheen - Will be populated with the current value of this field from the register.
 * @param[out] rxvhtmumimoen - Will be populated with the current value of this field from the register.
 * @param[out] rxndpnstsmax - Will be populated with the current value of this field from the register.
 * @param[out] rxstbcen - Will be populated with the current value of this field from the register.
 * @param[out] rxldpcen - Will be populated with the current value of this field from the register.
 * @param[out] rxnssmax - Will be populated with the current value of this field from the register.
 * @param[out] rxgfen - Will be populated with the current value of this field from the register.
 * @param[out] rxmmen - Will be populated with the current value of this field from the register.
 * @param[out] rxvhten - Will be populated with the current value of this field from the register.
 * @param[out] rxdsssen - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxmode_unpack(uint8_t* rxhemumimoen, uint8_t* rxcbwmax, uint8_t* rxdcmen, uint8_t* rxheen, uint8_t* rxvhtmumimoen, uint8_t* rxndpnstsmax, uint8_t* rxstbcen, uint8_t* rxldpcen, uint8_t* rxnssmax, uint8_t* rxgfen, uint8_t* rxmmen, uint8_t* rxvhten, uint8_t* rxdsssen)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);

    *rxhemumimoen = (localVal & ((uint32_t)0x10000000)) >> 28;
    *rxcbwmax = (localVal & ((uint32_t)0x03000000)) >> 24;
    *rxdcmen = (localVal & ((uint32_t)0x00200000)) >> 21;
    *rxheen = (localVal & ((uint32_t)0x00100000)) >> 20;
    *rxvhtmumimoen = (localVal & ((uint32_t)0x00010000)) >> 16;
    *rxndpnstsmax = (localVal & ((uint32_t)0x00007000)) >> 12;
    *rxstbcen = (localVal & ((uint32_t)0x00000200)) >> 9;
    *rxldpcen = (localVal & ((uint32_t)0x00000100)) >> 8;
    *rxnssmax = (localVal & ((uint32_t)0x00000070)) >> 4;
    *rxgfen = (localVal & ((uint32_t)0x00000008)) >> 3;
    *rxmmen = (localVal & ((uint32_t)0x00000004)) >> 2;
    *rxvhten = (localVal & ((uint32_t)0x00000002)) >> 1;
    *rxdsssen = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the RXHEMUMIMOEN field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXHEMUMIMOEN field's value will be returned.
 *
 * @return The current value of the RXHEMUMIMOEN field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxhemumimoen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the RXHEMUMIMOEN field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxhemumimoen - The value to set the field to.
 */
__INLINE void mdm_rxhemumimoen_setf(uint8_t rxhemumimoen)
{
    ASSERT_ERR((((uint32_t)rxhemumimoen << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)rxhemumimoen << 28));
}

/**
 * @brief Returns the current value of the RXCBWMAX field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXCBWMAX field's value will be returned.
 *
 * @return The current value of the RXCBWMAX field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxcbwmax_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

/**
 * @brief Sets the RXCBWMAX field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxcbwmax - The value to set the field to.
 */
__INLINE void mdm_rxcbwmax_setf(uint8_t rxcbwmax)
{
    ASSERT_ERR((((uint32_t)rxcbwmax << 24) & ~((uint32_t)0x03000000)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x03000000)) | ((uint32_t)rxcbwmax << 24));
}

/**
 * @brief Returns the current value of the RXDCMEN field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXDCMEN field's value will be returned.
 *
 * @return The current value of the RXDCMEN field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxdcmen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

/**
 * @brief Sets the RXDCMEN field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxdcmen - The value to set the field to.
 */
__INLINE void mdm_rxdcmen_setf(uint8_t rxdcmen)
{
    ASSERT_ERR((((uint32_t)rxdcmen << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x00200000)) | ((uint32_t)rxdcmen << 21));
}

/**
 * @brief Returns the current value of the RXHEEN field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXHEEN field's value will be returned.
 *
 * @return The current value of the RXHEEN field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxheen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the RXHEEN field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxheen - The value to set the field to.
 */
__INLINE void mdm_rxheen_setf(uint8_t rxheen)
{
    ASSERT_ERR((((uint32_t)rxheen << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)rxheen << 20));
}

/**
 * @brief Returns the current value of the RXVHTMUMIMOEN field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXVHTMUMIMOEN field's value will be returned.
 *
 * @return The current value of the RXVHTMUMIMOEN field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxvhtmumimoen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the RXVHTMUMIMOEN field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxvhtmumimoen - The value to set the field to.
 */
__INLINE void mdm_rxvhtmumimoen_setf(uint8_t rxvhtmumimoen)
{
    ASSERT_ERR((((uint32_t)rxvhtmumimoen << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)rxvhtmumimoen << 16));
}

/**
 * @brief Returns the current value of the RXNDPNSTSMAX field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXNDPNSTSMAX field's value will be returned.
 *
 * @return The current value of the RXNDPNSTSMAX field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxndpnstsmax_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

/**
 * @brief Sets the RXNDPNSTSMAX field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxndpnstsmax - The value to set the field to.
 */
__INLINE void mdm_rxndpnstsmax_setf(uint8_t rxndpnstsmax)
{
    ASSERT_ERR((((uint32_t)rxndpnstsmax << 12) & ~((uint32_t)0x00007000)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x00007000)) | ((uint32_t)rxndpnstsmax << 12));
}

/**
 * @brief Returns the current value of the RXSTBCEN field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXSTBCEN field's value will be returned.
 *
 * @return The current value of the RXSTBCEN field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxstbcen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the RXSTBCEN field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxstbcen - The value to set the field to.
 */
__INLINE void mdm_rxstbcen_setf(uint8_t rxstbcen)
{
    ASSERT_ERR((((uint32_t)rxstbcen << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)rxstbcen << 9));
}

/**
 * @brief Returns the current value of the RXLDPCEN field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXLDPCEN field's value will be returned.
 *
 * @return The current value of the RXLDPCEN field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxldpcen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the RXLDPCEN field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxldpcen - The value to set the field to.
 */
__INLINE void mdm_rxldpcen_setf(uint8_t rxldpcen)
{
    ASSERT_ERR((((uint32_t)rxldpcen << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)rxldpcen << 8));
}

/**
 * @brief Returns the current value of the RXNSSMAX field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXNSSMAX field's value will be returned.
 *
 * @return The current value of the RXNSSMAX field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxnssmax_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

/**
 * @brief Sets the RXNSSMAX field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxnssmax - The value to set the field to.
 */
__INLINE void mdm_rxnssmax_setf(uint8_t rxnssmax)
{
    ASSERT_ERR((((uint32_t)rxnssmax << 4) & ~((uint32_t)0x00000070)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x00000070)) | ((uint32_t)rxnssmax << 4));
}

/**
 * @brief Returns the current value of the RXGFEN field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXGFEN field's value will be returned.
 *
 * @return The current value of the RXGFEN field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxgfen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the RXGFEN field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxgfen - The value to set the field to.
 */
__INLINE void mdm_rxgfen_setf(uint8_t rxgfen)
{
    ASSERT_ERR((((uint32_t)rxgfen << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)rxgfen << 3));
}

/**
 * @brief Returns the current value of the RXMMEN field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXMMEN field's value will be returned.
 *
 * @return The current value of the RXMMEN field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxmmen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the RXMMEN field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxmmen - The value to set the field to.
 */
__INLINE void mdm_rxmmen_setf(uint8_t rxmmen)
{
    ASSERT_ERR((((uint32_t)rxmmen << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)rxmmen << 2));
}

/**
 * @brief Returns the current value of the RXVHTEN field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXVHTEN field's value will be returned.
 *
 * @return The current value of the RXVHTEN field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxvhten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the RXVHTEN field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxvhten - The value to set the field to.
 */
__INLINE void mdm_rxvhten_setf(uint8_t rxvhten)
{
    ASSERT_ERR((((uint32_t)rxvhten << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)rxvhten << 1));
}

/**
 * @brief Returns the current value of the RXDSSSEN field in the RXMODE register.
 *
 * The RXMODE register will be read and the RXDSSSEN field's value will be returned.
 *
 * @return The current value of the RXDSSSEN field in the RXMODE register.
 */
__INLINE uint8_t mdm_rxdsssen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the RXDSSSEN field of the RXMODE register.
 *
 * The RXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxdsssen - The value to set the field to.
 */
__INLINE void mdm_rxdsssen_setf(uint8_t rxdsssen)
{
    ASSERT_ERR((((uint32_t)rxdsssen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_RXMODE_ADDR, (REG_PL_RD(MDM_RXMODE_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)rxdsssen << 0));
}

/// @}

/**
 * @name TXMODE register definitions
 * <table>
 * <caption>TXMODE bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:24 <td>    TXCBWMAX <td> R <td> R/W <td> 0x0
 * <tr><td> 22:20 <td>      NTXMAX <td> R <td> R/W <td> 0x1
 * <tr><td> 17    <td>      TXHEEN <td> R <td> R/W <td> 1
 * <tr><td> 16    <td>  TXMUMIMOEN <td> R <td> R/W <td> 0
 * <tr><td> 11    <td>     TXESSEN <td> R <td> R/W <td> 0
 * <tr><td> 10    <td> TXUNEQMODEN <td> R <td> R/W <td> 0
 * <tr><td> 09    <td>    TXSTBCEN <td> R <td> R/W <td> 0
 * <tr><td> 08    <td>    TXLDPCEN <td> R <td> R/W <td> 0
 * <tr><td> 06:04 <td>    TXNSSMAX <td> R <td> R/W <td> 0x1
 * <tr><td> 03    <td>      TXGFEN <td> R <td> R/W <td> 0
 * <tr><td> 02    <td>      TXMMEN <td> R <td> R/W <td> 1
 * <tr><td> 01    <td>     TXVHTEN <td> R <td> R/W <td> 1
 * <tr><td> 00    <td>    TXDSSSEN <td> R <td> R/W <td> 1
 * </table>
 *
 * @{
 */

/// Address of the TXMODE register
#define MDM_TXMODE_ADDR   0x24C00824
/// Offset of the TXMODE register from the base address
#define MDM_TXMODE_OFFSET 0x00000824
/// Index of the TXMODE register
#define MDM_TXMODE_INDEX  0x00000209
/// Reset value of the TXMODE register
#define MDM_TXMODE_RESET  0x00120017

/**
 * @brief Returns the current value of the TXMODE register.
 * The TXMODE register will be read and its value returned.
 * @return The current value of the TXMODE register.
 */
__INLINE uint32_t mdm_txmode_get(void)
{
    return REG_PL_RD(MDM_TXMODE_ADDR);
}

/**
 * @brief Sets the TXMODE register to a value.
 * The TXMODE register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_txmode_set(uint32_t value)
{
    REG_PL_WR(MDM_TXMODE_ADDR, value);
}

// field definitions
/// TXCBWMAX field mask
#define MDM_TXCBWMAX_MASK      ((uint32_t)0x03000000)
/// TXCBWMAX field LSB position
#define MDM_TXCBWMAX_LSB       24
/// TXCBWMAX field width
#define MDM_TXCBWMAX_WIDTH     ((uint32_t)0x00000002)
/// NTXMAX field mask
#define MDM_NTXMAX_MASK        ((uint32_t)0x00700000)
/// NTXMAX field LSB position
#define MDM_NTXMAX_LSB         20
/// NTXMAX field width
#define MDM_NTXMAX_WIDTH       ((uint32_t)0x00000003)
/// TXHEEN field bit
#define MDM_TXHEEN_BIT         ((uint32_t)0x00020000)
/// TXHEEN field position
#define MDM_TXHEEN_POS         17
/// TXMUMIMOEN field bit
#define MDM_TXMUMIMOEN_BIT     ((uint32_t)0x00010000)
/// TXMUMIMOEN field position
#define MDM_TXMUMIMOEN_POS     16
/// TXESSEN field bit
#define MDM_TXESSEN_BIT        ((uint32_t)0x00000800)
/// TXESSEN field position
#define MDM_TXESSEN_POS        11
/// TXUNEQMODEN field bit
#define MDM_TXUNEQMODEN_BIT    ((uint32_t)0x00000400)
/// TXUNEQMODEN field position
#define MDM_TXUNEQMODEN_POS    10
/// TXSTBCEN field bit
#define MDM_TXSTBCEN_BIT       ((uint32_t)0x00000200)
/// TXSTBCEN field position
#define MDM_TXSTBCEN_POS       9
/// TXLDPCEN field bit
#define MDM_TXLDPCEN_BIT       ((uint32_t)0x00000100)
/// TXLDPCEN field position
#define MDM_TXLDPCEN_POS       8
/// TXNSSMAX field mask
#define MDM_TXNSSMAX_MASK      ((uint32_t)0x00000070)
/// TXNSSMAX field LSB position
#define MDM_TXNSSMAX_LSB       4
/// TXNSSMAX field width
#define MDM_TXNSSMAX_WIDTH     ((uint32_t)0x00000003)
/// TXGFEN field bit
#define MDM_TXGFEN_BIT         ((uint32_t)0x00000008)
/// TXGFEN field position
#define MDM_TXGFEN_POS         3
/// TXMMEN field bit
#define MDM_TXMMEN_BIT         ((uint32_t)0x00000004)
/// TXMMEN field position
#define MDM_TXMMEN_POS         2
/// TXVHTEN field bit
#define MDM_TXVHTEN_BIT        ((uint32_t)0x00000002)
/// TXVHTEN field position
#define MDM_TXVHTEN_POS        1
/// TXDSSSEN field bit
#define MDM_TXDSSSEN_BIT       ((uint32_t)0x00000001)
/// TXDSSSEN field position
#define MDM_TXDSSSEN_POS       0

/// TXCBWMAX field reset value
#define MDM_TXCBWMAX_RST       0x0
/// NTXMAX field reset value
#define MDM_NTXMAX_RST         0x1
/// TXHEEN field reset value
#define MDM_TXHEEN_RST         0x1
/// TXMUMIMOEN field reset value
#define MDM_TXMUMIMOEN_RST     0x0
/// TXESSEN field reset value
#define MDM_TXESSEN_RST        0x0
/// TXUNEQMODEN field reset value
#define MDM_TXUNEQMODEN_RST    0x0
/// TXSTBCEN field reset value
#define MDM_TXSTBCEN_RST       0x0
/// TXLDPCEN field reset value
#define MDM_TXLDPCEN_RST       0x0
/// TXNSSMAX field reset value
#define MDM_TXNSSMAX_RST       0x1
/// TXGFEN field reset value
#define MDM_TXGFEN_RST         0x0
/// TXMMEN field reset value
#define MDM_TXMMEN_RST         0x1
/// TXVHTEN field reset value
#define MDM_TXVHTEN_RST        0x1
/// TXDSSSEN field reset value
#define MDM_TXDSSSEN_RST       0x1

/**
 * @brief Constructs a value for the TXMODE register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txcbwmax - The value to use for the TXCBWMAX field.
 * @param[in] ntxmax - The value to use for the NTXMAX field.
 * @param[in] txheen - The value to use for the TXHEEN field.
 * @param[in] txmumimoen - The value to use for the TXMUMIMOEN field.
 * @param[in] txessen - The value to use for the TXESSEN field.
 * @param[in] txuneqmoden - The value to use for the TXUNEQMODEN field.
 * @param[in] txstbcen - The value to use for the TXSTBCEN field.
 * @param[in] txldpcen - The value to use for the TXLDPCEN field.
 * @param[in] txnssmax - The value to use for the TXNSSMAX field.
 * @param[in] txgfen - The value to use for the TXGFEN field.
 * @param[in] txmmen - The value to use for the TXMMEN field.
 * @param[in] txvhten - The value to use for the TXVHTEN field.
 * @param[in] txdsssen - The value to use for the TXDSSSEN field.
 */
__INLINE void mdm_txmode_pack(uint8_t txcbwmax, uint8_t ntxmax, uint8_t txheen, uint8_t txmumimoen, uint8_t txessen, uint8_t txuneqmoden, uint8_t txstbcen, uint8_t txldpcen, uint8_t txnssmax, uint8_t txgfen, uint8_t txmmen, uint8_t txvhten, uint8_t txdsssen)
{
    ASSERT_ERR((((uint32_t)txcbwmax << 24) & ~((uint32_t)0x03000000)) == 0);
    ASSERT_ERR((((uint32_t)ntxmax << 20) & ~((uint32_t)0x00700000)) == 0);
    ASSERT_ERR((((uint32_t)txheen << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)txmumimoen << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)txessen << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)txuneqmoden << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)txstbcen << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)txldpcen << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)txnssmax << 4) & ~((uint32_t)0x00000070)) == 0);
    ASSERT_ERR((((uint32_t)txgfen << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)txmmen << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)txvhten << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)txdsssen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR,  ((uint32_t)txcbwmax << 24) | ((uint32_t)ntxmax << 20) | ((uint32_t)txheen << 17) | ((uint32_t)txmumimoen << 16) | ((uint32_t)txessen << 11) | ((uint32_t)txuneqmoden << 10) | ((uint32_t)txstbcen << 9) | ((uint32_t)txldpcen << 8) | ((uint32_t)txnssmax << 4) | ((uint32_t)txgfen << 3) | ((uint32_t)txmmen << 2) | ((uint32_t)txvhten << 1) | ((uint32_t)txdsssen << 0));
}

/**
 * @brief Unpacks TXMODE's fields from current value of the TXMODE register.
 *
 * Reads the TXMODE register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txcbwmax - Will be populated with the current value of this field from the register.
 * @param[out] ntxmax - Will be populated with the current value of this field from the register.
 * @param[out] txheen - Will be populated with the current value of this field from the register.
 * @param[out] txmumimoen - Will be populated with the current value of this field from the register.
 * @param[out] txessen - Will be populated with the current value of this field from the register.
 * @param[out] txuneqmoden - Will be populated with the current value of this field from the register.
 * @param[out] txstbcen - Will be populated with the current value of this field from the register.
 * @param[out] txldpcen - Will be populated with the current value of this field from the register.
 * @param[out] txnssmax - Will be populated with the current value of this field from the register.
 * @param[out] txgfen - Will be populated with the current value of this field from the register.
 * @param[out] txmmen - Will be populated with the current value of this field from the register.
 * @param[out] txvhten - Will be populated with the current value of this field from the register.
 * @param[out] txdsssen - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_txmode_unpack(uint8_t* txcbwmax, uint8_t* ntxmax, uint8_t* txheen, uint8_t* txmumimoen, uint8_t* txessen, uint8_t* txuneqmoden, uint8_t* txstbcen, uint8_t* txldpcen, uint8_t* txnssmax, uint8_t* txgfen, uint8_t* txmmen, uint8_t* txvhten, uint8_t* txdsssen)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);

    *txcbwmax = (localVal & ((uint32_t)0x03000000)) >> 24;
    *ntxmax = (localVal & ((uint32_t)0x00700000)) >> 20;
    *txheen = (localVal & ((uint32_t)0x00020000)) >> 17;
    *txmumimoen = (localVal & ((uint32_t)0x00010000)) >> 16;
    *txessen = (localVal & ((uint32_t)0x00000800)) >> 11;
    *txuneqmoden = (localVal & ((uint32_t)0x00000400)) >> 10;
    *txstbcen = (localVal & ((uint32_t)0x00000200)) >> 9;
    *txldpcen = (localVal & ((uint32_t)0x00000100)) >> 8;
    *txnssmax = (localVal & ((uint32_t)0x00000070)) >> 4;
    *txgfen = (localVal & ((uint32_t)0x00000008)) >> 3;
    *txmmen = (localVal & ((uint32_t)0x00000004)) >> 2;
    *txvhten = (localVal & ((uint32_t)0x00000002)) >> 1;
    *txdsssen = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the TXCBWMAX field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXCBWMAX field's value will be returned.
 *
 * @return The current value of the TXCBWMAX field in the TXMODE register.
 */
__INLINE uint8_t mdm_txcbwmax_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

/**
 * @brief Sets the TXCBWMAX field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txcbwmax - The value to set the field to.
 */
__INLINE void mdm_txcbwmax_setf(uint8_t txcbwmax)
{
    ASSERT_ERR((((uint32_t)txcbwmax << 24) & ~((uint32_t)0x03000000)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x03000000)) | ((uint32_t)txcbwmax << 24));
}

/**
 * @brief Returns the current value of the NTXMAX field in the TXMODE register.
 *
 * The TXMODE register will be read and the NTXMAX field's value will be returned.
 *
 * @return The current value of the NTXMAX field in the TXMODE register.
 */
__INLINE uint8_t mdm_ntxmax_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

/**
 * @brief Sets the NTXMAX field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ntxmax - The value to set the field to.
 */
__INLINE void mdm_ntxmax_setf(uint8_t ntxmax)
{
    ASSERT_ERR((((uint32_t)ntxmax << 20) & ~((uint32_t)0x00700000)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00700000)) | ((uint32_t)ntxmax << 20));
}

/**
 * @brief Returns the current value of the TXHEEN field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXHEEN field's value will be returned.
 *
 * @return The current value of the TXHEEN field in the TXMODE register.
 */
__INLINE uint8_t mdm_txheen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

/**
 * @brief Sets the TXHEEN field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txheen - The value to set the field to.
 */
__INLINE void mdm_txheen_setf(uint8_t txheen)
{
    ASSERT_ERR((((uint32_t)txheen << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)txheen << 17));
}

/**
 * @brief Returns the current value of the TXMUMIMOEN field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXMUMIMOEN field's value will be returned.
 *
 * @return The current value of the TXMUMIMOEN field in the TXMODE register.
 */
__INLINE uint8_t mdm_txmumimoen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the TXMUMIMOEN field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txmumimoen - The value to set the field to.
 */
__INLINE void mdm_txmumimoen_setf(uint8_t txmumimoen)
{
    ASSERT_ERR((((uint32_t)txmumimoen << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)txmumimoen << 16));
}

/**
 * @brief Returns the current value of the TXESSEN field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXESSEN field's value will be returned.
 *
 * @return The current value of the TXESSEN field in the TXMODE register.
 */
__INLINE uint8_t mdm_txessen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the TXESSEN field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txessen - The value to set the field to.
 */
__INLINE void mdm_txessen_setf(uint8_t txessen)
{
    ASSERT_ERR((((uint32_t)txessen << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)txessen << 11));
}

/**
 * @brief Returns the current value of the TXUNEQMODEN field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXUNEQMODEN field's value will be returned.
 *
 * @return The current value of the TXUNEQMODEN field in the TXMODE register.
 */
__INLINE uint8_t mdm_txuneqmoden_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the TXUNEQMODEN field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txuneqmoden - The value to set the field to.
 */
__INLINE void mdm_txuneqmoden_setf(uint8_t txuneqmoden)
{
    ASSERT_ERR((((uint32_t)txuneqmoden << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)txuneqmoden << 10));
}

/**
 * @brief Returns the current value of the TXSTBCEN field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXSTBCEN field's value will be returned.
 *
 * @return The current value of the TXSTBCEN field in the TXMODE register.
 */
__INLINE uint8_t mdm_txstbcen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the TXSTBCEN field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txstbcen - The value to set the field to.
 */
__INLINE void mdm_txstbcen_setf(uint8_t txstbcen)
{
    ASSERT_ERR((((uint32_t)txstbcen << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)txstbcen << 9));
}

/**
 * @brief Returns the current value of the TXLDPCEN field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXLDPCEN field's value will be returned.
 *
 * @return The current value of the TXLDPCEN field in the TXMODE register.
 */
__INLINE uint8_t mdm_txldpcen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the TXLDPCEN field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txldpcen - The value to set the field to.
 */
__INLINE void mdm_txldpcen_setf(uint8_t txldpcen)
{
    ASSERT_ERR((((uint32_t)txldpcen << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)txldpcen << 8));
}

/**
 * @brief Returns the current value of the TXNSSMAX field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXNSSMAX field's value will be returned.
 *
 * @return The current value of the TXNSSMAX field in the TXMODE register.
 */
__INLINE uint8_t mdm_txnssmax_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

/**
 * @brief Sets the TXNSSMAX field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txnssmax - The value to set the field to.
 */
__INLINE void mdm_txnssmax_setf(uint8_t txnssmax)
{
    ASSERT_ERR((((uint32_t)txnssmax << 4) & ~((uint32_t)0x00000070)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00000070)) | ((uint32_t)txnssmax << 4));
}

/**
 * @brief Returns the current value of the TXGFEN field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXGFEN field's value will be returned.
 *
 * @return The current value of the TXGFEN field in the TXMODE register.
 */
__INLINE uint8_t mdm_txgfen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the TXGFEN field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txgfen - The value to set the field to.
 */
__INLINE void mdm_txgfen_setf(uint8_t txgfen)
{
    ASSERT_ERR((((uint32_t)txgfen << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)txgfen << 3));
}

/**
 * @brief Returns the current value of the TXMMEN field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXMMEN field's value will be returned.
 *
 * @return The current value of the TXMMEN field in the TXMODE register.
 */
__INLINE uint8_t mdm_txmmen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the TXMMEN field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txmmen - The value to set the field to.
 */
__INLINE void mdm_txmmen_setf(uint8_t txmmen)
{
    ASSERT_ERR((((uint32_t)txmmen << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)txmmen << 2));
}

/**
 * @brief Returns the current value of the TXVHTEN field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXVHTEN field's value will be returned.
 *
 * @return The current value of the TXVHTEN field in the TXMODE register.
 */
__INLINE uint8_t mdm_txvhten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the TXVHTEN field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txvhten - The value to set the field to.
 */
__INLINE void mdm_txvhten_setf(uint8_t txvhten)
{
    ASSERT_ERR((((uint32_t)txvhten << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)txvhten << 1));
}

/**
 * @brief Returns the current value of the TXDSSSEN field in the TXMODE register.
 *
 * The TXMODE register will be read and the TXDSSSEN field's value will be returned.
 *
 * @return The current value of the TXDSSSEN field in the TXMODE register.
 */
__INLINE uint8_t mdm_txdsssen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXMODE_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the TXDSSSEN field of the TXMODE register.
 *
 * The TXMODE register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txdsssen - The value to set the field to.
 */
__INLINE void mdm_txdsssen_setf(uint8_t txdsssen)
{
    ASSERT_ERR((((uint32_t)txdsssen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_TXMODE_ADDR, (REG_PL_RD(MDM_TXMODE_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)txdsssen << 0));
}

/// @}

/**
 * @name EQUALCTRL1 register definitions
 * <table>
 * <caption>EQUALCTRL1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 30:28 <td> SATSB_LDPCNSTS1 <td> R <td> R/W <td> 0x0
 * <tr><td> 26:24 <td>  SATSB_BCCNSTS1 <td> R <td> R/W <td> 0x1
 * <tr><td> 22:20 <td> SATSB_LDPCNSTS0 <td> R <td> R/W <td> 0x0
 * <tr><td> 18:16 <td>  SATSB_BCCNSTS0 <td> R <td> R/W <td> 0x0
 * <tr><td> 14:12 <td>      MHSHIFTVAL <td> R <td> R/W <td> 0x1
 * <tr><td> 11:08 <td>     HSCALEINDEX <td> R <td> R/W <td> 0x9
 * <tr><td> 07:00 <td>      MHSHIFTTHR <td> R <td> R/W <td> 0x28
 * </table>
 *
 * @{
 */

/// Address of the EQUALCTRL1 register
#define MDM_EQUALCTRL1_ADDR   0x24C00828
/// Offset of the EQUALCTRL1 register from the base address
#define MDM_EQUALCTRL1_OFFSET 0x00000828
/// Index of the EQUALCTRL1 register
#define MDM_EQUALCTRL1_INDEX  0x0000020A
/// Reset value of the EQUALCTRL1 register
#define MDM_EQUALCTRL1_RESET  0x01001928

/**
 * @brief Returns the current value of the EQUALCTRL1 register.
 * The EQUALCTRL1 register will be read and its value returned.
 * @return The current value of the EQUALCTRL1 register.
 */
__INLINE uint32_t mdm_equalctrl1_get(void)
{
    return REG_PL_RD(MDM_EQUALCTRL1_ADDR);
}

/**
 * @brief Sets the EQUALCTRL1 register to a value.
 * The EQUALCTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_equalctrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_EQUALCTRL1_ADDR, value);
}

// field definitions
/// SATSB_LDPCNSTS1 field mask
#define MDM_SATSB_LDPCNSTS1_MASK   ((uint32_t)0x70000000)
/// SATSB_LDPCNSTS1 field LSB position
#define MDM_SATSB_LDPCNSTS1_LSB    28
/// SATSB_LDPCNSTS1 field width
#define MDM_SATSB_LDPCNSTS1_WIDTH  ((uint32_t)0x00000003)
/// SATSB_BCCNSTS1 field mask
#define MDM_SATSB_BCCNSTS1_MASK    ((uint32_t)0x07000000)
/// SATSB_BCCNSTS1 field LSB position
#define MDM_SATSB_BCCNSTS1_LSB     24
/// SATSB_BCCNSTS1 field width
#define MDM_SATSB_BCCNSTS1_WIDTH   ((uint32_t)0x00000003)
/// SATSB_LDPCNSTS0 field mask
#define MDM_SATSB_LDPCNSTS0_MASK   ((uint32_t)0x00700000)
/// SATSB_LDPCNSTS0 field LSB position
#define MDM_SATSB_LDPCNSTS0_LSB    20
/// SATSB_LDPCNSTS0 field width
#define MDM_SATSB_LDPCNSTS0_WIDTH  ((uint32_t)0x00000003)
/// SATSB_BCCNSTS0 field mask
#define MDM_SATSB_BCCNSTS0_MASK    ((uint32_t)0x00070000)
/// SATSB_BCCNSTS0 field LSB position
#define MDM_SATSB_BCCNSTS0_LSB     16
/// SATSB_BCCNSTS0 field width
#define MDM_SATSB_BCCNSTS0_WIDTH   ((uint32_t)0x00000003)
/// MHSHIFTVAL field mask
#define MDM_MHSHIFTVAL_MASK        ((uint32_t)0x00007000)
/// MHSHIFTVAL field LSB position
#define MDM_MHSHIFTVAL_LSB         12
/// MHSHIFTVAL field width
#define MDM_MHSHIFTVAL_WIDTH       ((uint32_t)0x00000003)
/// HSCALEINDEX field mask
#define MDM_HSCALEINDEX_MASK       ((uint32_t)0x00000F00)
/// HSCALEINDEX field LSB position
#define MDM_HSCALEINDEX_LSB        8
/// HSCALEINDEX field width
#define MDM_HSCALEINDEX_WIDTH      ((uint32_t)0x00000004)
/// MHSHIFTTHR field mask
#define MDM_MHSHIFTTHR_MASK        ((uint32_t)0x000000FF)
/// MHSHIFTTHR field LSB position
#define MDM_MHSHIFTTHR_LSB         0
/// MHSHIFTTHR field width
#define MDM_MHSHIFTTHR_WIDTH       ((uint32_t)0x00000008)

/// SATSB_LDPCNSTS1 field reset value
#define MDM_SATSB_LDPCNSTS1_RST    0x0
/// SATSB_BCCNSTS1 field reset value
#define MDM_SATSB_BCCNSTS1_RST     0x1
/// SATSB_LDPCNSTS0 field reset value
#define MDM_SATSB_LDPCNSTS0_RST    0x0
/// SATSB_BCCNSTS0 field reset value
#define MDM_SATSB_BCCNSTS0_RST     0x0
/// MHSHIFTVAL field reset value
#define MDM_MHSHIFTVAL_RST         0x1
/// HSCALEINDEX field reset value
#define MDM_HSCALEINDEX_RST        0x9
/// MHSHIFTTHR field reset value
#define MDM_MHSHIFTTHR_RST         0x28

/**
 * @brief Constructs a value for the EQUALCTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] satsbldpcnsts1 - The value to use for the SATSB_LDPCNSTS1 field.
 * @param[in] satsbbccnsts1 - The value to use for the SATSB_BCCNSTS1 field.
 * @param[in] satsbldpcnsts0 - The value to use for the SATSB_LDPCNSTS0 field.
 * @param[in] satsbbccnsts0 - The value to use for the SATSB_BCCNSTS0 field.
 * @param[in] mhshiftval - The value to use for the MHSHIFTVAL field.
 * @param[in] hscaleindex - The value to use for the HSCALEINDEX field.
 * @param[in] mhshiftthr - The value to use for the MHSHIFTTHR field.
 */
__INLINE void mdm_equalctrl1_pack(uint8_t satsbldpcnsts1, uint8_t satsbbccnsts1, uint8_t satsbldpcnsts0, uint8_t satsbbccnsts0, uint8_t mhshiftval, uint8_t hscaleindex, uint8_t mhshiftthr)
{
    ASSERT_ERR((((uint32_t)satsbldpcnsts1 << 28) & ~((uint32_t)0x70000000)) == 0);
    ASSERT_ERR((((uint32_t)satsbbccnsts1 << 24) & ~((uint32_t)0x07000000)) == 0);
    ASSERT_ERR((((uint32_t)satsbldpcnsts0 << 20) & ~((uint32_t)0x00700000)) == 0);
    ASSERT_ERR((((uint32_t)satsbbccnsts0 << 16) & ~((uint32_t)0x00070000)) == 0);
    ASSERT_ERR((((uint32_t)mhshiftval << 12) & ~((uint32_t)0x00007000)) == 0);
    ASSERT_ERR((((uint32_t)hscaleindex << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)mhshiftthr << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_EQUALCTRL1_ADDR,  ((uint32_t)satsbldpcnsts1 << 28) | ((uint32_t)satsbbccnsts1 << 24) | ((uint32_t)satsbldpcnsts0 << 20) | ((uint32_t)satsbbccnsts0 << 16) | ((uint32_t)mhshiftval << 12) | ((uint32_t)hscaleindex << 8) | ((uint32_t)mhshiftthr << 0));
}

/**
 * @brief Unpacks EQUALCTRL1's fields from current value of the EQUALCTRL1 register.
 *
 * Reads the EQUALCTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] satsbldpcnsts1 - Will be populated with the current value of this field from the register.
 * @param[out] satsbbccnsts1 - Will be populated with the current value of this field from the register.
 * @param[out] satsbldpcnsts0 - Will be populated with the current value of this field from the register.
 * @param[out] satsbbccnsts0 - Will be populated with the current value of this field from the register.
 * @param[out] mhshiftval - Will be populated with the current value of this field from the register.
 * @param[out] hscaleindex - Will be populated with the current value of this field from the register.
 * @param[out] mhshiftthr - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_equalctrl1_unpack(uint8_t* satsbldpcnsts1, uint8_t* satsbbccnsts1, uint8_t* satsbldpcnsts0, uint8_t* satsbbccnsts0, uint8_t* mhshiftval, uint8_t* hscaleindex, uint8_t* mhshiftthr)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL1_ADDR);

    *satsbldpcnsts1 = (localVal & ((uint32_t)0x70000000)) >> 28;
    *satsbbccnsts1 = (localVal & ((uint32_t)0x07000000)) >> 24;
    *satsbldpcnsts0 = (localVal & ((uint32_t)0x00700000)) >> 20;
    *satsbbccnsts0 = (localVal & ((uint32_t)0x00070000)) >> 16;
    *mhshiftval = (localVal & ((uint32_t)0x00007000)) >> 12;
    *hscaleindex = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *mhshiftthr = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the SATSB_LDPCNSTS1 field in the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read and the SATSB_LDPCNSTS1 field's value will be returned.
 *
 * @return The current value of the SATSB_LDPCNSTS1 field in the EQUALCTRL1 register.
 */
__INLINE uint8_t mdm_satsb_ldpcnsts1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

/**
 * @brief Sets the SATSB_LDPCNSTS1 field of the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satsbldpcnsts1 - The value to set the field to.
 */
__INLINE void mdm_satsb_ldpcnsts1_setf(uint8_t satsbldpcnsts1)
{
    ASSERT_ERR((((uint32_t)satsbldpcnsts1 << 28) & ~((uint32_t)0x70000000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL1_ADDR, (REG_PL_RD(MDM_EQUALCTRL1_ADDR) & ~((uint32_t)0x70000000)) | ((uint32_t)satsbldpcnsts1 << 28));
}

/**
 * @brief Returns the current value of the SATSB_BCCNSTS1 field in the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read and the SATSB_BCCNSTS1 field's value will be returned.
 *
 * @return The current value of the SATSB_BCCNSTS1 field in the EQUALCTRL1 register.
 */
__INLINE uint8_t mdm_satsb_bccnsts1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

/**
 * @brief Sets the SATSB_BCCNSTS1 field of the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satsbbccnsts1 - The value to set the field to.
 */
__INLINE void mdm_satsb_bccnsts1_setf(uint8_t satsbbccnsts1)
{
    ASSERT_ERR((((uint32_t)satsbbccnsts1 << 24) & ~((uint32_t)0x07000000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL1_ADDR, (REG_PL_RD(MDM_EQUALCTRL1_ADDR) & ~((uint32_t)0x07000000)) | ((uint32_t)satsbbccnsts1 << 24));
}

/**
 * @brief Returns the current value of the SATSB_LDPCNSTS0 field in the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read and the SATSB_LDPCNSTS0 field's value will be returned.
 *
 * @return The current value of the SATSB_LDPCNSTS0 field in the EQUALCTRL1 register.
 */
__INLINE uint8_t mdm_satsb_ldpcnsts0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

/**
 * @brief Sets the SATSB_LDPCNSTS0 field of the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satsbldpcnsts0 - The value to set the field to.
 */
__INLINE void mdm_satsb_ldpcnsts0_setf(uint8_t satsbldpcnsts0)
{
    ASSERT_ERR((((uint32_t)satsbldpcnsts0 << 20) & ~((uint32_t)0x00700000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL1_ADDR, (REG_PL_RD(MDM_EQUALCTRL1_ADDR) & ~((uint32_t)0x00700000)) | ((uint32_t)satsbldpcnsts0 << 20));
}

/**
 * @brief Returns the current value of the SATSB_BCCNSTS0 field in the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read and the SATSB_BCCNSTS0 field's value will be returned.
 *
 * @return The current value of the SATSB_BCCNSTS0 field in the EQUALCTRL1 register.
 */
__INLINE uint8_t mdm_satsb_bccnsts0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

/**
 * @brief Sets the SATSB_BCCNSTS0 field of the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satsbbccnsts0 - The value to set the field to.
 */
__INLINE void mdm_satsb_bccnsts0_setf(uint8_t satsbbccnsts0)
{
    ASSERT_ERR((((uint32_t)satsbbccnsts0 << 16) & ~((uint32_t)0x00070000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL1_ADDR, (REG_PL_RD(MDM_EQUALCTRL1_ADDR) & ~((uint32_t)0x00070000)) | ((uint32_t)satsbbccnsts0 << 16));
}

/**
 * @brief Returns the current value of the MHSHIFTVAL field in the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read and the MHSHIFTVAL field's value will be returned.
 *
 * @return The current value of the MHSHIFTVAL field in the EQUALCTRL1 register.
 */
__INLINE uint8_t mdm_mhshiftval_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

/**
 * @brief Sets the MHSHIFTVAL field of the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mhshiftval - The value to set the field to.
 */
__INLINE void mdm_mhshiftval_setf(uint8_t mhshiftval)
{
    ASSERT_ERR((((uint32_t)mhshiftval << 12) & ~((uint32_t)0x00007000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL1_ADDR, (REG_PL_RD(MDM_EQUALCTRL1_ADDR) & ~((uint32_t)0x00007000)) | ((uint32_t)mhshiftval << 12));
}

/**
 * @brief Returns the current value of the HSCALEINDEX field in the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read and the HSCALEINDEX field's value will be returned.
 *
 * @return The current value of the HSCALEINDEX field in the EQUALCTRL1 register.
 */
__INLINE uint8_t mdm_hscaleindex_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the HSCALEINDEX field of the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hscaleindex - The value to set the field to.
 */
__INLINE void mdm_hscaleindex_setf(uint8_t hscaleindex)
{
    ASSERT_ERR((((uint32_t)hscaleindex << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(MDM_EQUALCTRL1_ADDR, (REG_PL_RD(MDM_EQUALCTRL1_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)hscaleindex << 8));
}

/**
 * @brief Returns the current value of the MHSHIFTTHR field in the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read and the MHSHIFTTHR field's value will be returned.
 *
 * @return The current value of the MHSHIFTTHR field in the EQUALCTRL1 register.
 */
__INLINE uint8_t mdm_mhshiftthr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the MHSHIFTTHR field of the EQUALCTRL1 register.
 *
 * The EQUALCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mhshiftthr - The value to set the field to.
 */
__INLINE void mdm_mhshiftthr_setf(uint8_t mhshiftthr)
{
    ASSERT_ERR((((uint32_t)mhshiftthr << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_EQUALCTRL1_ADDR, (REG_PL_RD(MDM_EQUALCTRL1_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)mhshiftthr << 0));
}

/// @}

#if RW_MUMIMO_RX_EN
/**
 * @name EQUALCTRL2 register definitions
 * <table>
 * <caption>EQUALCTRL2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 30:28 <td> SATSB_MU_LDPCNSTS1 <td> R <td> R/W <td> 0x0
 * <tr><td> 26:24 <td>  SATSB_MU_BCCNSTS1 <td> R <td> R/W <td> 0x1
 * <tr><td> 22:20 <td> SATSB_MU_LDPCNSTS0 <td> R <td> R/W <td> 0x0
 * <tr><td> 18:16 <td>  SATSB_MU_BCCNSTS0 <td> R <td> R/W <td> 0x1
 * </table>
 *
 * @{
 */

/// Address of the EQUALCTRL2 register
#define MDM_EQUALCTRL2_ADDR   0x24C0082C
/// Offset of the EQUALCTRL2 register from the base address
#define MDM_EQUALCTRL2_OFFSET 0x0000082C
/// Index of the EQUALCTRL2 register
#define MDM_EQUALCTRL2_INDEX  0x0000020B
/// Reset value of the EQUALCTRL2 register
#define MDM_EQUALCTRL2_RESET  0x01010000

/**
 * @brief Returns the current value of the EQUALCTRL2 register.
 * The EQUALCTRL2 register will be read and its value returned.
 * @return The current value of the EQUALCTRL2 register.
 */
__INLINE uint32_t mdm_equalctrl2_get(void)
{
    return REG_PL_RD(MDM_EQUALCTRL2_ADDR);
}

/**
 * @brief Sets the EQUALCTRL2 register to a value.
 * The EQUALCTRL2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_equalctrl2_set(uint32_t value)
{
    REG_PL_WR(MDM_EQUALCTRL2_ADDR, value);
}

// field definitions
/// SATSB_MU_LDPCNSTS1 field mask
#define MDM_SATSB_MU_LDPCNSTS1_MASK   ((uint32_t)0x70000000)
/// SATSB_MU_LDPCNSTS1 field LSB position
#define MDM_SATSB_MU_LDPCNSTS1_LSB    28
/// SATSB_MU_LDPCNSTS1 field width
#define MDM_SATSB_MU_LDPCNSTS1_WIDTH  ((uint32_t)0x00000003)
/// SATSB_MU_BCCNSTS1 field mask
#define MDM_SATSB_MU_BCCNSTS1_MASK    ((uint32_t)0x07000000)
/// SATSB_MU_BCCNSTS1 field LSB position
#define MDM_SATSB_MU_BCCNSTS1_LSB     24
/// SATSB_MU_BCCNSTS1 field width
#define MDM_SATSB_MU_BCCNSTS1_WIDTH   ((uint32_t)0x00000003)
/// SATSB_MU_LDPCNSTS0 field mask
#define MDM_SATSB_MU_LDPCNSTS0_MASK   ((uint32_t)0x00700000)
/// SATSB_MU_LDPCNSTS0 field LSB position
#define MDM_SATSB_MU_LDPCNSTS0_LSB    20
/// SATSB_MU_LDPCNSTS0 field width
#define MDM_SATSB_MU_LDPCNSTS0_WIDTH  ((uint32_t)0x00000003)
/// SATSB_MU_BCCNSTS0 field mask
#define MDM_SATSB_MU_BCCNSTS0_MASK    ((uint32_t)0x00070000)
/// SATSB_MU_BCCNSTS0 field LSB position
#define MDM_SATSB_MU_BCCNSTS0_LSB     16
/// SATSB_MU_BCCNSTS0 field width
#define MDM_SATSB_MU_BCCNSTS0_WIDTH   ((uint32_t)0x00000003)

/// SATSB_MU_LDPCNSTS1 field reset value
#define MDM_SATSB_MU_LDPCNSTS1_RST    0x0
/// SATSB_MU_BCCNSTS1 field reset value
#define MDM_SATSB_MU_BCCNSTS1_RST     0x1
/// SATSB_MU_LDPCNSTS0 field reset value
#define MDM_SATSB_MU_LDPCNSTS0_RST    0x0
/// SATSB_MU_BCCNSTS0 field reset value
#define MDM_SATSB_MU_BCCNSTS0_RST     0x1

/**
 * @brief Constructs a value for the EQUALCTRL2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] satsbmuldpcnsts1 - The value to use for the SATSB_MU_LDPCNSTS1 field.
 * @param[in] satsbmubccnsts1 - The value to use for the SATSB_MU_BCCNSTS1 field.
 * @param[in] satsbmuldpcnsts0 - The value to use for the SATSB_MU_LDPCNSTS0 field.
 * @param[in] satsbmubccnsts0 - The value to use for the SATSB_MU_BCCNSTS0 field.
 */
__INLINE void mdm_equalctrl2_pack(uint8_t satsbmuldpcnsts1, uint8_t satsbmubccnsts1, uint8_t satsbmuldpcnsts0, uint8_t satsbmubccnsts0)
{
    ASSERT_ERR((((uint32_t)satsbmuldpcnsts1 << 28) & ~((uint32_t)0x70000000)) == 0);
    ASSERT_ERR((((uint32_t)satsbmubccnsts1 << 24) & ~((uint32_t)0x07000000)) == 0);
    ASSERT_ERR((((uint32_t)satsbmuldpcnsts0 << 20) & ~((uint32_t)0x00700000)) == 0);
    ASSERT_ERR((((uint32_t)satsbmubccnsts0 << 16) & ~((uint32_t)0x00070000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL2_ADDR,  ((uint32_t)satsbmuldpcnsts1 << 28) | ((uint32_t)satsbmubccnsts1 << 24) | ((uint32_t)satsbmuldpcnsts0 << 20) | ((uint32_t)satsbmubccnsts0 << 16));
}

/**
 * @brief Unpacks EQUALCTRL2's fields from current value of the EQUALCTRL2 register.
 *
 * Reads the EQUALCTRL2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] satsbmuldpcnsts1 - Will be populated with the current value of this field from the register.
 * @param[out] satsbmubccnsts1 - Will be populated with the current value of this field from the register.
 * @param[out] satsbmuldpcnsts0 - Will be populated with the current value of this field from the register.
 * @param[out] satsbmubccnsts0 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_equalctrl2_unpack(uint8_t* satsbmuldpcnsts1, uint8_t* satsbmubccnsts1, uint8_t* satsbmuldpcnsts0, uint8_t* satsbmubccnsts0)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL2_ADDR);

    *satsbmuldpcnsts1 = (localVal & ((uint32_t)0x70000000)) >> 28;
    *satsbmubccnsts1 = (localVal & ((uint32_t)0x07000000)) >> 24;
    *satsbmuldpcnsts0 = (localVal & ((uint32_t)0x00700000)) >> 20;
    *satsbmubccnsts0 = (localVal & ((uint32_t)0x00070000)) >> 16;
}

/**
 * @brief Returns the current value of the SATSB_MU_LDPCNSTS1 field in the EQUALCTRL2 register.
 *
 * The EQUALCTRL2 register will be read and the SATSB_MU_LDPCNSTS1 field's value will be returned.
 *
 * @return The current value of the SATSB_MU_LDPCNSTS1 field in the EQUALCTRL2 register.
 */
__INLINE uint8_t mdm_satsb_mu_ldpcnsts1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

/**
 * @brief Sets the SATSB_MU_LDPCNSTS1 field of the EQUALCTRL2 register.
 *
 * The EQUALCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satsbmuldpcnsts1 - The value to set the field to.
 */
__INLINE void mdm_satsb_mu_ldpcnsts1_setf(uint8_t satsbmuldpcnsts1)
{
    ASSERT_ERR((((uint32_t)satsbmuldpcnsts1 << 28) & ~((uint32_t)0x70000000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL2_ADDR, (REG_PL_RD(MDM_EQUALCTRL2_ADDR) & ~((uint32_t)0x70000000)) | ((uint32_t)satsbmuldpcnsts1 << 28));
}

/**
 * @brief Returns the current value of the SATSB_MU_BCCNSTS1 field in the EQUALCTRL2 register.
 *
 * The EQUALCTRL2 register will be read and the SATSB_MU_BCCNSTS1 field's value will be returned.
 *
 * @return The current value of the SATSB_MU_BCCNSTS1 field in the EQUALCTRL2 register.
 */
__INLINE uint8_t mdm_satsb_mu_bccnsts1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

/**
 * @brief Sets the SATSB_MU_BCCNSTS1 field of the EQUALCTRL2 register.
 *
 * The EQUALCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satsbmubccnsts1 - The value to set the field to.
 */
__INLINE void mdm_satsb_mu_bccnsts1_setf(uint8_t satsbmubccnsts1)
{
    ASSERT_ERR((((uint32_t)satsbmubccnsts1 << 24) & ~((uint32_t)0x07000000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL2_ADDR, (REG_PL_RD(MDM_EQUALCTRL2_ADDR) & ~((uint32_t)0x07000000)) | ((uint32_t)satsbmubccnsts1 << 24));
}

/**
 * @brief Returns the current value of the SATSB_MU_LDPCNSTS0 field in the EQUALCTRL2 register.
 *
 * The EQUALCTRL2 register will be read and the SATSB_MU_LDPCNSTS0 field's value will be returned.
 *
 * @return The current value of the SATSB_MU_LDPCNSTS0 field in the EQUALCTRL2 register.
 */
__INLINE uint8_t mdm_satsb_mu_ldpcnsts0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

/**
 * @brief Sets the SATSB_MU_LDPCNSTS0 field of the EQUALCTRL2 register.
 *
 * The EQUALCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satsbmuldpcnsts0 - The value to set the field to.
 */
__INLINE void mdm_satsb_mu_ldpcnsts0_setf(uint8_t satsbmuldpcnsts0)
{
    ASSERT_ERR((((uint32_t)satsbmuldpcnsts0 << 20) & ~((uint32_t)0x00700000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL2_ADDR, (REG_PL_RD(MDM_EQUALCTRL2_ADDR) & ~((uint32_t)0x00700000)) | ((uint32_t)satsbmuldpcnsts0 << 20));
}

/**
 * @brief Returns the current value of the SATSB_MU_BCCNSTS0 field in the EQUALCTRL2 register.
 *
 * The EQUALCTRL2 register will be read and the SATSB_MU_BCCNSTS0 field's value will be returned.
 *
 * @return The current value of the SATSB_MU_BCCNSTS0 field in the EQUALCTRL2 register.
 */
__INLINE uint8_t mdm_satsb_mu_bccnsts0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

/**
 * @brief Sets the SATSB_MU_BCCNSTS0 field of the EQUALCTRL2 register.
 *
 * The EQUALCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] satsbmubccnsts0 - The value to set the field to.
 */
__INLINE void mdm_satsb_mu_bccnsts0_setf(uint8_t satsbmubccnsts0)
{
    ASSERT_ERR((((uint32_t)satsbmubccnsts0 << 16) & ~((uint32_t)0x00070000)) == 0);
    REG_PL_WR(MDM_EQUALCTRL2_ADDR, (REG_PL_RD(MDM_EQUALCTRL2_ADDR) & ~((uint32_t)0x00070000)) | ((uint32_t)satsbmubccnsts0 << 16));
}

#endif // RW_MUMIMO_RX_EN
/// @}

/**
 * @name SMOOTHSNRTHR register definitions
 * <table>
 * <caption>SMOOTHSNRTHR bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:08 <td> SMOOTHSNRTHRHIGH <td> R <td> R/W <td> 0x1B
 * <tr><td> 07:00 <td>  SMOOTHSNRTHRMID <td> R <td> R/W <td> 0xF
 * </table>
 *
 * @{
 */

/// Address of the SMOOTHSNRTHR register
#define MDM_SMOOTHSNRTHR_ADDR   0x24C00830
/// Offset of the SMOOTHSNRTHR register from the base address
#define MDM_SMOOTHSNRTHR_OFFSET 0x00000830
/// Index of the SMOOTHSNRTHR register
#define MDM_SMOOTHSNRTHR_INDEX  0x0000020C
/// Reset value of the SMOOTHSNRTHR register
#define MDM_SMOOTHSNRTHR_RESET  0x00001B0F

/**
 * @brief Returns the current value of the SMOOTHSNRTHR register.
 * The SMOOTHSNRTHR register will be read and its value returned.
 * @return The current value of the SMOOTHSNRTHR register.
 */
__INLINE uint32_t mdm_smoothsnrthr_get(void)
{
    return REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR);
}

/**
 * @brief Sets the SMOOTHSNRTHR register to a value.
 * The SMOOTHSNRTHR register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_smoothsnrthr_set(uint32_t value)
{
    REG_PL_WR(MDM_SMOOTHSNRTHR_ADDR, value);
}

// field definitions
/// SMOOTHSNRTHRHIGH field mask
#define MDM_SMOOTHSNRTHRHIGH_MASK   ((uint32_t)0x0000FF00)
/// SMOOTHSNRTHRHIGH field LSB position
#define MDM_SMOOTHSNRTHRHIGH_LSB    8
/// SMOOTHSNRTHRHIGH field width
#define MDM_SMOOTHSNRTHRHIGH_WIDTH  ((uint32_t)0x00000008)
/// SMOOTHSNRTHRMID field mask
#define MDM_SMOOTHSNRTHRMID_MASK    ((uint32_t)0x000000FF)
/// SMOOTHSNRTHRMID field LSB position
#define MDM_SMOOTHSNRTHRMID_LSB     0
/// SMOOTHSNRTHRMID field width
#define MDM_SMOOTHSNRTHRMID_WIDTH   ((uint32_t)0x00000008)

/// SMOOTHSNRTHRHIGH field reset value
#define MDM_SMOOTHSNRTHRHIGH_RST    0x1B
/// SMOOTHSNRTHRMID field reset value
#define MDM_SMOOTHSNRTHRMID_RST     0xF

/**
 * @brief Constructs a value for the SMOOTHSNRTHR register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] smoothsnrthrhigh - The value to use for the SMOOTHSNRTHRHIGH field.
 * @param[in] smoothsnrthrmid - The value to use for the SMOOTHSNRTHRMID field.
 */
__INLINE void mdm_smoothsnrthr_pack(uint8_t smoothsnrthrhigh, uint8_t smoothsnrthrmid)
{
    ASSERT_ERR((((uint32_t)smoothsnrthrhigh << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)smoothsnrthrmid << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_SMOOTHSNRTHR_ADDR,  ((uint32_t)smoothsnrthrhigh << 8) | ((uint32_t)smoothsnrthrmid << 0));
}

/**
 * @brief Unpacks SMOOTHSNRTHR's fields from current value of the SMOOTHSNRTHR register.
 *
 * Reads the SMOOTHSNRTHR register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] smoothsnrthrhigh - Will be populated with the current value of this field from the register.
 * @param[out] smoothsnrthrmid - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_smoothsnrthr_unpack(uint8_t* smoothsnrthrhigh, uint8_t* smoothsnrthrmid)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR);

    *smoothsnrthrhigh = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *smoothsnrthrmid = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the SMOOTHSNRTHRHIGH field in the SMOOTHSNRTHR register.
 *
 * The SMOOTHSNRTHR register will be read and the SMOOTHSNRTHRHIGH field's value will be returned.
 *
 * @return The current value of the SMOOTHSNRTHRHIGH field in the SMOOTHSNRTHR register.
 */
__INLINE uint8_t mdm_smoothsnrthrhigh_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the SMOOTHSNRTHRHIGH field of the SMOOTHSNRTHR register.
 *
 * The SMOOTHSNRTHR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] smoothsnrthrhigh - The value to set the field to.
 */
__INLINE void mdm_smoothsnrthrhigh_setf(uint8_t smoothsnrthrhigh)
{
    ASSERT_ERR((((uint32_t)smoothsnrthrhigh << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(MDM_SMOOTHSNRTHR_ADDR, (REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)smoothsnrthrhigh << 8));
}

/**
 * @brief Returns the current value of the SMOOTHSNRTHRMID field in the SMOOTHSNRTHR register.
 *
 * The SMOOTHSNRTHR register will be read and the SMOOTHSNRTHRMID field's value will be returned.
 *
 * @return The current value of the SMOOTHSNRTHRMID field in the SMOOTHSNRTHR register.
 */
__INLINE uint8_t mdm_smoothsnrthrmid_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the SMOOTHSNRTHRMID field of the SMOOTHSNRTHR register.
 *
 * The SMOOTHSNRTHR register will be read, modified to contain the new field value, and written.
 *
 * @param[in] smoothsnrthrmid - The value to set the field to.
 */
__INLINE void mdm_smoothsnrthrmid_setf(uint8_t smoothsnrthrmid)
{
    ASSERT_ERR((((uint32_t)smoothsnrthrmid << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_SMOOTHSNRTHR_ADDR, (REG_PL_RD(MDM_SMOOTHSNRTHR_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)smoothsnrthrmid << 0));
}

/// @}

/**
 * @name RXTDCTRL1 register definitions
 * <table>
 * <caption>RXTDCTRL1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:24 <td> TDDCHTSTFMARGIN <td> R <td> R/W <td> 0x1
 * <tr><td> 08    <td>        RXTDDCEN <td> R <td> R/W <td> 1
 * <tr><td> 04    <td>  HTSTFMCSWAITEN <td> R <td> R/W <td> 1
 * </table>
 *
 * @{
 */

/// Address of the RXTDCTRL1 register
#define MDM_RXTDCTRL1_ADDR   0x24C00834
/// Offset of the RXTDCTRL1 register from the base address
#define MDM_RXTDCTRL1_OFFSET 0x00000834
/// Index of the RXTDCTRL1 register
#define MDM_RXTDCTRL1_INDEX  0x0000020D
/// Reset value of the RXTDCTRL1 register
#define MDM_RXTDCTRL1_RESET  0x01000110

/**
 * @brief Returns the current value of the RXTDCTRL1 register.
 * The RXTDCTRL1 register will be read and its value returned.
 * @return The current value of the RXTDCTRL1 register.
 */
__INLINE uint32_t mdm_rxtdctrl1_get(void)
{
    return REG_PL_RD(MDM_RXTDCTRL1_ADDR);
}

/**
 * @brief Sets the RXTDCTRL1 register to a value.
 * The RXTDCTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxtdctrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_RXTDCTRL1_ADDR, value);
}

// field definitions
/// TDDCHTSTFMARGIN field mask
#define MDM_TDDCHTSTFMARGIN_MASK   ((uint32_t)0xFF000000)
/// TDDCHTSTFMARGIN field LSB position
#define MDM_TDDCHTSTFMARGIN_LSB    24
/// TDDCHTSTFMARGIN field width
#define MDM_TDDCHTSTFMARGIN_WIDTH  ((uint32_t)0x00000008)
/// RXTDDCEN field bit
#define MDM_RXTDDCEN_BIT           ((uint32_t)0x00000100)
/// RXTDDCEN field position
#define MDM_RXTDDCEN_POS           8
/// HTSTFMCSWAITEN field bit
#define MDM_HTSTFMCSWAITEN_BIT     ((uint32_t)0x00000010)
/// HTSTFMCSWAITEN field position
#define MDM_HTSTFMCSWAITEN_POS     4

/// TDDCHTSTFMARGIN field reset value
#define MDM_TDDCHTSTFMARGIN_RST    0x1
/// RXTDDCEN field reset value
#define MDM_RXTDDCEN_RST           0x1
/// HTSTFMCSWAITEN field reset value
#define MDM_HTSTFMCSWAITEN_RST     0x1

/**
 * @brief Constructs a value for the RXTDCTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tddchtstfmargin - The value to use for the TDDCHTSTFMARGIN field.
 * @param[in] rxtddcen - The value to use for the RXTDDCEN field.
 * @param[in] htstfmcswaiten - The value to use for the HTSTFMCSWAITEN field.
 */
__INLINE void mdm_rxtdctrl1_pack(uint8_t tddchtstfmargin, uint8_t rxtddcen, uint8_t htstfmcswaiten)
{
    ASSERT_ERR((((uint32_t)tddchtstfmargin << 24) & ~((uint32_t)0xFF000000)) == 0);
    ASSERT_ERR((((uint32_t)rxtddcen << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)htstfmcswaiten << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_RXTDCTRL1_ADDR,  ((uint32_t)tddchtstfmargin << 24) | ((uint32_t)rxtddcen << 8) | ((uint32_t)htstfmcswaiten << 4));
}

/**
 * @brief Unpacks RXTDCTRL1's fields from current value of the RXTDCTRL1 register.
 *
 * Reads the RXTDCTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tddchtstfmargin - Will be populated with the current value of this field from the register.
 * @param[out] rxtddcen - Will be populated with the current value of this field from the register.
 * @param[out] htstfmcswaiten - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxtdctrl1_unpack(uint8_t* tddchtstfmargin, uint8_t* rxtddcen, uint8_t* htstfmcswaiten)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL1_ADDR);

    *tddchtstfmargin = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *rxtddcen = (localVal & ((uint32_t)0x00000100)) >> 8;
    *htstfmcswaiten = (localVal & ((uint32_t)0x00000010)) >> 4;
}

/**
 * @brief Returns the current value of the TDDCHTSTFMARGIN field in the RXTDCTRL1 register.
 *
 * The RXTDCTRL1 register will be read and the TDDCHTSTFMARGIN field's value will be returned.
 *
 * @return The current value of the TDDCHTSTFMARGIN field in the RXTDCTRL1 register.
 */
__INLINE uint8_t mdm_tddchtstfmargin_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL1_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

/**
 * @brief Sets the TDDCHTSTFMARGIN field of the RXTDCTRL1 register.
 *
 * The RXTDCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tddchtstfmargin - The value to set the field to.
 */
__INLINE void mdm_tddchtstfmargin_setf(uint8_t tddchtstfmargin)
{
    ASSERT_ERR((((uint32_t)tddchtstfmargin << 24) & ~((uint32_t)0xFF000000)) == 0);
    REG_PL_WR(MDM_RXTDCTRL1_ADDR, (REG_PL_RD(MDM_RXTDCTRL1_ADDR) & ~((uint32_t)0xFF000000)) | ((uint32_t)tddchtstfmargin << 24));
}

/**
 * @brief Returns the current value of the RXTDDCEN field in the RXTDCTRL1 register.
 *
 * The RXTDCTRL1 register will be read and the RXTDDCEN field's value will be returned.
 *
 * @return The current value of the RXTDDCEN field in the RXTDCTRL1 register.
 */
__INLINE uint8_t mdm_rxtddcen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the RXTDDCEN field of the RXTDCTRL1 register.
 *
 * The RXTDCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxtddcen - The value to set the field to.
 */
__INLINE void mdm_rxtddcen_setf(uint8_t rxtddcen)
{
    ASSERT_ERR((((uint32_t)rxtddcen << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(MDM_RXTDCTRL1_ADDR, (REG_PL_RD(MDM_RXTDCTRL1_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)rxtddcen << 8));
}

/**
 * @brief Returns the current value of the HTSTFMCSWAITEN field in the RXTDCTRL1 register.
 *
 * The RXTDCTRL1 register will be read and the HTSTFMCSWAITEN field's value will be returned.
 *
 * @return The current value of the HTSTFMCSWAITEN field in the RXTDCTRL1 register.
 */
__INLINE uint8_t mdm_htstfmcswaiten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the HTSTFMCSWAITEN field of the RXTDCTRL1 register.
 *
 * The RXTDCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] htstfmcswaiten - The value to set the field to.
 */
__INLINE void mdm_htstfmcswaiten_setf(uint8_t htstfmcswaiten)
{
    ASSERT_ERR((((uint32_t)htstfmcswaiten << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_RXTDCTRL1_ADDR, (REG_PL_RD(MDM_RXTDCTRL1_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)htstfmcswaiten << 4));
}

/// @}

/**
 * @name TXCTRL0 register definitions
 * <table>
 * <caption>TXCTRL0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 09:00 <td> TXSTARTDELAY <td> R <td> R/W <td> 0x1A4
 * </table>
 *
 * @{
 */

/// Address of the TXCTRL0 register
#define MDM_TXCTRL0_ADDR   0x24C00838
/// Offset of the TXCTRL0 register from the base address
#define MDM_TXCTRL0_OFFSET 0x00000838
/// Index of the TXCTRL0 register
#define MDM_TXCTRL0_INDEX  0x0000020E
/// Reset value of the TXCTRL0 register
#define MDM_TXCTRL0_RESET  0x000001A4

/**
 * @brief Returns the current value of the TXCTRL0 register.
 * The TXCTRL0 register will be read and its value returned.
 * @return The current value of the TXCTRL0 register.
 */
__INLINE uint32_t mdm_txctrl0_get(void)
{
    return REG_PL_RD(MDM_TXCTRL0_ADDR);
}

/**
 * @brief Sets the TXCTRL0 register to a value.
 * The TXCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_txctrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_TXCTRL0_ADDR, value);
}

// field definitions
/// TXSTARTDELAY field mask
#define MDM_TXSTARTDELAY_MASK   ((uint32_t)0x000003FF)
/// TXSTARTDELAY field LSB position
#define MDM_TXSTARTDELAY_LSB    0
/// TXSTARTDELAY field width
#define MDM_TXSTARTDELAY_WIDTH  ((uint32_t)0x0000000A)

/// TXSTARTDELAY field reset value
#define MDM_TXSTARTDELAY_RST    0x1A4

/**
 * @brief Returns the current value of the TXSTARTDELAY field in the TXCTRL0 register.
 *
 * The TXCTRL0 register will be read and the TXSTARTDELAY field's value will be returned.
 *
 * @return The current value of the TXSTARTDELAY field in the TXCTRL0 register.
 */
__INLINE uint16_t mdm_txstartdelay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXCTRL0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000003FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the TXSTARTDELAY field of the TXCTRL0 register.
 *
 * The TXCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txstartdelay - The value to set the field to.
 */
__INLINE void mdm_txstartdelay_setf(uint16_t txstartdelay)
{
    ASSERT_ERR((((uint32_t)txstartdelay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_TXCTRL0_ADDR, (uint32_t)txstartdelay << 0);
}

/// @}

/**
 * @name RXCTRL1 register definitions
 * <table>
 * <caption>RXCTRL1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:16 <td> RXBCCNDBPSMAXSGI <td> R <td> R/W <td> 0x618
 * <tr><td> 15:00 <td> RXBCCNDBPSMAXLGI <td> R <td> R/W <td> 0x618
 * </table>
 *
 * @{
 */

/// Address of the RXCTRL1 register
#define MDM_RXCTRL1_ADDR   0x24C0083C
/// Offset of the RXCTRL1 register from the base address
#define MDM_RXCTRL1_OFFSET 0x0000083C
/// Index of the RXCTRL1 register
#define MDM_RXCTRL1_INDEX  0x0000020F
/// Reset value of the RXCTRL1 register
#define MDM_RXCTRL1_RESET  0x06180618

/**
 * @brief Returns the current value of the RXCTRL1 register.
 * The RXCTRL1 register will be read and its value returned.
 * @return The current value of the RXCTRL1 register.
 */
__INLINE uint32_t mdm_rxctrl1_get(void)
{
    return REG_PL_RD(MDM_RXCTRL1_ADDR);
}

/**
 * @brief Sets the RXCTRL1 register to a value.
 * The RXCTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL1_ADDR, value);
}

// field definitions
/// RXBCCNDBPSMAXSGI field mask
#define MDM_RXBCCNDBPSMAXSGI_MASK   ((uint32_t)0xFFFF0000)
/// RXBCCNDBPSMAXSGI field LSB position
#define MDM_RXBCCNDBPSMAXSGI_LSB    16
/// RXBCCNDBPSMAXSGI field width
#define MDM_RXBCCNDBPSMAXSGI_WIDTH  ((uint32_t)0x00000010)
/// RXBCCNDBPSMAXLGI field mask
#define MDM_RXBCCNDBPSMAXLGI_MASK   ((uint32_t)0x0000FFFF)
/// RXBCCNDBPSMAXLGI field LSB position
#define MDM_RXBCCNDBPSMAXLGI_LSB    0
/// RXBCCNDBPSMAXLGI field width
#define MDM_RXBCCNDBPSMAXLGI_WIDTH  ((uint32_t)0x00000010)

/// RXBCCNDBPSMAXSGI field reset value
#define MDM_RXBCCNDBPSMAXSGI_RST    0x618
/// RXBCCNDBPSMAXLGI field reset value
#define MDM_RXBCCNDBPSMAXLGI_RST    0x618

/**
 * @brief Constructs a value for the RXCTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxbccndbpsmaxsgi - The value to use for the RXBCCNDBPSMAXSGI field.
 * @param[in] rxbccndbpsmaxlgi - The value to use for the RXBCCNDBPSMAXLGI field.
 */
__INLINE void mdm_rxctrl1_pack(uint16_t rxbccndbpsmaxsgi, uint16_t rxbccndbpsmaxlgi)
{
    ASSERT_ERR((((uint32_t)rxbccndbpsmaxsgi << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    ASSERT_ERR((((uint32_t)rxbccndbpsmaxlgi << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(MDM_RXCTRL1_ADDR,  ((uint32_t)rxbccndbpsmaxsgi << 16) | ((uint32_t)rxbccndbpsmaxlgi << 0));
}

/**
 * @brief Unpacks RXCTRL1's fields from current value of the RXCTRL1 register.
 *
 * Reads the RXCTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxbccndbpsmaxsgi - Will be populated with the current value of this field from the register.
 * @param[out] rxbccndbpsmaxlgi - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxctrl1_unpack(uint16_t* rxbccndbpsmaxsgi, uint16_t* rxbccndbpsmaxlgi)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL1_ADDR);

    *rxbccndbpsmaxsgi = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *rxbccndbpsmaxlgi = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the RXBCCNDBPSMAXSGI field in the RXCTRL1 register.
 *
 * The RXCTRL1 register will be read and the RXBCCNDBPSMAXSGI field's value will be returned.
 *
 * @return The current value of the RXBCCNDBPSMAXSGI field in the RXCTRL1 register.
 */
__INLINE uint16_t mdm_rxbccndbpsmaxsgi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL1_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Sets the RXBCCNDBPSMAXSGI field of the RXCTRL1 register.
 *
 * The RXCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxbccndbpsmaxsgi - The value to set the field to.
 */
__INLINE void mdm_rxbccndbpsmaxsgi_setf(uint16_t rxbccndbpsmaxsgi)
{
    ASSERT_ERR((((uint32_t)rxbccndbpsmaxsgi << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    REG_PL_WR(MDM_RXCTRL1_ADDR, (REG_PL_RD(MDM_RXCTRL1_ADDR) & ~((uint32_t)0xFFFF0000)) | ((uint32_t)rxbccndbpsmaxsgi << 16));
}

/**
 * @brief Returns the current value of the RXBCCNDBPSMAXLGI field in the RXCTRL1 register.
 *
 * The RXCTRL1 register will be read and the RXBCCNDBPSMAXLGI field's value will be returned.
 *
 * @return The current value of the RXBCCNDBPSMAXLGI field in the RXCTRL1 register.
 */
__INLINE uint16_t mdm_rxbccndbpsmaxlgi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the RXBCCNDBPSMAXLGI field of the RXCTRL1 register.
 *
 * The RXCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxbccndbpsmaxlgi - The value to set the field to.
 */
__INLINE void mdm_rxbccndbpsmaxlgi_setf(uint16_t rxbccndbpsmaxlgi)
{
    ASSERT_ERR((((uint32_t)rxbccndbpsmaxlgi << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(MDM_RXCTRL1_ADDR, (REG_PL_RD(MDM_RXCTRL1_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)rxbccndbpsmaxlgi << 0));
}

/// @}

/**
 * @name SMOOTHFORCECTRL register definitions
 * <table>
 * <caption>SMOOTHFORCECTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>     SMOOTHNONHEMODE <td> R <td> R/W <td> 0
 * <tr><td> 30    <td>   intp1xpilotcomben <td> R <td> R/W <td> 1
 * <tr><td> 29    <td>   track1xallpiloten <td> R <td> R/W <td> 1
 * <tr><td> 27    <td>       MUSMOOTHFORCE <td> R <td> R/W <td> 0
 * <tr><td> 26    <td>       BFSMOOTHFORCE <td> R <td> R/W <td> 0
 * <tr><td> 16    <td> CFGNONHTSMOOTHFORCE <td> R <td> R/W <td> 0
 * <tr><td> 01:00 <td>      CFGNONHTSMOOTH <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SMOOTHFORCECTRL register
#define MDM_SMOOTHFORCECTRL_ADDR   0x24C00840
/// Offset of the SMOOTHFORCECTRL register from the base address
#define MDM_SMOOTHFORCECTRL_OFFSET 0x00000840
/// Index of the SMOOTHFORCECTRL register
#define MDM_SMOOTHFORCECTRL_INDEX  0x00000210
/// Reset value of the SMOOTHFORCECTRL register
#define MDM_SMOOTHFORCECTRL_RESET  0x60000000

/**
 * @brief Returns the current value of the SMOOTHFORCECTRL register.
 * The SMOOTHFORCECTRL register will be read and its value returned.
 * @return The current value of the SMOOTHFORCECTRL register.
 */
__INLINE uint32_t mdm_smoothforcectrl_get(void)
{
    return REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR);
}

/**
 * @brief Sets the SMOOTHFORCECTRL register to a value.
 * The SMOOTHFORCECTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_smoothforcectrl_set(uint32_t value)
{
    REG_PL_WR(MDM_SMOOTHFORCECTRL_ADDR, value);
}

// field definitions
/// SMOOTHNONHEMODE field bit
#define MDM_SMOOTHNONHEMODE_BIT        ((uint32_t)0x80000000)
/// SMOOTHNONHEMODE field position
#define MDM_SMOOTHNONHEMODE_POS        31
/// INTP_1XPILOTCOMBEN field bit
#define MDM_INTP_1XPILOTCOMBEN_BIT     ((uint32_t)0x40000000)
/// INTP_1XPILOTCOMBEN field position
#define MDM_INTP_1XPILOTCOMBEN_POS     30
/// TRACK_1XALLPILOTEN field bit
#define MDM_TRACK_1XALLPILOTEN_BIT     ((uint32_t)0x20000000)
/// TRACK_1XALLPILOTEN field position
#define MDM_TRACK_1XALLPILOTEN_POS     29
/// MUSMOOTHFORCE field bit
#define MDM_MUSMOOTHFORCE_BIT          ((uint32_t)0x08000000)
/// MUSMOOTHFORCE field position
#define MDM_MUSMOOTHFORCE_POS          27
/// BFSMOOTHFORCE field bit
#define MDM_BFSMOOTHFORCE_BIT          ((uint32_t)0x04000000)
/// BFSMOOTHFORCE field position
#define MDM_BFSMOOTHFORCE_POS          26
/// CFGNONHTSMOOTHFORCE field bit
#define MDM_CFGNONHTSMOOTHFORCE_BIT    ((uint32_t)0x00010000)
/// CFGNONHTSMOOTHFORCE field position
#define MDM_CFGNONHTSMOOTHFORCE_POS    16
/// CFGNONHTSMOOTH field mask
#define MDM_CFGNONHTSMOOTH_MASK        ((uint32_t)0x00000003)
/// CFGNONHTSMOOTH field LSB position
#define MDM_CFGNONHTSMOOTH_LSB         0
/// CFGNONHTSMOOTH field width
#define MDM_CFGNONHTSMOOTH_WIDTH       ((uint32_t)0x00000002)

/// SMOOTHNONHEMODE field reset value
#define MDM_SMOOTHNONHEMODE_RST        0x0
/// INTP_1XPILOTCOMBEN field reset value
#define MDM_INTP_1XPILOTCOMBEN_RST     0x1
/// TRACK_1XALLPILOTEN field reset value
#define MDM_TRACK_1XALLPILOTEN_RST     0x1
/// MUSMOOTHFORCE field reset value
#define MDM_MUSMOOTHFORCE_RST          0x0
/// BFSMOOTHFORCE field reset value
#define MDM_BFSMOOTHFORCE_RST          0x0
/// CFGNONHTSMOOTHFORCE field reset value
#define MDM_CFGNONHTSMOOTHFORCE_RST    0x0
/// CFGNONHTSMOOTH field reset value
#define MDM_CFGNONHTSMOOTH_RST         0x0

/**
 * @brief Constructs a value for the SMOOTHFORCECTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] smoothnonhemode - The value to use for the SMOOTHNONHEMODE field.
 * @param[in] intp1xpilotcomben - The value to use for the intp1xpilotcomben field.
 * @param[in] track1xallpiloten - The value to use for the track1xallpiloten field.
 * @param[in] musmoothforce - The value to use for the MUSMOOTHFORCE field.
 * @param[in] bfsmoothforce - The value to use for the BFSMOOTHFORCE field.
 * @param[in] cfgnonhtsmoothforce - The value to use for the CFGNONHTSMOOTHFORCE field.
 * @param[in] cfgnonhtsmooth - The value to use for the CFGNONHTSMOOTH field.
 */
__INLINE void mdm_smoothforcectrl_pack(uint8_t smoothnonhemode, uint8_t intp1xpilotcomben, uint8_t track1xallpiloten, uint8_t musmoothforce, uint8_t bfsmoothforce, uint8_t cfgnonhtsmoothforce, uint8_t cfgnonhtsmooth)
{
    ASSERT_ERR((((uint32_t)smoothnonhemode << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)intp1xpilotcomben << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)track1xallpiloten << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)musmoothforce << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)bfsmoothforce << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)cfgnonhtsmoothforce << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)cfgnonhtsmooth << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(MDM_SMOOTHFORCECTRL_ADDR,  ((uint32_t)smoothnonhemode << 31) | ((uint32_t)intp1xpilotcomben << 30) | ((uint32_t)track1xallpiloten << 29) | ((uint32_t)musmoothforce << 27) | ((uint32_t)bfsmoothforce << 26) | ((uint32_t)cfgnonhtsmoothforce << 16) | ((uint32_t)cfgnonhtsmooth << 0));
}

/**
 * @brief Unpacks SMOOTHFORCECTRL's fields from current value of the SMOOTHFORCECTRL register.
 *
 * Reads the SMOOTHFORCECTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] smoothnonhemode - Will be populated with the current value of this field from the register.
 * @param[out] intp1xpilotcomben - Will be populated with the current value of this field from the register.
 * @param[out] track1xallpiloten - Will be populated with the current value of this field from the register.
 * @param[out] musmoothforce - Will be populated with the current value of this field from the register.
 * @param[out] bfsmoothforce - Will be populated with the current value of this field from the register.
 * @param[out] cfgnonhtsmoothforce - Will be populated with the current value of this field from the register.
 * @param[out] cfgnonhtsmooth - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_smoothforcectrl_unpack(uint8_t* smoothnonhemode, uint8_t* intp1xpilotcomben, uint8_t* track1xallpiloten, uint8_t* musmoothforce, uint8_t* bfsmoothforce, uint8_t* cfgnonhtsmoothforce, uint8_t* cfgnonhtsmooth)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR);

    *smoothnonhemode = (localVal & ((uint32_t)0x80000000)) >> 31;
    *intp1xpilotcomben = (localVal & ((uint32_t)0x40000000)) >> 30;
    *track1xallpiloten = (localVal & ((uint32_t)0x20000000)) >> 29;
    *musmoothforce = (localVal & ((uint32_t)0x08000000)) >> 27;
    *bfsmoothforce = (localVal & ((uint32_t)0x04000000)) >> 26;
    *cfgnonhtsmoothforce = (localVal & ((uint32_t)0x00010000)) >> 16;
    *cfgnonhtsmooth = (localVal & ((uint32_t)0x00000003)) >> 0;
}

/**
 * @brief Returns the current value of the SMOOTHNONHEMODE field in the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read and the SMOOTHNONHEMODE field's value will be returned.
 *
 * @return The current value of the SMOOTHNONHEMODE field in the SMOOTHFORCECTRL register.
 */
__INLINE uint8_t mdm_smoothnonhemode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the SMOOTHNONHEMODE field of the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] smoothnonhemode - The value to set the field to.
 */
__INLINE void mdm_smoothnonhemode_setf(uint8_t smoothnonhemode)
{
    ASSERT_ERR((((uint32_t)smoothnonhemode << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MDM_SMOOTHFORCECTRL_ADDR, (REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)smoothnonhemode << 31));
}

/**
 * @brief Returns the current value of the intp1xpilotcomben field in the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read and the intp1xpilotcomben field's value will be returned.
 *
 * @return The current value of the intp1xpilotcomben field in the SMOOTHFORCECTRL register.
 */
__INLINE uint8_t mdm_intp_1xpilotcomben_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the intp1xpilotcomben field of the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] intp1xpilotcomben - The value to set the field to.
 */
__INLINE void mdm_intp_1xpilotcomben_setf(uint8_t intp1xpilotcomben)
{
    ASSERT_ERR((((uint32_t)intp1xpilotcomben << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(MDM_SMOOTHFORCECTRL_ADDR, (REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)intp1xpilotcomben << 30));
}

/**
 * @brief Returns the current value of the track1xallpiloten field in the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read and the track1xallpiloten field's value will be returned.
 *
 * @return The current value of the track1xallpiloten field in the SMOOTHFORCECTRL register.
 */
__INLINE uint8_t mdm_track_1xallpiloten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the track1xallpiloten field of the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] track1xallpiloten - The value to set the field to.
 */
__INLINE void mdm_track_1xallpiloten_setf(uint8_t track1xallpiloten)
{
    ASSERT_ERR((((uint32_t)track1xallpiloten << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(MDM_SMOOTHFORCECTRL_ADDR, (REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)track1xallpiloten << 29));
}

/**
 * @brief Returns the current value of the MUSMOOTHFORCE field in the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read and the MUSMOOTHFORCE field's value will be returned.
 *
 * @return The current value of the MUSMOOTHFORCE field in the SMOOTHFORCECTRL register.
 */
__INLINE uint8_t mdm_musmoothforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the MUSMOOTHFORCE field of the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] musmoothforce - The value to set the field to.
 */
__INLINE void mdm_musmoothforce_setf(uint8_t musmoothforce)
{
    ASSERT_ERR((((uint32_t)musmoothforce << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(MDM_SMOOTHFORCECTRL_ADDR, (REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)musmoothforce << 27));
}

/**
 * @brief Returns the current value of the BFSMOOTHFORCE field in the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read and the BFSMOOTHFORCE field's value will be returned.
 *
 * @return The current value of the BFSMOOTHFORCE field in the SMOOTHFORCECTRL register.
 */
__INLINE uint8_t mdm_bfsmoothforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the BFSMOOTHFORCE field of the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] bfsmoothforce - The value to set the field to.
 */
__INLINE void mdm_bfsmoothforce_setf(uint8_t bfsmoothforce)
{
    ASSERT_ERR((((uint32_t)bfsmoothforce << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(MDM_SMOOTHFORCECTRL_ADDR, (REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)bfsmoothforce << 26));
}

/**
 * @brief Returns the current value of the CFGNONHTSMOOTHFORCE field in the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read and the CFGNONHTSMOOTHFORCE field's value will be returned.
 *
 * @return The current value of the CFGNONHTSMOOTHFORCE field in the SMOOTHFORCECTRL register.
 */
__INLINE uint8_t mdm_cfgnonhtsmoothforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

/**
 * @brief Sets the CFGNONHTSMOOTHFORCE field of the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgnonhtsmoothforce - The value to set the field to.
 */
__INLINE void mdm_cfgnonhtsmoothforce_setf(uint8_t cfgnonhtsmoothforce)
{
    ASSERT_ERR((((uint32_t)cfgnonhtsmoothforce << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(MDM_SMOOTHFORCECTRL_ADDR, (REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)cfgnonhtsmoothforce << 16));
}

/**
 * @brief Returns the current value of the CFGNONHTSMOOTH field in the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read and the CFGNONHTSMOOTH field's value will be returned.
 *
 * @return The current value of the CFGNONHTSMOOTH field in the SMOOTHFORCECTRL register.
 */
__INLINE uint8_t mdm_cfgnonhtsmooth_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

/**
 * @brief Sets the CFGNONHTSMOOTH field of the SMOOTHFORCECTRL register.
 *
 * The SMOOTHFORCECTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] cfgnonhtsmooth - The value to set the field to.
 */
__INLINE void mdm_cfgnonhtsmooth_setf(uint8_t cfgnonhtsmooth)
{
    ASSERT_ERR((((uint32_t)cfgnonhtsmooth << 0) & ~((uint32_t)0x00000003)) == 0);
    REG_PL_WR(MDM_SMOOTHFORCECTRL_ADDR, (REG_PL_RD(MDM_SMOOTHFORCECTRL_ADDR) & ~((uint32_t)0x00000003)) | ((uint32_t)cfgnonhtsmooth << 0));
}

/// @}

/**
 * @name TXTDCFOCTRL register definitions
 * <table>
 * <caption>TXTDCFOCTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 30    <td>  TXTDCFOPHASEFORCE <td> R <td> R/W <td> 0
 * <tr><td> 29    <td> TXTDCFOPHASETESTEN <td> R <td> R/W <td> 0
 * <tr><td> 28    <td>     TXTDCFOPHASEEN <td> R <td> R/W <td> 1
 * <tr><td> 24:00 <td>       TXTDCFOPHASE <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TXTDCFOCTRL register
#define MDM_TXTDCFOCTRL_ADDR   0x24C00844
/// Offset of the TXTDCFOCTRL register from the base address
#define MDM_TXTDCFOCTRL_OFFSET 0x00000844
/// Index of the TXTDCFOCTRL register
#define MDM_TXTDCFOCTRL_INDEX  0x00000211
/// Reset value of the TXTDCFOCTRL register
#define MDM_TXTDCFOCTRL_RESET  0x10000000

/**
 * @brief Returns the current value of the TXTDCFOCTRL register.
 * The TXTDCFOCTRL register will be read and its value returned.
 * @return The current value of the TXTDCFOCTRL register.
 */
__INLINE uint32_t mdm_txtdcfoctrl_get(void)
{
    return REG_PL_RD(MDM_TXTDCFOCTRL_ADDR);
}

/**
 * @brief Sets the TXTDCFOCTRL register to a value.
 * The TXTDCFOCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_txtdcfoctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_TXTDCFOCTRL_ADDR, value);
}

// field definitions
/// TXTDCFOPHASEFORCE field bit
#define MDM_TXTDCFOPHASEFORCE_BIT     ((uint32_t)0x40000000)
/// TXTDCFOPHASEFORCE field position
#define MDM_TXTDCFOPHASEFORCE_POS     30
/// TXTDCFOPHASETESTEN field bit
#define MDM_TXTDCFOPHASETESTEN_BIT    ((uint32_t)0x20000000)
/// TXTDCFOPHASETESTEN field position
#define MDM_TXTDCFOPHASETESTEN_POS    29
/// TXTDCFOPHASEEN field bit
#define MDM_TXTDCFOPHASEEN_BIT        ((uint32_t)0x10000000)
/// TXTDCFOPHASEEN field position
#define MDM_TXTDCFOPHASEEN_POS        28
/// TXTDCFOPHASE field mask
#define MDM_TXTDCFOPHASE_MASK         ((uint32_t)0x01FFFFFF)
/// TXTDCFOPHASE field LSB position
#define MDM_TXTDCFOPHASE_LSB          0
/// TXTDCFOPHASE field width
#define MDM_TXTDCFOPHASE_WIDTH        ((uint32_t)0x00000019)

/// TXTDCFOPHASEFORCE field reset value
#define MDM_TXTDCFOPHASEFORCE_RST     0x0
/// TXTDCFOPHASETESTEN field reset value
#define MDM_TXTDCFOPHASETESTEN_RST    0x0
/// TXTDCFOPHASEEN field reset value
#define MDM_TXTDCFOPHASEEN_RST        0x1
/// TXTDCFOPHASE field reset value
#define MDM_TXTDCFOPHASE_RST          0x0

/**
 * @brief Constructs a value for the TXTDCFOCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txtdcfophaseforce - The value to use for the TXTDCFOPHASEFORCE field.
 * @param[in] txtdcfophasetesten - The value to use for the TXTDCFOPHASETESTEN field.
 * @param[in] txtdcfophaseen - The value to use for the TXTDCFOPHASEEN field.
 * @param[in] txtdcfophase - The value to use for the TXTDCFOPHASE field.
 */
__INLINE void mdm_txtdcfoctrl_pack(uint8_t txtdcfophaseforce, uint8_t txtdcfophasetesten, uint8_t txtdcfophaseen, uint32_t txtdcfophase)
{
    ASSERT_ERR((((uint32_t)txtdcfophaseforce << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)txtdcfophasetesten << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)txtdcfophaseen << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)txtdcfophase << 0) & ~((uint32_t)0x01FFFFFF)) == 0);
    REG_PL_WR(MDM_TXTDCFOCTRL_ADDR,  ((uint32_t)txtdcfophaseforce << 30) | ((uint32_t)txtdcfophasetesten << 29) | ((uint32_t)txtdcfophaseen << 28) | ((uint32_t)txtdcfophase << 0));
}

/**
 * @brief Unpacks TXTDCFOCTRL's fields from current value of the TXTDCFOCTRL register.
 *
 * Reads the TXTDCFOCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txtdcfophaseforce - Will be populated with the current value of this field from the register.
 * @param[out] txtdcfophasetesten - Will be populated with the current value of this field from the register.
 * @param[out] txtdcfophaseen - Will be populated with the current value of this field from the register.
 * @param[out] txtdcfophase - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_txtdcfoctrl_unpack(uint8_t* txtdcfophaseforce, uint8_t* txtdcfophasetesten, uint8_t* txtdcfophaseen, uint32_t* txtdcfophase)
{
    uint32_t localVal = REG_PL_RD(MDM_TXTDCFOCTRL_ADDR);

    *txtdcfophaseforce = (localVal & ((uint32_t)0x40000000)) >> 30;
    *txtdcfophasetesten = (localVal & ((uint32_t)0x20000000)) >> 29;
    *txtdcfophaseen = (localVal & ((uint32_t)0x10000000)) >> 28;
    *txtdcfophase = (localVal & ((uint32_t)0x01FFFFFF)) >> 0;
}

/**
 * @brief Returns the current value of the TXTDCFOPHASEFORCE field in the TXTDCFOCTRL register.
 *
 * The TXTDCFOCTRL register will be read and the TXTDCFOPHASEFORCE field's value will be returned.
 *
 * @return The current value of the TXTDCFOPHASEFORCE field in the TXTDCFOCTRL register.
 */
__INLINE uint8_t mdm_txtdcfophaseforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXTDCFOCTRL_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the TXTDCFOPHASEFORCE field of the TXTDCFOCTRL register.
 *
 * The TXTDCFOCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txtdcfophaseforce - The value to set the field to.
 */
__INLINE void mdm_txtdcfophaseforce_setf(uint8_t txtdcfophaseforce)
{
    ASSERT_ERR((((uint32_t)txtdcfophaseforce << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(MDM_TXTDCFOCTRL_ADDR, (REG_PL_RD(MDM_TXTDCFOCTRL_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)txtdcfophaseforce << 30));
}

/**
 * @brief Returns the current value of the TXTDCFOPHASETESTEN field in the TXTDCFOCTRL register.
 *
 * The TXTDCFOCTRL register will be read and the TXTDCFOPHASETESTEN field's value will be returned.
 *
 * @return The current value of the TXTDCFOPHASETESTEN field in the TXTDCFOCTRL register.
 */
__INLINE uint8_t mdm_txtdcfophasetesten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXTDCFOCTRL_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the TXTDCFOPHASETESTEN field of the TXTDCFOCTRL register.
 *
 * The TXTDCFOCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txtdcfophasetesten - The value to set the field to.
 */
__INLINE void mdm_txtdcfophasetesten_setf(uint8_t txtdcfophasetesten)
{
    ASSERT_ERR((((uint32_t)txtdcfophasetesten << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(MDM_TXTDCFOCTRL_ADDR, (REG_PL_RD(MDM_TXTDCFOCTRL_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)txtdcfophasetesten << 29));
}

/**
 * @brief Returns the current value of the TXTDCFOPHASEEN field in the TXTDCFOCTRL register.
 *
 * The TXTDCFOCTRL register will be read and the TXTDCFOPHASEEN field's value will be returned.
 *
 * @return The current value of the TXTDCFOPHASEEN field in the TXTDCFOCTRL register.
 */
__INLINE uint8_t mdm_txtdcfophaseen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXTDCFOCTRL_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the TXTDCFOPHASEEN field of the TXTDCFOCTRL register.
 *
 * The TXTDCFOCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txtdcfophaseen - The value to set the field to.
 */
__INLINE void mdm_txtdcfophaseen_setf(uint8_t txtdcfophaseen)
{
    ASSERT_ERR((((uint32_t)txtdcfophaseen << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(MDM_TXTDCFOCTRL_ADDR, (REG_PL_RD(MDM_TXTDCFOCTRL_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)txtdcfophaseen << 28));
}

/**
 * @brief Returns the current value of the TXTDCFOPHASE field in the TXTDCFOCTRL register.
 *
 * The TXTDCFOCTRL register will be read and the TXTDCFOPHASE field's value will be returned.
 *
 * @return The current value of the TXTDCFOPHASE field in the TXTDCFOCTRL register.
 */
__INLINE uint32_t mdm_txtdcfophase_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXTDCFOCTRL_ADDR);
    return ((localVal & ((uint32_t)0x01FFFFFF)) >> 0);
}

/**
 * @brief Sets the TXTDCFOPHASE field of the TXTDCFOCTRL register.
 *
 * The TXTDCFOCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txtdcfophase - The value to set the field to.
 */
__INLINE void mdm_txtdcfophase_setf(uint32_t txtdcfophase)
{
    ASSERT_ERR((((uint32_t)txtdcfophase << 0) & ~((uint32_t)0x01FFFFFF)) == 0);
    REG_PL_WR(MDM_TXTDCFOCTRL_ADDR, (REG_PL_RD(MDM_TXTDCFOCTRL_ADDR) & ~((uint32_t)0x01FFFFFF)) | ((uint32_t)txtdcfophase << 0));
}

/// @}

/**
 * @name TXTDSFOCTRL register definitions
 * <table>
 * <caption>TXTDSFOCTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 30    <td>  TXTDSFOFSRATIOFORCE <td> R <td> R/W <td> 0
 * <tr><td> 29    <td> TXTDSFOFSRATIOTESTEN <td> R <td> R/W <td> 0
 * <tr><td> 28    <td>     TXTDSFOFSRATIOEN <td> R <td> R/W <td> 1
 * <tr><td> 26:00 <td>       TXTDSFOFSRATIO <td> R <td> R/W <td> 0x4000000
 * </table>
 *
 * @{
 */

/// Address of the TXTDSFOCTRL register
#define MDM_TXTDSFOCTRL_ADDR   0x24C00848
/// Offset of the TXTDSFOCTRL register from the base address
#define MDM_TXTDSFOCTRL_OFFSET 0x00000848
/// Index of the TXTDSFOCTRL register
#define MDM_TXTDSFOCTRL_INDEX  0x00000212
/// Reset value of the TXTDSFOCTRL register
#define MDM_TXTDSFOCTRL_RESET  0x14000000

/**
 * @brief Returns the current value of the TXTDSFOCTRL register.
 * The TXTDSFOCTRL register will be read and its value returned.
 * @return The current value of the TXTDSFOCTRL register.
 */
__INLINE uint32_t mdm_txtdsfoctrl_get(void)
{
    return REG_PL_RD(MDM_TXTDSFOCTRL_ADDR);
}

/**
 * @brief Sets the TXTDSFOCTRL register to a value.
 * The TXTDSFOCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_txtdsfoctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_TXTDSFOCTRL_ADDR, value);
}

// field definitions
/// TXTDSFOFSRATIOFORCE field bit
#define MDM_TXTDSFOFSRATIOFORCE_BIT     ((uint32_t)0x40000000)
/// TXTDSFOFSRATIOFORCE field position
#define MDM_TXTDSFOFSRATIOFORCE_POS     30
/// TXTDSFOFSRATIOTESTEN field bit
#define MDM_TXTDSFOFSRATIOTESTEN_BIT    ((uint32_t)0x20000000)
/// TXTDSFOFSRATIOTESTEN field position
#define MDM_TXTDSFOFSRATIOTESTEN_POS    29
/// TXTDSFOFSRATIOEN field bit
#define MDM_TXTDSFOFSRATIOEN_BIT        ((uint32_t)0x10000000)
/// TXTDSFOFSRATIOEN field position
#define MDM_TXTDSFOFSRATIOEN_POS        28
/// TXTDSFOFSRATIO field mask
#define MDM_TXTDSFOFSRATIO_MASK         ((uint32_t)0x07FFFFFF)
/// TXTDSFOFSRATIO field LSB position
#define MDM_TXTDSFOFSRATIO_LSB          0
/// TXTDSFOFSRATIO field width
#define MDM_TXTDSFOFSRATIO_WIDTH        ((uint32_t)0x0000001B)

/// TXTDSFOFSRATIOFORCE field reset value
#define MDM_TXTDSFOFSRATIOFORCE_RST     0x0
/// TXTDSFOFSRATIOTESTEN field reset value
#define MDM_TXTDSFOFSRATIOTESTEN_RST    0x0
/// TXTDSFOFSRATIOEN field reset value
#define MDM_TXTDSFOFSRATIOEN_RST        0x1
/// TXTDSFOFSRATIO field reset value
#define MDM_TXTDSFOFSRATIO_RST          0x4000000

/**
 * @brief Constructs a value for the TXTDSFOCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txtdsfofsratioforce - The value to use for the TXTDSFOFSRATIOFORCE field.
 * @param[in] txtdsfofsratiotesten - The value to use for the TXTDSFOFSRATIOTESTEN field.
 * @param[in] txtdsfofsratioen - The value to use for the TXTDSFOFSRATIOEN field.
 * @param[in] txtdsfofsratio - The value to use for the TXTDSFOFSRATIO field.
 */
__INLINE void mdm_txtdsfoctrl_pack(uint8_t txtdsfofsratioforce, uint8_t txtdsfofsratiotesten, uint8_t txtdsfofsratioen, uint32_t txtdsfofsratio)
{
    ASSERT_ERR((((uint32_t)txtdsfofsratioforce << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)txtdsfofsratiotesten << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)txtdsfofsratioen << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)txtdsfofsratio << 0) & ~((uint32_t)0x07FFFFFF)) == 0);
    REG_PL_WR(MDM_TXTDSFOCTRL_ADDR,  ((uint32_t)txtdsfofsratioforce << 30) | ((uint32_t)txtdsfofsratiotesten << 29) | ((uint32_t)txtdsfofsratioen << 28) | ((uint32_t)txtdsfofsratio << 0));
}

/**
 * @brief Unpacks TXTDSFOCTRL's fields from current value of the TXTDSFOCTRL register.
 *
 * Reads the TXTDSFOCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txtdsfofsratioforce - Will be populated with the current value of this field from the register.
 * @param[out] txtdsfofsratiotesten - Will be populated with the current value of this field from the register.
 * @param[out] txtdsfofsratioen - Will be populated with the current value of this field from the register.
 * @param[out] txtdsfofsratio - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_txtdsfoctrl_unpack(uint8_t* txtdsfofsratioforce, uint8_t* txtdsfofsratiotesten, uint8_t* txtdsfofsratioen, uint32_t* txtdsfofsratio)
{
    uint32_t localVal = REG_PL_RD(MDM_TXTDSFOCTRL_ADDR);

    *txtdsfofsratioforce = (localVal & ((uint32_t)0x40000000)) >> 30;
    *txtdsfofsratiotesten = (localVal & ((uint32_t)0x20000000)) >> 29;
    *txtdsfofsratioen = (localVal & ((uint32_t)0x10000000)) >> 28;
    *txtdsfofsratio = (localVal & ((uint32_t)0x07FFFFFF)) >> 0;
}

/**
 * @brief Returns the current value of the TXTDSFOFSRATIOFORCE field in the TXTDSFOCTRL register.
 *
 * The TXTDSFOCTRL register will be read and the TXTDSFOFSRATIOFORCE field's value will be returned.
 *
 * @return The current value of the TXTDSFOFSRATIOFORCE field in the TXTDSFOCTRL register.
 */
__INLINE uint8_t mdm_txtdsfofsratioforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXTDSFOCTRL_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the TXTDSFOFSRATIOFORCE field of the TXTDSFOCTRL register.
 *
 * The TXTDSFOCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txtdsfofsratioforce - The value to set the field to.
 */
__INLINE void mdm_txtdsfofsratioforce_setf(uint8_t txtdsfofsratioforce)
{
    ASSERT_ERR((((uint32_t)txtdsfofsratioforce << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(MDM_TXTDSFOCTRL_ADDR, (REG_PL_RD(MDM_TXTDSFOCTRL_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)txtdsfofsratioforce << 30));
}

/**
 * @brief Returns the current value of the TXTDSFOFSRATIOTESTEN field in the TXTDSFOCTRL register.
 *
 * The TXTDSFOCTRL register will be read and the TXTDSFOFSRATIOTESTEN field's value will be returned.
 *
 * @return The current value of the TXTDSFOFSRATIOTESTEN field in the TXTDSFOCTRL register.
 */
__INLINE uint8_t mdm_txtdsfofsratiotesten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXTDSFOCTRL_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the TXTDSFOFSRATIOTESTEN field of the TXTDSFOCTRL register.
 *
 * The TXTDSFOCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txtdsfofsratiotesten - The value to set the field to.
 */
__INLINE void mdm_txtdsfofsratiotesten_setf(uint8_t txtdsfofsratiotesten)
{
    ASSERT_ERR((((uint32_t)txtdsfofsratiotesten << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(MDM_TXTDSFOCTRL_ADDR, (REG_PL_RD(MDM_TXTDSFOCTRL_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)txtdsfofsratiotesten << 29));
}

/**
 * @brief Returns the current value of the TXTDSFOFSRATIOEN field in the TXTDSFOCTRL register.
 *
 * The TXTDSFOCTRL register will be read and the TXTDSFOFSRATIOEN field's value will be returned.
 *
 * @return The current value of the TXTDSFOFSRATIOEN field in the TXTDSFOCTRL register.
 */
__INLINE uint8_t mdm_txtdsfofsratioen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXTDSFOCTRL_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the TXTDSFOFSRATIOEN field of the TXTDSFOCTRL register.
 *
 * The TXTDSFOCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txtdsfofsratioen - The value to set the field to.
 */
__INLINE void mdm_txtdsfofsratioen_setf(uint8_t txtdsfofsratioen)
{
    ASSERT_ERR((((uint32_t)txtdsfofsratioen << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(MDM_TXTDSFOCTRL_ADDR, (REG_PL_RD(MDM_TXTDSFOCTRL_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)txtdsfofsratioen << 28));
}

/**
 * @brief Returns the current value of the TXTDSFOFSRATIO field in the TXTDSFOCTRL register.
 *
 * The TXTDSFOCTRL register will be read and the TXTDSFOFSRATIO field's value will be returned.
 *
 * @return The current value of the TXTDSFOFSRATIO field in the TXTDSFOCTRL register.
 */
__INLINE uint32_t mdm_txtdsfofsratio_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXTDSFOCTRL_ADDR);
    return ((localVal & ((uint32_t)0x07FFFFFF)) >> 0);
}

/**
 * @brief Sets the TXTDSFOFSRATIO field of the TXTDSFOCTRL register.
 *
 * The TXTDSFOCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txtdsfofsratio - The value to set the field to.
 */
__INLINE void mdm_txtdsfofsratio_setf(uint32_t txtdsfofsratio)
{
    ASSERT_ERR((((uint32_t)txtdsfofsratio << 0) & ~((uint32_t)0x07FFFFFF)) == 0);
    REG_PL_WR(MDM_TXTDSFOCTRL_ADDR, (REG_PL_RD(MDM_TXTDSFOCTRL_ADDR) & ~((uint32_t)0x07FFFFFF)) | ((uint32_t)txtdsfofsratio << 0));
}

/// @}

/**
 * @name RXFOESTCTRL register definitions
 * <table>
 * <caption>RXFOESTCTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 26    <td>   FOESTFDDATAEN <td> R <td> R/W <td> 1
 * <tr><td> 25    <td> FOESTFDPREAMBEN <td> R <td> R/W <td> 0
 * <tr><td> 24    <td>       FOESTTDEN <td> R <td> R/W <td> 1
 * <tr><td> 14:00 <td>  INVCARRIERFREQ <td> R <td> R/W <td> 0x2BB1
 * </table>
 *
 * @{
 */

/// Address of the RXFOESTCTRL register
#define MDM_RXFOESTCTRL_ADDR   0x24C0084C
/// Offset of the RXFOESTCTRL register from the base address
#define MDM_RXFOESTCTRL_OFFSET 0x0000084C
/// Index of the RXFOESTCTRL register
#define MDM_RXFOESTCTRL_INDEX  0x00000213
/// Reset value of the RXFOESTCTRL register
#define MDM_RXFOESTCTRL_RESET  0x05002BB1

/**
 * @brief Returns the current value of the RXFOESTCTRL register.
 * The RXFOESTCTRL register will be read and its value returned.
 * @return The current value of the RXFOESTCTRL register.
 */
__INLINE uint32_t mdm_rxfoestctrl_get(void)
{
    return REG_PL_RD(MDM_RXFOESTCTRL_ADDR);
}

/**
 * @brief Sets the RXFOESTCTRL register to a value.
 * The RXFOESTCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxfoestctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_RXFOESTCTRL_ADDR, value);
}

// field definitions
/// FOESTFDDATAEN field bit
#define MDM_FOESTFDDATAEN_BIT      ((uint32_t)0x04000000)
/// FOESTFDDATAEN field position
#define MDM_FOESTFDDATAEN_POS      26
/// FOESTFDPREAMBEN field bit
#define MDM_FOESTFDPREAMBEN_BIT    ((uint32_t)0x02000000)
/// FOESTFDPREAMBEN field position
#define MDM_FOESTFDPREAMBEN_POS    25
/// FOESTTDEN field bit
#define MDM_FOESTTDEN_BIT          ((uint32_t)0x01000000)
/// FOESTTDEN field position
#define MDM_FOESTTDEN_POS          24
/// INVCARRIERFREQ field mask
#define MDM_INVCARRIERFREQ_MASK    ((uint32_t)0x00007FFF)
/// INVCARRIERFREQ field LSB position
#define MDM_INVCARRIERFREQ_LSB     0
/// INVCARRIERFREQ field width
#define MDM_INVCARRIERFREQ_WIDTH   ((uint32_t)0x0000000F)

/// FOESTFDDATAEN field reset value
#define MDM_FOESTFDDATAEN_RST      0x1
/// FOESTFDPREAMBEN field reset value
#define MDM_FOESTFDPREAMBEN_RST    0x0
/// FOESTTDEN field reset value
#define MDM_FOESTTDEN_RST          0x1
/// INVCARRIERFREQ field reset value
#define MDM_INVCARRIERFREQ_RST     0x2BB1

/**
 * @brief Constructs a value for the RXFOESTCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] foestfddataen - The value to use for the FOESTFDDATAEN field.
 * @param[in] foestfdpreamben - The value to use for the FOESTFDPREAMBEN field.
 * @param[in] foesttden - The value to use for the FOESTTDEN field.
 * @param[in] invcarrierfreq - The value to use for the INVCARRIERFREQ field.
 */
__INLINE void mdm_rxfoestctrl_pack(uint8_t foestfddataen, uint8_t foestfdpreamben, uint8_t foesttden, uint16_t invcarrierfreq)
{
    ASSERT_ERR((((uint32_t)foestfddataen << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)foestfdpreamben << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)foesttden << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)invcarrierfreq << 0) & ~((uint32_t)0x00007FFF)) == 0);
    REG_PL_WR(MDM_RXFOESTCTRL_ADDR,  ((uint32_t)foestfddataen << 26) | ((uint32_t)foestfdpreamben << 25) | ((uint32_t)foesttden << 24) | ((uint32_t)invcarrierfreq << 0));
}

/**
 * @brief Unpacks RXFOESTCTRL's fields from current value of the RXFOESTCTRL register.
 *
 * Reads the RXFOESTCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] foestfddataen - Will be populated with the current value of this field from the register.
 * @param[out] foestfdpreamben - Will be populated with the current value of this field from the register.
 * @param[out] foesttden - Will be populated with the current value of this field from the register.
 * @param[out] invcarrierfreq - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxfoestctrl_unpack(uint8_t* foestfddataen, uint8_t* foestfdpreamben, uint8_t* foesttden, uint16_t* invcarrierfreq)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFOESTCTRL_ADDR);

    *foestfddataen = (localVal & ((uint32_t)0x04000000)) >> 26;
    *foestfdpreamben = (localVal & ((uint32_t)0x02000000)) >> 25;
    *foesttden = (localVal & ((uint32_t)0x01000000)) >> 24;
    *invcarrierfreq = (localVal & ((uint32_t)0x00007FFF)) >> 0;
}

/**
 * @brief Returns the current value of the FOESTFDDATAEN field in the RXFOESTCTRL register.
 *
 * The RXFOESTCTRL register will be read and the FOESTFDDATAEN field's value will be returned.
 *
 * @return The current value of the FOESTFDDATAEN field in the RXFOESTCTRL register.
 */
__INLINE uint8_t mdm_foestfddataen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFOESTCTRL_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

/**
 * @brief Sets the FOESTFDDATAEN field of the RXFOESTCTRL register.
 *
 * The RXFOESTCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] foestfddataen - The value to set the field to.
 */
__INLINE void mdm_foestfddataen_setf(uint8_t foestfddataen)
{
    ASSERT_ERR((((uint32_t)foestfddataen << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_PL_WR(MDM_RXFOESTCTRL_ADDR, (REG_PL_RD(MDM_RXFOESTCTRL_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)foestfddataen << 26));
}

/**
 * @brief Returns the current value of the FOESTFDPREAMBEN field in the RXFOESTCTRL register.
 *
 * The RXFOESTCTRL register will be read and the FOESTFDPREAMBEN field's value will be returned.
 *
 * @return The current value of the FOESTFDPREAMBEN field in the RXFOESTCTRL register.
 */
__INLINE uint8_t mdm_foestfdpreamben_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFOESTCTRL_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

/**
 * @brief Sets the FOESTFDPREAMBEN field of the RXFOESTCTRL register.
 *
 * The RXFOESTCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] foestfdpreamben - The value to set the field to.
 */
__INLINE void mdm_foestfdpreamben_setf(uint8_t foestfdpreamben)
{
    ASSERT_ERR((((uint32_t)foestfdpreamben << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_PL_WR(MDM_RXFOESTCTRL_ADDR, (REG_PL_RD(MDM_RXFOESTCTRL_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)foestfdpreamben << 25));
}

/**
 * @brief Returns the current value of the FOESTTDEN field in the RXFOESTCTRL register.
 *
 * The RXFOESTCTRL register will be read and the FOESTTDEN field's value will be returned.
 *
 * @return The current value of the FOESTTDEN field in the RXFOESTCTRL register.
 */
__INLINE uint8_t mdm_foesttden_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFOESTCTRL_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the FOESTTDEN field of the RXFOESTCTRL register.
 *
 * The RXFOESTCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] foesttden - The value to set the field to.
 */
__INLINE void mdm_foesttden_setf(uint8_t foesttden)
{
    ASSERT_ERR((((uint32_t)foesttden << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(MDM_RXFOESTCTRL_ADDR, (REG_PL_RD(MDM_RXFOESTCTRL_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)foesttden << 24));
}

/**
 * @brief Returns the current value of the INVCARRIERFREQ field in the RXFOESTCTRL register.
 *
 * The RXFOESTCTRL register will be read and the INVCARRIERFREQ field's value will be returned.
 *
 * @return The current value of the INVCARRIERFREQ field in the RXFOESTCTRL register.
 */
__INLINE uint16_t mdm_invcarrierfreq_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXFOESTCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00007FFF)) >> 0);
}

/**
 * @brief Sets the INVCARRIERFREQ field of the RXFOESTCTRL register.
 *
 * The RXFOESTCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] invcarrierfreq - The value to set the field to.
 */
__INLINE void mdm_invcarrierfreq_setf(uint16_t invcarrierfreq)
{
    ASSERT_ERR((((uint32_t)invcarrierfreq << 0) & ~((uint32_t)0x00007FFF)) == 0);
    REG_PL_WR(MDM_RXFOESTCTRL_ADDR, (REG_PL_RD(MDM_RXFOESTCTRL_ADDR) & ~((uint32_t)0x00007FFF)) | ((uint32_t)invcarrierfreq << 0));
}

/// @}

/**
 * @name PRIMARYIND register definitions
 * <table>
 * <caption>PRIMARYIND bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 02:00 <td> PRIMARY <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the PRIMARYIND register
#define MDM_PRIMARYIND_ADDR   0x24C00850
/// Offset of the PRIMARYIND register from the base address
#define MDM_PRIMARYIND_OFFSET 0x00000850
/// Index of the PRIMARYIND register
#define MDM_PRIMARYIND_INDEX  0x00000214
/// Reset value of the PRIMARYIND register
#define MDM_PRIMARYIND_RESET  0x00000000

/**
 * @brief Returns the current value of the PRIMARYIND register.
 * The PRIMARYIND register will be read and its value returned.
 * @return The current value of the PRIMARYIND register.
 */
__INLINE uint32_t mdm_primaryind_get(void)
{
    return REG_PL_RD(MDM_PRIMARYIND_ADDR);
}

/**
 * @brief Sets the PRIMARYIND register to a value.
 * The PRIMARYIND register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_primaryind_set(uint32_t value)
{
    REG_PL_WR(MDM_PRIMARYIND_ADDR, value);
}

// field definitions
/// PRIMARY field mask
#define MDM_PRIMARY_MASK   ((uint32_t)0x00000007)
/// PRIMARY field LSB position
#define MDM_PRIMARY_LSB    0
/// PRIMARY field width
#define MDM_PRIMARY_WIDTH  ((uint32_t)0x00000003)

/// PRIMARY field reset value
#define MDM_PRIMARY_RST    0x0

/**
 * @brief Returns the current value of the PRIMARY field in the PRIMARYIND register.
 *
 * The PRIMARYIND register will be read and the PRIMARY field's value will be returned.
 *
 * @return The current value of the PRIMARY field in the PRIMARYIND register.
 */
__INLINE uint8_t mdm_primary_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_PRIMARYIND_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000007)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the PRIMARY field of the PRIMARYIND register.
 *
 * The PRIMARYIND register will be read, modified to contain the new field value, and written.
 *
 * @param[in] primary - The value to set the field to.
 */
__INLINE void mdm_primary_setf(uint8_t primary)
{
    ASSERT_ERR((((uint32_t)primary << 0) & ~((uint32_t)0x00000007)) == 0);
    REG_PL_WR(MDM_PRIMARYIND_ADDR, (uint32_t)primary << 0);
}

/// @}

/**
 * @name SCRAMBLERCTRL register definitions
 * <table>
 * <caption>SCRAMBLERCTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 07:00 <td> SCRAMSEED <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the SCRAMBLERCTRL register
#define MDM_SCRAMBLERCTRL_ADDR   0x24C00854
/// Offset of the SCRAMBLERCTRL register from the base address
#define MDM_SCRAMBLERCTRL_OFFSET 0x00000854
/// Index of the SCRAMBLERCTRL register
#define MDM_SCRAMBLERCTRL_INDEX  0x00000215
/// Reset value of the SCRAMBLERCTRL register
#define MDM_SCRAMBLERCTRL_RESET  0x00000000

/**
 * @brief Returns the current value of the SCRAMBLERCTRL register.
 * The SCRAMBLERCTRL register will be read and its value returned.
 * @return The current value of the SCRAMBLERCTRL register.
 */
__INLINE uint32_t mdm_scramblerctrl_get(void)
{
    return REG_PL_RD(MDM_SCRAMBLERCTRL_ADDR);
}

/**
 * @brief Sets the SCRAMBLERCTRL register to a value.
 * The SCRAMBLERCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_scramblerctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_SCRAMBLERCTRL_ADDR, value);
}

// field definitions
/// SCRAMSEED field mask
#define MDM_SCRAMSEED_MASK   ((uint32_t)0x000000FF)
/// SCRAMSEED field LSB position
#define MDM_SCRAMSEED_LSB    0
/// SCRAMSEED field width
#define MDM_SCRAMSEED_WIDTH  ((uint32_t)0x00000008)

/// SCRAMSEED field reset value
#define MDM_SCRAMSEED_RST    0x0

/**
 * @brief Returns the current value of the SCRAMSEED field in the SCRAMBLERCTRL register.
 *
 * The SCRAMBLERCTRL register will be read and the SCRAMSEED field's value will be returned.
 *
 * @return The current value of the SCRAMSEED field in the SCRAMBLERCTRL register.
 */
__INLINE uint8_t mdm_scramseed_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SCRAMBLERCTRL_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the SCRAMSEED field of the SCRAMBLERCTRL register.
 *
 * The SCRAMBLERCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] scramseed - The value to set the field to.
 */
__INLINE void mdm_scramseed_setf(uint8_t scramseed)
{
    ASSERT_ERR((((uint32_t)scramseed << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_SCRAMBLERCTRL_ADDR, (uint32_t)scramseed << 0);
}

/// @}

/**
 * @name TBECTRL0 register definitions
 * <table>
 * <caption>TBECTRL0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 07:00 <td>    TBECountAdjust20 <td> R <td> R/W <td> 0x4
 * </table>
 *
 * @{
 */

/// Address of the TBECTRL0 register
#define MDM_TBECTRL0_ADDR   0x24C00858
/// Offset of the TBECTRL0 register from the base address
#define MDM_TBECTRL0_OFFSET 0x00000858
/// Index of the TBECTRL0 register
#define MDM_TBECTRL0_INDEX  0x00000216
/// Reset value of the TBECTRL0 register
#define MDM_TBECTRL0_RESET  0x00000004

/**
 * @brief Returns the current value of the TBECTRL0 register.
 * The TBECTRL0 register will be read and its value returned.
 * @return The current value of the TBECTRL0 register.
 */
__INLINE uint32_t mdm_tbectrl0_get(void)
{
    return REG_PL_RD(MDM_TBECTRL0_ADDR);
}

/**
 * @brief Sets the TBECTRL0 register to a value.
 * The TBECTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tbectrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_TBECTRL0_ADDR, value);
}

// field definitions
/// TBE_COUNT_ADJUST_20 field mask
#define MDM_TBE_COUNT_ADJUST_20_MASK   ((uint32_t)0x000000FF)
/// TBE_COUNT_ADJUST_20 field LSB position
#define MDM_TBE_COUNT_ADJUST_20_LSB    0
/// TBE_COUNT_ADJUST_20 field width
#define MDM_TBE_COUNT_ADJUST_20_WIDTH  ((uint32_t)0x00000008)

/// TBE_COUNT_ADJUST_20 field reset value
#define MDM_TBE_COUNT_ADJUST_20_RST    0x4

/**
 * @brief Returns the current value of the TBECountAdjust20 field in the TBECTRL0 register.
 *
 * The TBECTRL0 register will be read and the TBECountAdjust20 field's value will be returned.
 *
 * @return The current value of the TBECountAdjust20 field in the TBECTRL0 register.
 */
__INLINE uint8_t mdm_tbe_count_adjust_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the TBECountAdjust20 field of the TBECTRL0 register.
 *
 * The TBECTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbecountadjust20 - The value to set the field to.
 */
__INLINE void mdm_tbe_count_adjust_20_setf(uint8_t tbecountadjust20)
{
    ASSERT_ERR((((uint32_t)tbecountadjust20 << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_TBECTRL0_ADDR, (uint32_t)tbecountadjust20 << 0);
}

/// @}

/**
 * @name TBECTRL1 register definitions
 * <table>
 * <caption>TBECTRL1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 14:08 <td>    PeakSearchStop <td> R <td> R/W <td> 0x3E
 * <tr><td> 06:00 <td>   PeakSearchStart <td> R <td> R/W <td> 0x14
 * </table>
 *
 * @{
 */

/// Address of the TBECTRL1 register
#define MDM_TBECTRL1_ADDR   0x24C0085C
/// Offset of the TBECTRL1 register from the base address
#define MDM_TBECTRL1_OFFSET 0x0000085C
/// Index of the TBECTRL1 register
#define MDM_TBECTRL1_INDEX  0x00000217
/// Reset value of the TBECTRL1 register
#define MDM_TBECTRL1_RESET  0x00003E14

/**
 * @brief Returns the current value of the TBECTRL1 register.
 * The TBECTRL1 register will be read and its value returned.
 * @return The current value of the TBECTRL1 register.
 */
__INLINE uint32_t mdm_tbectrl1_get(void)
{
    return REG_PL_RD(MDM_TBECTRL1_ADDR);
}

/**
 * @brief Sets the TBECTRL1 register to a value.
 * The TBECTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tbectrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_TBECTRL1_ADDR, value);
}

// field definitions
/// PEAK_SEARCH_STOP field mask
#define MDM_PEAK_SEARCH_STOP_MASK    ((uint32_t)0x00007F00)
/// PEAK_SEARCH_STOP field LSB position
#define MDM_PEAK_SEARCH_STOP_LSB     8
/// PEAK_SEARCH_STOP field width
#define MDM_PEAK_SEARCH_STOP_WIDTH   ((uint32_t)0x00000007)
/// PEAK_SEARCH_START field mask
#define MDM_PEAK_SEARCH_START_MASK   ((uint32_t)0x0000007F)
/// PEAK_SEARCH_START field LSB position
#define MDM_PEAK_SEARCH_START_LSB    0
/// PEAK_SEARCH_START field width
#define MDM_PEAK_SEARCH_START_WIDTH  ((uint32_t)0x00000007)

/// PEAK_SEARCH_STOP field reset value
#define MDM_PEAK_SEARCH_STOP_RST     0x3E
/// PEAK_SEARCH_START field reset value
#define MDM_PEAK_SEARCH_START_RST    0x14

/**
 * @brief Constructs a value for the TBECTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] peaksearchstop - The value to use for the PeakSearchStop field.
 * @param[in] peaksearchstart - The value to use for the PeakSearchStart field.
 */
__INLINE void mdm_tbectrl1_pack(uint8_t peaksearchstop, uint8_t peaksearchstart)
{
    ASSERT_ERR((((uint32_t)peaksearchstop << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)peaksearchstart << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_TBECTRL1_ADDR,  ((uint32_t)peaksearchstop << 8) | ((uint32_t)peaksearchstart << 0));
}

/**
 * @brief Unpacks TBECTRL1's fields from current value of the TBECTRL1 register.
 *
 * Reads the TBECTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] peaksearchstop - Will be populated with the current value of this field from the register.
 * @param[out] peaksearchstart - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tbectrl1_unpack(uint8_t* peaksearchstop, uint8_t* peaksearchstart)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL1_ADDR);

    *peaksearchstop = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *peaksearchstart = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the PeakSearchStop field in the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read and the PeakSearchStop field's value will be returned.
 *
 * @return The current value of the PeakSearchStop field in the TBECTRL1 register.
 */
__INLINE uint8_t mdm_peak_search_stop_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

/**
 * @brief Sets the PeakSearchStop field of the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] peaksearchstop - The value to set the field to.
 */
__INLINE void mdm_peak_search_stop_setf(uint8_t peaksearchstop)
{
    ASSERT_ERR((((uint32_t)peaksearchstop << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_PL_WR(MDM_TBECTRL1_ADDR, (REG_PL_RD(MDM_TBECTRL1_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)peaksearchstop << 8));
}

/**
 * @brief Returns the current value of the PeakSearchStart field in the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read and the PeakSearchStart field's value will be returned.
 *
 * @return The current value of the PeakSearchStart field in the TBECTRL1 register.
 */
__INLINE uint8_t mdm_peak_search_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the PeakSearchStart field of the TBECTRL1 register.
 *
 * The TBECTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] peaksearchstart - The value to set the field to.
 */
__INLINE void mdm_peak_search_start_setf(uint8_t peaksearchstart)
{
    ASSERT_ERR((((uint32_t)peaksearchstart << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_TBECTRL1_ADDR, (REG_PL_RD(MDM_TBECTRL1_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)peaksearchstart << 0));
}

/// @}

/**
 * @name TBECTRL2 register definitions
 * <table>
 * <caption>TBECTRL2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 16:12 <td>       PeakSearchDelta <td> R <td> R/W <td> 0x7
 * <tr><td> 11    <td>       PeakSearchNdlEn <td> R <td> R/W <td> 1
 * <tr><td> 10:08 <td>    PeakSearchNdlIndex <td> R <td> R/W <td> 0x7
 * <tr><td> 07:00 <td>               TBEBias <td> R <td> R/W <td> 0x4
 * </table>
 *
 * @{
 */

/// Address of the TBECTRL2 register
#define MDM_TBECTRL2_ADDR   0x24C00860
/// Offset of the TBECTRL2 register from the base address
#define MDM_TBECTRL2_OFFSET 0x00000860
/// Index of the TBECTRL2 register
#define MDM_TBECTRL2_INDEX  0x00000218
/// Reset value of the TBECTRL2 register
#define MDM_TBECTRL2_RESET  0x00007F04

/**
 * @brief Returns the current value of the TBECTRL2 register.
 * The TBECTRL2 register will be read and its value returned.
 * @return The current value of the TBECTRL2 register.
 */
__INLINE uint32_t mdm_tbectrl2_get(void)
{
    return REG_PL_RD(MDM_TBECTRL2_ADDR);
}

/**
 * @brief Sets the TBECTRL2 register to a value.
 * The TBECTRL2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tbectrl2_set(uint32_t value)
{
    REG_PL_WR(MDM_TBECTRL2_ADDR, value);
}

// field definitions
/// PEAK_SEARCH_DELTA field mask
#define MDM_PEAK_SEARCH_DELTA_MASK       ((uint32_t)0x0001F000)
/// PEAK_SEARCH_DELTA field LSB position
#define MDM_PEAK_SEARCH_DELTA_LSB        12
/// PEAK_SEARCH_DELTA field width
#define MDM_PEAK_SEARCH_DELTA_WIDTH      ((uint32_t)0x00000005)
/// PEAK_SEARCH_NDL_EN field bit
#define MDM_PEAK_SEARCH_NDL_EN_BIT       ((uint32_t)0x00000800)
/// PEAK_SEARCH_NDL_EN field position
#define MDM_PEAK_SEARCH_NDL_EN_POS       11
/// PEAK_SEARCH_NDL_INDEX field mask
#define MDM_PEAK_SEARCH_NDL_INDEX_MASK   ((uint32_t)0x00000700)
/// PEAK_SEARCH_NDL_INDEX field LSB position
#define MDM_PEAK_SEARCH_NDL_INDEX_LSB    8
/// PEAK_SEARCH_NDL_INDEX field width
#define MDM_PEAK_SEARCH_NDL_INDEX_WIDTH  ((uint32_t)0x00000003)
/// TBE_BIAS field mask
#define MDM_TBE_BIAS_MASK                ((uint32_t)0x000000FF)
/// TBE_BIAS field LSB position
#define MDM_TBE_BIAS_LSB                 0
/// TBE_BIAS field width
#define MDM_TBE_BIAS_WIDTH               ((uint32_t)0x00000008)

/// PEAK_SEARCH_DELTA field reset value
#define MDM_PEAK_SEARCH_DELTA_RST        0x7
/// PEAK_SEARCH_NDL_EN field reset value
#define MDM_PEAK_SEARCH_NDL_EN_RST       0x1
/// PEAK_SEARCH_NDL_INDEX field reset value
#define MDM_PEAK_SEARCH_NDL_INDEX_RST    0x7
/// TBE_BIAS field reset value
#define MDM_TBE_BIAS_RST                 0x4

/**
 * @brief Constructs a value for the TBECTRL2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] peaksearchdelta - The value to use for the PeakSearchDelta field.
 * @param[in] peaksearchndlen - The value to use for the PeakSearchNdlEn field.
 * @param[in] peaksearchndlindex - The value to use for the PeakSearchNdlIndex field.
 * @param[in] tbebias - The value to use for the TBEBias field.
 */
__INLINE void mdm_tbectrl2_pack(uint8_t peaksearchdelta, uint8_t peaksearchndlen, uint8_t peaksearchndlindex, uint8_t tbebias)
{
    ASSERT_ERR((((uint32_t)peaksearchdelta << 12) & ~((uint32_t)0x0001F000)) == 0);
    ASSERT_ERR((((uint32_t)peaksearchndlen << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)peaksearchndlindex << 8) & ~((uint32_t)0x00000700)) == 0);
    ASSERT_ERR((((uint32_t)tbebias << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_TBECTRL2_ADDR,  ((uint32_t)peaksearchdelta << 12) | ((uint32_t)peaksearchndlen << 11) | ((uint32_t)peaksearchndlindex << 8) | ((uint32_t)tbebias << 0));
}

/**
 * @brief Unpacks TBECTRL2's fields from current value of the TBECTRL2 register.
 *
 * Reads the TBECTRL2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] peaksearchdelta - Will be populated with the current value of this field from the register.
 * @param[out] peaksearchndlen - Will be populated with the current value of this field from the register.
 * @param[out] peaksearchndlindex - Will be populated with the current value of this field from the register.
 * @param[out] tbebias - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tbectrl2_unpack(uint8_t* peaksearchdelta, uint8_t* peaksearchndlen, uint8_t* peaksearchndlindex, uint8_t* tbebias)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL2_ADDR);

    *peaksearchdelta = (localVal & ((uint32_t)0x0001F000)) >> 12;
    *peaksearchndlen = (localVal & ((uint32_t)0x00000800)) >> 11;
    *peaksearchndlindex = (localVal & ((uint32_t)0x00000700)) >> 8;
    *tbebias = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the PeakSearchDelta field in the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read and the PeakSearchDelta field's value will be returned.
 *
 * @return The current value of the PeakSearchDelta field in the TBECTRL2 register.
 */
__INLINE uint8_t mdm_peak_search_delta_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL2_ADDR);
    return ((localVal & ((uint32_t)0x0001F000)) >> 12);
}

/**
 * @brief Sets the PeakSearchDelta field of the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] peaksearchdelta - The value to set the field to.
 */
__INLINE void mdm_peak_search_delta_setf(uint8_t peaksearchdelta)
{
    ASSERT_ERR((((uint32_t)peaksearchdelta << 12) & ~((uint32_t)0x0001F000)) == 0);
    REG_PL_WR(MDM_TBECTRL2_ADDR, (REG_PL_RD(MDM_TBECTRL2_ADDR) & ~((uint32_t)0x0001F000)) | ((uint32_t)peaksearchdelta << 12));
}

/**
 * @brief Returns the current value of the PeakSearchNdlEn field in the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read and the PeakSearchNdlEn field's value will be returned.
 *
 * @return The current value of the PeakSearchNdlEn field in the TBECTRL2 register.
 */
__INLINE uint8_t mdm_peak_search_ndl_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the PeakSearchNdlEn field of the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] peaksearchndlen - The value to set the field to.
 */
__INLINE void mdm_peak_search_ndl_en_setf(uint8_t peaksearchndlen)
{
    ASSERT_ERR((((uint32_t)peaksearchndlen << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(MDM_TBECTRL2_ADDR, (REG_PL_RD(MDM_TBECTRL2_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)peaksearchndlen << 11));
}

/**
 * @brief Returns the current value of the PeakSearchNdlIndex field in the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read and the PeakSearchNdlIndex field's value will be returned.
 *
 * @return The current value of the PeakSearchNdlIndex field in the TBECTRL2 register.
 */
__INLINE uint8_t mdm_peak_search_ndl_index_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

/**
 * @brief Sets the PeakSearchNdlIndex field of the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] peaksearchndlindex - The value to set the field to.
 */
__INLINE void mdm_peak_search_ndl_index_setf(uint8_t peaksearchndlindex)
{
    ASSERT_ERR((((uint32_t)peaksearchndlindex << 8) & ~((uint32_t)0x00000700)) == 0);
    REG_PL_WR(MDM_TBECTRL2_ADDR, (REG_PL_RD(MDM_TBECTRL2_ADDR) & ~((uint32_t)0x00000700)) | ((uint32_t)peaksearchndlindex << 8));
}

/**
 * @brief Returns the current value of the TBEBias field in the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read and the TBEBias field's value will be returned.
 *
 * @return The current value of the TBEBias field in the TBECTRL2 register.
 */
__INLINE uint8_t mdm_tbe_bias_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TBECTRL2_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the TBEBias field of the TBECTRL2 register.
 *
 * The TBECTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tbebias - The value to set the field to.
 */
__INLINE void mdm_tbe_bias_setf(uint8_t tbebias)
{
    ASSERT_ERR((((uint32_t)tbebias << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_TBECTRL2_ADDR, (REG_PL_RD(MDM_TBECTRL2_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)tbebias << 0));
}

/// @}

/**
 * @name TDFOCTRL0 register definitions
 * <table>
 * <caption>TDFOCTRL0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:12 <td>     AutoCorrCompareRatioHigh <td> R <td> R/W <td> 0x340
 * <tr><td> 09:00 <td>      AutoCorrCompareRatioLow <td> R <td> R/W <td> 0x100
 * </table>
 *
 * @{
 */

/// Address of the TDFOCTRL0 register
#define MDM_TDFOCTRL0_ADDR   0x24C00864
/// Offset of the TDFOCTRL0 register from the base address
#define MDM_TDFOCTRL0_OFFSET 0x00000864
/// Index of the TDFOCTRL0 register
#define MDM_TDFOCTRL0_INDEX  0x00000219
/// Reset value of the TDFOCTRL0 register
#define MDM_TDFOCTRL0_RESET  0x00340100

/**
 * @brief Returns the current value of the TDFOCTRL0 register.
 * The TDFOCTRL0 register will be read and its value returned.
 * @return The current value of the TDFOCTRL0 register.
 */
__INLINE uint32_t mdm_tdfoctrl0_get(void)
{
    return REG_PL_RD(MDM_TDFOCTRL0_ADDR);
}

/**
 * @brief Sets the TDFOCTRL0 register to a value.
 * The TDFOCTRL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tdfoctrl0_set(uint32_t value)
{
    REG_PL_WR(MDM_TDFOCTRL0_ADDR, value);
}

// field definitions
/// AUTO_CORR_COMPARE_RATIO_HIGH field mask
#define MDM_AUTO_CORR_COMPARE_RATIO_HIGH_MASK   ((uint32_t)0x003FF000)
/// AUTO_CORR_COMPARE_RATIO_HIGH field LSB position
#define MDM_AUTO_CORR_COMPARE_RATIO_HIGH_LSB    12
/// AUTO_CORR_COMPARE_RATIO_HIGH field width
#define MDM_AUTO_CORR_COMPARE_RATIO_HIGH_WIDTH  ((uint32_t)0x0000000A)
/// AUTO_CORR_COMPARE_RATIO_LOW field mask
#define MDM_AUTO_CORR_COMPARE_RATIO_LOW_MASK    ((uint32_t)0x000003FF)
/// AUTO_CORR_COMPARE_RATIO_LOW field LSB position
#define MDM_AUTO_CORR_COMPARE_RATIO_LOW_LSB     0
/// AUTO_CORR_COMPARE_RATIO_LOW field width
#define MDM_AUTO_CORR_COMPARE_RATIO_LOW_WIDTH   ((uint32_t)0x0000000A)

/// AUTO_CORR_COMPARE_RATIO_HIGH field reset value
#define MDM_AUTO_CORR_COMPARE_RATIO_HIGH_RST    0x340
/// AUTO_CORR_COMPARE_RATIO_LOW field reset value
#define MDM_AUTO_CORR_COMPARE_RATIO_LOW_RST     0x100

/**
 * @brief Constructs a value for the TDFOCTRL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] autocorrcompareratiohigh - The value to use for the AutoCorrCompareRatioHigh field.
 * @param[in] autocorrcompareratiolow - The value to use for the AutoCorrCompareRatioLow field.
 */
__INLINE void mdm_tdfoctrl0_pack(uint16_t autocorrcompareratiohigh, uint16_t autocorrcompareratiolow)
{
    ASSERT_ERR((((uint32_t)autocorrcompareratiohigh << 12) & ~((uint32_t)0x003FF000)) == 0);
    ASSERT_ERR((((uint32_t)autocorrcompareratiolow << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL0_ADDR,  ((uint32_t)autocorrcompareratiohigh << 12) | ((uint32_t)autocorrcompareratiolow << 0));
}

/**
 * @brief Unpacks TDFOCTRL0's fields from current value of the TDFOCTRL0 register.
 *
 * Reads the TDFOCTRL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] autocorrcompareratiohigh - Will be populated with the current value of this field from the register.
 * @param[out] autocorrcompareratiolow - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tdfoctrl0_unpack(uint16_t* autocorrcompareratiohigh, uint16_t* autocorrcompareratiolow)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL0_ADDR);

    *autocorrcompareratiohigh = (localVal & ((uint32_t)0x003FF000)) >> 12;
    *autocorrcompareratiolow = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the AutoCorrCompareRatioHigh field in the TDFOCTRL0 register.
 *
 * The TDFOCTRL0 register will be read and the AutoCorrCompareRatioHigh field's value will be returned.
 *
 * @return The current value of the AutoCorrCompareRatioHigh field in the TDFOCTRL0 register.
 */
__INLINE uint16_t mdm_auto_corr_compare_ratio_high_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x003FF000)) >> 12);
}

/**
 * @brief Sets the AutoCorrCompareRatioHigh field of the TDFOCTRL0 register.
 *
 * The TDFOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrcompareratiohigh - The value to set the field to.
 */
__INLINE void mdm_auto_corr_compare_ratio_high_setf(uint16_t autocorrcompareratiohigh)
{
    ASSERT_ERR((((uint32_t)autocorrcompareratiohigh << 12) & ~((uint32_t)0x003FF000)) == 0);
    REG_PL_WR(MDM_TDFOCTRL0_ADDR, (REG_PL_RD(MDM_TDFOCTRL0_ADDR) & ~((uint32_t)0x003FF000)) | ((uint32_t)autocorrcompareratiohigh << 12));
}

/**
 * @brief Returns the current value of the AutoCorrCompareRatioLow field in the TDFOCTRL0 register.
 *
 * The TDFOCTRL0 register will be read and the AutoCorrCompareRatioLow field's value will be returned.
 *
 * @return The current value of the AutoCorrCompareRatioLow field in the TDFOCTRL0 register.
 */
__INLINE uint16_t mdm_auto_corr_compare_ratio_low_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the AutoCorrCompareRatioLow field of the TDFOCTRL0 register.
 *
 * The TDFOCTRL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrcompareratiolow - The value to set the field to.
 */
__INLINE void mdm_auto_corr_compare_ratio_low_setf(uint16_t autocorrcompareratiolow)
{
    ASSERT_ERR((((uint32_t)autocorrcompareratiolow << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL0_ADDR, (REG_PL_RD(MDM_TDFOCTRL0_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)autocorrcompareratiolow << 0));
}

/// @}

/**
 * @name TDFOCTRL1 register definitions
 * <table>
 * <caption>TDFOCTRL1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 20:12 <td>       AutoCorrPlatSumEnd <td> R <td> R/W <td> 0x42
 * <tr><td> 08:00 <td>     AutoCorrPlatSumStart <td> R <td> R/W <td> 0x32
 * </table>
 *
 * @{
 */

/// Address of the TDFOCTRL1 register
#define MDM_TDFOCTRL1_ADDR   0x24C00868
/// Offset of the TDFOCTRL1 register from the base address
#define MDM_TDFOCTRL1_OFFSET 0x00000868
/// Index of the TDFOCTRL1 register
#define MDM_TDFOCTRL1_INDEX  0x0000021A
/// Reset value of the TDFOCTRL1 register
#define MDM_TDFOCTRL1_RESET  0x00042032

/**
 * @brief Returns the current value of the TDFOCTRL1 register.
 * The TDFOCTRL1 register will be read and its value returned.
 * @return The current value of the TDFOCTRL1 register.
 */
__INLINE uint32_t mdm_tdfoctrl1_get(void)
{
    return REG_PL_RD(MDM_TDFOCTRL1_ADDR);
}

/**
 * @brief Sets the TDFOCTRL1 register to a value.
 * The TDFOCTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tdfoctrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_TDFOCTRL1_ADDR, value);
}

// field definitions
/// AUTO_CORR_PLAT_SUM_END field mask
#define MDM_AUTO_CORR_PLAT_SUM_END_MASK     ((uint32_t)0x001FF000)
/// AUTO_CORR_PLAT_SUM_END field LSB position
#define MDM_AUTO_CORR_PLAT_SUM_END_LSB      12
/// AUTO_CORR_PLAT_SUM_END field width
#define MDM_AUTO_CORR_PLAT_SUM_END_WIDTH    ((uint32_t)0x00000009)
/// AUTO_CORR_PLAT_SUM_START field mask
#define MDM_AUTO_CORR_PLAT_SUM_START_MASK   ((uint32_t)0x000001FF)
/// AUTO_CORR_PLAT_SUM_START field LSB position
#define MDM_AUTO_CORR_PLAT_SUM_START_LSB    0
/// AUTO_CORR_PLAT_SUM_START field width
#define MDM_AUTO_CORR_PLAT_SUM_START_WIDTH  ((uint32_t)0x00000009)

/// AUTO_CORR_PLAT_SUM_END field reset value
#define MDM_AUTO_CORR_PLAT_SUM_END_RST      0x42
/// AUTO_CORR_PLAT_SUM_START field reset value
#define MDM_AUTO_CORR_PLAT_SUM_START_RST    0x32

/**
 * @brief Constructs a value for the TDFOCTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] autocorrplatsumend - The value to use for the AutoCorrPlatSumEnd field.
 * @param[in] autocorrplatsumstart - The value to use for the AutoCorrPlatSumStart field.
 */
__INLINE void mdm_tdfoctrl1_pack(uint16_t autocorrplatsumend, uint16_t autocorrplatsumstart)
{
    ASSERT_ERR((((uint32_t)autocorrplatsumend << 12) & ~((uint32_t)0x001FF000)) == 0);
    ASSERT_ERR((((uint32_t)autocorrplatsumstart << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL1_ADDR,  ((uint32_t)autocorrplatsumend << 12) | ((uint32_t)autocorrplatsumstart << 0));
}

/**
 * @brief Unpacks TDFOCTRL1's fields from current value of the TDFOCTRL1 register.
 *
 * Reads the TDFOCTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] autocorrplatsumend - Will be populated with the current value of this field from the register.
 * @param[out] autocorrplatsumstart - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tdfoctrl1_unpack(uint16_t* autocorrplatsumend, uint16_t* autocorrplatsumstart)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL1_ADDR);

    *autocorrplatsumend = (localVal & ((uint32_t)0x001FF000)) >> 12;
    *autocorrplatsumstart = (localVal & ((uint32_t)0x000001FF)) >> 0;
}

/**
 * @brief Returns the current value of the AutoCorrPlatSumEnd field in the TDFOCTRL1 register.
 *
 * The TDFOCTRL1 register will be read and the AutoCorrPlatSumEnd field's value will be returned.
 *
 * @return The current value of the AutoCorrPlatSumEnd field in the TDFOCTRL1 register.
 */
__INLINE uint16_t mdm_auto_corr_plat_sum_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x001FF000)) >> 12);
}

/**
 * @brief Sets the AutoCorrPlatSumEnd field of the TDFOCTRL1 register.
 *
 * The TDFOCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrplatsumend - The value to set the field to.
 */
__INLINE void mdm_auto_corr_plat_sum_end_setf(uint16_t autocorrplatsumend)
{
    ASSERT_ERR((((uint32_t)autocorrplatsumend << 12) & ~((uint32_t)0x001FF000)) == 0);
    REG_PL_WR(MDM_TDFOCTRL1_ADDR, (REG_PL_RD(MDM_TDFOCTRL1_ADDR) & ~((uint32_t)0x001FF000)) | ((uint32_t)autocorrplatsumend << 12));
}

/**
 * @brief Returns the current value of the AutoCorrPlatSumStart field in the TDFOCTRL1 register.
 *
 * The TDFOCTRL1 register will be read and the AutoCorrPlatSumStart field's value will be returned.
 *
 * @return The current value of the AutoCorrPlatSumStart field in the TDFOCTRL1 register.
 */
__INLINE uint16_t mdm_auto_corr_plat_sum_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x000001FF)) >> 0);
}

/**
 * @brief Sets the AutoCorrPlatSumStart field of the TDFOCTRL1 register.
 *
 * The TDFOCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrplatsumstart - The value to set the field to.
 */
__INLINE void mdm_auto_corr_plat_sum_start_setf(uint16_t autocorrplatsumstart)
{
    ASSERT_ERR((((uint32_t)autocorrplatsumstart << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL1_ADDR, (REG_PL_RD(MDM_TDFOCTRL1_ADDR) & ~((uint32_t)0x000001FF)) | ((uint32_t)autocorrplatsumstart << 0));
}

/// @}

/**
 * @name TDFOCTRL2 register definitions
 * <table>
 * <caption>TDFOCTRL2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 28:20 <td>                 AutoCorrPhSumEnd <td> R <td> R/W <td> 0x4C
 * <tr><td> 18:10 <td>               AutoCorrPhSumStart <td> R <td> R/W <td> 0x1C
 * <tr><td> 08:00 <td>      AutoCorrPlatFallSearchStart <td> R <td> R/W <td> 0x44
 * </table>
 *
 * @{
 */

/// Address of the TDFOCTRL2 register
#define MDM_TDFOCTRL2_ADDR   0x24C0086C
/// Offset of the TDFOCTRL2 register from the base address
#define MDM_TDFOCTRL2_OFFSET 0x0000086C
/// Index of the TDFOCTRL2 register
#define MDM_TDFOCTRL2_INDEX  0x0000021B
/// Reset value of the TDFOCTRL2 register
#define MDM_TDFOCTRL2_RESET  0x04C07044

/**
 * @brief Returns the current value of the TDFOCTRL2 register.
 * The TDFOCTRL2 register will be read and its value returned.
 * @return The current value of the TDFOCTRL2 register.
 */
__INLINE uint32_t mdm_tdfoctrl2_get(void)
{
    return REG_PL_RD(MDM_TDFOCTRL2_ADDR);
}

/**
 * @brief Sets the TDFOCTRL2 register to a value.
 * The TDFOCTRL2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tdfoctrl2_set(uint32_t value)
{
    REG_PL_WR(MDM_TDFOCTRL2_ADDR, value);
}

// field definitions
/// AUTO_CORR_PH_SUM_END field mask
#define MDM_AUTO_CORR_PH_SUM_END_MASK               ((uint32_t)0x1FF00000)
/// AUTO_CORR_PH_SUM_END field LSB position
#define MDM_AUTO_CORR_PH_SUM_END_LSB                20
/// AUTO_CORR_PH_SUM_END field width
#define MDM_AUTO_CORR_PH_SUM_END_WIDTH              ((uint32_t)0x00000009)
/// AUTO_CORR_PH_SUM_START field mask
#define MDM_AUTO_CORR_PH_SUM_START_MASK             ((uint32_t)0x0007FC00)
/// AUTO_CORR_PH_SUM_START field LSB position
#define MDM_AUTO_CORR_PH_SUM_START_LSB              10
/// AUTO_CORR_PH_SUM_START field width
#define MDM_AUTO_CORR_PH_SUM_START_WIDTH            ((uint32_t)0x00000009)
/// AUTO_CORR_PLAT_FALL_SEARCH_START field mask
#define MDM_AUTO_CORR_PLAT_FALL_SEARCH_START_MASK   ((uint32_t)0x000001FF)
/// AUTO_CORR_PLAT_FALL_SEARCH_START field LSB position
#define MDM_AUTO_CORR_PLAT_FALL_SEARCH_START_LSB    0
/// AUTO_CORR_PLAT_FALL_SEARCH_START field width
#define MDM_AUTO_CORR_PLAT_FALL_SEARCH_START_WIDTH  ((uint32_t)0x00000009)

/// AUTO_CORR_PH_SUM_END field reset value
#define MDM_AUTO_CORR_PH_SUM_END_RST                0x4C
/// AUTO_CORR_PH_SUM_START field reset value
#define MDM_AUTO_CORR_PH_SUM_START_RST              0x1C
/// AUTO_CORR_PLAT_FALL_SEARCH_START field reset value
#define MDM_AUTO_CORR_PLAT_FALL_SEARCH_START_RST    0x44

/**
 * @brief Constructs a value for the TDFOCTRL2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] autocorrphsumend - The value to use for the AutoCorrPhSumEnd field.
 * @param[in] autocorrphsumstart - The value to use for the AutoCorrPhSumStart field.
 * @param[in] autocorrplatfallsearchstart - The value to use for the AutoCorrPlatFallSearchStart field.
 */
__INLINE void mdm_tdfoctrl2_pack(uint16_t autocorrphsumend, uint16_t autocorrphsumstart, uint16_t autocorrplatfallsearchstart)
{
    ASSERT_ERR((((uint32_t)autocorrphsumend << 20) & ~((uint32_t)0x1FF00000)) == 0);
    ASSERT_ERR((((uint32_t)autocorrphsumstart << 10) & ~((uint32_t)0x0007FC00)) == 0);
    ASSERT_ERR((((uint32_t)autocorrplatfallsearchstart << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL2_ADDR,  ((uint32_t)autocorrphsumend << 20) | ((uint32_t)autocorrphsumstart << 10) | ((uint32_t)autocorrplatfallsearchstart << 0));
}

/**
 * @brief Unpacks TDFOCTRL2's fields from current value of the TDFOCTRL2 register.
 *
 * Reads the TDFOCTRL2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] autocorrphsumend - Will be populated with the current value of this field from the register.
 * @param[out] autocorrphsumstart - Will be populated with the current value of this field from the register.
 * @param[out] autocorrplatfallsearchstart - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tdfoctrl2_unpack(uint16_t* autocorrphsumend, uint16_t* autocorrphsumstart, uint16_t* autocorrplatfallsearchstart)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL2_ADDR);

    *autocorrphsumend = (localVal & ((uint32_t)0x1FF00000)) >> 20;
    *autocorrphsumstart = (localVal & ((uint32_t)0x0007FC00)) >> 10;
    *autocorrplatfallsearchstart = (localVal & ((uint32_t)0x000001FF)) >> 0;
}

/**
 * @brief Returns the current value of the AutoCorrPhSumEnd field in the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read and the AutoCorrPhSumEnd field's value will be returned.
 *
 * @return The current value of the AutoCorrPhSumEnd field in the TDFOCTRL2 register.
 */
__INLINE uint16_t mdm_auto_corr_ph_sum_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x1FF00000)) >> 20);
}

/**
 * @brief Sets the AutoCorrPhSumEnd field of the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrphsumend - The value to set the field to.
 */
__INLINE void mdm_auto_corr_ph_sum_end_setf(uint16_t autocorrphsumend)
{
    ASSERT_ERR((((uint32_t)autocorrphsumend << 20) & ~((uint32_t)0x1FF00000)) == 0);
    REG_PL_WR(MDM_TDFOCTRL2_ADDR, (REG_PL_RD(MDM_TDFOCTRL2_ADDR) & ~((uint32_t)0x1FF00000)) | ((uint32_t)autocorrphsumend << 20));
}

/**
 * @brief Returns the current value of the AutoCorrPhSumStart field in the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read and the AutoCorrPhSumStart field's value will be returned.
 *
 * @return The current value of the AutoCorrPhSumStart field in the TDFOCTRL2 register.
 */
__INLINE uint16_t mdm_auto_corr_ph_sum_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x0007FC00)) >> 10);
}

/**
 * @brief Sets the AutoCorrPhSumStart field of the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrphsumstart - The value to set the field to.
 */
__INLINE void mdm_auto_corr_ph_sum_start_setf(uint16_t autocorrphsumstart)
{
    ASSERT_ERR((((uint32_t)autocorrphsumstart << 10) & ~((uint32_t)0x0007FC00)) == 0);
    REG_PL_WR(MDM_TDFOCTRL2_ADDR, (REG_PL_RD(MDM_TDFOCTRL2_ADDR) & ~((uint32_t)0x0007FC00)) | ((uint32_t)autocorrphsumstart << 10));
}

/**
 * @brief Returns the current value of the AutoCorrPlatFallSearchStart field in the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read and the AutoCorrPlatFallSearchStart field's value will be returned.
 *
 * @return The current value of the AutoCorrPlatFallSearchStart field in the TDFOCTRL2 register.
 */
__INLINE uint16_t mdm_auto_corr_plat_fall_search_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x000001FF)) >> 0);
}

/**
 * @brief Sets the AutoCorrPlatFallSearchStart field of the TDFOCTRL2 register.
 *
 * The TDFOCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrplatfallsearchstart - The value to set the field to.
 */
__INLINE void mdm_auto_corr_plat_fall_search_start_setf(uint16_t autocorrplatfallsearchstart)
{
    ASSERT_ERR((((uint32_t)autocorrplatfallsearchstart << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_PL_WR(MDM_TDFOCTRL2_ADDR, (REG_PL_RD(MDM_TDFOCTRL2_ADDR) & ~((uint32_t)0x000001FF)) | ((uint32_t)autocorrplatfallsearchstart << 0));
}

/// @}

/**
 * @name TDFOCTRL3 register definitions
 * <table>
 * <caption>TDFOCTRL3 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>                       TDFOCOMPEN <td> R <td> R/W <td> 1
 * <tr><td> 27    <td>                    TDFOFINEESTEN <td> R <td> R/W <td> 1
 * <tr><td> 24    <td>                   TDFOCPESLOPEEN <td> R <td> R/W <td> 0
 * <tr><td> 23:12 <td>              TDFOPlatFallTimeOut <td> R <td> R/W <td> 0x1F4
 * <tr><td> 07:04 <td>      AutoCorrPlatFallIndexOffset <td> R <td> R/W <td> 0x8
 * <tr><td> 03:00 <td>                AutoCorrFallCount <td> R <td> R/W <td> 0x1
 * </table>
 *
 * @{
 */

/// Address of the TDFOCTRL3 register
#define MDM_TDFOCTRL3_ADDR   0x24C00870
/// Offset of the TDFOCTRL3 register from the base address
#define MDM_TDFOCTRL3_OFFSET 0x00000870
/// Index of the TDFOCTRL3 register
#define MDM_TDFOCTRL3_INDEX  0x0000021C
/// Reset value of the TDFOCTRL3 register
#define MDM_TDFOCTRL3_RESET  0x881F4081

/**
 * @brief Returns the current value of the TDFOCTRL3 register.
 * The TDFOCTRL3 register will be read and its value returned.
 * @return The current value of the TDFOCTRL3 register.
 */
__INLINE uint32_t mdm_tdfoctrl3_get(void)
{
    return REG_PL_RD(MDM_TDFOCTRL3_ADDR);
}

/**
 * @brief Sets the TDFOCTRL3 register to a value.
 * The TDFOCTRL3 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_tdfoctrl3_set(uint32_t value)
{
    REG_PL_WR(MDM_TDFOCTRL3_ADDR, value);
}

// field definitions
/// TDFOCOMPEN field bit
#define MDM_TDFOCOMPEN_BIT                          ((uint32_t)0x80000000)
/// TDFOCOMPEN field position
#define MDM_TDFOCOMPEN_POS                          31
/// TDFOFINEESTEN field bit
#define MDM_TDFOFINEESTEN_BIT                       ((uint32_t)0x08000000)
/// TDFOFINEESTEN field position
#define MDM_TDFOFINEESTEN_POS                       27
/// TDFOCPESLOPEEN field bit
#define MDM_TDFOCPESLOPEEN_BIT                      ((uint32_t)0x01000000)
/// TDFOCPESLOPEEN field position
#define MDM_TDFOCPESLOPEEN_POS                      24
/// TDFO_PLAT_FALL_TIME_OUT field mask
#define MDM_TDFO_PLAT_FALL_TIME_OUT_MASK            ((uint32_t)0x00FFF000)
/// TDFO_PLAT_FALL_TIME_OUT field LSB position
#define MDM_TDFO_PLAT_FALL_TIME_OUT_LSB             12
/// TDFO_PLAT_FALL_TIME_OUT field width
#define MDM_TDFO_PLAT_FALL_TIME_OUT_WIDTH           ((uint32_t)0x0000000C)
/// AUTO_CORR_PLAT_FALL_INDEX_OFFSET field mask
#define MDM_AUTO_CORR_PLAT_FALL_INDEX_OFFSET_MASK   ((uint32_t)0x000000F0)
/// AUTO_CORR_PLAT_FALL_INDEX_OFFSET field LSB position
#define MDM_AUTO_CORR_PLAT_FALL_INDEX_OFFSET_LSB    4
/// AUTO_CORR_PLAT_FALL_INDEX_OFFSET field width
#define MDM_AUTO_CORR_PLAT_FALL_INDEX_OFFSET_WIDTH  ((uint32_t)0x00000004)
/// AUTO_CORR_FALL_COUNT field mask
#define MDM_AUTO_CORR_FALL_COUNT_MASK               ((uint32_t)0x0000000F)
/// AUTO_CORR_FALL_COUNT field LSB position
#define MDM_AUTO_CORR_FALL_COUNT_LSB                0
/// AUTO_CORR_FALL_COUNT field width
#define MDM_AUTO_CORR_FALL_COUNT_WIDTH              ((uint32_t)0x00000004)

/// TDFOCOMPEN field reset value
#define MDM_TDFOCOMPEN_RST                          0x1
/// TDFOFINEESTEN field reset value
#define MDM_TDFOFINEESTEN_RST                       0x1
/// TDFOCPESLOPEEN field reset value
#define MDM_TDFOCPESLOPEEN_RST                      0x0
/// TDFO_PLAT_FALL_TIME_OUT field reset value
#define MDM_TDFO_PLAT_FALL_TIME_OUT_RST             0x1F4
/// AUTO_CORR_PLAT_FALL_INDEX_OFFSET field reset value
#define MDM_AUTO_CORR_PLAT_FALL_INDEX_OFFSET_RST    0x8
/// AUTO_CORR_FALL_COUNT field reset value
#define MDM_AUTO_CORR_FALL_COUNT_RST                0x1

/**
 * @brief Constructs a value for the TDFOCTRL3 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tdfocompen - The value to use for the TDFOCOMPEN field.
 * @param[in] tdfofineesten - The value to use for the TDFOFINEESTEN field.
 * @param[in] tdfocpeslopeen - The value to use for the TDFOCPESLOPEEN field.
 * @param[in] tdfoplatfalltimeout - The value to use for the TDFOPlatFallTimeOut field.
 * @param[in] autocorrplatfallindexoffset - The value to use for the AutoCorrPlatFallIndexOffset field.
 * @param[in] autocorrfallcount - The value to use for the AutoCorrFallCount field.
 */
__INLINE void mdm_tdfoctrl3_pack(uint8_t tdfocompen, uint8_t tdfofineesten, uint8_t tdfocpeslopeen, uint16_t tdfoplatfalltimeout, uint8_t autocorrplatfallindexoffset, uint8_t autocorrfallcount)
{
    ASSERT_ERR((((uint32_t)tdfocompen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)tdfofineesten << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)tdfocpeslopeen << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)tdfoplatfalltimeout << 12) & ~((uint32_t)0x00FFF000)) == 0);
    ASSERT_ERR((((uint32_t)autocorrplatfallindexoffset << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)autocorrfallcount << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_TDFOCTRL3_ADDR,  ((uint32_t)tdfocompen << 31) | ((uint32_t)tdfofineesten << 27) | ((uint32_t)tdfocpeslopeen << 24) | ((uint32_t)tdfoplatfalltimeout << 12) | ((uint32_t)autocorrplatfallindexoffset << 4) | ((uint32_t)autocorrfallcount << 0));
}

/**
 * @brief Unpacks TDFOCTRL3's fields from current value of the TDFOCTRL3 register.
 *
 * Reads the TDFOCTRL3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tdfocompen - Will be populated with the current value of this field from the register.
 * @param[out] tdfofineesten - Will be populated with the current value of this field from the register.
 * @param[out] tdfocpeslopeen - Will be populated with the current value of this field from the register.
 * @param[out] tdfoplatfalltimeout - Will be populated with the current value of this field from the register.
 * @param[out] autocorrplatfallindexoffset - Will be populated with the current value of this field from the register.
 * @param[out] autocorrfallcount - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_tdfoctrl3_unpack(uint8_t* tdfocompen, uint8_t* tdfofineesten, uint8_t* tdfocpeslopeen, uint16_t* tdfoplatfalltimeout, uint8_t* autocorrplatfallindexoffset, uint8_t* autocorrfallcount)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL3_ADDR);

    *tdfocompen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *tdfofineesten = (localVal & ((uint32_t)0x08000000)) >> 27;
    *tdfocpeslopeen = (localVal & ((uint32_t)0x01000000)) >> 24;
    *tdfoplatfalltimeout = (localVal & ((uint32_t)0x00FFF000)) >> 12;
    *autocorrplatfallindexoffset = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *autocorrfallcount = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the TDFOCOMPEN field in the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read and the TDFOCOMPEN field's value will be returned.
 *
 * @return The current value of the TDFOCOMPEN field in the TDFOCTRL3 register.
 */
__INLINE uint8_t mdm_tdfocompen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the TDFOCOMPEN field of the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdfocompen - The value to set the field to.
 */
__INLINE void mdm_tdfocompen_setf(uint8_t tdfocompen)
{
    ASSERT_ERR((((uint32_t)tdfocompen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MDM_TDFOCTRL3_ADDR, (REG_PL_RD(MDM_TDFOCTRL3_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)tdfocompen << 31));
}

/**
 * @brief Returns the current value of the TDFOFINEESTEN field in the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read and the TDFOFINEESTEN field's value will be returned.
 *
 * @return The current value of the TDFOFINEESTEN field in the TDFOCTRL3 register.
 */
__INLINE uint8_t mdm_tdfofineesten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

/**
 * @brief Sets the TDFOFINEESTEN field of the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdfofineesten - The value to set the field to.
 */
__INLINE void mdm_tdfofineesten_setf(uint8_t tdfofineesten)
{
    ASSERT_ERR((((uint32_t)tdfofineesten << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_PL_WR(MDM_TDFOCTRL3_ADDR, (REG_PL_RD(MDM_TDFOCTRL3_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)tdfofineesten << 27));
}

/**
 * @brief Returns the current value of the TDFOCPESLOPEEN field in the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read and the TDFOCPESLOPEEN field's value will be returned.
 *
 * @return The current value of the TDFOCPESLOPEEN field in the TDFOCTRL3 register.
 */
__INLINE uint8_t mdm_tdfocpeslopeen_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the TDFOCPESLOPEEN field of the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdfocpeslopeen - The value to set the field to.
 */
__INLINE void mdm_tdfocpeslopeen_setf(uint8_t tdfocpeslopeen)
{
    ASSERT_ERR((((uint32_t)tdfocpeslopeen << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(MDM_TDFOCTRL3_ADDR, (REG_PL_RD(MDM_TDFOCTRL3_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)tdfocpeslopeen << 24));
}

/**
 * @brief Returns the current value of the TDFOPlatFallTimeOut field in the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read and the TDFOPlatFallTimeOut field's value will be returned.
 *
 * @return The current value of the TDFOPlatFallTimeOut field in the TDFOCTRL3 register.
 */
__INLINE uint16_t mdm_tdfo_plat_fall_time_out_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00FFF000)) >> 12);
}

/**
 * @brief Sets the TDFOPlatFallTimeOut field of the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdfoplatfalltimeout - The value to set the field to.
 */
__INLINE void mdm_tdfo_plat_fall_time_out_setf(uint16_t tdfoplatfalltimeout)
{
    ASSERT_ERR((((uint32_t)tdfoplatfalltimeout << 12) & ~((uint32_t)0x00FFF000)) == 0);
    REG_PL_WR(MDM_TDFOCTRL3_ADDR, (REG_PL_RD(MDM_TDFOCTRL3_ADDR) & ~((uint32_t)0x00FFF000)) | ((uint32_t)tdfoplatfalltimeout << 12));
}

/**
 * @brief Returns the current value of the AutoCorrPlatFallIndexOffset field in the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read and the AutoCorrPlatFallIndexOffset field's value will be returned.
 *
 * @return The current value of the AutoCorrPlatFallIndexOffset field in the TDFOCTRL3 register.
 */
__INLINE uint8_t mdm_auto_corr_plat_fall_index_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the AutoCorrPlatFallIndexOffset field of the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrplatfallindexoffset - The value to set the field to.
 */
__INLINE void mdm_auto_corr_plat_fall_index_offset_setf(uint8_t autocorrplatfallindexoffset)
{
    ASSERT_ERR((((uint32_t)autocorrplatfallindexoffset << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_TDFOCTRL3_ADDR, (REG_PL_RD(MDM_TDFOCTRL3_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)autocorrplatfallindexoffset << 4));
}

/**
 * @brief Returns the current value of the AutoCorrFallCount field in the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read and the AutoCorrFallCount field's value will be returned.
 *
 * @return The current value of the AutoCorrFallCount field in the TDFOCTRL3 register.
 */
__INLINE uint8_t mdm_auto_corr_fall_count_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TDFOCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the AutoCorrFallCount field of the TDFOCTRL3 register.
 *
 * The TDFOCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] autocorrfallcount - The value to set the field to.
 */
__INLINE void mdm_auto_corr_fall_count_setf(uint8_t autocorrfallcount)
{
    ASSERT_ERR((((uint32_t)autocorrfallcount << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_TDFOCTRL3_ADDR, (REG_PL_RD(MDM_TDFOCTRL3_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)autocorrfallcount << 0));
}

/// @}

/**
 * @name RXTDCTRL2 register definitions
 * <table>
 * <caption>RXTDCTRL2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 21:16 <td>     TDAdjust80ShortGI <td> R <td> R/W <td> 0x10
 * <tr><td> 13:08 <td>     TDAdjust40ShortGI <td> R <td> R/W <td> 0x8
 * <tr><td> 05:00 <td>     TDAdjust20ShortGI <td> R <td> R/W <td> 0x4
 * </table>
 *
 * @{
 */

/// Address of the RXTDCTRL2 register
#define MDM_RXTDCTRL2_ADDR   0x24C00880
/// Offset of the RXTDCTRL2 register from the base address
#define MDM_RXTDCTRL2_OFFSET 0x00000880
/// Index of the RXTDCTRL2 register
#define MDM_RXTDCTRL2_INDEX  0x00000220
/// Reset value of the RXTDCTRL2 register
#define MDM_RXTDCTRL2_RESET  0x00100804

/**
 * @brief Returns the current value of the RXTDCTRL2 register.
 * The RXTDCTRL2 register will be read and its value returned.
 * @return The current value of the RXTDCTRL2 register.
 */
__INLINE uint32_t mdm_rxtdctrl2_get(void)
{
    return REG_PL_RD(MDM_RXTDCTRL2_ADDR);
}

/**
 * @brief Sets the RXTDCTRL2 register to a value.
 * The RXTDCTRL2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxtdctrl2_set(uint32_t value)
{
    REG_PL_WR(MDM_RXTDCTRL2_ADDR, value);
}

// field definitions
/// TD_ADJUST_80_SHORT_GI field mask
#define MDM_TD_ADJUST_80_SHORT_GI_MASK   ((uint32_t)0x003F0000)
/// TD_ADJUST_80_SHORT_GI field LSB position
#define MDM_TD_ADJUST_80_SHORT_GI_LSB    16
/// TD_ADJUST_80_SHORT_GI field width
#define MDM_TD_ADJUST_80_SHORT_GI_WIDTH  ((uint32_t)0x00000006)
/// TD_ADJUST_40_SHORT_GI field mask
#define MDM_TD_ADJUST_40_SHORT_GI_MASK   ((uint32_t)0x00003F00)
/// TD_ADJUST_40_SHORT_GI field LSB position
#define MDM_TD_ADJUST_40_SHORT_GI_LSB    8
/// TD_ADJUST_40_SHORT_GI field width
#define MDM_TD_ADJUST_40_SHORT_GI_WIDTH  ((uint32_t)0x00000006)
/// TD_ADJUST_20_SHORT_GI field mask
#define MDM_TD_ADJUST_20_SHORT_GI_MASK   ((uint32_t)0x0000003F)
/// TD_ADJUST_20_SHORT_GI field LSB position
#define MDM_TD_ADJUST_20_SHORT_GI_LSB    0
/// TD_ADJUST_20_SHORT_GI field width
#define MDM_TD_ADJUST_20_SHORT_GI_WIDTH  ((uint32_t)0x00000006)

/// TD_ADJUST_80_SHORT_GI field reset value
#define MDM_TD_ADJUST_80_SHORT_GI_RST    0x10
/// TD_ADJUST_40_SHORT_GI field reset value
#define MDM_TD_ADJUST_40_SHORT_GI_RST    0x8
/// TD_ADJUST_20_SHORT_GI field reset value
#define MDM_TD_ADJUST_20_SHORT_GI_RST    0x4

/**
 * @brief Constructs a value for the RXTDCTRL2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] tdadjust80shortgi - The value to use for the TDAdjust80ShortGI field.
 * @param[in] tdadjust40shortgi - The value to use for the TDAdjust40ShortGI field.
 * @param[in] tdadjust20shortgi - The value to use for the TDAdjust20ShortGI field.
 */
__INLINE void mdm_rxtdctrl2_pack(uint8_t tdadjust80shortgi, uint8_t tdadjust40shortgi, uint8_t tdadjust20shortgi)
{
    ASSERT_ERR((((uint32_t)tdadjust80shortgi << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)tdadjust40shortgi << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)tdadjust20shortgi << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(MDM_RXTDCTRL2_ADDR,  ((uint32_t)tdadjust80shortgi << 16) | ((uint32_t)tdadjust40shortgi << 8) | ((uint32_t)tdadjust20shortgi << 0));
}

/**
 * @brief Unpacks RXTDCTRL2's fields from current value of the RXTDCTRL2 register.
 *
 * Reads the RXTDCTRL2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] tdadjust80shortgi - Will be populated with the current value of this field from the register.
 * @param[out] tdadjust40shortgi - Will be populated with the current value of this field from the register.
 * @param[out] tdadjust20shortgi - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxtdctrl2_unpack(uint8_t* tdadjust80shortgi, uint8_t* tdadjust40shortgi, uint8_t* tdadjust20shortgi)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL2_ADDR);

    *tdadjust80shortgi = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *tdadjust40shortgi = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *tdadjust20shortgi = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the TDAdjust80ShortGI field in the RXTDCTRL2 register.
 *
 * The RXTDCTRL2 register will be read and the TDAdjust80ShortGI field's value will be returned.
 *
 * @return The current value of the TDAdjust80ShortGI field in the RXTDCTRL2 register.
 */
__INLINE uint8_t mdm_td_adjust_80_short_gi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

/**
 * @brief Sets the TDAdjust80ShortGI field of the RXTDCTRL2 register.
 *
 * The RXTDCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdadjust80shortgi - The value to set the field to.
 */
__INLINE void mdm_td_adjust_80_short_gi_setf(uint8_t tdadjust80shortgi)
{
    ASSERT_ERR((((uint32_t)tdadjust80shortgi << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_PL_WR(MDM_RXTDCTRL2_ADDR, (REG_PL_RD(MDM_RXTDCTRL2_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)tdadjust80shortgi << 16));
}

/**
 * @brief Returns the current value of the TDAdjust40ShortGI field in the RXTDCTRL2 register.
 *
 * The RXTDCTRL2 register will be read and the TDAdjust40ShortGI field's value will be returned.
 *
 * @return The current value of the TDAdjust40ShortGI field in the RXTDCTRL2 register.
 */
__INLINE uint8_t mdm_td_adjust_40_short_gi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

/**
 * @brief Sets the TDAdjust40ShortGI field of the RXTDCTRL2 register.
 *
 * The RXTDCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdadjust40shortgi - The value to set the field to.
 */
__INLINE void mdm_td_adjust_40_short_gi_setf(uint8_t tdadjust40shortgi)
{
    ASSERT_ERR((((uint32_t)tdadjust40shortgi << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_PL_WR(MDM_RXTDCTRL2_ADDR, (REG_PL_RD(MDM_RXTDCTRL2_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)tdadjust40shortgi << 8));
}

/**
 * @brief Returns the current value of the TDAdjust20ShortGI field in the RXTDCTRL2 register.
 *
 * The RXTDCTRL2 register will be read and the TDAdjust20ShortGI field's value will be returned.
 *
 * @return The current value of the TDAdjust20ShortGI field in the RXTDCTRL2 register.
 */
__INLINE uint8_t mdm_td_adjust_20_short_gi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXTDCTRL2_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the TDAdjust20ShortGI field of the RXTDCTRL2 register.
 *
 * The RXTDCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] tdadjust20shortgi - The value to set the field to.
 */
__INLINE void mdm_td_adjust_20_short_gi_setf(uint8_t tdadjust20shortgi)
{
    ASSERT_ERR((((uint32_t)tdadjust20shortgi << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(MDM_RXTDCTRL2_ADDR, (REG_PL_RD(MDM_RXTDCTRL2_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)tdadjust20shortgi << 0));
}

/// @}

/**
 * @name TXCTRL1 register definitions
 * <table>
 * <caption>TXCTRL1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:08 <td> TXFEOFDM20DELAY <td> R <td> R/W <td> 0x1C
 * <tr><td> 07:00 <td>   TXFEDSSSDELAY <td> R <td> R/W <td> 0x30
 * </table>
 *
 * @{
 */

/// Address of the TXCTRL1 register
#define MDM_TXCTRL1_ADDR   0x24C0088C
/// Offset of the TXCTRL1 register from the base address
#define MDM_TXCTRL1_OFFSET 0x0000088C
/// Index of the TXCTRL1 register
#define MDM_TXCTRL1_INDEX  0x00000223
/// Reset value of the TXCTRL1 register
#define MDM_TXCTRL1_RESET  0x00001C30

/**
 * @brief Returns the current value of the TXCTRL1 register.
 * The TXCTRL1 register will be read and its value returned.
 * @return The current value of the TXCTRL1 register.
 */
__INLINE uint32_t mdm_txctrl1_get(void)
{
    return REG_PL_RD(MDM_TXCTRL1_ADDR);
}

/**
 * @brief Sets the TXCTRL1 register to a value.
 * The TXCTRL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_txctrl1_set(uint32_t value)
{
    REG_PL_WR(MDM_TXCTRL1_ADDR, value);
}

// field definitions
/// TXFEOFDM20DELAY field mask
#define MDM_TXFEOFDM20DELAY_MASK   ((uint32_t)0x0000FF00)
/// TXFEOFDM20DELAY field LSB position
#define MDM_TXFEOFDM20DELAY_LSB    8
/// TXFEOFDM20DELAY field width
#define MDM_TXFEOFDM20DELAY_WIDTH  ((uint32_t)0x00000008)
/// TXFEDSSSDELAY field mask
#define MDM_TXFEDSSSDELAY_MASK     ((uint32_t)0x000000FF)
/// TXFEDSSSDELAY field LSB position
#define MDM_TXFEDSSSDELAY_LSB      0
/// TXFEDSSSDELAY field width
#define MDM_TXFEDSSSDELAY_WIDTH    ((uint32_t)0x00000008)

/// TXFEOFDM20DELAY field reset value
#define MDM_TXFEOFDM20DELAY_RST    0x1C
/// TXFEDSSSDELAY field reset value
#define MDM_TXFEDSSSDELAY_RST      0x30

/**
 * @brief Constructs a value for the TXCTRL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] txfeofdm20delay - The value to use for the TXFEOFDM20DELAY field.
 * @param[in] txfedsssdelay - The value to use for the TXFEDSSSDELAY field.
 */
__INLINE void mdm_txctrl1_pack(uint8_t txfeofdm20delay, uint8_t txfedsssdelay)
{
    ASSERT_ERR((((uint32_t)txfeofdm20delay << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)txfedsssdelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_TXCTRL1_ADDR,  ((uint32_t)txfeofdm20delay << 8) | ((uint32_t)txfedsssdelay << 0));
}

/**
 * @brief Unpacks TXCTRL1's fields from current value of the TXCTRL1 register.
 *
 * Reads the TXCTRL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] txfeofdm20delay - Will be populated with the current value of this field from the register.
 * @param[out] txfedsssdelay - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_txctrl1_unpack(uint8_t* txfeofdm20delay, uint8_t* txfedsssdelay)
{
    uint32_t localVal = REG_PL_RD(MDM_TXCTRL1_ADDR);

    *txfeofdm20delay = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *txfedsssdelay = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the TXFEOFDM20DELAY field in the TXCTRL1 register.
 *
 * The TXCTRL1 register will be read and the TXFEOFDM20DELAY field's value will be returned.
 *
 * @return The current value of the TXFEOFDM20DELAY field in the TXCTRL1 register.
 */
__INLINE uint8_t mdm_txfeofdm20delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the TXFEOFDM20DELAY field of the TXCTRL1 register.
 *
 * The TXCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txfeofdm20delay - The value to set the field to.
 */
__INLINE void mdm_txfeofdm20delay_setf(uint8_t txfeofdm20delay)
{
    ASSERT_ERR((((uint32_t)txfeofdm20delay << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(MDM_TXCTRL1_ADDR, (REG_PL_RD(MDM_TXCTRL1_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)txfeofdm20delay << 8));
}

/**
 * @brief Returns the current value of the TXFEDSSSDELAY field in the TXCTRL1 register.
 *
 * The TXCTRL1 register will be read and the TXFEDSSSDELAY field's value will be returned.
 *
 * @return The current value of the TXFEDSSSDELAY field in the TXCTRL1 register.
 */
__INLINE uint8_t mdm_txfedsssdelay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXCTRL1_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the TXFEDSSSDELAY field of the TXCTRL1 register.
 *
 * The TXCTRL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txfedsssdelay - The value to set the field to.
 */
__INLINE void mdm_txfedsssdelay_setf(uint8_t txfedsssdelay)
{
    ASSERT_ERR((((uint32_t)txfedsssdelay << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_TXCTRL1_ADDR, (REG_PL_RD(MDM_TXCTRL1_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)txfedsssdelay << 0));
}

/// @}

/**
 * @name TXTDCSD register definitions
 * <table>
 * <caption>TXTDCSD bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 04:00 <td> TXTDCSD <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the TXTDCSD register
#define MDM_TXTDCSD_ADDR   0x24C00890
/// Offset of the TXTDCSD register from the base address
#define MDM_TXTDCSD_OFFSET 0x00000890
/// Index of the TXTDCSD register
#define MDM_TXTDCSD_INDEX  0x00000224
/// Reset value of the TXTDCSD register
#define MDM_TXTDCSD_RESET  0x00000000

/**
 * @brief Returns the current value of the TXTDCSD register.
 * The TXTDCSD register will be read and its value returned.
 * @return The current value of the TXTDCSD register.
 */
__INLINE uint32_t mdm_txtdcsd_get(void)
{
    return REG_PL_RD(MDM_TXTDCSD_ADDR);
}

/**
 * @brief Sets the TXTDCSD register to a value.
 * The TXTDCSD register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_txtdcsd_set(uint32_t value)
{
    REG_PL_WR(MDM_TXTDCSD_ADDR, value);
}

// field definitions
/// TXTDCSD field mask
#define MDM_TXTDCSD_MASK   ((uint32_t)0x0000001F)
/// TXTDCSD field LSB position
#define MDM_TXTDCSD_LSB    0
/// TXTDCSD field width
#define MDM_TXTDCSD_WIDTH  ((uint32_t)0x00000005)

/// TXTDCSD field reset value
#define MDM_TXTDCSD_RST    0x0

/**
 * @brief Returns the current value of the TXTDCSD field in the TXTDCSD register.
 *
 * The TXTDCSD register will be read and the TXTDCSD field's value will be returned.
 *
 * @return The current value of the TXTDCSD field in the TXTDCSD register.
 */
__INLINE uint8_t mdm_txtdcsd_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_TXTDCSD_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000001F)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the TXTDCSD field of the TXTDCSD register.
 *
 * The TXTDCSD register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txtdcsd - The value to set the field to.
 */
__INLINE void mdm_txtdcsd_setf(uint8_t txtdcsd)
{
    ASSERT_ERR((((uint32_t)txtdcsd << 0) & ~((uint32_t)0x0000001F)) == 0);
    REG_PL_WR(MDM_TXTDCSD_ADDR, (uint32_t)txtdcsd << 0);
}

/// @}

#if RW_NX_LDPC_DEC
/**
 * @name LDPCDECCTRL register definitions
 * <table>
 * <caption>LDPCDECCTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 28    <td>   LDPCDECSTATEN <td> R <td> R/W <td> 1
 * <tr><td> 24    <td> LDPCDECITEFORCE <td> R <td> R/W <td> 0
 * <tr><td> 20    <td>  LDPCDECSPLITEN <td> R <td> R/W <td> 0
 * <tr><td> 16:08 <td>  LDPCDECFIFOTHR <td> R <td> R/W <td> 0x52
 * <tr><td> 05:00 <td> LDPCDECLLRUNITY <td> R <td> R/W <td> 0x2
 * </table>
 *
 * @{
 */

/// Address of the LDPCDECCTRL register
#define MDM_LDPCDECCTRL_ADDR   0x24C00894
/// Offset of the LDPCDECCTRL register from the base address
#define MDM_LDPCDECCTRL_OFFSET 0x00000894
/// Index of the LDPCDECCTRL register
#define MDM_LDPCDECCTRL_INDEX  0x00000225
/// Reset value of the LDPCDECCTRL register
#define MDM_LDPCDECCTRL_RESET  0x10005202

/**
 * @brief Returns the current value of the LDPCDECCTRL register.
 * The LDPCDECCTRL register will be read and its value returned.
 * @return The current value of the LDPCDECCTRL register.
 */
__INLINE uint32_t mdm_ldpcdecctrl_get(void)
{
    return REG_PL_RD(MDM_LDPCDECCTRL_ADDR);
}

/**
 * @brief Sets the LDPCDECCTRL register to a value.
 * The LDPCDECCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_ldpcdecctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_LDPCDECCTRL_ADDR, value);
}

// field definitions
/// LDPCDECSTATEN field bit
#define MDM_LDPCDECSTATEN_BIT      ((uint32_t)0x10000000)
/// LDPCDECSTATEN field position
#define MDM_LDPCDECSTATEN_POS      28
/// LDPCDECITEFORCE field bit
#define MDM_LDPCDECITEFORCE_BIT    ((uint32_t)0x01000000)
/// LDPCDECITEFORCE field position
#define MDM_LDPCDECITEFORCE_POS    24
/// LDPCDECSPLITEN field bit
#define MDM_LDPCDECSPLITEN_BIT     ((uint32_t)0x00100000)
/// LDPCDECSPLITEN field position
#define MDM_LDPCDECSPLITEN_POS     20
/// LDPCDECFIFOTHR field mask
#define MDM_LDPCDECFIFOTHR_MASK    ((uint32_t)0x0001FF00)
/// LDPCDECFIFOTHR field LSB position
#define MDM_LDPCDECFIFOTHR_LSB     8
/// LDPCDECFIFOTHR field width
#define MDM_LDPCDECFIFOTHR_WIDTH   ((uint32_t)0x00000009)
/// LDPCDECLLRUNITY field mask
#define MDM_LDPCDECLLRUNITY_MASK   ((uint32_t)0x0000003F)
/// LDPCDECLLRUNITY field LSB position
#define MDM_LDPCDECLLRUNITY_LSB    0
/// LDPCDECLLRUNITY field width
#define MDM_LDPCDECLLRUNITY_WIDTH  ((uint32_t)0x00000006)

/// LDPCDECSTATEN field reset value
#define MDM_LDPCDECSTATEN_RST      0x1
/// LDPCDECITEFORCE field reset value
#define MDM_LDPCDECITEFORCE_RST    0x0
/// LDPCDECSPLITEN field reset value
#define MDM_LDPCDECSPLITEN_RST     0x0
/// LDPCDECFIFOTHR field reset value
#define MDM_LDPCDECFIFOTHR_RST     0x52
/// LDPCDECLLRUNITY field reset value
#define MDM_LDPCDECLLRUNITY_RST    0x2

/**
 * @brief Constructs a value for the LDPCDECCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ldpcdecstaten - The value to use for the LDPCDECSTATEN field.
 * @param[in] ldpcdeciteforce - The value to use for the LDPCDECITEFORCE field.
 * @param[in] ldpcdecspliten - The value to use for the LDPCDECSPLITEN field.
 * @param[in] ldpcdecfifothr - The value to use for the LDPCDECFIFOTHR field.
 * @param[in] ldpcdecllrunity - The value to use for the LDPCDECLLRUNITY field.
 */
__INLINE void mdm_ldpcdecctrl_pack(uint8_t ldpcdecstaten, uint8_t ldpcdeciteforce, uint8_t ldpcdecspliten, uint16_t ldpcdecfifothr, uint8_t ldpcdecllrunity)
{
    ASSERT_ERR((((uint32_t)ldpcdecstaten << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdeciteforce << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecspliten << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecfifothr << 8) & ~((uint32_t)0x0001FF00)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecllrunity << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(MDM_LDPCDECCTRL_ADDR,  ((uint32_t)ldpcdecstaten << 28) | ((uint32_t)ldpcdeciteforce << 24) | ((uint32_t)ldpcdecspliten << 20) | ((uint32_t)ldpcdecfifothr << 8) | ((uint32_t)ldpcdecllrunity << 0));
}

/**
 * @brief Unpacks LDPCDECCTRL's fields from current value of the LDPCDECCTRL register.
 *
 * Reads the LDPCDECCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ldpcdecstaten - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdeciteforce - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecspliten - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecfifothr - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecllrunity - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_ldpcdecctrl_unpack(uint8_t* ldpcdecstaten, uint8_t* ldpcdeciteforce, uint8_t* ldpcdecspliten, uint16_t* ldpcdecfifothr, uint8_t* ldpcdecllrunity)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECCTRL_ADDR);

    *ldpcdecstaten = (localVal & ((uint32_t)0x10000000)) >> 28;
    *ldpcdeciteforce = (localVal & ((uint32_t)0x01000000)) >> 24;
    *ldpcdecspliten = (localVal & ((uint32_t)0x00100000)) >> 20;
    *ldpcdecfifothr = (localVal & ((uint32_t)0x0001FF00)) >> 8;
    *ldpcdecllrunity = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

/**
 * @brief Returns the current value of the LDPCDECSTATEN field in the LDPCDECCTRL register.
 *
 * The LDPCDECCTRL register will be read and the LDPCDECSTATEN field's value will be returned.
 *
 * @return The current value of the LDPCDECSTATEN field in the LDPCDECCTRL register.
 */
__INLINE uint8_t mdm_ldpcdecstaten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECCTRL_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the LDPCDECSTATEN field of the LDPCDECCTRL register.
 *
 * The LDPCDECCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecstaten - The value to set the field to.
 */
__INLINE void mdm_ldpcdecstaten_setf(uint8_t ldpcdecstaten)
{
    ASSERT_ERR((((uint32_t)ldpcdecstaten << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(MDM_LDPCDECCTRL_ADDR, (REG_PL_RD(MDM_LDPCDECCTRL_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)ldpcdecstaten << 28));
}

/**
 * @brief Returns the current value of the LDPCDECITEFORCE field in the LDPCDECCTRL register.
 *
 * The LDPCDECCTRL register will be read and the LDPCDECITEFORCE field's value will be returned.
 *
 * @return The current value of the LDPCDECITEFORCE field in the LDPCDECCTRL register.
 */
__INLINE uint8_t mdm_ldpcdeciteforce_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECCTRL_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

/**
 * @brief Sets the LDPCDECITEFORCE field of the LDPCDECCTRL register.
 *
 * The LDPCDECCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdeciteforce - The value to set the field to.
 */
__INLINE void mdm_ldpcdeciteforce_setf(uint8_t ldpcdeciteforce)
{
    ASSERT_ERR((((uint32_t)ldpcdeciteforce << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_PL_WR(MDM_LDPCDECCTRL_ADDR, (REG_PL_RD(MDM_LDPCDECCTRL_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)ldpcdeciteforce << 24));
}

/**
 * @brief Returns the current value of the LDPCDECSPLITEN field in the LDPCDECCTRL register.
 *
 * The LDPCDECCTRL register will be read and the LDPCDECSPLITEN field's value will be returned.
 *
 * @return The current value of the LDPCDECSPLITEN field in the LDPCDECCTRL register.
 */
__INLINE uint8_t mdm_ldpcdecspliten_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

/**
 * @brief Sets the LDPCDECSPLITEN field of the LDPCDECCTRL register.
 *
 * The LDPCDECCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecspliten - The value to set the field to.
 */
__INLINE void mdm_ldpcdecspliten_setf(uint8_t ldpcdecspliten)
{
    ASSERT_ERR((((uint32_t)ldpcdecspliten << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_PL_WR(MDM_LDPCDECCTRL_ADDR, (REG_PL_RD(MDM_LDPCDECCTRL_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)ldpcdecspliten << 20));
}

/**
 * @brief Returns the current value of the LDPCDECFIFOTHR field in the LDPCDECCTRL register.
 *
 * The LDPCDECCTRL register will be read and the LDPCDECFIFOTHR field's value will be returned.
 *
 * @return The current value of the LDPCDECFIFOTHR field in the LDPCDECCTRL register.
 */
__INLINE uint16_t mdm_ldpcdecfifothr_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECCTRL_ADDR);
    return ((localVal & ((uint32_t)0x0001FF00)) >> 8);
}

/**
 * @brief Sets the LDPCDECFIFOTHR field of the LDPCDECCTRL register.
 *
 * The LDPCDECCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecfifothr - The value to set the field to.
 */
__INLINE void mdm_ldpcdecfifothr_setf(uint16_t ldpcdecfifothr)
{
    ASSERT_ERR((((uint32_t)ldpcdecfifothr << 8) & ~((uint32_t)0x0001FF00)) == 0);
    REG_PL_WR(MDM_LDPCDECCTRL_ADDR, (REG_PL_RD(MDM_LDPCDECCTRL_ADDR) & ~((uint32_t)0x0001FF00)) | ((uint32_t)ldpcdecfifothr << 8));
}

/**
 * @brief Returns the current value of the LDPCDECLLRUNITY field in the LDPCDECCTRL register.
 *
 * The LDPCDECCTRL register will be read and the LDPCDECLLRUNITY field's value will be returned.
 *
 * @return The current value of the LDPCDECLLRUNITY field in the LDPCDECCTRL register.
 */
__INLINE uint8_t mdm_ldpcdecllrunity_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECCTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

/**
 * @brief Sets the LDPCDECLLRUNITY field of the LDPCDECCTRL register.
 *
 * The LDPCDECCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecllrunity - The value to set the field to.
 */
__INLINE void mdm_ldpcdecllrunity_setf(uint8_t ldpcdecllrunity)
{
    ASSERT_ERR((((uint32_t)ldpcdecllrunity << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_PL_WR(MDM_LDPCDECCTRL_ADDR, (REG_PL_RD(MDM_LDPCDECCTRL_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)ldpcdecllrunity << 0));
}

#endif // RW_NX_LDPC_DEC
/// @}

#if RW_NX_LDPC_DEC
/**
 * @name LDPCDECITECTRL register definitions
 * <table>
 * <caption>LDPCDECITECTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 28:24 <td>    LDPCDECEARLYITE <td> R <td> R/W <td> 0x1F
 * <tr><td> 23:16 <td> LDPCDECMAXRUNCOUNT <td> R <td> R/W <td> 0x2F
 * <tr><td> 15:08 <td> LDPCDECENDRUNCOUNT <td> R <td> R/W <td> 0x2F
 * <tr><td> 07:00 <td>      LDPCDECNOMITE <td> R <td> R/W <td> 0x2F
 * </table>
 *
 * @{
 */

/// Address of the LDPCDECITECTRL register
#define MDM_LDPCDECITECTRL_ADDR   0x24C00898
/// Offset of the LDPCDECITECTRL register from the base address
#define MDM_LDPCDECITECTRL_OFFSET 0x00000898
/// Index of the LDPCDECITECTRL register
#define MDM_LDPCDECITECTRL_INDEX  0x00000226
/// Reset value of the LDPCDECITECTRL register
#define MDM_LDPCDECITECTRL_RESET  0x1F2F2F2F

/**
 * @brief Returns the current value of the LDPCDECITECTRL register.
 * The LDPCDECITECTRL register will be read and its value returned.
 * @return The current value of the LDPCDECITECTRL register.
 */
__INLINE uint32_t mdm_ldpcdecitectrl_get(void)
{
    return REG_PL_RD(MDM_LDPCDECITECTRL_ADDR);
}

/**
 * @brief Sets the LDPCDECITECTRL register to a value.
 * The LDPCDECITECTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_ldpcdecitectrl_set(uint32_t value)
{
    REG_PL_WR(MDM_LDPCDECITECTRL_ADDR, value);
}

// field definitions
/// LDPCDECEARLYITE field mask
#define MDM_LDPCDECEARLYITE_MASK      ((uint32_t)0x1F000000)
/// LDPCDECEARLYITE field LSB position
#define MDM_LDPCDECEARLYITE_LSB       24
/// LDPCDECEARLYITE field width
#define MDM_LDPCDECEARLYITE_WIDTH     ((uint32_t)0x00000005)
/// LDPCDECMAXRUNCOUNT field mask
#define MDM_LDPCDECMAXRUNCOUNT_MASK   ((uint32_t)0x00FF0000)
/// LDPCDECMAXRUNCOUNT field LSB position
#define MDM_LDPCDECMAXRUNCOUNT_LSB    16
/// LDPCDECMAXRUNCOUNT field width
#define MDM_LDPCDECMAXRUNCOUNT_WIDTH  ((uint32_t)0x00000008)
/// LDPCDECENDRUNCOUNT field mask
#define MDM_LDPCDECENDRUNCOUNT_MASK   ((uint32_t)0x0000FF00)
/// LDPCDECENDRUNCOUNT field LSB position
#define MDM_LDPCDECENDRUNCOUNT_LSB    8
/// LDPCDECENDRUNCOUNT field width
#define MDM_LDPCDECENDRUNCOUNT_WIDTH  ((uint32_t)0x00000008)
/// LDPCDECNOMITE field mask
#define MDM_LDPCDECNOMITE_MASK        ((uint32_t)0x000000FF)
/// LDPCDECNOMITE field LSB position
#define MDM_LDPCDECNOMITE_LSB         0
/// LDPCDECNOMITE field width
#define MDM_LDPCDECNOMITE_WIDTH       ((uint32_t)0x00000008)

/// LDPCDECEARLYITE field reset value
#define MDM_LDPCDECEARLYITE_RST       0x1F
/// LDPCDECMAXRUNCOUNT field reset value
#define MDM_LDPCDECMAXRUNCOUNT_RST    0x2F
/// LDPCDECENDRUNCOUNT field reset value
#define MDM_LDPCDECENDRUNCOUNT_RST    0x2F
/// LDPCDECNOMITE field reset value
#define MDM_LDPCDECNOMITE_RST         0x2F

/**
 * @brief Constructs a value for the LDPCDECITECTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ldpcdecearlyite - The value to use for the LDPCDECEARLYITE field.
 * @param[in] ldpcdecmaxruncount - The value to use for the LDPCDECMAXRUNCOUNT field.
 * @param[in] ldpcdecendruncount - The value to use for the LDPCDECENDRUNCOUNT field.
 * @param[in] ldpcdecnomite - The value to use for the LDPCDECNOMITE field.
 */
__INLINE void mdm_ldpcdecitectrl_pack(uint8_t ldpcdecearlyite, uint8_t ldpcdecmaxruncount, uint8_t ldpcdecendruncount, uint8_t ldpcdecnomite)
{
    ASSERT_ERR((((uint32_t)ldpcdecearlyite << 24) & ~((uint32_t)0x1F000000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecmaxruncount << 16) & ~((uint32_t)0x00FF0000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecendruncount << 8) & ~((uint32_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecnomite << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_LDPCDECITECTRL_ADDR,  ((uint32_t)ldpcdecearlyite << 24) | ((uint32_t)ldpcdecmaxruncount << 16) | ((uint32_t)ldpcdecendruncount << 8) | ((uint32_t)ldpcdecnomite << 0));
}

/**
 * @brief Unpacks LDPCDECITECTRL's fields from current value of the LDPCDECITECTRL register.
 *
 * Reads the LDPCDECITECTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ldpcdecearlyite - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecmaxruncount - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecendruncount - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecnomite - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_ldpcdecitectrl_unpack(uint8_t* ldpcdecearlyite, uint8_t* ldpcdecmaxruncount, uint8_t* ldpcdecendruncount, uint8_t* ldpcdecnomite)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECITECTRL_ADDR);

    *ldpcdecearlyite = (localVal & ((uint32_t)0x1F000000)) >> 24;
    *ldpcdecmaxruncount = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *ldpcdecendruncount = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *ldpcdecnomite = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

/**
 * @brief Returns the current value of the LDPCDECEARLYITE field in the LDPCDECITECTRL register.
 *
 * The LDPCDECITECTRL register will be read and the LDPCDECEARLYITE field's value will be returned.
 *
 * @return The current value of the LDPCDECEARLYITE field in the LDPCDECITECTRL register.
 */
__INLINE uint8_t mdm_ldpcdecearlyite_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECITECTRL_ADDR);
    return ((localVal & ((uint32_t)0x1F000000)) >> 24);
}

/**
 * @brief Sets the LDPCDECEARLYITE field of the LDPCDECITECTRL register.
 *
 * The LDPCDECITECTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecearlyite - The value to set the field to.
 */
__INLINE void mdm_ldpcdecearlyite_setf(uint8_t ldpcdecearlyite)
{
    ASSERT_ERR((((uint32_t)ldpcdecearlyite << 24) & ~((uint32_t)0x1F000000)) == 0);
    REG_PL_WR(MDM_LDPCDECITECTRL_ADDR, (REG_PL_RD(MDM_LDPCDECITECTRL_ADDR) & ~((uint32_t)0x1F000000)) | ((uint32_t)ldpcdecearlyite << 24));
}

/**
 * @brief Returns the current value of the LDPCDECMAXRUNCOUNT field in the LDPCDECITECTRL register.
 *
 * The LDPCDECITECTRL register will be read and the LDPCDECMAXRUNCOUNT field's value will be returned.
 *
 * @return The current value of the LDPCDECMAXRUNCOUNT field in the LDPCDECITECTRL register.
 */
__INLINE uint8_t mdm_ldpcdecmaxruncount_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECITECTRL_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

/**
 * @brief Sets the LDPCDECMAXRUNCOUNT field of the LDPCDECITECTRL register.
 *
 * The LDPCDECITECTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecmaxruncount - The value to set the field to.
 */
__INLINE void mdm_ldpcdecmaxruncount_setf(uint8_t ldpcdecmaxruncount)
{
    ASSERT_ERR((((uint32_t)ldpcdecmaxruncount << 16) & ~((uint32_t)0x00FF0000)) == 0);
    REG_PL_WR(MDM_LDPCDECITECTRL_ADDR, (REG_PL_RD(MDM_LDPCDECITECTRL_ADDR) & ~((uint32_t)0x00FF0000)) | ((uint32_t)ldpcdecmaxruncount << 16));
}

/**
 * @brief Returns the current value of the LDPCDECENDRUNCOUNT field in the LDPCDECITECTRL register.
 *
 * The LDPCDECITECTRL register will be read and the LDPCDECENDRUNCOUNT field's value will be returned.
 *
 * @return The current value of the LDPCDECENDRUNCOUNT field in the LDPCDECITECTRL register.
 */
__INLINE uint8_t mdm_ldpcdecendruncount_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECITECTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

/**
 * @brief Sets the LDPCDECENDRUNCOUNT field of the LDPCDECITECTRL register.
 *
 * The LDPCDECITECTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecendruncount - The value to set the field to.
 */
__INLINE void mdm_ldpcdecendruncount_setf(uint8_t ldpcdecendruncount)
{
    ASSERT_ERR((((uint32_t)ldpcdecendruncount << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(MDM_LDPCDECITECTRL_ADDR, (REG_PL_RD(MDM_LDPCDECITECTRL_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)ldpcdecendruncount << 8));
}

/**
 * @brief Returns the current value of the LDPCDECNOMITE field in the LDPCDECITECTRL register.
 *
 * The LDPCDECITECTRL register will be read and the LDPCDECNOMITE field's value will be returned.
 *
 * @return The current value of the LDPCDECNOMITE field in the LDPCDECITECTRL register.
 */
__INLINE uint8_t mdm_ldpcdecnomite_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_LDPCDECITECTRL_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief Sets the LDPCDECNOMITE field of the LDPCDECITECTRL register.
 *
 * The LDPCDECITECTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecnomite - The value to set the field to.
 */
__INLINE void mdm_ldpcdecnomite_setf(uint8_t ldpcdecnomite)
{
    ASSERT_ERR((((uint32_t)ldpcdecnomite << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(MDM_LDPCDECITECTRL_ADDR, (REG_PL_RD(MDM_LDPCDECITECTRL_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)ldpcdecnomite << 0));
}

#endif // RW_NX_LDPC_DEC
/// @}

/**
 * @name RXCTRL2 register definitions
 * <table>
 * <caption>RXCTRL2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> RXFRAMEVIOLATIONMASK <td> R <td> R/W <td> 0xFFFFFFFF
 * </table>
 *
 * @{
 */

/// Address of the RXCTRL2 register
#define MDM_RXCTRL2_ADDR   0x24C0089C
/// Offset of the RXCTRL2 register from the base address
#define MDM_RXCTRL2_OFFSET 0x0000089C
/// Index of the RXCTRL2 register
#define MDM_RXCTRL2_INDEX  0x00000227
/// Reset value of the RXCTRL2 register
#define MDM_RXCTRL2_RESET  0xFFFFFFFF

/**
 * @brief Returns the current value of the RXCTRL2 register.
 * The RXCTRL2 register will be read and its value returned.
 * @return The current value of the RXCTRL2 register.
 */
__INLINE uint32_t mdm_rxctrl2_get(void)
{
    return REG_PL_RD(MDM_RXCTRL2_ADDR);
}

/**
 * @brief Sets the RXCTRL2 register to a value.
 * The RXCTRL2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl2_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL2_ADDR, value);
}

// field definitions
/// RXFRAMEVIOLATIONMASK field mask
#define MDM_RXFRAMEVIOLATIONMASK_MASK   ((uint32_t)0xFFFFFFFF)
/// RXFRAMEVIOLATIONMASK field LSB position
#define MDM_RXFRAMEVIOLATIONMASK_LSB    0
/// RXFRAMEVIOLATIONMASK field width
#define MDM_RXFRAMEVIOLATIONMASK_WIDTH  ((uint32_t)0x00000020)

/// RXFRAMEVIOLATIONMASK field reset value
#define MDM_RXFRAMEVIOLATIONMASK_RST    0xFFFFFFFF

/**
 * @brief Returns the current value of the RXFRAMEVIOLATIONMASK field in the RXCTRL2 register.
 *
 * The RXCTRL2 register will be read and the RXFRAMEVIOLATIONMASK field's value will be returned.
 *
 * @return The current value of the RXFRAMEVIOLATIONMASK field in the RXCTRL2 register.
 */
__INLINE uint32_t mdm_rxframeviolationmask_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the RXFRAMEVIOLATIONMASK field of the RXCTRL2 register.
 *
 * The RXCTRL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxframeviolationmask - The value to set the field to.
 */
__INLINE void mdm_rxframeviolationmask_setf(uint32_t rxframeviolationmask)
{
    ASSERT_ERR((((uint32_t)rxframeviolationmask << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(MDM_RXCTRL2_ADDR, (uint32_t)rxframeviolationmask << 0);
}

/// @}

#if RW_NX_LDPC_DEC
/**
 * @name RXCTRL3 register definitions
 * <table>
 * <caption>RXCTRL3 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:16 <td> RXLDPCNDBPSMAXSGI <td> R <td> R/W <td> 0x618
 * <tr><td> 15:00 <td> RXLDPCNDBPSMAXLGI <td> R <td> R/W <td> 0x618
 * </table>
 *
 * @{
 */

/// Address of the RXCTRL3 register
#define MDM_RXCTRL3_ADDR   0x24C008A0
/// Offset of the RXCTRL3 register from the base address
#define MDM_RXCTRL3_OFFSET 0x000008A0
/// Index of the RXCTRL3 register
#define MDM_RXCTRL3_INDEX  0x00000228
/// Reset value of the RXCTRL3 register
#define MDM_RXCTRL3_RESET  0x06180618

/**
 * @brief Returns the current value of the RXCTRL3 register.
 * The RXCTRL3 register will be read and its value returned.
 * @return The current value of the RXCTRL3 register.
 */
__INLINE uint32_t mdm_rxctrl3_get(void)
{
    return REG_PL_RD(MDM_RXCTRL3_ADDR);
}

/**
 * @brief Sets the RXCTRL3 register to a value.
 * The RXCTRL3 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl3_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL3_ADDR, value);
}

// field definitions
/// RXLDPCNDBPSMAXSGI field mask
#define MDM_RXLDPCNDBPSMAXSGI_MASK   ((uint32_t)0xFFFF0000)
/// RXLDPCNDBPSMAXSGI field LSB position
#define MDM_RXLDPCNDBPSMAXSGI_LSB    16
/// RXLDPCNDBPSMAXSGI field width
#define MDM_RXLDPCNDBPSMAXSGI_WIDTH  ((uint32_t)0x00000010)
/// RXLDPCNDBPSMAXLGI field mask
#define MDM_RXLDPCNDBPSMAXLGI_MASK   ((uint32_t)0x0000FFFF)
/// RXLDPCNDBPSMAXLGI field LSB position
#define MDM_RXLDPCNDBPSMAXLGI_LSB    0
/// RXLDPCNDBPSMAXLGI field width
#define MDM_RXLDPCNDBPSMAXLGI_WIDTH  ((uint32_t)0x00000010)

/// RXLDPCNDBPSMAXSGI field reset value
#define MDM_RXLDPCNDBPSMAXSGI_RST    0x618
/// RXLDPCNDBPSMAXLGI field reset value
#define MDM_RXLDPCNDBPSMAXLGI_RST    0x618

/**
 * @brief Constructs a value for the RXCTRL3 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxldpcndbpsmaxsgi - The value to use for the RXLDPCNDBPSMAXSGI field.
 * @param[in] rxldpcndbpsmaxlgi - The value to use for the RXLDPCNDBPSMAXLGI field.
 */
__INLINE void mdm_rxctrl3_pack(uint16_t rxldpcndbpsmaxsgi, uint16_t rxldpcndbpsmaxlgi)
{
    ASSERT_ERR((((uint32_t)rxldpcndbpsmaxsgi << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    ASSERT_ERR((((uint32_t)rxldpcndbpsmaxlgi << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(MDM_RXCTRL3_ADDR,  ((uint32_t)rxldpcndbpsmaxsgi << 16) | ((uint32_t)rxldpcndbpsmaxlgi << 0));
}

/**
 * @brief Unpacks RXCTRL3's fields from current value of the RXCTRL3 register.
 *
 * Reads the RXCTRL3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxldpcndbpsmaxsgi - Will be populated with the current value of this field from the register.
 * @param[out] rxldpcndbpsmaxlgi - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxctrl3_unpack(uint16_t* rxldpcndbpsmaxsgi, uint16_t* rxldpcndbpsmaxlgi)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL3_ADDR);

    *rxldpcndbpsmaxsgi = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *rxldpcndbpsmaxlgi = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

/**
 * @brief Returns the current value of the RXLDPCNDBPSMAXSGI field in the RXCTRL3 register.
 *
 * The RXCTRL3 register will be read and the RXLDPCNDBPSMAXSGI field's value will be returned.
 *
 * @return The current value of the RXLDPCNDBPSMAXSGI field in the RXCTRL3 register.
 */
__INLINE uint16_t mdm_rxldpcndbpsmaxsgi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL3_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

/**
 * @brief Sets the RXLDPCNDBPSMAXSGI field of the RXCTRL3 register.
 *
 * The RXCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxldpcndbpsmaxsgi - The value to set the field to.
 */
__INLINE void mdm_rxldpcndbpsmaxsgi_setf(uint16_t rxldpcndbpsmaxsgi)
{
    ASSERT_ERR((((uint32_t)rxldpcndbpsmaxsgi << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    REG_PL_WR(MDM_RXCTRL3_ADDR, (REG_PL_RD(MDM_RXCTRL3_ADDR) & ~((uint32_t)0xFFFF0000)) | ((uint32_t)rxldpcndbpsmaxsgi << 16));
}

/**
 * @brief Returns the current value of the RXLDPCNDBPSMAXLGI field in the RXCTRL3 register.
 *
 * The RXCTRL3 register will be read and the RXLDPCNDBPSMAXLGI field's value will be returned.
 *
 * @return The current value of the RXLDPCNDBPSMAXLGI field in the RXCTRL3 register.
 */
__INLINE uint16_t mdm_rxldpcndbpsmaxlgi_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

/**
 * @brief Sets the RXLDPCNDBPSMAXLGI field of the RXCTRL3 register.
 *
 * The RXCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxldpcndbpsmaxlgi - The value to set the field to.
 */
__INLINE void mdm_rxldpcndbpsmaxlgi_setf(uint16_t rxldpcndbpsmaxlgi)
{
    ASSERT_ERR((((uint32_t)rxldpcndbpsmaxlgi << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(MDM_RXCTRL3_ADDR, (REG_PL_RD(MDM_RXCTRL3_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)rxldpcndbpsmaxlgi << 0));
}

#endif // RW_NX_LDPC_DEC
/// @}

#if RW_BFMEE_EN
/**
 * @name SVDCTRL register definitions
 * <table>
 * <caption>SVDCTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 23:20 <td>   RXANTSEL <td> R <td> R/W <td> 0x3
 * <tr><td> 09:00 <td> SNRSCALING <td> R <td> R/W <td> 0x56
 * </table>
 *
 * @{
 */

/// Address of the SVDCTRL register
#define MDM_SVDCTRL_ADDR   0x24C008A4
/// Offset of the SVDCTRL register from the base address
#define MDM_SVDCTRL_OFFSET 0x000008A4
/// Index of the SVDCTRL register
#define MDM_SVDCTRL_INDEX  0x00000229
/// Reset value of the SVDCTRL register
#define MDM_SVDCTRL_RESET  0x00300056

/**
 * @brief Returns the current value of the SVDCTRL register.
 * The SVDCTRL register will be read and its value returned.
 * @return The current value of the SVDCTRL register.
 */
__INLINE uint32_t mdm_svdctrl_get(void)
{
    return REG_PL_RD(MDM_SVDCTRL_ADDR);
}

/**
 * @brief Sets the SVDCTRL register to a value.
 * The SVDCTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_svdctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_SVDCTRL_ADDR, value);
}

// field definitions
/// RXANTSEL field mask
#define MDM_RXANTSEL_MASK     ((uint32_t)0x00F00000)
/// RXANTSEL field LSB position
#define MDM_RXANTSEL_LSB      20
/// RXANTSEL field width
#define MDM_RXANTSEL_WIDTH    ((uint32_t)0x00000004)
/// SNRSCALING field mask
#define MDM_SNRSCALING_MASK   ((uint32_t)0x000003FF)
/// SNRSCALING field LSB position
#define MDM_SNRSCALING_LSB    0
/// SNRSCALING field width
#define MDM_SNRSCALING_WIDTH  ((uint32_t)0x0000000A)

/// RXANTSEL field reset value
#define MDM_RXANTSEL_RST      0x3
/// SNRSCALING field reset value
#define MDM_SNRSCALING_RST    0x56

/**
 * @brief Constructs a value for the SVDCTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxantsel - The value to use for the RXANTSEL field.
 * @param[in] snrscaling - The value to use for the SNRSCALING field.
 */
__INLINE void mdm_svdctrl_pack(uint8_t rxantsel, uint16_t snrscaling)
{
    ASSERT_ERR((((uint32_t)rxantsel << 20) & ~((uint32_t)0x00F00000)) == 0);
    ASSERT_ERR((((uint32_t)snrscaling << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_SVDCTRL_ADDR,  ((uint32_t)rxantsel << 20) | ((uint32_t)snrscaling << 0));
}

/**
 * @brief Unpacks SVDCTRL's fields from current value of the SVDCTRL register.
 *
 * Reads the SVDCTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxantsel - Will be populated with the current value of this field from the register.
 * @param[out] snrscaling - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_svdctrl_unpack(uint8_t* rxantsel, uint16_t* snrscaling)
{
    uint32_t localVal = REG_PL_RD(MDM_SVDCTRL_ADDR);

    *rxantsel = (localVal & ((uint32_t)0x00F00000)) >> 20;
    *snrscaling = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the RXANTSEL field in the SVDCTRL register.
 *
 * The SVDCTRL register will be read and the RXANTSEL field's value will be returned.
 *
 * @return The current value of the RXANTSEL field in the SVDCTRL register.
 */
__INLINE uint8_t mdm_rxantsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SVDCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00F00000)) >> 20);
}

/**
 * @brief Sets the RXANTSEL field of the SVDCTRL register.
 *
 * The SVDCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxantsel - The value to set the field to.
 */
__INLINE void mdm_rxantsel_setf(uint8_t rxantsel)
{
    ASSERT_ERR((((uint32_t)rxantsel << 20) & ~((uint32_t)0x00F00000)) == 0);
    REG_PL_WR(MDM_SVDCTRL_ADDR, (REG_PL_RD(MDM_SVDCTRL_ADDR) & ~((uint32_t)0x00F00000)) | ((uint32_t)rxantsel << 20));
}

/**
 * @brief Returns the current value of the SNRSCALING field in the SVDCTRL register.
 *
 * The SVDCTRL register will be read and the SNRSCALING field's value will be returned.
 *
 * @return The current value of the SNRSCALING field in the SVDCTRL register.
 */
__INLINE uint16_t mdm_snrscaling_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_SVDCTRL_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the SNRSCALING field of the SVDCTRL register.
 *
 * The SVDCTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] snrscaling - The value to set the field to.
 */
__INLINE void mdm_snrscaling_setf(uint16_t snrscaling)
{
    ASSERT_ERR((((uint32_t)snrscaling << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_SVDCTRL_ADDR, (REG_PL_RD(MDM_SVDCTRL_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)snrscaling << 0));
}

#endif // RW_BFMEE_EN
/// @}

#if RW_MUMIMO_RX_EN
/**
 * @name MUMIMO_GROUPID_TAB register definitions
 * <table>
 * <caption>MUMIMO_GROUPID_TAB bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> VHTMEMBERSHIPSTAT <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the MUMIMO_GROUPID_TAB register
#define MDM_MUMIMO_GROUPID_TAB_ADDR   0x24C008A8
/// Offset of the MUMIMO_GROUPID_TAB register from the base address
#define MDM_MUMIMO_GROUPID_TAB_OFFSET 0x000008A8
/// Index of the MUMIMO_GROUPID_TAB register
#define MDM_MUMIMO_GROUPID_TAB_INDEX  0x0000022A
/// Reset value of the MUMIMO_GROUPID_TAB register
#define MDM_MUMIMO_GROUPID_TAB_RESET  0x00000000
/// Number of elements of the MUMIMO_GROUPID_TAB register array
#define MDM_MUMIMO_GROUPID_TAB_COUNT  2

/**
 * @brief Returns the current value of the MUMIMO_GROUPID_TAB register.
 * The MUMIMO_GROUPID_TAB register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the MUMIMO_GROUPID_TAB register.
 */
__INLINE uint32_t mdm_mumimo_groupid_tab_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 1);
    return REG_PL_RD(MDM_MUMIMO_GROUPID_TAB_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the MUMIMO_GROUPID_TAB register to a value.
 * The MUMIMO_GROUPID_TAB register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void mdm_mumimo_groupid_tab_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 1);
    REG_PL_WR(MDM_MUMIMO_GROUPID_TAB_ADDR + reg_idx * 4, value);
}

// field definitions
/// VHTMEMBERSHIPSTAT field mask
#define MDM_VHTMEMBERSHIPSTAT_MASK   ((uint32_t)0xFFFFFFFF)
/// VHTMEMBERSHIPSTAT field LSB position
#define MDM_VHTMEMBERSHIPSTAT_LSB    0
/// VHTMEMBERSHIPSTAT field width
#define MDM_VHTMEMBERSHIPSTAT_WIDTH  ((uint32_t)0x00000020)

/// VHTMEMBERSHIPSTAT field reset value
#define MDM_VHTMEMBERSHIPSTAT_RST    0x0

/**
 * @brief Returns the current value of the VHTMEMBERSHIPSTAT field in the MUMIMO_GROUPID_TAB register.
 *
 * The MUMIMO_GROUPID_TAB register will be read and the VHTMEMBERSHIPSTAT field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the VHTMEMBERSHIPSTAT field in the MUMIMO_GROUPID_TAB register.
 */
__INLINE uint32_t mdm_vhtmembershipstat_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 1);
    uint32_t localVal = REG_PL_RD(MDM_MUMIMO_GROUPID_TAB_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the VHTMEMBERSHIPSTAT field of the MUMIMO_GROUPID_TAB register.
 *
 * The MUMIMO_GROUPID_TAB register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] vhtmembershipstat - The value to set the field to.
 */
__INLINE void mdm_vhtmembershipstat_setf(int reg_idx, uint32_t vhtmembershipstat)
{
    ASSERT_ERR(reg_idx <= 1);
    ASSERT_ERR((((uint32_t)vhtmembershipstat << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(MDM_MUMIMO_GROUPID_TAB_ADDR + reg_idx * 4, (uint32_t)vhtmembershipstat << 0);
}

#endif // RW_MUMIMO_RX_EN
/// @}

#if RW_MUMIMO_RX_EN
/**
 * @name MUMIMO_USERPOSITION_TAB register definitions
 * <table>
 * <caption>MUMIMO_USERPOSITION_TAB bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> VHTUSERPOSID <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the MUMIMO_USERPOSITION_TAB register
#define MDM_MUMIMO_USERPOSITION_TAB_ADDR   0x24C008B0
/// Offset of the MUMIMO_USERPOSITION_TAB register from the base address
#define MDM_MUMIMO_USERPOSITION_TAB_OFFSET 0x000008B0
/// Index of the MUMIMO_USERPOSITION_TAB register
#define MDM_MUMIMO_USERPOSITION_TAB_INDEX  0x0000022C
/// Reset value of the MUMIMO_USERPOSITION_TAB register
#define MDM_MUMIMO_USERPOSITION_TAB_RESET  0x00000000
/// Number of elements of the MUMIMO_USERPOSITION_TAB register array
#define MDM_MUMIMO_USERPOSITION_TAB_COUNT  4

/**
 * @brief Returns the current value of the MUMIMO_USERPOSITION_TAB register.
 * The MUMIMO_USERPOSITION_TAB register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the MUMIMO_USERPOSITION_TAB register.
 */
__INLINE uint32_t mdm_mumimo_userposition_tab_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 3);
    return REG_PL_RD(MDM_MUMIMO_USERPOSITION_TAB_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the MUMIMO_USERPOSITION_TAB register to a value.
 * The MUMIMO_USERPOSITION_TAB register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void mdm_mumimo_userposition_tab_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 3);
    REG_PL_WR(MDM_MUMIMO_USERPOSITION_TAB_ADDR + reg_idx * 4, value);
}

// field definitions
/// VHTUSERPOSID field mask
#define MDM_VHTUSERPOSID_MASK   ((uint32_t)0xFFFFFFFF)
/// VHTUSERPOSID field LSB position
#define MDM_VHTUSERPOSID_LSB    0
/// VHTUSERPOSID field width
#define MDM_VHTUSERPOSID_WIDTH  ((uint32_t)0x00000020)

/// VHTUSERPOSID field reset value
#define MDM_VHTUSERPOSID_RST    0x0

/**
 * @brief Returns the current value of the VHTUSERPOSID field in the MUMIMO_USERPOSITION_TAB register.
 *
 * The MUMIMO_USERPOSITION_TAB register will be read and the VHTUSERPOSID field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the VHTUSERPOSID field in the MUMIMO_USERPOSITION_TAB register.
 */
__INLINE uint32_t mdm_vhtuserposid_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 3);
    uint32_t localVal = REG_PL_RD(MDM_MUMIMO_USERPOSITION_TAB_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the VHTUSERPOSID field of the MUMIMO_USERPOSITION_TAB register.
 *
 * The MUMIMO_USERPOSITION_TAB register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] vhtuserposid - The value to set the field to.
 */
__INLINE void mdm_vhtuserposid_setf(int reg_idx, uint32_t vhtuserposid)
{
    ASSERT_ERR(reg_idx <= 3);
    ASSERT_ERR((((uint32_t)vhtuserposid << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(MDM_MUMIMO_USERPOSITION_TAB_ADDR + reg_idx * 4, (uint32_t)vhtuserposid << 0);
}

#endif // RW_MUMIMO_RX_EN
/// @}

/**
 * @name HE_STAID_TAB register definitions
 * <table>
 * <caption>HE_STAID_TAB bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 10:00 <td> HESTAID <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the HE_STAID_TAB register
#define MDM_HE_STAID_TAB_ADDR   0x24C008C0
/// Offset of the HE_STAID_TAB register from the base address
#define MDM_HE_STAID_TAB_OFFSET 0x000008C0
/// Index of the HE_STAID_TAB register
#define MDM_HE_STAID_TAB_INDEX  0x00000230
/// Reset value of the HE_STAID_TAB register
#define MDM_HE_STAID_TAB_RESET  0x00000000
/// Number of elements of the HE_STAID_TAB register array
#define MDM_HE_STAID_TAB_COUNT  4

/**
 * @brief Returns the current value of the HE_STAID_TAB register.
 * The HE_STAID_TAB register will be read and its value returned.
 * @param[in] reg_idx Index of the register
 * @return The current value of the HE_STAID_TAB register.
 */
__INLINE uint32_t mdm_he_staid_tab_get(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 3);
    return REG_PL_RD(MDM_HE_STAID_TAB_ADDR + reg_idx * 4);
}

/**
 * @brief Sets the HE_STAID_TAB register to a value.
 * The HE_STAID_TAB register will be written.
 * @param[in] reg_idx Index of the register
 * @param value - The value to write.
 */
__INLINE void mdm_he_staid_tab_set(int reg_idx, uint32_t value)
{
    ASSERT_ERR(reg_idx <= 3);
    REG_PL_WR(MDM_HE_STAID_TAB_ADDR + reg_idx * 4, value);
}

// field definitions
/// HESTAID field mask
#define MDM_HESTAID_MASK   ((uint32_t)0x000007FF)
/// HESTAID field LSB position
#define MDM_HESTAID_LSB    0
/// HESTAID field width
#define MDM_HESTAID_WIDTH  ((uint32_t)0x0000000B)

/// HESTAID field reset value
#define MDM_HESTAID_RST    0x0

/**
 * @brief Returns the current value of the HESTAID field in the HE_STAID_TAB register.
 *
 * The HE_STAID_TAB register will be read and the HESTAID field's value will be returned.
 *
 * @param[in] reg_idx Index of the register
 * @return The current value of the HESTAID field in the HE_STAID_TAB register.
 */
__INLINE uint16_t mdm_hestaid_getf(int reg_idx)
{
    ASSERT_ERR(reg_idx <= 3);
    uint32_t localVal = REG_PL_RD(MDM_HE_STAID_TAB_ADDR + reg_idx * 4);
    ASSERT_ERR((localVal & ~((uint32_t)0x000007FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the HESTAID field of the HE_STAID_TAB register.
 *
 * The HE_STAID_TAB register will be read, modified to contain the new field value, and written.
 *
 * @param[in] reg_idx Index of the register
 * @param[in] hestaid - The value to set the field to.
 */
__INLINE void mdm_hestaid_setf(int reg_idx, uint16_t hestaid)
{
    ASSERT_ERR(reg_idx <= 3);
    ASSERT_ERR((((uint32_t)hestaid << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(MDM_HE_STAID_TAB_ADDR + reg_idx * 4, (uint32_t)hestaid << 0);
}

/// @}

/**
 * @name RXCTRL4 register definitions
 * <table>
 * <caption>RXCTRL4 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:00 <td> RXHEFRAMEVIOLATIONMASK <td> R <td> R/W <td> 0xFFFFFFFF
 * </table>
 *
 * @{
 */

/// Address of the RXCTRL4 register
#define MDM_RXCTRL4_ADDR   0x24C008D0
/// Offset of the RXCTRL4 register from the base address
#define MDM_RXCTRL4_OFFSET 0x000008D0
/// Index of the RXCTRL4 register
#define MDM_RXCTRL4_INDEX  0x00000234
/// Reset value of the RXCTRL4 register
#define MDM_RXCTRL4_RESET  0xFFFFFFFF

/**
 * @brief Returns the current value of the RXCTRL4 register.
 * The RXCTRL4 register will be read and its value returned.
 * @return The current value of the RXCTRL4 register.
 */
__INLINE uint32_t mdm_rxctrl4_get(void)
{
    return REG_PL_RD(MDM_RXCTRL4_ADDR);
}

/**
 * @brief Sets the RXCTRL4 register to a value.
 * The RXCTRL4 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl4_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL4_ADDR, value);
}

// field definitions
/// RXHEFRAMEVIOLATIONMASK field mask
#define MDM_RXHEFRAMEVIOLATIONMASK_MASK   ((uint32_t)0xFFFFFFFF)
/// RXHEFRAMEVIOLATIONMASK field LSB position
#define MDM_RXHEFRAMEVIOLATIONMASK_LSB    0
/// RXHEFRAMEVIOLATIONMASK field width
#define MDM_RXHEFRAMEVIOLATIONMASK_WIDTH  ((uint32_t)0x00000020)

/// RXHEFRAMEVIOLATIONMASK field reset value
#define MDM_RXHEFRAMEVIOLATIONMASK_RST    0xFFFFFFFF

/**
 * @brief Returns the current value of the RXHEFRAMEVIOLATIONMASK field in the RXCTRL4 register.
 *
 * The RXCTRL4 register will be read and the RXHEFRAMEVIOLATIONMASK field's value will be returned.
 *
 * @return The current value of the RXHEFRAMEVIOLATIONMASK field in the RXCTRL4 register.
 */
__INLINE uint32_t mdm_rxheframeviolationmask_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL4_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the RXHEFRAMEVIOLATIONMASK field of the RXCTRL4 register.
 *
 * The RXCTRL4 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxheframeviolationmask - The value to set the field to.
 */
__INLINE void mdm_rxheframeviolationmask_setf(uint32_t rxheframeviolationmask)
{
    ASSERT_ERR((((uint32_t)rxheframeviolationmask << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_PL_WR(MDM_RXCTRL4_ADDR, (uint32_t)rxheframeviolationmask << 0);
}

/// @}

/**
 * @name RXCTRL5 register definitions
 * <table>
 * <caption>RXCTRL5 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> RXFEOFDM20DELAY <td> R <td> R/W <td> 0x2C
 * <tr><td> 12:00 <td>     RXSIFSDELAY <td> R <td> R/W <td> 0x780
 * </table>
 *
 * @{
 */

/// Address of the RXCTRL5 register
#define MDM_RXCTRL5_ADDR   0x24C008D4
/// Offset of the RXCTRL5 register from the base address
#define MDM_RXCTRL5_OFFSET 0x000008D4
/// Index of the RXCTRL5 register
#define MDM_RXCTRL5_INDEX  0x00000235
/// Reset value of the RXCTRL5 register
#define MDM_RXCTRL5_RESET  0x002C0780

/**
 * @brief Returns the current value of the RXCTRL5 register.
 * The RXCTRL5 register will be read and its value returned.
 * @return The current value of the RXCTRL5 register.
 */
__INLINE uint32_t mdm_rxctrl5_get(void)
{
    return REG_PL_RD(MDM_RXCTRL5_ADDR);
}

/**
 * @brief Sets the RXCTRL5 register to a value.
 * The RXCTRL5 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl5_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL5_ADDR, value);
}

// field definitions
/// RXFEOFDM20DELAY field mask
#define MDM_RXFEOFDM20DELAY_MASK   ((uint32_t)0x03FF0000)
/// RXFEOFDM20DELAY field LSB position
#define MDM_RXFEOFDM20DELAY_LSB    16
/// RXFEOFDM20DELAY field width
#define MDM_RXFEOFDM20DELAY_WIDTH  ((uint32_t)0x0000000A)
/// RXSIFSDELAY field mask
#define MDM_RXSIFSDELAY_MASK       ((uint32_t)0x00001FFF)
/// RXSIFSDELAY field LSB position
#define MDM_RXSIFSDELAY_LSB        0
/// RXSIFSDELAY field width
#define MDM_RXSIFSDELAY_WIDTH      ((uint32_t)0x0000000D)

/// RXFEOFDM20DELAY field reset value
#define MDM_RXFEOFDM20DELAY_RST    0x2C
/// RXSIFSDELAY field reset value
#define MDM_RXSIFSDELAY_RST        0x780

/**
 * @brief Constructs a value for the RXCTRL5 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxfeofdm20delay - The value to use for the RXFEOFDM20DELAY field.
 * @param[in] rxsifsdelay - The value to use for the RXSIFSDELAY field.
 */
__INLINE void mdm_rxctrl5_pack(uint16_t rxfeofdm20delay, uint16_t rxsifsdelay)
{
    ASSERT_ERR((((uint32_t)rxfeofdm20delay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)rxsifsdelay << 0) & ~((uint32_t)0x00001FFF)) == 0);
    REG_PL_WR(MDM_RXCTRL5_ADDR,  ((uint32_t)rxfeofdm20delay << 16) | ((uint32_t)rxsifsdelay << 0));
}

/**
 * @brief Unpacks RXCTRL5's fields from current value of the RXCTRL5 register.
 *
 * Reads the RXCTRL5 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxfeofdm20delay - Will be populated with the current value of this field from the register.
 * @param[out] rxsifsdelay - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxctrl5_unpack(uint16_t* rxfeofdm20delay, uint16_t* rxsifsdelay)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL5_ADDR);

    *rxfeofdm20delay = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *rxsifsdelay = (localVal & ((uint32_t)0x00001FFF)) >> 0;
}

/**
 * @brief Returns the current value of the RXFEOFDM20DELAY field in the RXCTRL5 register.
 *
 * The RXCTRL5 register will be read and the RXFEOFDM20DELAY field's value will be returned.
 *
 * @return The current value of the RXFEOFDM20DELAY field in the RXCTRL5 register.
 */
__INLINE uint16_t mdm_rxfeofdm20delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL5_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the RXFEOFDM20DELAY field of the RXCTRL5 register.
 *
 * The RXCTRL5 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxfeofdm20delay - The value to set the field to.
 */
__INLINE void mdm_rxfeofdm20delay_setf(uint16_t rxfeofdm20delay)
{
    ASSERT_ERR((((uint32_t)rxfeofdm20delay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(MDM_RXCTRL5_ADDR, (REG_PL_RD(MDM_RXCTRL5_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)rxfeofdm20delay << 16));
}

/**
 * @brief Returns the current value of the RXSIFSDELAY field in the RXCTRL5 register.
 *
 * The RXCTRL5 register will be read and the RXSIFSDELAY field's value will be returned.
 *
 * @return The current value of the RXSIFSDELAY field in the RXCTRL5 register.
 */
__INLINE uint16_t mdm_rxsifsdelay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL5_ADDR);
    return ((localVal & ((uint32_t)0x00001FFF)) >> 0);
}

/**
 * @brief Sets the RXSIFSDELAY field of the RXCTRL5 register.
 *
 * The RXCTRL5 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxsifsdelay - The value to set the field to.
 */
__INLINE void mdm_rxsifsdelay_setf(uint16_t rxsifsdelay)
{
    ASSERT_ERR((((uint32_t)rxsifsdelay << 0) & ~((uint32_t)0x00001FFF)) == 0);
    REG_PL_WR(MDM_RXCTRL5_ADDR, (REG_PL_RD(MDM_RXCTRL5_ADDR) & ~((uint32_t)0x00001FFF)) | ((uint32_t)rxsifsdelay << 0));
}

/// @}

/**
 * @name RXCTRL6 register definitions
 * <table>
 * <caption>RXCTRL6 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> RXFEOFDM4020DELAY <td> R <td> R/W <td> 0x4B
 * <tr><td> 09:00 <td>   RXFEOFDM40DELAY <td> R <td> R/W <td> 0x1F
 * </table>
 *
 * @{
 */

/// Address of the RXCTRL6 register
#define MDM_RXCTRL6_ADDR   0x24C008D8
/// Offset of the RXCTRL6 register from the base address
#define MDM_RXCTRL6_OFFSET 0x000008D8
/// Index of the RXCTRL6 register
#define MDM_RXCTRL6_INDEX  0x00000236
/// Reset value of the RXCTRL6 register
#define MDM_RXCTRL6_RESET  0x004B001F

/**
 * @brief Returns the current value of the RXCTRL6 register.
 * The RXCTRL6 register will be read and its value returned.
 * @return The current value of the RXCTRL6 register.
 */
__INLINE uint32_t mdm_rxctrl6_get(void)
{
    return REG_PL_RD(MDM_RXCTRL6_ADDR);
}

/**
 * @brief Sets the RXCTRL6 register to a value.
 * The RXCTRL6 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl6_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL6_ADDR, value);
}

// field definitions
/// RXFEOFDM4020DELAY field mask
#define MDM_RXFEOFDM4020DELAY_MASK   ((uint32_t)0x03FF0000)
/// RXFEOFDM4020DELAY field LSB position
#define MDM_RXFEOFDM4020DELAY_LSB    16
/// RXFEOFDM4020DELAY field width
#define MDM_RXFEOFDM4020DELAY_WIDTH  ((uint32_t)0x0000000A)
/// RXFEOFDM40DELAY field mask
#define MDM_RXFEOFDM40DELAY_MASK     ((uint32_t)0x000003FF)
/// RXFEOFDM40DELAY field LSB position
#define MDM_RXFEOFDM40DELAY_LSB      0
/// RXFEOFDM40DELAY field width
#define MDM_RXFEOFDM40DELAY_WIDTH    ((uint32_t)0x0000000A)

/// RXFEOFDM4020DELAY field reset value
#define MDM_RXFEOFDM4020DELAY_RST    0x4B
/// RXFEOFDM40DELAY field reset value
#define MDM_RXFEOFDM40DELAY_RST      0x1F

/**
 * @brief Constructs a value for the RXCTRL6 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxfeofdm4020delay - The value to use for the RXFEOFDM4020DELAY field.
 * @param[in] rxfeofdm40delay - The value to use for the RXFEOFDM40DELAY field.
 */
__INLINE void mdm_rxctrl6_pack(uint16_t rxfeofdm4020delay, uint16_t rxfeofdm40delay)
{
    ASSERT_ERR((((uint32_t)rxfeofdm4020delay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)rxfeofdm40delay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_RXCTRL6_ADDR,  ((uint32_t)rxfeofdm4020delay << 16) | ((uint32_t)rxfeofdm40delay << 0));
}

/**
 * @brief Unpacks RXCTRL6's fields from current value of the RXCTRL6 register.
 *
 * Reads the RXCTRL6 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxfeofdm4020delay - Will be populated with the current value of this field from the register.
 * @param[out] rxfeofdm40delay - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxctrl6_unpack(uint16_t* rxfeofdm4020delay, uint16_t* rxfeofdm40delay)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL6_ADDR);

    *rxfeofdm4020delay = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *rxfeofdm40delay = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the RXFEOFDM4020DELAY field in the RXCTRL6 register.
 *
 * The RXCTRL6 register will be read and the RXFEOFDM4020DELAY field's value will be returned.
 *
 * @return The current value of the RXFEOFDM4020DELAY field in the RXCTRL6 register.
 */
__INLINE uint16_t mdm_rxfeofdm4020delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL6_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the RXFEOFDM4020DELAY field of the RXCTRL6 register.
 *
 * The RXCTRL6 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxfeofdm4020delay - The value to set the field to.
 */
__INLINE void mdm_rxfeofdm4020delay_setf(uint16_t rxfeofdm4020delay)
{
    ASSERT_ERR((((uint32_t)rxfeofdm4020delay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(MDM_RXCTRL6_ADDR, (REG_PL_RD(MDM_RXCTRL6_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)rxfeofdm4020delay << 16));
}

/**
 * @brief Returns the current value of the RXFEOFDM40DELAY field in the RXCTRL6 register.
 *
 * The RXCTRL6 register will be read and the RXFEOFDM40DELAY field's value will be returned.
 *
 * @return The current value of the RXFEOFDM40DELAY field in the RXCTRL6 register.
 */
__INLINE uint16_t mdm_rxfeofdm40delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL6_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the RXFEOFDM40DELAY field of the RXCTRL6 register.
 *
 * The RXCTRL6 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxfeofdm40delay - The value to set the field to.
 */
__INLINE void mdm_rxfeofdm40delay_setf(uint16_t rxfeofdm40delay)
{
    ASSERT_ERR((((uint32_t)rxfeofdm40delay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_RXCTRL6_ADDR, (REG_PL_RD(MDM_RXCTRL6_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)rxfeofdm40delay << 0));
}

/// @}

/**
 * @name RXCTRL7 register definitions
 * <table>
 * <caption>RXCTRL7 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 07:04 <td> RXVHTMUFRAMEVIOLATIONMASK <td> R <td> R/W <td> 0x7
 * </table>
 *
 * @{
 */

/// Address of the RXCTRL7 register
#define MDM_RXCTRL7_ADDR   0x24C008DC
/// Offset of the RXCTRL7 register from the base address
#define MDM_RXCTRL7_OFFSET 0x000008DC
/// Index of the RXCTRL7 register
#define MDM_RXCTRL7_INDEX  0x00000237
/// Reset value of the RXCTRL7 register
#define MDM_RXCTRL7_RESET  0x00000070

/**
 * @brief Returns the current value of the RXCTRL7 register.
 * The RXCTRL7 register will be read and its value returned.
 * @return The current value of the RXCTRL7 register.
 */
__INLINE uint32_t mdm_rxctrl7_get(void)
{
    return REG_PL_RD(MDM_RXCTRL7_ADDR);
}

/**
 * @brief Sets the RXCTRL7 register to a value.
 * The RXCTRL7 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl7_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL7_ADDR, value);
}

// field definitions
/// RXVHTMUFRAMEVIOLATIONMASK field mask
#define MDM_RXVHTMUFRAMEVIOLATIONMASK_MASK   ((uint32_t)0x000000F0)
/// RXVHTMUFRAMEVIOLATIONMASK field LSB position
#define MDM_RXVHTMUFRAMEVIOLATIONMASK_LSB    4
/// RXVHTMUFRAMEVIOLATIONMASK field width
#define MDM_RXVHTMUFRAMEVIOLATIONMASK_WIDTH  ((uint32_t)0x00000004)

/// RXVHTMUFRAMEVIOLATIONMASK field reset value
#define MDM_RXVHTMUFRAMEVIOLATIONMASK_RST    0x7

/**
 * @brief Returns the current value of the RXVHTMUFRAMEVIOLATIONMASK field in the RXCTRL7 register.
 *
 * The RXCTRL7 register will be read and the RXVHTMUFRAMEVIOLATIONMASK field's value will be returned.
 *
 * @return The current value of the RXVHTMUFRAMEVIOLATIONMASK field in the RXCTRL7 register.
 */
__INLINE uint8_t mdm_rxvhtmuframeviolationmask_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL7_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000000F0)) == 0);
    return (localVal >> 4);
}

/**
 * @brief Sets the RXVHTMUFRAMEVIOLATIONMASK field of the RXCTRL7 register.
 *
 * The RXCTRL7 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxvhtmuframeviolationmask - The value to set the field to.
 */
__INLINE void mdm_rxvhtmuframeviolationmask_setf(uint8_t rxvhtmuframeviolationmask)
{
    ASSERT_ERR((((uint32_t)rxvhtmuframeviolationmask << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_RXCTRL7_ADDR, (uint32_t)rxvhtmuframeviolationmask << 4);
}

/// @}

/**
 * @name RXCTRL8 register definitions
 * <table>
 * <caption>RXCTRL8 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> RXFEOFDM8020DELAY <td> R <td> R/W <td> 0x69
 * <tr><td> 09:00 <td>   RXFEOFDM80DELAY <td> R <td> R/W <td> 0x1E
 * </table>
 *
 * @{
 */

/// Address of the RXCTRL8 register
#define MDM_RXCTRL8_ADDR   0x24C008E0
/// Offset of the RXCTRL8 register from the base address
#define MDM_RXCTRL8_OFFSET 0x000008E0
/// Index of the RXCTRL8 register
#define MDM_RXCTRL8_INDEX  0x00000238
/// Reset value of the RXCTRL8 register
#define MDM_RXCTRL8_RESET  0x0069001E

/**
 * @brief Returns the current value of the RXCTRL8 register.
 * The RXCTRL8 register will be read and its value returned.
 * @return The current value of the RXCTRL8 register.
 */
__INLINE uint32_t mdm_rxctrl8_get(void)
{
    return REG_PL_RD(MDM_RXCTRL8_ADDR);
}

/**
 * @brief Sets the RXCTRL8 register to a value.
 * The RXCTRL8 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl8_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL8_ADDR, value);
}

// field definitions
/// RXFEOFDM8020DELAY field mask
#define MDM_RXFEOFDM8020DELAY_MASK   ((uint32_t)0x03FF0000)
/// RXFEOFDM8020DELAY field LSB position
#define MDM_RXFEOFDM8020DELAY_LSB    16
/// RXFEOFDM8020DELAY field width
#define MDM_RXFEOFDM8020DELAY_WIDTH  ((uint32_t)0x0000000A)
/// RXFEOFDM80DELAY field mask
#define MDM_RXFEOFDM80DELAY_MASK     ((uint32_t)0x000003FF)
/// RXFEOFDM80DELAY field LSB position
#define MDM_RXFEOFDM80DELAY_LSB      0
/// RXFEOFDM80DELAY field width
#define MDM_RXFEOFDM80DELAY_WIDTH    ((uint32_t)0x0000000A)

/// RXFEOFDM8020DELAY field reset value
#define MDM_RXFEOFDM8020DELAY_RST    0x69
/// RXFEOFDM80DELAY field reset value
#define MDM_RXFEOFDM80DELAY_RST      0x1E

/**
 * @brief Constructs a value for the RXCTRL8 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxfeofdm8020delay - The value to use for the RXFEOFDM8020DELAY field.
 * @param[in] rxfeofdm80delay - The value to use for the RXFEOFDM80DELAY field.
 */
__INLINE void mdm_rxctrl8_pack(uint16_t rxfeofdm8020delay, uint16_t rxfeofdm80delay)
{
    ASSERT_ERR((((uint32_t)rxfeofdm8020delay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)rxfeofdm80delay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_RXCTRL8_ADDR,  ((uint32_t)rxfeofdm8020delay << 16) | ((uint32_t)rxfeofdm80delay << 0));
}

/**
 * @brief Unpacks RXCTRL8's fields from current value of the RXCTRL8 register.
 *
 * Reads the RXCTRL8 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxfeofdm8020delay - Will be populated with the current value of this field from the register.
 * @param[out] rxfeofdm80delay - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxctrl8_unpack(uint16_t* rxfeofdm8020delay, uint16_t* rxfeofdm80delay)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL8_ADDR);

    *rxfeofdm8020delay = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *rxfeofdm80delay = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

/**
 * @brief Returns the current value of the RXFEOFDM8020DELAY field in the RXCTRL8 register.
 *
 * The RXCTRL8 register will be read and the RXFEOFDM8020DELAY field's value will be returned.
 *
 * @return The current value of the RXFEOFDM8020DELAY field in the RXCTRL8 register.
 */
__INLINE uint16_t mdm_rxfeofdm8020delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL8_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

/**
 * @brief Sets the RXFEOFDM8020DELAY field of the RXCTRL8 register.
 *
 * The RXCTRL8 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxfeofdm8020delay - The value to set the field to.
 */
__INLINE void mdm_rxfeofdm8020delay_setf(uint16_t rxfeofdm8020delay)
{
    ASSERT_ERR((((uint32_t)rxfeofdm8020delay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(MDM_RXCTRL8_ADDR, (REG_PL_RD(MDM_RXCTRL8_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)rxfeofdm8020delay << 16));
}

/**
 * @brief Returns the current value of the RXFEOFDM80DELAY field in the RXCTRL8 register.
 *
 * The RXCTRL8 register will be read and the RXFEOFDM80DELAY field's value will be returned.
 *
 * @return The current value of the RXFEOFDM80DELAY field in the RXCTRL8 register.
 */
__INLINE uint16_t mdm_rxfeofdm80delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL8_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

/**
 * @brief Sets the RXFEOFDM80DELAY field of the RXCTRL8 register.
 *
 * The RXCTRL8 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxfeofdm80delay - The value to set the field to.
 */
__INLINE void mdm_rxfeofdm80delay_setf(uint16_t rxfeofdm80delay)
{
    ASSERT_ERR((((uint32_t)rxfeofdm80delay << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_PL_WR(MDM_RXCTRL8_ADDR, (REG_PL_RD(MDM_RXCTRL8_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)rxfeofdm80delay << 0));
}

/// @}

/**
 * @name RXCTRL9 register definitions
 * <table>
 * <caption>RXCTRL9 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 25:16 <td> RXFEOFDM8040DELAY <td> R <td> R/W <td> 0x3D
 * </table>
 *
 * @{
 */

/// Address of the RXCTRL9 register
#define MDM_RXCTRL9_ADDR   0x24C008E4
/// Offset of the RXCTRL9 register from the base address
#define MDM_RXCTRL9_OFFSET 0x000008E4
/// Index of the RXCTRL9 register
#define MDM_RXCTRL9_INDEX  0x00000239
/// Reset value of the RXCTRL9 register
#define MDM_RXCTRL9_RESET  0x003D0000

/**
 * @brief Returns the current value of the RXCTRL9 register.
 * The RXCTRL9 register will be read and its value returned.
 * @return The current value of the RXCTRL9 register.
 */
__INLINE uint32_t mdm_rxctrl9_get(void)
{
    return REG_PL_RD(MDM_RXCTRL9_ADDR);
}

/**
 * @brief Sets the RXCTRL9 register to a value.
 * The RXCTRL9 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl9_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL9_ADDR, value);
}

// field definitions
/// RXFEOFDM8040DELAY field mask
#define MDM_RXFEOFDM8040DELAY_MASK   ((uint32_t)0x03FF0000)
/// RXFEOFDM8040DELAY field LSB position
#define MDM_RXFEOFDM8040DELAY_LSB    16
/// RXFEOFDM8040DELAY field width
#define MDM_RXFEOFDM8040DELAY_WIDTH  ((uint32_t)0x0000000A)

/// RXFEOFDM8040DELAY field reset value
#define MDM_RXFEOFDM8040DELAY_RST    0x3D

/**
 * @brief Returns the current value of the RXFEOFDM8040DELAY field in the RXCTRL9 register.
 *
 * The RXCTRL9 register will be read and the RXFEOFDM8040DELAY field's value will be returned.
 *
 * @return The current value of the RXFEOFDM8040DELAY field in the RXCTRL9 register.
 */
__INLINE uint16_t mdm_rxfeofdm8040delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL9_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x03FF0000)) == 0);
    return (localVal >> 16);
}

/**
 * @brief Sets the RXFEOFDM8040DELAY field of the RXCTRL9 register.
 *
 * The RXCTRL9 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxfeofdm8040delay - The value to set the field to.
 */
__INLINE void mdm_rxfeofdm8040delay_setf(uint16_t rxfeofdm8040delay)
{
    ASSERT_ERR((((uint32_t)rxfeofdm8040delay << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_PL_WR(MDM_RXCTRL9_ADDR, (uint32_t)rxfeofdm8040delay << 16);
}

/// @}

/**
 * @name RXCTRL10 register definitions
 * <table>
 * <caption>RXCTRL10 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 10:00 <td> RXLATENCYMAX <td> R <td> R/W <td> 0x5D2
 * </table>
 *
 * @{
 */

/// Address of the RXCTRL10 register
#define MDM_RXCTRL10_ADDR   0x24C008E8
/// Offset of the RXCTRL10 register from the base address
#define MDM_RXCTRL10_OFFSET 0x000008E8
/// Index of the RXCTRL10 register
#define MDM_RXCTRL10_INDEX  0x0000023A
/// Reset value of the RXCTRL10 register
#define MDM_RXCTRL10_RESET  0x000005D2

/**
 * @brief Returns the current value of the RXCTRL10 register.
 * The RXCTRL10 register will be read and its value returned.
 * @return The current value of the RXCTRL10 register.
 */
__INLINE uint32_t mdm_rxctrl10_get(void)
{
    return REG_PL_RD(MDM_RXCTRL10_ADDR);
}

/**
 * @brief Sets the RXCTRL10 register to a value.
 * The RXCTRL10 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl10_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL10_ADDR, value);
}

// field definitions
/// RXLATENCYMAX field mask
#define MDM_RXLATENCYMAX_MASK   ((uint32_t)0x000007FF)
/// RXLATENCYMAX field LSB position
#define MDM_RXLATENCYMAX_LSB    0
/// RXLATENCYMAX field width
#define MDM_RXLATENCYMAX_WIDTH  ((uint32_t)0x0000000B)

/// RXLATENCYMAX field reset value
#define MDM_RXLATENCYMAX_RST    0x5D2

/**
 * @brief Returns the current value of the RXLATENCYMAX field in the RXCTRL10 register.
 *
 * The RXCTRL10 register will be read and the RXLATENCYMAX field's value will be returned.
 *
 * @return The current value of the RXLATENCYMAX field in the RXCTRL10 register.
 */
__INLINE uint16_t mdm_rxlatencymax_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL10_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000007FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the RXLATENCYMAX field of the RXCTRL10 register.
 *
 * The RXCTRL10 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxlatencymax - The value to set the field to.
 */
__INLINE void mdm_rxlatencymax_setf(uint16_t rxlatencymax)
{
    ASSERT_ERR((((uint32_t)rxlatencymax << 0) & ~((uint32_t)0x000007FF)) == 0);
    REG_PL_WR(MDM_RXCTRL10_ADDR, (uint32_t)rxlatencymax << 0);
}

/// @}

/**
 * @name RXCFOBIASEST register definitions
 * <table>
 * <caption>RXCFOBIASEST bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 15:00 <td> OFFSETBIAS <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the RXCFOBIASEST register
#define MDM_RXCFOBIASEST_ADDR   0x24C008EC
/// Offset of the RXCFOBIASEST register from the base address
#define MDM_RXCFOBIASEST_OFFSET 0x000008EC
/// Index of the RXCFOBIASEST register
#define MDM_RXCFOBIASEST_INDEX  0x0000023B
/// Reset value of the RXCFOBIASEST register
#define MDM_RXCFOBIASEST_RESET  0x00000000

/**
 * @brief Returns the current value of the RXCFOBIASEST register.
 * The RXCFOBIASEST register will be read and its value returned.
 * @return The current value of the RXCFOBIASEST register.
 */
__INLINE uint32_t mdm_rxcfobiasest_get(void)
{
    return REG_PL_RD(MDM_RXCFOBIASEST_ADDR);
}

/**
 * @brief Sets the RXCFOBIASEST register to a value.
 * The RXCFOBIASEST register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxcfobiasest_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCFOBIASEST_ADDR, value);
}

// field definitions
/// OFFSETBIAS field mask
#define MDM_OFFSETBIAS_MASK   ((uint32_t)0x0000FFFF)
/// OFFSETBIAS field LSB position
#define MDM_OFFSETBIAS_LSB    0
/// OFFSETBIAS field width
#define MDM_OFFSETBIAS_WIDTH  ((uint32_t)0x00000010)

/// OFFSETBIAS field reset value
#define MDM_OFFSETBIAS_RST    0x0

/**
 * @brief Returns the current value of the OFFSETBIAS field in the RXCFOBIASEST register.
 *
 * The RXCFOBIASEST register will be read and the OFFSETBIAS field's value will be returned.
 *
 * @return The current value of the OFFSETBIAS field in the RXCFOBIASEST register.
 */
__INLINE uint16_t mdm_offsetbias_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCFOBIASEST_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief Sets the OFFSETBIAS field of the RXCFOBIASEST register.
 *
 * The RXCFOBIASEST register will be read, modified to contain the new field value, and written.
 *
 * @param[in] offsetbias - The value to set the field to.
 */
__INLINE void mdm_offsetbias_setf(uint16_t offsetbias)
{
    ASSERT_ERR((((uint32_t)offsetbias << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_PL_WR(MDM_RXCFOBIASEST_ADDR, (uint32_t)offsetbias << 0);
}

/// @}

/**
 * @name RXCTRL11 register definitions
 * <table>
 * <caption>RXCTRL11 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 10 <td> RXSKIPMIDAMBLE4X80M <td> R <td> R/W <td> 0
 * <tr><td> 09 <td> RXSKIPMIDAMBLE4X40M <td> R <td> R/W <td> 0
 * <tr><td> 08 <td> RXSKIPMIDAMBLE4X20M <td> R <td> R/W <td> 0
 * <tr><td> 06 <td> RXSKIPMIDAMBLE2X80M <td> R <td> R/W <td> 0
 * <tr><td> 05 <td> RXSKIPMIDAMBLE2X40M <td> R <td> R/W <td> 0
 * <tr><td> 04 <td> RXSKIPMIDAMBLE2X20M <td> R <td> R/W <td> 0
 * <tr><td> 02 <td> RXSKIPMIDAMBLE1X80M <td> R <td> R/W <td> 1
 * <tr><td> 01 <td> RXSKIPMIDAMBLE1X40M <td> R <td> R/W <td> 0
 * <tr><td> 00 <td> RXSKIPMIDAMBLE1X20M <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the RXCTRL11 register
#define MDM_RXCTRL11_ADDR   0x24C008F0
/// Offset of the RXCTRL11 register from the base address
#define MDM_RXCTRL11_OFFSET 0x000008F0
/// Index of the RXCTRL11 register
#define MDM_RXCTRL11_INDEX  0x0000023C
/// Reset value of the RXCTRL11 register
#define MDM_RXCTRL11_RESET  0x00000004

/**
 * @brief Returns the current value of the RXCTRL11 register.
 * The RXCTRL11 register will be read and its value returned.
 * @return The current value of the RXCTRL11 register.
 */
__INLINE uint32_t mdm_rxctrl11_get(void)
{
    return REG_PL_RD(MDM_RXCTRL11_ADDR);
}

/**
 * @brief Sets the RXCTRL11 register to a value.
 * The RXCTRL11 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_rxctrl11_set(uint32_t value)
{
    REG_PL_WR(MDM_RXCTRL11_ADDR, value);
}

// field definitions
/// RXSKIPMIDAMBLE4X80M field bit
#define MDM_RXSKIPMIDAMBLE4X80M_BIT    ((uint32_t)0x00000400)
/// RXSKIPMIDAMBLE4X80M field position
#define MDM_RXSKIPMIDAMBLE4X80M_POS    10
/// RXSKIPMIDAMBLE4X40M field bit
#define MDM_RXSKIPMIDAMBLE4X40M_BIT    ((uint32_t)0x00000200)
/// RXSKIPMIDAMBLE4X40M field position
#define MDM_RXSKIPMIDAMBLE4X40M_POS    9
/// RXSKIPMIDAMBLE4X20M field bit
#define MDM_RXSKIPMIDAMBLE4X20M_BIT    ((uint32_t)0x00000100)
/// RXSKIPMIDAMBLE4X20M field position
#define MDM_RXSKIPMIDAMBLE4X20M_POS    8
/// RXSKIPMIDAMBLE2X80M field bit
#define MDM_RXSKIPMIDAMBLE2X80M_BIT    ((uint32_t)0x00000040)
/// RXSKIPMIDAMBLE2X80M field position
#define MDM_RXSKIPMIDAMBLE2X80M_POS    6
/// RXSKIPMIDAMBLE2X40M field bit
#define MDM_RXSKIPMIDAMBLE2X40M_BIT    ((uint32_t)0x00000020)
/// RXSKIPMIDAMBLE2X40M field position
#define MDM_RXSKIPMIDAMBLE2X40M_POS    5
/// RXSKIPMIDAMBLE2X20M field bit
#define MDM_RXSKIPMIDAMBLE2X20M_BIT    ((uint32_t)0x00000010)
/// RXSKIPMIDAMBLE2X20M field position
#define MDM_RXSKIPMIDAMBLE2X20M_POS    4
/// RXSKIPMIDAMBLE1X80M field bit
#define MDM_RXSKIPMIDAMBLE1X80M_BIT    ((uint32_t)0x00000004)
/// RXSKIPMIDAMBLE1X80M field position
#define MDM_RXSKIPMIDAMBLE1X80M_POS    2
/// RXSKIPMIDAMBLE1X40M field bit
#define MDM_RXSKIPMIDAMBLE1X40M_BIT    ((uint32_t)0x00000002)
/// RXSKIPMIDAMBLE1X40M field position
#define MDM_RXSKIPMIDAMBLE1X40M_POS    1
/// RXSKIPMIDAMBLE1X20M field bit
#define MDM_RXSKIPMIDAMBLE1X20M_BIT    ((uint32_t)0x00000001)
/// RXSKIPMIDAMBLE1X20M field position
#define MDM_RXSKIPMIDAMBLE1X20M_POS    0

/// RXSKIPMIDAMBLE4X80M field reset value
#define MDM_RXSKIPMIDAMBLE4X80M_RST    0x0
/// RXSKIPMIDAMBLE4X40M field reset value
#define MDM_RXSKIPMIDAMBLE4X40M_RST    0x0
/// RXSKIPMIDAMBLE4X20M field reset value
#define MDM_RXSKIPMIDAMBLE4X20M_RST    0x0
/// RXSKIPMIDAMBLE2X80M field reset value
#define MDM_RXSKIPMIDAMBLE2X80M_RST    0x0
/// RXSKIPMIDAMBLE2X40M field reset value
#define MDM_RXSKIPMIDAMBLE2X40M_RST    0x0
/// RXSKIPMIDAMBLE2X20M field reset value
#define MDM_RXSKIPMIDAMBLE2X20M_RST    0x0
/// RXSKIPMIDAMBLE1X80M field reset value
#define MDM_RXSKIPMIDAMBLE1X80M_RST    0x1
/// RXSKIPMIDAMBLE1X40M field reset value
#define MDM_RXSKIPMIDAMBLE1X40M_RST    0x0
/// RXSKIPMIDAMBLE1X20M field reset value
#define MDM_RXSKIPMIDAMBLE1X20M_RST    0x0

/**
 * @brief Constructs a value for the RXCTRL11 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxskipmidamble4x80m - The value to use for the RXSKIPMIDAMBLE4X80M field.
 * @param[in] rxskipmidamble4x40m - The value to use for the RXSKIPMIDAMBLE4X40M field.
 * @param[in] rxskipmidamble4x20m - The value to use for the RXSKIPMIDAMBLE4X20M field.
 * @param[in] rxskipmidamble2x80m - The value to use for the RXSKIPMIDAMBLE2X80M field.
 * @param[in] rxskipmidamble2x40m - The value to use for the RXSKIPMIDAMBLE2X40M field.
 * @param[in] rxskipmidamble2x20m - The value to use for the RXSKIPMIDAMBLE2X20M field.
 * @param[in] rxskipmidamble1x80m - The value to use for the RXSKIPMIDAMBLE1X80M field.
 * @param[in] rxskipmidamble1x40m - The value to use for the RXSKIPMIDAMBLE1X40M field.
 * @param[in] rxskipmidamble1x20m - The value to use for the RXSKIPMIDAMBLE1X20M field.
 */
__INLINE void mdm_rxctrl11_pack(uint8_t rxskipmidamble4x80m, uint8_t rxskipmidamble4x40m, uint8_t rxskipmidamble4x20m, uint8_t rxskipmidamble2x80m, uint8_t rxskipmidamble2x40m, uint8_t rxskipmidamble2x20m, uint8_t rxskipmidamble1x80m, uint8_t rxskipmidamble1x40m, uint8_t rxskipmidamble1x20m)
{
    ASSERT_ERR((((uint32_t)rxskipmidamble4x80m << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)rxskipmidamble4x40m << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)rxskipmidamble4x20m << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)rxskipmidamble2x80m << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)rxskipmidamble2x40m << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)rxskipmidamble2x20m << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)rxskipmidamble1x80m << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)rxskipmidamble1x40m << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)rxskipmidamble1x20m << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_RXCTRL11_ADDR,  ((uint32_t)rxskipmidamble4x80m << 10) | ((uint32_t)rxskipmidamble4x40m << 9) | ((uint32_t)rxskipmidamble4x20m << 8) | ((uint32_t)rxskipmidamble2x80m << 6) | ((uint32_t)rxskipmidamble2x40m << 5) | ((uint32_t)rxskipmidamble2x20m << 4) | ((uint32_t)rxskipmidamble1x80m << 2) | ((uint32_t)rxskipmidamble1x40m << 1) | ((uint32_t)rxskipmidamble1x20m << 0));
}

/**
 * @brief Unpacks RXCTRL11's fields from current value of the RXCTRL11 register.
 *
 * Reads the RXCTRL11 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxskipmidamble4x80m - Will be populated with the current value of this field from the register.
 * @param[out] rxskipmidamble4x40m - Will be populated with the current value of this field from the register.
 * @param[out] rxskipmidamble4x20m - Will be populated with the current value of this field from the register.
 * @param[out] rxskipmidamble2x80m - Will be populated with the current value of this field from the register.
 * @param[out] rxskipmidamble2x40m - Will be populated with the current value of this field from the register.
 * @param[out] rxskipmidamble2x20m - Will be populated with the current value of this field from the register.
 * @param[out] rxskipmidamble1x80m - Will be populated with the current value of this field from the register.
 * @param[out] rxskipmidamble1x40m - Will be populated with the current value of this field from the register.
 * @param[out] rxskipmidamble1x20m - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_rxctrl11_unpack(uint8_t* rxskipmidamble4x80m, uint8_t* rxskipmidamble4x40m, uint8_t* rxskipmidamble4x20m, uint8_t* rxskipmidamble2x80m, uint8_t* rxskipmidamble2x40m, uint8_t* rxskipmidamble2x20m, uint8_t* rxskipmidamble1x80m, uint8_t* rxskipmidamble1x40m, uint8_t* rxskipmidamble1x20m)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL11_ADDR);

    *rxskipmidamble4x80m = (localVal & ((uint32_t)0x00000400)) >> 10;
    *rxskipmidamble4x40m = (localVal & ((uint32_t)0x00000200)) >> 9;
    *rxskipmidamble4x20m = (localVal & ((uint32_t)0x00000100)) >> 8;
    *rxskipmidamble2x80m = (localVal & ((uint32_t)0x00000040)) >> 6;
    *rxskipmidamble2x40m = (localVal & ((uint32_t)0x00000020)) >> 5;
    *rxskipmidamble2x20m = (localVal & ((uint32_t)0x00000010)) >> 4;
    *rxskipmidamble1x80m = (localVal & ((uint32_t)0x00000004)) >> 2;
    *rxskipmidamble1x40m = (localVal & ((uint32_t)0x00000002)) >> 1;
    *rxskipmidamble1x20m = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the RXSKIPMIDAMBLE4X80M field in the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read and the RXSKIPMIDAMBLE4X80M field's value will be returned.
 *
 * @return The current value of the RXSKIPMIDAMBLE4X80M field in the RXCTRL11 register.
 */
__INLINE uint8_t mdm_rxskipmidamble4x80m_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL11_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the RXSKIPMIDAMBLE4X80M field of the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxskipmidamble4x80m - The value to set the field to.
 */
__INLINE void mdm_rxskipmidamble4x80m_setf(uint8_t rxskipmidamble4x80m)
{
    ASSERT_ERR((((uint32_t)rxskipmidamble4x80m << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(MDM_RXCTRL11_ADDR, (REG_PL_RD(MDM_RXCTRL11_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)rxskipmidamble4x80m << 10));
}

/**
 * @brief Returns the current value of the RXSKIPMIDAMBLE4X40M field in the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read and the RXSKIPMIDAMBLE4X40M field's value will be returned.
 *
 * @return The current value of the RXSKIPMIDAMBLE4X40M field in the RXCTRL11 register.
 */
__INLINE uint8_t mdm_rxskipmidamble4x40m_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL11_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the RXSKIPMIDAMBLE4X40M field of the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxskipmidamble4x40m - The value to set the field to.
 */
__INLINE void mdm_rxskipmidamble4x40m_setf(uint8_t rxskipmidamble4x40m)
{
    ASSERT_ERR((((uint32_t)rxskipmidamble4x40m << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(MDM_RXCTRL11_ADDR, (REG_PL_RD(MDM_RXCTRL11_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)rxskipmidamble4x40m << 9));
}

/**
 * @brief Returns the current value of the RXSKIPMIDAMBLE4X20M field in the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read and the RXSKIPMIDAMBLE4X20M field's value will be returned.
 *
 * @return The current value of the RXSKIPMIDAMBLE4X20M field in the RXCTRL11 register.
 */
__INLINE uint8_t mdm_rxskipmidamble4x20m_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL11_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the RXSKIPMIDAMBLE4X20M field of the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxskipmidamble4x20m - The value to set the field to.
 */
__INLINE void mdm_rxskipmidamble4x20m_setf(uint8_t rxskipmidamble4x20m)
{
    ASSERT_ERR((((uint32_t)rxskipmidamble4x20m << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(MDM_RXCTRL11_ADDR, (REG_PL_RD(MDM_RXCTRL11_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)rxskipmidamble4x20m << 8));
}

/**
 * @brief Returns the current value of the RXSKIPMIDAMBLE2X80M field in the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read and the RXSKIPMIDAMBLE2X80M field's value will be returned.
 *
 * @return The current value of the RXSKIPMIDAMBLE2X80M field in the RXCTRL11 register.
 */
__INLINE uint8_t mdm_rxskipmidamble2x80m_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL11_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

/**
 * @brief Sets the RXSKIPMIDAMBLE2X80M field of the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxskipmidamble2x80m - The value to set the field to.
 */
__INLINE void mdm_rxskipmidamble2x80m_setf(uint8_t rxskipmidamble2x80m)
{
    ASSERT_ERR((((uint32_t)rxskipmidamble2x80m << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(MDM_RXCTRL11_ADDR, (REG_PL_RD(MDM_RXCTRL11_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)rxskipmidamble2x80m << 6));
}

/**
 * @brief Returns the current value of the RXSKIPMIDAMBLE2X40M field in the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read and the RXSKIPMIDAMBLE2X40M field's value will be returned.
 *
 * @return The current value of the RXSKIPMIDAMBLE2X40M field in the RXCTRL11 register.
 */
__INLINE uint8_t mdm_rxskipmidamble2x40m_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL11_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

/**
 * @brief Sets the RXSKIPMIDAMBLE2X40M field of the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxskipmidamble2x40m - The value to set the field to.
 */
__INLINE void mdm_rxskipmidamble2x40m_setf(uint8_t rxskipmidamble2x40m)
{
    ASSERT_ERR((((uint32_t)rxskipmidamble2x40m << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(MDM_RXCTRL11_ADDR, (REG_PL_RD(MDM_RXCTRL11_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)rxskipmidamble2x40m << 5));
}

/**
 * @brief Returns the current value of the RXSKIPMIDAMBLE2X20M field in the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read and the RXSKIPMIDAMBLE2X20M field's value will be returned.
 *
 * @return The current value of the RXSKIPMIDAMBLE2X20M field in the RXCTRL11 register.
 */
__INLINE uint8_t mdm_rxskipmidamble2x20m_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL11_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the RXSKIPMIDAMBLE2X20M field of the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxskipmidamble2x20m - The value to set the field to.
 */
__INLINE void mdm_rxskipmidamble2x20m_setf(uint8_t rxskipmidamble2x20m)
{
    ASSERT_ERR((((uint32_t)rxskipmidamble2x20m << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_RXCTRL11_ADDR, (REG_PL_RD(MDM_RXCTRL11_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)rxskipmidamble2x20m << 4));
}

/**
 * @brief Returns the current value of the RXSKIPMIDAMBLE1X80M field in the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read and the RXSKIPMIDAMBLE1X80M field's value will be returned.
 *
 * @return The current value of the RXSKIPMIDAMBLE1X80M field in the RXCTRL11 register.
 */
__INLINE uint8_t mdm_rxskipmidamble1x80m_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL11_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the RXSKIPMIDAMBLE1X80M field of the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxskipmidamble1x80m - The value to set the field to.
 */
__INLINE void mdm_rxskipmidamble1x80m_setf(uint8_t rxskipmidamble1x80m)
{
    ASSERT_ERR((((uint32_t)rxskipmidamble1x80m << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_RXCTRL11_ADDR, (REG_PL_RD(MDM_RXCTRL11_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)rxskipmidamble1x80m << 2));
}

/**
 * @brief Returns the current value of the RXSKIPMIDAMBLE1X40M field in the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read and the RXSKIPMIDAMBLE1X40M field's value will be returned.
 *
 * @return The current value of the RXSKIPMIDAMBLE1X40M field in the RXCTRL11 register.
 */
__INLINE uint8_t mdm_rxskipmidamble1x40m_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL11_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the RXSKIPMIDAMBLE1X40M field of the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxskipmidamble1x40m - The value to set the field to.
 */
__INLINE void mdm_rxskipmidamble1x40m_setf(uint8_t rxskipmidamble1x40m)
{
    ASSERT_ERR((((uint32_t)rxskipmidamble1x40m << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_RXCTRL11_ADDR, (REG_PL_RD(MDM_RXCTRL11_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)rxskipmidamble1x40m << 1));
}

/**
 * @brief Returns the current value of the RXSKIPMIDAMBLE1X20M field in the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read and the RXSKIPMIDAMBLE1X20M field's value will be returned.
 *
 * @return The current value of the RXSKIPMIDAMBLE1X20M field in the RXCTRL11 register.
 */
__INLINE uint8_t mdm_rxskipmidamble1x20m_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_RXCTRL11_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the RXSKIPMIDAMBLE1X20M field of the RXCTRL11 register.
 *
 * The RXCTRL11 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxskipmidamble1x20m - The value to set the field to.
 */
__INLINE void mdm_rxskipmidamble1x20m_setf(uint8_t rxskipmidamble1x20m)
{
    ASSERT_ERR((((uint32_t)rxskipmidamble1x20m << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_RXCTRL11_ADDR, (REG_PL_RD(MDM_RXCTRL11_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)rxskipmidamble1x20m << 0));
}

/// @}

/**
 * @name EQUALCTRL3 register definitions
 * <table>
 * <caption>EQUALCTRL3 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 10 <td>   HSCALE_HEMU_EN <td> R <td> R/W <td> 1
 * <tr><td> 09 <td>   HSCALE_HESU_EN <td> R <td> R/W <td> 1
 * <tr><td> 08 <td>  HSCALE_NONHE_EN <td> R <td> R/W <td> 1
 * <tr><td> 02 <td>  MH_HEMU_CH_MODE <td> R <td> R/W <td> 0
 * <tr><td> 01 <td>  MH_HESU_CH_MODE <td> R <td> R/W <td> 0
 * <tr><td> 00 <td> MH_NONHE_CH_MODE <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the EQUALCTRL3 register
#define MDM_EQUALCTRL3_ADDR   0x24C008F4
/// Offset of the EQUALCTRL3 register from the base address
#define MDM_EQUALCTRL3_OFFSET 0x000008F4
/// Index of the EQUALCTRL3 register
#define MDM_EQUALCTRL3_INDEX  0x0000023D
/// Reset value of the EQUALCTRL3 register
#define MDM_EQUALCTRL3_RESET  0x00000700

/**
 * @brief Returns the current value of the EQUALCTRL3 register.
 * The EQUALCTRL3 register will be read and its value returned.
 * @return The current value of the EQUALCTRL3 register.
 */
__INLINE uint32_t mdm_equalctrl3_get(void)
{
    return REG_PL_RD(MDM_EQUALCTRL3_ADDR);
}

/**
 * @brief Sets the EQUALCTRL3 register to a value.
 * The EQUALCTRL3 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_equalctrl3_set(uint32_t value)
{
    REG_PL_WR(MDM_EQUALCTRL3_ADDR, value);
}

// field definitions
/// HSCALE_HEMU_EN field bit
#define MDM_HSCALE_HEMU_EN_BIT      ((uint32_t)0x00000400)
/// HSCALE_HEMU_EN field position
#define MDM_HSCALE_HEMU_EN_POS      10
/// HSCALE_HESU_EN field bit
#define MDM_HSCALE_HESU_EN_BIT      ((uint32_t)0x00000200)
/// HSCALE_HESU_EN field position
#define MDM_HSCALE_HESU_EN_POS      9
/// HSCALE_NONHE_EN field bit
#define MDM_HSCALE_NONHE_EN_BIT     ((uint32_t)0x00000100)
/// HSCALE_NONHE_EN field position
#define MDM_HSCALE_NONHE_EN_POS     8
/// MH_HEMU_CH_MODE field bit
#define MDM_MH_HEMU_CH_MODE_BIT     ((uint32_t)0x00000004)
/// MH_HEMU_CH_MODE field position
#define MDM_MH_HEMU_CH_MODE_POS     2
/// MH_HESU_CH_MODE field bit
#define MDM_MH_HESU_CH_MODE_BIT     ((uint32_t)0x00000002)
/// MH_HESU_CH_MODE field position
#define MDM_MH_HESU_CH_MODE_POS     1
/// MH_NONHE_CH_MODE field bit
#define MDM_MH_NONHE_CH_MODE_BIT    ((uint32_t)0x00000001)
/// MH_NONHE_CH_MODE field position
#define MDM_MH_NONHE_CH_MODE_POS    0

/// HSCALE_HEMU_EN field reset value
#define MDM_HSCALE_HEMU_EN_RST      0x1
/// HSCALE_HESU_EN field reset value
#define MDM_HSCALE_HESU_EN_RST      0x1
/// HSCALE_NONHE_EN field reset value
#define MDM_HSCALE_NONHE_EN_RST     0x1
/// MH_HEMU_CH_MODE field reset value
#define MDM_MH_HEMU_CH_MODE_RST     0x0
/// MH_HESU_CH_MODE field reset value
#define MDM_MH_HESU_CH_MODE_RST     0x0
/// MH_NONHE_CH_MODE field reset value
#define MDM_MH_NONHE_CH_MODE_RST    0x0

/**
 * @brief Constructs a value for the EQUALCTRL3 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] hscalehemuen - The value to use for the HSCALE_HEMU_EN field.
 * @param[in] hscalehesuen - The value to use for the HSCALE_HESU_EN field.
 * @param[in] hscalenonheen - The value to use for the HSCALE_NONHE_EN field.
 * @param[in] mhhemuchmode - The value to use for the MH_HEMU_CH_MODE field.
 * @param[in] mhhesuchmode - The value to use for the MH_HESU_CH_MODE field.
 * @param[in] mhnonhechmode - The value to use for the MH_NONHE_CH_MODE field.
 */
__INLINE void mdm_equalctrl3_pack(uint8_t hscalehemuen, uint8_t hscalehesuen, uint8_t hscalenonheen, uint8_t mhhemuchmode, uint8_t mhhesuchmode, uint8_t mhnonhechmode)
{
    ASSERT_ERR((((uint32_t)hscalehemuen << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)hscalehesuen << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)hscalenonheen << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)mhhemuchmode << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)mhhesuchmode << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)mhnonhechmode << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_EQUALCTRL3_ADDR,  ((uint32_t)hscalehemuen << 10) | ((uint32_t)hscalehesuen << 9) | ((uint32_t)hscalenonheen << 8) | ((uint32_t)mhhemuchmode << 2) | ((uint32_t)mhhesuchmode << 1) | ((uint32_t)mhnonhechmode << 0));
}

/**
 * @brief Unpacks EQUALCTRL3's fields from current value of the EQUALCTRL3 register.
 *
 * Reads the EQUALCTRL3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] hscalehemuen - Will be populated with the current value of this field from the register.
 * @param[out] hscalehesuen - Will be populated with the current value of this field from the register.
 * @param[out] hscalenonheen - Will be populated with the current value of this field from the register.
 * @param[out] mhhemuchmode - Will be populated with the current value of this field from the register.
 * @param[out] mhhesuchmode - Will be populated with the current value of this field from the register.
 * @param[out] mhnonhechmode - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_equalctrl3_unpack(uint8_t* hscalehemuen, uint8_t* hscalehesuen, uint8_t* hscalenonheen, uint8_t* mhhemuchmode, uint8_t* mhhesuchmode, uint8_t* mhnonhechmode)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL3_ADDR);

    *hscalehemuen = (localVal & ((uint32_t)0x00000400)) >> 10;
    *hscalehesuen = (localVal & ((uint32_t)0x00000200)) >> 9;
    *hscalenonheen = (localVal & ((uint32_t)0x00000100)) >> 8;
    *mhhemuchmode = (localVal & ((uint32_t)0x00000004)) >> 2;
    *mhhesuchmode = (localVal & ((uint32_t)0x00000002)) >> 1;
    *mhnonhechmode = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the HSCALE_HEMU_EN field in the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read and the HSCALE_HEMU_EN field's value will be returned.
 *
 * @return The current value of the HSCALE_HEMU_EN field in the EQUALCTRL3 register.
 */
__INLINE uint8_t mdm_hscale_hemu_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the HSCALE_HEMU_EN field of the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hscalehemuen - The value to set the field to.
 */
__INLINE void mdm_hscale_hemu_en_setf(uint8_t hscalehemuen)
{
    ASSERT_ERR((((uint32_t)hscalehemuen << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(MDM_EQUALCTRL3_ADDR, (REG_PL_RD(MDM_EQUALCTRL3_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)hscalehemuen << 10));
}

/**
 * @brief Returns the current value of the HSCALE_HESU_EN field in the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read and the HSCALE_HESU_EN field's value will be returned.
 *
 * @return The current value of the HSCALE_HESU_EN field in the EQUALCTRL3 register.
 */
__INLINE uint8_t mdm_hscale_hesu_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

/**
 * @brief Sets the HSCALE_HESU_EN field of the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hscalehesuen - The value to set the field to.
 */
__INLINE void mdm_hscale_hesu_en_setf(uint8_t hscalehesuen)
{
    ASSERT_ERR((((uint32_t)hscalehesuen << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_PL_WR(MDM_EQUALCTRL3_ADDR, (REG_PL_RD(MDM_EQUALCTRL3_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)hscalehesuen << 9));
}

/**
 * @brief Returns the current value of the HSCALE_NONHE_EN field in the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read and the HSCALE_NONHE_EN field's value will be returned.
 *
 * @return The current value of the HSCALE_NONHE_EN field in the EQUALCTRL3 register.
 */
__INLINE uint8_t mdm_hscale_nonhe_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

/**
 * @brief Sets the HSCALE_NONHE_EN field of the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] hscalenonheen - The value to set the field to.
 */
__INLINE void mdm_hscale_nonhe_en_setf(uint8_t hscalenonheen)
{
    ASSERT_ERR((((uint32_t)hscalenonheen << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_PL_WR(MDM_EQUALCTRL3_ADDR, (REG_PL_RD(MDM_EQUALCTRL3_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)hscalenonheen << 8));
}

/**
 * @brief Returns the current value of the MH_HEMU_CH_MODE field in the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read and the MH_HEMU_CH_MODE field's value will be returned.
 *
 * @return The current value of the MH_HEMU_CH_MODE field in the EQUALCTRL3 register.
 */
__INLINE uint8_t mdm_mh_hemu_ch_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the MH_HEMU_CH_MODE field of the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mhhemuchmode - The value to set the field to.
 */
__INLINE void mdm_mh_hemu_ch_mode_setf(uint8_t mhhemuchmode)
{
    ASSERT_ERR((((uint32_t)mhhemuchmode << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_EQUALCTRL3_ADDR, (REG_PL_RD(MDM_EQUALCTRL3_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)mhhemuchmode << 2));
}

/**
 * @brief Returns the current value of the MH_HESU_CH_MODE field in the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read and the MH_HESU_CH_MODE field's value will be returned.
 *
 * @return The current value of the MH_HESU_CH_MODE field in the EQUALCTRL3 register.
 */
__INLINE uint8_t mdm_mh_hesu_ch_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the MH_HESU_CH_MODE field of the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mhhesuchmode - The value to set the field to.
 */
__INLINE void mdm_mh_hesu_ch_mode_setf(uint8_t mhhesuchmode)
{
    ASSERT_ERR((((uint32_t)mhhesuchmode << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_EQUALCTRL3_ADDR, (REG_PL_RD(MDM_EQUALCTRL3_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)mhhesuchmode << 1));
}

/**
 * @brief Returns the current value of the MH_NONHE_CH_MODE field in the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read and the MH_NONHE_CH_MODE field's value will be returned.
 *
 * @return The current value of the MH_NONHE_CH_MODE field in the EQUALCTRL3 register.
 */
__INLINE uint8_t mdm_mh_nonhe_ch_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_EQUALCTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the MH_NONHE_CH_MODE field of the EQUALCTRL3 register.
 *
 * The EQUALCTRL3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] mhnonhechmode - The value to set the field to.
 */
__INLINE void mdm_mh_nonhe_ch_mode_setf(uint8_t mhnonhechmode)
{
    ASSERT_ERR((((uint32_t)mhnonhechmode << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_EQUALCTRL3_ADDR, (REG_PL_RD(MDM_EQUALCTRL3_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)mhnonhechmode << 0));
}

/// @}

#if RW_NX_CSI
/**
 * @name CSIRDCTL register definitions
 * <table>
 * <caption>CSIRDCTL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 01 <td>  CSI_RDENABLE <td> R <td> R/W <td> 0
 * <tr><td> 00 <td> CSI_RDREQUEST <td> R <td> R/W <td> 0
 * </table>
 *
 * @{
 */

/// Address of the CSIRDCTL register
#define MDM_CSIRDCTL_ADDR   0x24C008F8
/// Offset of the CSIRDCTL register from the base address
#define MDM_CSIRDCTL_OFFSET 0x000008F8
/// Index of the CSIRDCTL register
#define MDM_CSIRDCTL_INDEX  0x0000023E
/// Reset value of the CSIRDCTL register
#define MDM_CSIRDCTL_RESET  0x00000000

/**
 * @brief Returns the current value of the CSIRDCTL register.
 * The CSIRDCTL register will be read and its value returned.
 * @return The current value of the CSIRDCTL register.
 */
__INLINE uint32_t mdm_csirdctl_get(void)
{
    return REG_PL_RD(MDM_CSIRDCTL_ADDR);
}

/**
 * @brief Sets the CSIRDCTL register to a value.
 * The CSIRDCTL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_csirdctl_set(uint32_t value)
{
    REG_PL_WR(MDM_CSIRDCTL_ADDR, value);
}

// field definitions
/// CSI_RDENABLE field bit
#define MDM_CSI_RDENABLE_BIT     ((uint32_t)0x00000002)
/// CSI_RDENABLE field position
#define MDM_CSI_RDENABLE_POS     1
/// CSI_RDREQUEST field bit
#define MDM_CSI_RDREQUEST_BIT    ((uint32_t)0x00000001)
/// CSI_RDREQUEST field position
#define MDM_CSI_RDREQUEST_POS    0

/// CSI_RDENABLE field reset value
#define MDM_CSI_RDENABLE_RST     0x0
/// CSI_RDREQUEST field reset value
#define MDM_CSI_RDREQUEST_RST    0x0

/**
 * @brief Constructs a value for the CSIRDCTL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] csirdenable - The value to use for the CSI_RDENABLE field.
 * @param[in] csirdrequest - The value to use for the CSI_RDREQUEST field.
 */
__INLINE void mdm_csirdctl_pack(uint8_t csirdenable, uint8_t csirdrequest)
{
    ASSERT_ERR((((uint32_t)csirdenable << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)csirdrequest << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_CSIRDCTL_ADDR,  ((uint32_t)csirdenable << 1) | ((uint32_t)csirdrequest << 0));
}

/**
 * @brief Unpacks CSIRDCTL's fields from current value of the CSIRDCTL register.
 *
 * Reads the CSIRDCTL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] csirdenable - Will be populated with the current value of this field from the register.
 * @param[out] csirdrequest - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_csirdctl_unpack(uint8_t* csirdenable, uint8_t* csirdrequest)
{
    uint32_t localVal = REG_PL_RD(MDM_CSIRDCTL_ADDR);

    *csirdenable = (localVal & ((uint32_t)0x00000002)) >> 1;
    *csirdrequest = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the CSI_RDENABLE field in the CSIRDCTL register.
 *
 * The CSIRDCTL register will be read and the CSI_RDENABLE field's value will be returned.
 *
 * @return The current value of the CSI_RDENABLE field in the CSIRDCTL register.
 */
__INLINE uint8_t mdm_csi_rdenable_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CSIRDCTL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the CSI_RDENABLE field of the CSIRDCTL register.
 *
 * The CSIRDCTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] csirdenable - The value to set the field to.
 */
__INLINE void mdm_csi_rdenable_setf(uint8_t csirdenable)
{
    ASSERT_ERR((((uint32_t)csirdenable << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_CSIRDCTL_ADDR, (REG_PL_RD(MDM_CSIRDCTL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)csirdenable << 1));
}

/**
 * @brief Returns the current value of the CSI_RDREQUEST field in the CSIRDCTL register.
 *
 * The CSIRDCTL register will be read and the CSI_RDREQUEST field's value will be returned.
 *
 * @return The current value of the CSI_RDREQUEST field in the CSIRDCTL register.
 */
__INLINE uint8_t mdm_csi_rdrequest_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_CSIRDCTL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the CSI_RDREQUEST field of the CSIRDCTL register.
 *
 * The CSIRDCTL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] csirdrequest - The value to set the field to.
 */
__INLINE void mdm_csi_rdrequest_setf(uint8_t csirdrequest)
{
    ASSERT_ERR((((uint32_t)csirdrequest << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_CSIRDCTL_ADDR, (REG_PL_RD(MDM_CSIRDCTL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)csirdrequest << 0));
}

#endif // RW_NX_CSI
/// @}

/**
 * @name BOUF_CTRL register definitions
 * <table>
 * <caption>BOUF_CTRL bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 11    <td>             RX_LDPC_DCM_MODE <td> R <td> R/W <td> 1
 * <tr><td> 10    <td>             TX_LDPC_DCM_MODE <td> R <td> R/W <td> 1
 * <tr><td> 09:05 <td>          LDPCDEC_DELTA_EARLY <td> R <td> R/W <td> 0x1F
 * <tr><td> 04    <td>             LDPCDEC_DELTA_EN <td> R <td> R/W <td> 0
 * <tr><td> 03    <td> RX_HE_STBC_LDPC_POSTPAD_TYPE <td> R <td> R/W <td> 1
 * <tr><td> 02    <td>  RX_HE_STBC_BCC_POSTPAD_TYPE <td> R <td> R/W <td> 1
 * <tr><td> 01    <td>                CHESTNSTSSUP2 <td> R <td> R/W <td> 1
 * <tr><td> 00    <td>      TX_INIT_SEED_UPD_BY_PKT <td> R <td> R/W <td> 1
 * </table>
 *
 * @{
 */

/// Address of the BOUF_CTRL register
#define MDM_BOUF_CTRL_ADDR   0x24C008FC
/// Offset of the BOUF_CTRL register from the base address
#define MDM_BOUF_CTRL_OFFSET 0x000008FC
/// Index of the BOUF_CTRL register
#define MDM_BOUF_CTRL_INDEX  0x0000023F
/// Reset value of the BOUF_CTRL register
#define MDM_BOUF_CTRL_RESET  0x00000FEF

/**
 * @brief Returns the current value of the BOUF_CTRL register.
 * The BOUF_CTRL register will be read and its value returned.
 * @return The current value of the BOUF_CTRL register.
 */
__INLINE uint32_t mdm_bouf_ctrl_get(void)
{
    return REG_PL_RD(MDM_BOUF_CTRL_ADDR);
}

/**
 * @brief Sets the BOUF_CTRL register to a value.
 * The BOUF_CTRL register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_bouf_ctrl_set(uint32_t value)
{
    REG_PL_WR(MDM_BOUF_CTRL_ADDR, value);
}

// field definitions
/// RX_LDPC_DCM_MODE field bit
#define MDM_RX_LDPC_DCM_MODE_BIT                ((uint32_t)0x00000800)
/// RX_LDPC_DCM_MODE field position
#define MDM_RX_LDPC_DCM_MODE_POS                11
/// TX_LDPC_DCM_MODE field bit
#define MDM_TX_LDPC_DCM_MODE_BIT                ((uint32_t)0x00000400)
/// TX_LDPC_DCM_MODE field position
#define MDM_TX_LDPC_DCM_MODE_POS                10
/// LDPCDEC_DELTA_EARLY field mask
#define MDM_LDPCDEC_DELTA_EARLY_MASK            ((uint32_t)0x000003E0)
/// LDPCDEC_DELTA_EARLY field LSB position
#define MDM_LDPCDEC_DELTA_EARLY_LSB             5
/// LDPCDEC_DELTA_EARLY field width
#define MDM_LDPCDEC_DELTA_EARLY_WIDTH           ((uint32_t)0x00000005)
/// LDPCDEC_DELTA_EN field bit
#define MDM_LDPCDEC_DELTA_EN_BIT                ((uint32_t)0x00000010)
/// LDPCDEC_DELTA_EN field position
#define MDM_LDPCDEC_DELTA_EN_POS                4
/// RX_HE_STBC_LDPC_POSTPAD_TYPE field bit
#define MDM_RX_HE_STBC_LDPC_POSTPAD_TYPE_BIT    ((uint32_t)0x00000008)
/// RX_HE_STBC_LDPC_POSTPAD_TYPE field position
#define MDM_RX_HE_STBC_LDPC_POSTPAD_TYPE_POS    3
/// RX_HE_STBC_BCC_POSTPAD_TYPE field bit
#define MDM_RX_HE_STBC_BCC_POSTPAD_TYPE_BIT     ((uint32_t)0x00000004)
/// RX_HE_STBC_BCC_POSTPAD_TYPE field position
#define MDM_RX_HE_STBC_BCC_POSTPAD_TYPE_POS     2
/// CHESTNSTSSUP2 field bit
#define MDM_CHESTNSTSSUP2_BIT                   ((uint32_t)0x00000002)
/// CHESTNSTSSUP2 field position
#define MDM_CHESTNSTSSUP2_POS                   1
/// TX_INIT_SEED_UPD_BY_PKT field bit
#define MDM_TX_INIT_SEED_UPD_BY_PKT_BIT         ((uint32_t)0x00000001)
/// TX_INIT_SEED_UPD_BY_PKT field position
#define MDM_TX_INIT_SEED_UPD_BY_PKT_POS         0

/// RX_LDPC_DCM_MODE field reset value
#define MDM_RX_LDPC_DCM_MODE_RST                0x1
/// TX_LDPC_DCM_MODE field reset value
#define MDM_TX_LDPC_DCM_MODE_RST                0x1
/// LDPCDEC_DELTA_EARLY field reset value
#define MDM_LDPCDEC_DELTA_EARLY_RST             0x1F
/// LDPCDEC_DELTA_EN field reset value
#define MDM_LDPCDEC_DELTA_EN_RST                0x0
/// RX_HE_STBC_LDPC_POSTPAD_TYPE field reset value
#define MDM_RX_HE_STBC_LDPC_POSTPAD_TYPE_RST    0x1
/// RX_HE_STBC_BCC_POSTPAD_TYPE field reset value
#define MDM_RX_HE_STBC_BCC_POSTPAD_TYPE_RST     0x1
/// CHESTNSTSSUP2 field reset value
#define MDM_CHESTNSTSSUP2_RST                   0x1
/// TX_INIT_SEED_UPD_BY_PKT field reset value
#define MDM_TX_INIT_SEED_UPD_BY_PKT_RST         0x1

/**
 * @brief Constructs a value for the BOUF_CTRL register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] rxldpcdcmmode - The value to use for the RX_LDPC_DCM_MODE field.
 * @param[in] txldpcdcmmode - The value to use for the TX_LDPC_DCM_MODE field.
 * @param[in] ldpcdecdeltaearly - The value to use for the LDPCDEC_DELTA_EARLY field.
 * @param[in] ldpcdecdeltaen - The value to use for the LDPCDEC_DELTA_EN field.
 * @param[in] rxhestbcldpcpostpadtype - The value to use for the RX_HE_STBC_LDPC_POSTPAD_TYPE field.
 * @param[in] rxhestbcbccpostpadtype - The value to use for the RX_HE_STBC_BCC_POSTPAD_TYPE field.
 * @param[in] chestnstssup2 - The value to use for the CHESTNSTSSUP2 field.
 * @param[in] txinitseedupdbypkt - The value to use for the TX_INIT_SEED_UPD_BY_PKT field.
 */
__INLINE void mdm_bouf_ctrl_pack(uint8_t rxldpcdcmmode, uint8_t txldpcdcmmode, uint8_t ldpcdecdeltaearly, uint8_t ldpcdecdeltaen, uint8_t rxhestbcldpcpostpadtype, uint8_t rxhestbcbccpostpadtype, uint8_t chestnstssup2, uint8_t txinitseedupdbypkt)
{
    ASSERT_ERR((((uint32_t)rxldpcdcmmode << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)txldpcdcmmode << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdeltaearly << 5) & ~((uint32_t)0x000003E0)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdeltaen << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)rxhestbcldpcpostpadtype << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)rxhestbcbccpostpadtype << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)chestnstssup2 << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)txinitseedupdbypkt << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_BOUF_CTRL_ADDR,  ((uint32_t)rxldpcdcmmode << 11) | ((uint32_t)txldpcdcmmode << 10) | ((uint32_t)ldpcdecdeltaearly << 5) | ((uint32_t)ldpcdecdeltaen << 4) | ((uint32_t)rxhestbcldpcpostpadtype << 3) | ((uint32_t)rxhestbcbccpostpadtype << 2) | ((uint32_t)chestnstssup2 << 1) | ((uint32_t)txinitseedupdbypkt << 0));
}

/**
 * @brief Unpacks BOUF_CTRL's fields from current value of the BOUF_CTRL register.
 *
 * Reads the BOUF_CTRL register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] rxldpcdcmmode - Will be populated with the current value of this field from the register.
 * @param[out] txldpcdcmmode - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdeltaearly - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdeltaen - Will be populated with the current value of this field from the register.
 * @param[out] rxhestbcldpcpostpadtype - Will be populated with the current value of this field from the register.
 * @param[out] rxhestbcbccpostpadtype - Will be populated with the current value of this field from the register.
 * @param[out] chestnstssup2 - Will be populated with the current value of this field from the register.
 * @param[out] txinitseedupdbypkt - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_bouf_ctrl_unpack(uint8_t* rxldpcdcmmode, uint8_t* txldpcdcmmode, uint8_t* ldpcdecdeltaearly, uint8_t* ldpcdecdeltaen, uint8_t* rxhestbcldpcpostpadtype, uint8_t* rxhestbcbccpostpadtype, uint8_t* chestnstssup2, uint8_t* txinitseedupdbypkt)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_CTRL_ADDR);

    *rxldpcdcmmode = (localVal & ((uint32_t)0x00000800)) >> 11;
    *txldpcdcmmode = (localVal & ((uint32_t)0x00000400)) >> 10;
    *ldpcdecdeltaearly = (localVal & ((uint32_t)0x000003E0)) >> 5;
    *ldpcdecdeltaen = (localVal & ((uint32_t)0x00000010)) >> 4;
    *rxhestbcldpcpostpadtype = (localVal & ((uint32_t)0x00000008)) >> 3;
    *rxhestbcbccpostpadtype = (localVal & ((uint32_t)0x00000004)) >> 2;
    *chestnstssup2 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *txinitseedupdbypkt = (localVal & ((uint32_t)0x00000001)) >> 0;
}

/**
 * @brief Returns the current value of the RX_LDPC_DCM_MODE field in the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read and the RX_LDPC_DCM_MODE field's value will be returned.
 *
 * @return The current value of the RX_LDPC_DCM_MODE field in the BOUF_CTRL register.
 */
__INLINE uint8_t mdm_rx_ldpc_dcm_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

/**
 * @brief Sets the RX_LDPC_DCM_MODE field of the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxldpcdcmmode - The value to set the field to.
 */
__INLINE void mdm_rx_ldpc_dcm_mode_setf(uint8_t rxldpcdcmmode)
{
    ASSERT_ERR((((uint32_t)rxldpcdcmmode << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_PL_WR(MDM_BOUF_CTRL_ADDR, (REG_PL_RD(MDM_BOUF_CTRL_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)rxldpcdcmmode << 11));
}

/**
 * @brief Returns the current value of the TX_LDPC_DCM_MODE field in the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read and the TX_LDPC_DCM_MODE field's value will be returned.
 *
 * @return The current value of the TX_LDPC_DCM_MODE field in the BOUF_CTRL register.
 */
__INLINE uint8_t mdm_tx_ldpc_dcm_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

/**
 * @brief Sets the TX_LDPC_DCM_MODE field of the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txldpcdcmmode - The value to set the field to.
 */
__INLINE void mdm_tx_ldpc_dcm_mode_setf(uint8_t txldpcdcmmode)
{
    ASSERT_ERR((((uint32_t)txldpcdcmmode << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_PL_WR(MDM_BOUF_CTRL_ADDR, (REG_PL_RD(MDM_BOUF_CTRL_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)txldpcdcmmode << 10));
}

/**
 * @brief Returns the current value of the LDPCDEC_DELTA_EARLY field in the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read and the LDPCDEC_DELTA_EARLY field's value will be returned.
 *
 * @return The current value of the LDPCDEC_DELTA_EARLY field in the BOUF_CTRL register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_early_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x000003E0)) >> 5);
}

/**
 * @brief Sets the LDPCDEC_DELTA_EARLY field of the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdeltaearly - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_early_setf(uint8_t ldpcdecdeltaearly)
{
    ASSERT_ERR((((uint32_t)ldpcdecdeltaearly << 5) & ~((uint32_t)0x000003E0)) == 0);
    REG_PL_WR(MDM_BOUF_CTRL_ADDR, (REG_PL_RD(MDM_BOUF_CTRL_ADDR) & ~((uint32_t)0x000003E0)) | ((uint32_t)ldpcdecdeltaearly << 5));
}

/**
 * @brief Returns the current value of the LDPCDEC_DELTA_EN field in the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read and the LDPCDEC_DELTA_EN field's value will be returned.
 *
 * @return The current value of the LDPCDEC_DELTA_EN field in the BOUF_CTRL register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

/**
 * @brief Sets the LDPCDEC_DELTA_EN field of the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdeltaen - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_en_setf(uint8_t ldpcdecdeltaen)
{
    ASSERT_ERR((((uint32_t)ldpcdecdeltaen << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(MDM_BOUF_CTRL_ADDR, (REG_PL_RD(MDM_BOUF_CTRL_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)ldpcdecdeltaen << 4));
}

/**
 * @brief Returns the current value of the RX_HE_STBC_LDPC_POSTPAD_TYPE field in the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read and the RX_HE_STBC_LDPC_POSTPAD_TYPE field's value will be returned.
 *
 * @return The current value of the RX_HE_STBC_LDPC_POSTPAD_TYPE field in the BOUF_CTRL register.
 */
__INLINE uint8_t mdm_rx_he_stbc_ldpc_postpad_type_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

/**
 * @brief Sets the RX_HE_STBC_LDPC_POSTPAD_TYPE field of the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxhestbcldpcpostpadtype - The value to set the field to.
 */
__INLINE void mdm_rx_he_stbc_ldpc_postpad_type_setf(uint8_t rxhestbcldpcpostpadtype)
{
    ASSERT_ERR((((uint32_t)rxhestbcldpcpostpadtype << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(MDM_BOUF_CTRL_ADDR, (REG_PL_RD(MDM_BOUF_CTRL_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)rxhestbcldpcpostpadtype << 3));
}

/**
 * @brief Returns the current value of the RX_HE_STBC_BCC_POSTPAD_TYPE field in the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read and the RX_HE_STBC_BCC_POSTPAD_TYPE field's value will be returned.
 *
 * @return The current value of the RX_HE_STBC_BCC_POSTPAD_TYPE field in the BOUF_CTRL register.
 */
__INLINE uint8_t mdm_rx_he_stbc_bcc_postpad_type_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

/**
 * @brief Sets the RX_HE_STBC_BCC_POSTPAD_TYPE field of the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] rxhestbcbccpostpadtype - The value to set the field to.
 */
__INLINE void mdm_rx_he_stbc_bcc_postpad_type_setf(uint8_t rxhestbcbccpostpadtype)
{
    ASSERT_ERR((((uint32_t)rxhestbcbccpostpadtype << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(MDM_BOUF_CTRL_ADDR, (REG_PL_RD(MDM_BOUF_CTRL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)rxhestbcbccpostpadtype << 2));
}

/**
 * @brief Returns the current value of the CHESTNSTSSUP2 field in the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read and the CHESTNSTSSUP2 field's value will be returned.
 *
 * @return The current value of the CHESTNSTSSUP2 field in the BOUF_CTRL register.
 */
__INLINE uint8_t mdm_chestnstssup2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

/**
 * @brief Sets the CHESTNSTSSUP2 field of the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] chestnstssup2 - The value to set the field to.
 */
__INLINE void mdm_chestnstssup2_setf(uint8_t chestnstssup2)
{
    ASSERT_ERR((((uint32_t)chestnstssup2 << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(MDM_BOUF_CTRL_ADDR, (REG_PL_RD(MDM_BOUF_CTRL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)chestnstssup2 << 1));
}

/**
 * @brief Returns the current value of the TX_INIT_SEED_UPD_BY_PKT field in the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read and the TX_INIT_SEED_UPD_BY_PKT field's value will be returned.
 *
 * @return The current value of the TX_INIT_SEED_UPD_BY_PKT field in the BOUF_CTRL register.
 */
__INLINE uint8_t mdm_tx_init_seed_upd_by_pkt_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief Sets the TX_INIT_SEED_UPD_BY_PKT field of the BOUF_CTRL register.
 *
 * The BOUF_CTRL register will be read, modified to contain the new field value, and written.
 *
 * @param[in] txinitseedupdbypkt - The value to set the field to.
 */
__INLINE void mdm_tx_init_seed_upd_by_pkt_setf(uint8_t txinitseedupdbypkt)
{
    ASSERT_ERR((((uint32_t)txinitseedupdbypkt << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(MDM_BOUF_CTRL_ADDR, (REG_PL_RD(MDM_BOUF_CTRL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)txinitseedupdbypkt << 0));
}

/// @}

#if RW_NX_LDPC_DEC
/**
 * @name BOUF_LDPC0 register definitions
 * <table>
 * <caption>BOUF_LDPC0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:28 <td>   ldpcdec_delta1296_nbpsc1_stbc <td> R <td> R/W <td> 0x4
 * <tr><td> 27:24 <td>   ldpcdec_delta1296_nbpsc0_stbc <td> R <td> R/W <td> 0x5
 * <tr><td> 23:20 <td>        ldpcdec_delta1296_nbpsc1 <td> R <td> R/W <td> 0x8
 * <tr><td> 19:16 <td>        ldpcdec_delta1296_nbpsc0 <td> R <td> R/W <td> 0xC
 * <tr><td> 15:12 <td>    ldpcdec_delta648_nbpsc1_stbc <td> R <td> R/W <td> 0x6
 * <tr><td> 11:08 <td>    ldpcdec_delta648_nbpsc0_stbc <td> R <td> R/W <td> 0x6
 * <tr><td> 07:04 <td>         ldpcdec_delta648_nbpsc1 <td> R <td> R/W <td> 0xF
 * <tr><td> 03:00 <td>         ldpcdec_delta648_nbpsc0 <td> R <td> R/W <td> 0xE
 * </table>
 *
 * @{
 */

/// Address of the BOUF_LDPC0 register
#define MDM_BOUF_LDPC0_ADDR   0x24C00900
/// Offset of the BOUF_LDPC0 register from the base address
#define MDM_BOUF_LDPC0_OFFSET 0x00000900
/// Index of the BOUF_LDPC0 register
#define MDM_BOUF_LDPC0_INDEX  0x00000240
/// Reset value of the BOUF_LDPC0 register
#define MDM_BOUF_LDPC0_RESET  0x458C66FE

/**
 * @brief Returns the current value of the BOUF_LDPC0 register.
 * The BOUF_LDPC0 register will be read and its value returned.
 * @return The current value of the BOUF_LDPC0 register.
 */
__INLINE uint32_t mdm_bouf_ldpc0_get(void)
{
    return REG_PL_RD(MDM_BOUF_LDPC0_ADDR);
}

/**
 * @brief Sets the BOUF_LDPC0 register to a value.
 * The BOUF_LDPC0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_bouf_ldpc0_set(uint32_t value)
{
    REG_PL_WR(MDM_BOUF_LDPC0_ADDR, value);
}

// field definitions
/// LDPCDEC_DELTA_1296_NBPSC_1_STBC field mask
#define MDM_LDPCDEC_DELTA_1296_NBPSC_1_STBC_MASK   ((uint32_t)0xF0000000)
/// LDPCDEC_DELTA_1296_NBPSC_1_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1296_NBPSC_1_STBC_LSB    28
/// LDPCDEC_DELTA_1296_NBPSC_1_STBC field width
#define MDM_LDPCDEC_DELTA_1296_NBPSC_1_STBC_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1296_NBPSC_0_STBC field mask
#define MDM_LDPCDEC_DELTA_1296_NBPSC_0_STBC_MASK   ((uint32_t)0x0F000000)
/// LDPCDEC_DELTA_1296_NBPSC_0_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1296_NBPSC_0_STBC_LSB    24
/// LDPCDEC_DELTA_1296_NBPSC_0_STBC field width
#define MDM_LDPCDEC_DELTA_1296_NBPSC_0_STBC_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1296_NBPSC_1 field mask
#define MDM_LDPCDEC_DELTA_1296_NBPSC_1_MASK        ((uint32_t)0x00F00000)
/// LDPCDEC_DELTA_1296_NBPSC_1 field LSB position
#define MDM_LDPCDEC_DELTA_1296_NBPSC_1_LSB         20
/// LDPCDEC_DELTA_1296_NBPSC_1 field width
#define MDM_LDPCDEC_DELTA_1296_NBPSC_1_WIDTH       ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1296_NBPSC_0 field mask
#define MDM_LDPCDEC_DELTA_1296_NBPSC_0_MASK        ((uint32_t)0x000F0000)
/// LDPCDEC_DELTA_1296_NBPSC_0 field LSB position
#define MDM_LDPCDEC_DELTA_1296_NBPSC_0_LSB         16
/// LDPCDEC_DELTA_1296_NBPSC_0 field width
#define MDM_LDPCDEC_DELTA_1296_NBPSC_0_WIDTH       ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_648_NBPSC_1_STBC field mask
#define MDM_LDPCDEC_DELTA_648_NBPSC_1_STBC_MASK    ((uint32_t)0x0000F000)
/// LDPCDEC_DELTA_648_NBPSC_1_STBC field LSB position
#define MDM_LDPCDEC_DELTA_648_NBPSC_1_STBC_LSB     12
/// LDPCDEC_DELTA_648_NBPSC_1_STBC field width
#define MDM_LDPCDEC_DELTA_648_NBPSC_1_STBC_WIDTH   ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_648_NBPSC_0_STBC field mask
#define MDM_LDPCDEC_DELTA_648_NBPSC_0_STBC_MASK    ((uint32_t)0x00000F00)
/// LDPCDEC_DELTA_648_NBPSC_0_STBC field LSB position
#define MDM_LDPCDEC_DELTA_648_NBPSC_0_STBC_LSB     8
/// LDPCDEC_DELTA_648_NBPSC_0_STBC field width
#define MDM_LDPCDEC_DELTA_648_NBPSC_0_STBC_WIDTH   ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_648_NBPSC_1 field mask
#define MDM_LDPCDEC_DELTA_648_NBPSC_1_MASK         ((uint32_t)0x000000F0)
/// LDPCDEC_DELTA_648_NBPSC_1 field LSB position
#define MDM_LDPCDEC_DELTA_648_NBPSC_1_LSB          4
/// LDPCDEC_DELTA_648_NBPSC_1 field width
#define MDM_LDPCDEC_DELTA_648_NBPSC_1_WIDTH        ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_648_NBPSC_0 field mask
#define MDM_LDPCDEC_DELTA_648_NBPSC_0_MASK         ((uint32_t)0x0000000F)
/// LDPCDEC_DELTA_648_NBPSC_0 field LSB position
#define MDM_LDPCDEC_DELTA_648_NBPSC_0_LSB          0
/// LDPCDEC_DELTA_648_NBPSC_0 field width
#define MDM_LDPCDEC_DELTA_648_NBPSC_0_WIDTH        ((uint32_t)0x00000004)

/// LDPCDEC_DELTA_1296_NBPSC_1_STBC field reset value
#define MDM_LDPCDEC_DELTA_1296_NBPSC_1_STBC_RST    0x4
/// LDPCDEC_DELTA_1296_NBPSC_0_STBC field reset value
#define MDM_LDPCDEC_DELTA_1296_NBPSC_0_STBC_RST    0x5
/// LDPCDEC_DELTA_1296_NBPSC_1 field reset value
#define MDM_LDPCDEC_DELTA_1296_NBPSC_1_RST         0x8
/// LDPCDEC_DELTA_1296_NBPSC_0 field reset value
#define MDM_LDPCDEC_DELTA_1296_NBPSC_0_RST         0xC
/// LDPCDEC_DELTA_648_NBPSC_1_STBC field reset value
#define MDM_LDPCDEC_DELTA_648_NBPSC_1_STBC_RST     0x6
/// LDPCDEC_DELTA_648_NBPSC_0_STBC field reset value
#define MDM_LDPCDEC_DELTA_648_NBPSC_0_STBC_RST     0x6
/// LDPCDEC_DELTA_648_NBPSC_1 field reset value
#define MDM_LDPCDEC_DELTA_648_NBPSC_1_RST          0xF
/// LDPCDEC_DELTA_648_NBPSC_0 field reset value
#define MDM_LDPCDEC_DELTA_648_NBPSC_0_RST          0xE

/**
 * @brief Constructs a value for the BOUF_LDPC0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ldpcdecdelta1296nbpsc1stbc - The value to use for the ldpcdec_delta1296_nbpsc1_stbc field.
 * @param[in] ldpcdecdelta1296nbpsc0stbc - The value to use for the ldpcdec_delta1296_nbpsc0_stbc field.
 * @param[in] ldpcdecdelta1296nbpsc1 - The value to use for the ldpcdec_delta1296_nbpsc1 field.
 * @param[in] ldpcdecdelta1296nbpsc0 - The value to use for the ldpcdec_delta1296_nbpsc0 field.
 * @param[in] ldpcdecdelta648nbpsc1stbc - The value to use for the ldpcdec_delta648_nbpsc1_stbc field.
 * @param[in] ldpcdecdelta648nbpsc0stbc - The value to use for the ldpcdec_delta648_nbpsc0_stbc field.
 * @param[in] ldpcdecdelta648nbpsc1 - The value to use for the ldpcdec_delta648_nbpsc1 field.
 * @param[in] ldpcdecdelta648nbpsc0 - The value to use for the ldpcdec_delta648_nbpsc0 field.
 */
__INLINE void mdm_bouf_ldpc0_pack(uint8_t ldpcdecdelta1296nbpsc1stbc, uint8_t ldpcdecdelta1296nbpsc0stbc, uint8_t ldpcdecdelta1296nbpsc1, uint8_t ldpcdecdelta1296nbpsc0, uint8_t ldpcdecdelta648nbpsc1stbc, uint8_t ldpcdecdelta648nbpsc0stbc, uint8_t ldpcdecdelta648nbpsc1, uint8_t ldpcdecdelta648nbpsc0)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1296nbpsc1stbc << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1296nbpsc0stbc << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1296nbpsc1 << 20) & ~((uint32_t)0x00F00000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1296nbpsc0 << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta648nbpsc1stbc << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta648nbpsc0stbc << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta648nbpsc1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta648nbpsc0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC0_ADDR,  ((uint32_t)ldpcdecdelta1296nbpsc1stbc << 28) | ((uint32_t)ldpcdecdelta1296nbpsc0stbc << 24) | ((uint32_t)ldpcdecdelta1296nbpsc1 << 20) | ((uint32_t)ldpcdecdelta1296nbpsc0 << 16) | ((uint32_t)ldpcdecdelta648nbpsc1stbc << 12) | ((uint32_t)ldpcdecdelta648nbpsc0stbc << 8) | ((uint32_t)ldpcdecdelta648nbpsc1 << 4) | ((uint32_t)ldpcdecdelta648nbpsc0 << 0));
}

/**
 * @brief Unpacks BOUF_LDPC0's fields from current value of the BOUF_LDPC0 register.
 *
 * Reads the BOUF_LDPC0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ldpcdecdelta1296nbpsc1stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1296nbpsc0stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1296nbpsc1 - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1296nbpsc0 - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta648nbpsc1stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta648nbpsc0stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta648nbpsc1 - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta648nbpsc0 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_bouf_ldpc0_unpack(uint8_t* ldpcdecdelta1296nbpsc1stbc, uint8_t* ldpcdecdelta1296nbpsc0stbc, uint8_t* ldpcdecdelta1296nbpsc1, uint8_t* ldpcdecdelta1296nbpsc0, uint8_t* ldpcdecdelta648nbpsc1stbc, uint8_t* ldpcdecdelta648nbpsc0stbc, uint8_t* ldpcdecdelta648nbpsc1, uint8_t* ldpcdecdelta648nbpsc0)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC0_ADDR);

    *ldpcdecdelta1296nbpsc1stbc = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *ldpcdecdelta1296nbpsc0stbc = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *ldpcdecdelta1296nbpsc1 = (localVal & ((uint32_t)0x00F00000)) >> 20;
    *ldpcdecdelta1296nbpsc0 = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *ldpcdecdelta648nbpsc1stbc = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *ldpcdecdelta648nbpsc0stbc = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *ldpcdecdelta648nbpsc1 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *ldpcdecdelta648nbpsc0 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the ldpcdec_delta1296_nbpsc1_stbc field in the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read and the ldpcdec_delta1296_nbpsc1_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1296_nbpsc1_stbc field in the BOUF_LDPC0 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1296_nbpsc_1_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC0_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the ldpcdec_delta1296_nbpsc1_stbc field of the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1296nbpsc1stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1296_nbpsc_1_stbc_setf(uint8_t ldpcdecdelta1296nbpsc1stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1296nbpsc1stbc << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC0_ADDR, (REG_PL_RD(MDM_BOUF_LDPC0_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)ldpcdecdelta1296nbpsc1stbc << 28));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1296_nbpsc0_stbc field in the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read and the ldpcdec_delta1296_nbpsc0_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1296_nbpsc0_stbc field in the BOUF_LDPC0 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1296_nbpsc_0_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC0_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the ldpcdec_delta1296_nbpsc0_stbc field of the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1296nbpsc0stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1296_nbpsc_0_stbc_setf(uint8_t ldpcdecdelta1296nbpsc0stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1296nbpsc0stbc << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC0_ADDR, (REG_PL_RD(MDM_BOUF_LDPC0_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)ldpcdecdelta1296nbpsc0stbc << 24));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1296_nbpsc1 field in the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read and the ldpcdec_delta1296_nbpsc1 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1296_nbpsc1 field in the BOUF_LDPC0 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1296_nbpsc_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC0_ADDR);
    return ((localVal & ((uint32_t)0x00F00000)) >> 20);
}

/**
 * @brief Sets the ldpcdec_delta1296_nbpsc1 field of the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1296nbpsc1 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1296_nbpsc_1_setf(uint8_t ldpcdecdelta1296nbpsc1)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1296nbpsc1 << 20) & ~((uint32_t)0x00F00000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC0_ADDR, (REG_PL_RD(MDM_BOUF_LDPC0_ADDR) & ~((uint32_t)0x00F00000)) | ((uint32_t)ldpcdecdelta1296nbpsc1 << 20));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1296_nbpsc0 field in the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read and the ldpcdec_delta1296_nbpsc0 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1296_nbpsc0 field in the BOUF_LDPC0 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1296_nbpsc_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC0_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the ldpcdec_delta1296_nbpsc0 field of the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1296nbpsc0 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1296_nbpsc_0_setf(uint8_t ldpcdecdelta1296nbpsc0)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1296nbpsc0 << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC0_ADDR, (REG_PL_RD(MDM_BOUF_LDPC0_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)ldpcdecdelta1296nbpsc0 << 16));
}

/**
 * @brief Returns the current value of the ldpcdec_delta648_nbpsc1_stbc field in the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read and the ldpcdec_delta648_nbpsc1_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta648_nbpsc1_stbc field in the BOUF_LDPC0 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_648_nbpsc_1_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC0_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the ldpcdec_delta648_nbpsc1_stbc field of the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta648nbpsc1stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_648_nbpsc_1_stbc_setf(uint8_t ldpcdecdelta648nbpsc1stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta648nbpsc1stbc << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC0_ADDR, (REG_PL_RD(MDM_BOUF_LDPC0_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)ldpcdecdelta648nbpsc1stbc << 12));
}

/**
 * @brief Returns the current value of the ldpcdec_delta648_nbpsc0_stbc field in the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read and the ldpcdec_delta648_nbpsc0_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta648_nbpsc0_stbc field in the BOUF_LDPC0 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_648_nbpsc_0_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC0_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the ldpcdec_delta648_nbpsc0_stbc field of the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta648nbpsc0stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_648_nbpsc_0_stbc_setf(uint8_t ldpcdecdelta648nbpsc0stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta648nbpsc0stbc << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC0_ADDR, (REG_PL_RD(MDM_BOUF_LDPC0_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)ldpcdecdelta648nbpsc0stbc << 8));
}

/**
 * @brief Returns the current value of the ldpcdec_delta648_nbpsc1 field in the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read and the ldpcdec_delta648_nbpsc1 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta648_nbpsc1 field in the BOUF_LDPC0 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_648_nbpsc_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC0_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the ldpcdec_delta648_nbpsc1 field of the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta648nbpsc1 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_648_nbpsc_1_setf(uint8_t ldpcdecdelta648nbpsc1)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta648nbpsc1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC0_ADDR, (REG_PL_RD(MDM_BOUF_LDPC0_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)ldpcdecdelta648nbpsc1 << 4));
}

/**
 * @brief Returns the current value of the ldpcdec_delta648_nbpsc0 field in the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read and the ldpcdec_delta648_nbpsc0 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta648_nbpsc0 field in the BOUF_LDPC0 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_648_nbpsc_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC0_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the ldpcdec_delta648_nbpsc0 field of the BOUF_LDPC0 register.
 *
 * The BOUF_LDPC0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta648nbpsc0 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_648_nbpsc_0_setf(uint8_t ldpcdecdelta648nbpsc0)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta648nbpsc0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC0_ADDR, (REG_PL_RD(MDM_BOUF_LDPC0_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)ldpcdecdelta648nbpsc0 << 0));
}

#endif // RW_NX_LDPC_DEC
/// @}

#if RW_NX_LDPC_DEC
/**
 * @name BOUF_LDPC1 register definitions
 * <table>
 * <caption>BOUF_LDPC1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:28 <td>    ldpcdec_delta1944_ndpsc3_ncw1 <td> R <td> R/W <td> 0x5
 * <tr><td> 27:24 <td>    ldpcdec_delta1944_ndpsc3_ncw0 <td> R <td> R/W <td> 0x7
 * <tr><td> 23:20 <td>    ldpcdec_delta1944_ndpsc2_ncw3 <td> R <td> R/W <td> 0x8
 * <tr><td> 19:16 <td>    ldpcdec_delta1944_ndpsc2_ncw2 <td> R <td> R/W <td> 0x5
 * <tr><td> 15:12 <td>    ldpcdec_delta1944_ndpsc2_ncw1 <td> R <td> R/W <td> 0x6
 * <tr><td> 11:08 <td>    ldpcdec_delta1944_ndpsc2_ncw0 <td> R <td> R/W <td> 0x7
 * <tr><td> 07:04 <td>         ldpcdec_delta1944_nbpsc1 <td> R <td> R/W <td> 0x7
 * <tr><td> 03:00 <td>         ldpcdec_delta1944_nbpsc0 <td> R <td> R/W <td> 0xD
 * </table>
 *
 * @{
 */

/// Address of the BOUF_LDPC1 register
#define MDM_BOUF_LDPC1_ADDR   0x24C00904
/// Offset of the BOUF_LDPC1 register from the base address
#define MDM_BOUF_LDPC1_OFFSET 0x00000904
/// Index of the BOUF_LDPC1 register
#define MDM_BOUF_LDPC1_INDEX  0x00000241
/// Reset value of the BOUF_LDPC1 register
#define MDM_BOUF_LDPC1_RESET  0x5785677D

/**
 * @brief Returns the current value of the BOUF_LDPC1 register.
 * The BOUF_LDPC1 register will be read and its value returned.
 * @return The current value of the BOUF_LDPC1 register.
 */
__INLINE uint32_t mdm_bouf_ldpc1_get(void)
{
    return REG_PL_RD(MDM_BOUF_LDPC1_ADDR);
}

/**
 * @brief Sets the BOUF_LDPC1 register to a value.
 * The BOUF_LDPC1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_bouf_ldpc1_set(uint32_t value)
{
    REG_PL_WR(MDM_BOUF_LDPC1_ADDR, value);
}

// field definitions
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_1 field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_MASK   ((uint32_t)0xF0000000)
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_1 field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_LSB    28
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_1 field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_0 field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_MASK   ((uint32_t)0x0F000000)
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_0 field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_LSB    24
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_0 field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_3 field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_MASK   ((uint32_t)0x00F00000)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_3 field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_LSB    20
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_3 field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_2 field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_MASK   ((uint32_t)0x000F0000)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_2 field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_LSB    16
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_2 field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_1 field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_MASK   ((uint32_t)0x0000F000)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_1 field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_LSB    12
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_1 field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_0 field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_MASK   ((uint32_t)0x00000F00)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_0 field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_LSB    8
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_0 field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NBPSC_1 field mask
#define MDM_LDPCDEC_DELTA_1944_NBPSC_1_MASK         ((uint32_t)0x000000F0)
/// LDPCDEC_DELTA_1944_NBPSC_1 field LSB position
#define MDM_LDPCDEC_DELTA_1944_NBPSC_1_LSB          4
/// LDPCDEC_DELTA_1944_NBPSC_1 field width
#define MDM_LDPCDEC_DELTA_1944_NBPSC_1_WIDTH        ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NBPSC_0 field mask
#define MDM_LDPCDEC_DELTA_1944_NBPSC_0_MASK         ((uint32_t)0x0000000F)
/// LDPCDEC_DELTA_1944_NBPSC_0 field LSB position
#define MDM_LDPCDEC_DELTA_1944_NBPSC_0_LSB          0
/// LDPCDEC_DELTA_1944_NBPSC_0 field width
#define MDM_LDPCDEC_DELTA_1944_NBPSC_0_WIDTH        ((uint32_t)0x00000004)

/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_1 field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_RST    0x5
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_0 field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_RST    0x7
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_3 field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_RST    0x8
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_2 field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_RST    0x5
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_1 field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_RST    0x6
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_0 field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_RST    0x7
/// LDPCDEC_DELTA_1944_NBPSC_1 field reset value
#define MDM_LDPCDEC_DELTA_1944_NBPSC_1_RST          0x7
/// LDPCDEC_DELTA_1944_NBPSC_0 field reset value
#define MDM_LDPCDEC_DELTA_1944_NBPSC_0_RST          0xD

/**
 * @brief Constructs a value for the BOUF_LDPC1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ldpcdecdelta1944ndpsc3ncw1 - The value to use for the ldpcdec_delta1944_ndpsc3_ncw1 field.
 * @param[in] ldpcdecdelta1944ndpsc3ncw0 - The value to use for the ldpcdec_delta1944_ndpsc3_ncw0 field.
 * @param[in] ldpcdecdelta1944ndpsc2ncw3 - The value to use for the ldpcdec_delta1944_ndpsc2_ncw3 field.
 * @param[in] ldpcdecdelta1944ndpsc2ncw2 - The value to use for the ldpcdec_delta1944_ndpsc2_ncw2 field.
 * @param[in] ldpcdecdelta1944ndpsc2ncw1 - The value to use for the ldpcdec_delta1944_ndpsc2_ncw1 field.
 * @param[in] ldpcdecdelta1944ndpsc2ncw0 - The value to use for the ldpcdec_delta1944_ndpsc2_ncw0 field.
 * @param[in] ldpcdecdelta1944nbpsc1 - The value to use for the ldpcdec_delta1944_nbpsc1 field.
 * @param[in] ldpcdecdelta1944nbpsc0 - The value to use for the ldpcdec_delta1944_nbpsc0 field.
 */
__INLINE void mdm_bouf_ldpc1_pack(uint8_t ldpcdecdelta1944ndpsc3ncw1, uint8_t ldpcdecdelta1944ndpsc3ncw0, uint8_t ldpcdecdelta1944ndpsc2ncw3, uint8_t ldpcdecdelta1944ndpsc2ncw2, uint8_t ldpcdecdelta1944ndpsc2ncw1, uint8_t ldpcdecdelta1944ndpsc2ncw0, uint8_t ldpcdecdelta1944nbpsc1, uint8_t ldpcdecdelta1944nbpsc0)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw1 << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw0 << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw3 << 20) & ~((uint32_t)0x00F00000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw2 << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw1 << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw0 << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944nbpsc1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944nbpsc0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC1_ADDR,  ((uint32_t)ldpcdecdelta1944ndpsc3ncw1 << 28) | ((uint32_t)ldpcdecdelta1944ndpsc3ncw0 << 24) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw3 << 20) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw2 << 16) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw1 << 12) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw0 << 8) | ((uint32_t)ldpcdecdelta1944nbpsc1 << 4) | ((uint32_t)ldpcdecdelta1944nbpsc0 << 0));
}

/**
 * @brief Unpacks BOUF_LDPC1's fields from current value of the BOUF_LDPC1 register.
 *
 * Reads the BOUF_LDPC1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ldpcdecdelta1944ndpsc3ncw1 - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc3ncw0 - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc2ncw3 - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc2ncw2 - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc2ncw1 - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc2ncw0 - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944nbpsc1 - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944nbpsc0 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_bouf_ldpc1_unpack(uint8_t* ldpcdecdelta1944ndpsc3ncw1, uint8_t* ldpcdecdelta1944ndpsc3ncw0, uint8_t* ldpcdecdelta1944ndpsc2ncw3, uint8_t* ldpcdecdelta1944ndpsc2ncw2, uint8_t* ldpcdecdelta1944ndpsc2ncw1, uint8_t* ldpcdecdelta1944ndpsc2ncw0, uint8_t* ldpcdecdelta1944nbpsc1, uint8_t* ldpcdecdelta1944nbpsc0)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC1_ADDR);

    *ldpcdecdelta1944ndpsc3ncw1 = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *ldpcdecdelta1944ndpsc3ncw0 = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *ldpcdecdelta1944ndpsc2ncw3 = (localVal & ((uint32_t)0x00F00000)) >> 20;
    *ldpcdecdelta1944ndpsc2ncw2 = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *ldpcdecdelta1944ndpsc2ncw1 = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *ldpcdecdelta1944ndpsc2ncw0 = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *ldpcdecdelta1944nbpsc1 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *ldpcdecdelta1944nbpsc0 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc3_ncw1 field in the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read and the ldpcdec_delta1944_ndpsc3_ncw1 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc3_ncw1 field in the BOUF_LDPC1 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_3_ncw_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC1_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc3_ncw1 field of the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc3ncw1 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_3_ncw_1_setf(uint8_t ldpcdecdelta1944ndpsc3ncw1)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw1 << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC1_ADDR, (REG_PL_RD(MDM_BOUF_LDPC1_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)ldpcdecdelta1944ndpsc3ncw1 << 28));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc3_ncw0 field in the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read and the ldpcdec_delta1944_ndpsc3_ncw0 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc3_ncw0 field in the BOUF_LDPC1 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_3_ncw_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC1_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc3_ncw0 field of the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc3ncw0 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_3_ncw_0_setf(uint8_t ldpcdecdelta1944ndpsc3ncw0)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw0 << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC1_ADDR, (REG_PL_RD(MDM_BOUF_LDPC1_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)ldpcdecdelta1944ndpsc3ncw0 << 24));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc2_ncw3 field in the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read and the ldpcdec_delta1944_ndpsc2_ncw3 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc2_ncw3 field in the BOUF_LDPC1 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_2_ncw_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC1_ADDR);
    return ((localVal & ((uint32_t)0x00F00000)) >> 20);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc2_ncw3 field of the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc2ncw3 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_2_ncw_3_setf(uint8_t ldpcdecdelta1944ndpsc2ncw3)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw3 << 20) & ~((uint32_t)0x00F00000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC1_ADDR, (REG_PL_RD(MDM_BOUF_LDPC1_ADDR) & ~((uint32_t)0x00F00000)) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw3 << 20));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc2_ncw2 field in the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read and the ldpcdec_delta1944_ndpsc2_ncw2 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc2_ncw2 field in the BOUF_LDPC1 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_2_ncw_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC1_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc2_ncw2 field of the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc2ncw2 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_2_ncw_2_setf(uint8_t ldpcdecdelta1944ndpsc2ncw2)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw2 << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC1_ADDR, (REG_PL_RD(MDM_BOUF_LDPC1_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw2 << 16));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc2_ncw1 field in the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read and the ldpcdec_delta1944_ndpsc2_ncw1 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc2_ncw1 field in the BOUF_LDPC1 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_2_ncw_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC1_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc2_ncw1 field of the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc2ncw1 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_2_ncw_1_setf(uint8_t ldpcdecdelta1944ndpsc2ncw1)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw1 << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC1_ADDR, (REG_PL_RD(MDM_BOUF_LDPC1_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw1 << 12));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc2_ncw0 field in the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read and the ldpcdec_delta1944_ndpsc2_ncw0 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc2_ncw0 field in the BOUF_LDPC1 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_2_ncw_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC1_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc2_ncw0 field of the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc2ncw0 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_2_ncw_0_setf(uint8_t ldpcdecdelta1944ndpsc2ncw0)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw0 << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC1_ADDR, (REG_PL_RD(MDM_BOUF_LDPC1_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw0 << 8));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_nbpsc1 field in the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read and the ldpcdec_delta1944_nbpsc1 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_nbpsc1 field in the BOUF_LDPC1 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_nbpsc_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC1_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the ldpcdec_delta1944_nbpsc1 field of the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944nbpsc1 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_nbpsc_1_setf(uint8_t ldpcdecdelta1944nbpsc1)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944nbpsc1 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC1_ADDR, (REG_PL_RD(MDM_BOUF_LDPC1_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)ldpcdecdelta1944nbpsc1 << 4));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_nbpsc0 field in the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read and the ldpcdec_delta1944_nbpsc0 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_nbpsc0 field in the BOUF_LDPC1 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_nbpsc_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC1_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the ldpcdec_delta1944_nbpsc0 field of the BOUF_LDPC1 register.
 *
 * The BOUF_LDPC1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944nbpsc0 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_nbpsc_0_setf(uint8_t ldpcdecdelta1944nbpsc0)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944nbpsc0 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC1_ADDR, (REG_PL_RD(MDM_BOUF_LDPC1_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)ldpcdecdelta1944nbpsc0 << 0));
}

#endif // RW_NX_LDPC_DEC
/// @}

#if RW_NX_LDPC_DEC
/**
 * @name BOUF_LDPC2 register definitions
 * <table>
 * <caption>BOUF_LDPC2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:28 <td>    ldpcdec_delta1944_ndpsc2_ncw3_stbc <td> R <td> R/W <td> 0x3
 * <tr><td> 27:24 <td>    ldpcdec_delta1944_ndpsc2_ncw2_stbc <td> R <td> R/W <td> 0x3
 * <tr><td> 23:20 <td>    ldpcdec_delta1944_ndpsc2_ncw1_stbc <td> R <td> R/W <td> 0x4
 * <tr><td> 19:16 <td>    ldpcdec_delta1944_ndpsc2_ncw0_stbc <td> R <td> R/W <td> 0x4
 * <tr><td> 15:12 <td>         ldpcdec_delta1944_ndpsc1_stbc <td> R <td> R/W <td> 0x3
 * <tr><td> 11:08 <td>         ldpcdec_delta1944_ndpsc0_stbc <td> R <td> R/W <td> 0x4
 * <tr><td> 07:04 <td>         ldpcdec_delta1944_nbpsc3_ncw3 <td> R <td> R/W <td> 0x3
 * <tr><td> 03:00 <td>         ldpcdec_delta1944_nbpsc3_ncw2 <td> R <td> R/W <td> 0x4
 * </table>
 *
 * @{
 */

/// Address of the BOUF_LDPC2 register
#define MDM_BOUF_LDPC2_ADDR   0x24C00908
/// Offset of the BOUF_LDPC2 register from the base address
#define MDM_BOUF_LDPC2_OFFSET 0x00000908
/// Index of the BOUF_LDPC2 register
#define MDM_BOUF_LDPC2_INDEX  0x00000242
/// Reset value of the BOUF_LDPC2 register
#define MDM_BOUF_LDPC2_RESET  0x33443434

/**
 * @brief Returns the current value of the BOUF_LDPC2 register.
 * The BOUF_LDPC2 register will be read and its value returned.
 * @return The current value of the BOUF_LDPC2 register.
 */
__INLINE uint32_t mdm_bouf_ldpc2_get(void)
{
    return REG_PL_RD(MDM_BOUF_LDPC2_ADDR);
}

/**
 * @brief Sets the BOUF_LDPC2 register to a value.
 * The BOUF_LDPC2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_bouf_ldpc2_set(uint32_t value)
{
    REG_PL_WR(MDM_BOUF_LDPC2_ADDR, value);
}

// field definitions
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_STBC field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_STBC_MASK   ((uint32_t)0xF0000000)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_STBC_LSB    28
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_STBC field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_STBC_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_STBC field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_STBC_MASK   ((uint32_t)0x0F000000)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_STBC_LSB    24
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_STBC field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_STBC_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_STBC field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_STBC_MASK   ((uint32_t)0x00F00000)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_STBC_LSB    20
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_STBC field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_STBC_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_STBC field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_STBC_MASK   ((uint32_t)0x000F0000)
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_STBC_LSB    16
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_STBC field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_STBC_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_1_STBC field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_1_STBC_MASK         ((uint32_t)0x0000F000)
/// LDPCDEC_DELTA_1944_NDPSC_1_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_1_STBC_LSB          12
/// LDPCDEC_DELTA_1944_NDPSC_1_STBC field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_1_STBC_WIDTH        ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_0_STBC field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_0_STBC_MASK         ((uint32_t)0x00000F00)
/// LDPCDEC_DELTA_1944_NDPSC_0_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_0_STBC_LSB          8
/// LDPCDEC_DELTA_1944_NDPSC_0_STBC field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_0_STBC_WIDTH        ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NBPSC_3_NCW_3 field mask
#define MDM_LDPCDEC_DELTA_1944_NBPSC_3_NCW_3_MASK        ((uint32_t)0x000000F0)
/// LDPCDEC_DELTA_1944_NBPSC_3_NCW_3 field LSB position
#define MDM_LDPCDEC_DELTA_1944_NBPSC_3_NCW_3_LSB         4
/// LDPCDEC_DELTA_1944_NBPSC_3_NCW_3 field width
#define MDM_LDPCDEC_DELTA_1944_NBPSC_3_NCW_3_WIDTH       ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NBPSC_3_NCW_2 field mask
#define MDM_LDPCDEC_DELTA_1944_NBPSC_3_NCW_2_MASK        ((uint32_t)0x0000000F)
/// LDPCDEC_DELTA_1944_NBPSC_3_NCW_2 field LSB position
#define MDM_LDPCDEC_DELTA_1944_NBPSC_3_NCW_2_LSB         0
/// LDPCDEC_DELTA_1944_NBPSC_3_NCW_2 field width
#define MDM_LDPCDEC_DELTA_1944_NBPSC_3_NCW_2_WIDTH       ((uint32_t)0x00000004)

/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_STBC field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_3_STBC_RST    0x3
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_STBC field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_2_STBC_RST    0x3
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_STBC field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_1_STBC_RST    0x4
/// LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_STBC field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_2_NCW_0_STBC_RST    0x4
/// LDPCDEC_DELTA_1944_NDPSC_1_STBC field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_1_STBC_RST          0x3
/// LDPCDEC_DELTA_1944_NDPSC_0_STBC field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_0_STBC_RST          0x4
/// LDPCDEC_DELTA_1944_NBPSC_3_NCW_3 field reset value
#define MDM_LDPCDEC_DELTA_1944_NBPSC_3_NCW_3_RST         0x3
/// LDPCDEC_DELTA_1944_NBPSC_3_NCW_2 field reset value
#define MDM_LDPCDEC_DELTA_1944_NBPSC_3_NCW_2_RST         0x4

/**
 * @brief Constructs a value for the BOUF_LDPC2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ldpcdecdelta1944ndpsc2ncw3stbc - The value to use for the ldpcdec_delta1944_ndpsc2_ncw3_stbc field.
 * @param[in] ldpcdecdelta1944ndpsc2ncw2stbc - The value to use for the ldpcdec_delta1944_ndpsc2_ncw2_stbc field.
 * @param[in] ldpcdecdelta1944ndpsc2ncw1stbc - The value to use for the ldpcdec_delta1944_ndpsc2_ncw1_stbc field.
 * @param[in] ldpcdecdelta1944ndpsc2ncw0stbc - The value to use for the ldpcdec_delta1944_ndpsc2_ncw0_stbc field.
 * @param[in] ldpcdecdelta1944ndpsc1stbc - The value to use for the ldpcdec_delta1944_ndpsc1_stbc field.
 * @param[in] ldpcdecdelta1944ndpsc0stbc - The value to use for the ldpcdec_delta1944_ndpsc0_stbc field.
 * @param[in] ldpcdecdelta1944nbpsc3ncw3 - The value to use for the ldpcdec_delta1944_nbpsc3_ncw3 field.
 * @param[in] ldpcdecdelta1944nbpsc3ncw2 - The value to use for the ldpcdec_delta1944_nbpsc3_ncw2 field.
 */
__INLINE void mdm_bouf_ldpc2_pack(uint8_t ldpcdecdelta1944ndpsc2ncw3stbc, uint8_t ldpcdecdelta1944ndpsc2ncw2stbc, uint8_t ldpcdecdelta1944ndpsc2ncw1stbc, uint8_t ldpcdecdelta1944ndpsc2ncw0stbc, uint8_t ldpcdecdelta1944ndpsc1stbc, uint8_t ldpcdecdelta1944ndpsc0stbc, uint8_t ldpcdecdelta1944nbpsc3ncw3, uint8_t ldpcdecdelta1944nbpsc3ncw2)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw3stbc << 28) & ~((uint32_t)0xF0000000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw2stbc << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw1stbc << 20) & ~((uint32_t)0x00F00000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw0stbc << 16) & ~((uint32_t)0x000F0000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc1stbc << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc0stbc << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944nbpsc3ncw3 << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944nbpsc3ncw2 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC2_ADDR,  ((uint32_t)ldpcdecdelta1944ndpsc2ncw3stbc << 28) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw2stbc << 24) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw1stbc << 20) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw0stbc << 16) | ((uint32_t)ldpcdecdelta1944ndpsc1stbc << 12) | ((uint32_t)ldpcdecdelta1944ndpsc0stbc << 8) | ((uint32_t)ldpcdecdelta1944nbpsc3ncw3 << 4) | ((uint32_t)ldpcdecdelta1944nbpsc3ncw2 << 0));
}

/**
 * @brief Unpacks BOUF_LDPC2's fields from current value of the BOUF_LDPC2 register.
 *
 * Reads the BOUF_LDPC2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ldpcdecdelta1944ndpsc2ncw3stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc2ncw2stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc2ncw1stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc2ncw0stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc1stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc0stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944nbpsc3ncw3 - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944nbpsc3ncw2 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_bouf_ldpc2_unpack(uint8_t* ldpcdecdelta1944ndpsc2ncw3stbc, uint8_t* ldpcdecdelta1944ndpsc2ncw2stbc, uint8_t* ldpcdecdelta1944ndpsc2ncw1stbc, uint8_t* ldpcdecdelta1944ndpsc2ncw0stbc, uint8_t* ldpcdecdelta1944ndpsc1stbc, uint8_t* ldpcdecdelta1944ndpsc0stbc, uint8_t* ldpcdecdelta1944nbpsc3ncw3, uint8_t* ldpcdecdelta1944nbpsc3ncw2)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC2_ADDR);

    *ldpcdecdelta1944ndpsc2ncw3stbc = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *ldpcdecdelta1944ndpsc2ncw2stbc = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *ldpcdecdelta1944ndpsc2ncw1stbc = (localVal & ((uint32_t)0x00F00000)) >> 20;
    *ldpcdecdelta1944ndpsc2ncw0stbc = (localVal & ((uint32_t)0x000F0000)) >> 16;
    *ldpcdecdelta1944ndpsc1stbc = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *ldpcdecdelta1944ndpsc0stbc = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *ldpcdecdelta1944nbpsc3ncw3 = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *ldpcdecdelta1944nbpsc3ncw2 = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc2_ncw3_stbc field in the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read and the ldpcdec_delta1944_ndpsc2_ncw3_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc2_ncw3_stbc field in the BOUF_LDPC2 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_2_ncw_3_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC2_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc2_ncw3_stbc field of the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc2ncw3stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_2_ncw_3_stbc_setf(uint8_t ldpcdecdelta1944ndpsc2ncw3stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw3stbc << 28) & ~((uint32_t)0xF0000000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC2_ADDR, (REG_PL_RD(MDM_BOUF_LDPC2_ADDR) & ~((uint32_t)0xF0000000)) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw3stbc << 28));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc2_ncw2_stbc field in the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read and the ldpcdec_delta1944_ndpsc2_ncw2_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc2_ncw2_stbc field in the BOUF_LDPC2 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_2_ncw_2_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC2_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc2_ncw2_stbc field of the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc2ncw2stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_2_ncw_2_stbc_setf(uint8_t ldpcdecdelta1944ndpsc2ncw2stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw2stbc << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC2_ADDR, (REG_PL_RD(MDM_BOUF_LDPC2_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw2stbc << 24));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc2_ncw1_stbc field in the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read and the ldpcdec_delta1944_ndpsc2_ncw1_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc2_ncw1_stbc field in the BOUF_LDPC2 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_2_ncw_1_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC2_ADDR);
    return ((localVal & ((uint32_t)0x00F00000)) >> 20);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc2_ncw1_stbc field of the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc2ncw1stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_2_ncw_1_stbc_setf(uint8_t ldpcdecdelta1944ndpsc2ncw1stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw1stbc << 20) & ~((uint32_t)0x00F00000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC2_ADDR, (REG_PL_RD(MDM_BOUF_LDPC2_ADDR) & ~((uint32_t)0x00F00000)) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw1stbc << 20));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc2_ncw0_stbc field in the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read and the ldpcdec_delta1944_ndpsc2_ncw0_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc2_ncw0_stbc field in the BOUF_LDPC2 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_2_ncw_0_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC2_ADDR);
    return ((localVal & ((uint32_t)0x000F0000)) >> 16);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc2_ncw0_stbc field of the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc2ncw0stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_2_ncw_0_stbc_setf(uint8_t ldpcdecdelta1944ndpsc2ncw0stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc2ncw0stbc << 16) & ~((uint32_t)0x000F0000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC2_ADDR, (REG_PL_RD(MDM_BOUF_LDPC2_ADDR) & ~((uint32_t)0x000F0000)) | ((uint32_t)ldpcdecdelta1944ndpsc2ncw0stbc << 16));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc1_stbc field in the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read and the ldpcdec_delta1944_ndpsc1_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc1_stbc field in the BOUF_LDPC2 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_1_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC2_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc1_stbc field of the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc1stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_1_stbc_setf(uint8_t ldpcdecdelta1944ndpsc1stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc1stbc << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC2_ADDR, (REG_PL_RD(MDM_BOUF_LDPC2_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)ldpcdecdelta1944ndpsc1stbc << 12));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc0_stbc field in the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read and the ldpcdec_delta1944_ndpsc0_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc0_stbc field in the BOUF_LDPC2 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_0_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC2_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc0_stbc field of the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc0stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_0_stbc_setf(uint8_t ldpcdecdelta1944ndpsc0stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc0stbc << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC2_ADDR, (REG_PL_RD(MDM_BOUF_LDPC2_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)ldpcdecdelta1944ndpsc0stbc << 8));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_nbpsc3_ncw3 field in the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read and the ldpcdec_delta1944_nbpsc3_ncw3 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_nbpsc3_ncw3 field in the BOUF_LDPC2 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_nbpsc_3_ncw_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC2_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the ldpcdec_delta1944_nbpsc3_ncw3 field of the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944nbpsc3ncw3 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_nbpsc_3_ncw_3_setf(uint8_t ldpcdecdelta1944nbpsc3ncw3)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944nbpsc3ncw3 << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC2_ADDR, (REG_PL_RD(MDM_BOUF_LDPC2_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)ldpcdecdelta1944nbpsc3ncw3 << 4));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_nbpsc3_ncw2 field in the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read and the ldpcdec_delta1944_nbpsc3_ncw2 field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_nbpsc3_ncw2 field in the BOUF_LDPC2 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_nbpsc_3_ncw_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC2_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the ldpcdec_delta1944_nbpsc3_ncw2 field of the BOUF_LDPC2 register.
 *
 * The BOUF_LDPC2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944nbpsc3ncw2 - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_nbpsc_3_ncw_2_setf(uint8_t ldpcdecdelta1944nbpsc3ncw2)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944nbpsc3ncw2 << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC2_ADDR, (REG_PL_RD(MDM_BOUF_LDPC2_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)ldpcdecdelta1944nbpsc3ncw2 << 0));
}

#endif // RW_NX_LDPC_DEC
/// @}

#if RW_NX_LDPC_DEC
/**
 * @name BOUF_LDPC3 register definitions
 * <table>
 * <caption>BOUF_LDPC3 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 30:26 <td>                  ldpcdec_deltanom_ite <td> R <td> R/W <td> 0x1E
 * <tr><td> 25:21 <td>                ldpcdec_deltamaxruncnt <td> R <td> R/W <td> 0x2
 * <tr><td> 20:16 <td>                ldpcdec_deltaendruncnt <td> R <td> R/W <td> 0x16
 * <tr><td> 15:12 <td>    ldpcdec_delta1944_ndpsc3_ncw3_stbc <td> R <td> R/W <td> 0x7
 * <tr><td> 11:08 <td>    ldpcdec_delta1944_ndpsc3_ncw2_stbc <td> R <td> R/W <td> 0x7
 * <tr><td> 07:04 <td>    ldpcdec_delta1944_ndpsc3_ncw1_stbc <td> R <td> R/W <td> 0x7
 * <tr><td> 03:00 <td>    ldpcdec_delta1944_ndpsc3_ncw0_stbc <td> R <td> R/W <td> 0xD
 * </table>
 *
 * @{
 */

/// Address of the BOUF_LDPC3 register
#define MDM_BOUF_LDPC3_ADDR   0x24C0090C
/// Offset of the BOUF_LDPC3 register from the base address
#define MDM_BOUF_LDPC3_OFFSET 0x0000090C
/// Index of the BOUF_LDPC3 register
#define MDM_BOUF_LDPC3_INDEX  0x00000243
/// Reset value of the BOUF_LDPC3 register
#define MDM_BOUF_LDPC3_RESET  0x7856777D

/**
 * @brief Returns the current value of the BOUF_LDPC3 register.
 * The BOUF_LDPC3 register will be read and its value returned.
 * @return The current value of the BOUF_LDPC3 register.
 */
__INLINE uint32_t mdm_bouf_ldpc3_get(void)
{
    return REG_PL_RD(MDM_BOUF_LDPC3_ADDR);
}

/**
 * @brief Sets the BOUF_LDPC3 register to a value.
 * The BOUF_LDPC3 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_bouf_ldpc3_set(uint32_t value)
{
    REG_PL_WR(MDM_BOUF_LDPC3_ADDR, value);
}

// field definitions
/// LDPCDEC_DELTANOM_ITE field mask
#define MDM_LDPCDEC_DELTANOM_ITE_MASK                    ((uint32_t)0x7C000000)
/// LDPCDEC_DELTANOM_ITE field LSB position
#define MDM_LDPCDEC_DELTANOM_ITE_LSB                     26
/// LDPCDEC_DELTANOM_ITE field width
#define MDM_LDPCDEC_DELTANOM_ITE_WIDTH                   ((uint32_t)0x00000005)
/// LDPCDEC_DELTAMAXRUNCNT field mask
#define MDM_LDPCDEC_DELTAMAXRUNCNT_MASK                  ((uint32_t)0x03E00000)
/// LDPCDEC_DELTAMAXRUNCNT field LSB position
#define MDM_LDPCDEC_DELTAMAXRUNCNT_LSB                   21
/// LDPCDEC_DELTAMAXRUNCNT field width
#define MDM_LDPCDEC_DELTAMAXRUNCNT_WIDTH                 ((uint32_t)0x00000005)
/// LDPCDEC_DELTAENDRUNCNT field mask
#define MDM_LDPCDEC_DELTAENDRUNCNT_MASK                  ((uint32_t)0x001F0000)
/// LDPCDEC_DELTAENDRUNCNT field LSB position
#define MDM_LDPCDEC_DELTAENDRUNCNT_LSB                   16
/// LDPCDEC_DELTAENDRUNCNT field width
#define MDM_LDPCDEC_DELTAENDRUNCNT_WIDTH                 ((uint32_t)0x00000005)
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_3_STBC field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_3_STBC_MASK   ((uint32_t)0x0000F000)
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_3_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_3_STBC_LSB    12
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_3_STBC field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_3_STBC_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_2_STBC field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_2_STBC_MASK   ((uint32_t)0x00000F00)
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_2_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_2_STBC_LSB    8
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_2_STBC field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_2_STBC_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_STBC field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_STBC_MASK   ((uint32_t)0x000000F0)
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_STBC_LSB    4
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_STBC field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_STBC_WIDTH  ((uint32_t)0x00000004)
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_STBC field mask
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_STBC_MASK   ((uint32_t)0x0000000F)
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_STBC field LSB position
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_STBC_LSB    0
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_STBC field width
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_STBC_WIDTH  ((uint32_t)0x00000004)

/// LDPCDEC_DELTANOM_ITE field reset value
#define MDM_LDPCDEC_DELTANOM_ITE_RST                     0x1E
/// LDPCDEC_DELTAMAXRUNCNT field reset value
#define MDM_LDPCDEC_DELTAMAXRUNCNT_RST                   0x2
/// LDPCDEC_DELTAENDRUNCNT field reset value
#define MDM_LDPCDEC_DELTAENDRUNCNT_RST                   0x16
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_3_STBC field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_3_STBC_RST    0x7
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_2_STBC field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_2_STBC_RST    0x7
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_STBC field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_1_STBC_RST    0x7
/// LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_STBC field reset value
#define MDM_LDPCDEC_DELTA_1944_NDPSC_3_NCW_0_STBC_RST    0xD

/**
 * @brief Constructs a value for the BOUF_LDPC3 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] ldpcdecdeltanomite - The value to use for the ldpcdec_deltanom_ite field.
 * @param[in] ldpcdecdeltamaxruncnt - The value to use for the ldpcdec_deltamaxruncnt field.
 * @param[in] ldpcdecdeltaendruncnt - The value to use for the ldpcdec_deltaendruncnt field.
 * @param[in] ldpcdecdelta1944ndpsc3ncw3stbc - The value to use for the ldpcdec_delta1944_ndpsc3_ncw3_stbc field.
 * @param[in] ldpcdecdelta1944ndpsc3ncw2stbc - The value to use for the ldpcdec_delta1944_ndpsc3_ncw2_stbc field.
 * @param[in] ldpcdecdelta1944ndpsc3ncw1stbc - The value to use for the ldpcdec_delta1944_ndpsc3_ncw1_stbc field.
 * @param[in] ldpcdecdelta1944ndpsc3ncw0stbc - The value to use for the ldpcdec_delta1944_ndpsc3_ncw0_stbc field.
 */
__INLINE void mdm_bouf_ldpc3_pack(uint8_t ldpcdecdeltanomite, uint8_t ldpcdecdeltamaxruncnt, uint8_t ldpcdecdeltaendruncnt, uint8_t ldpcdecdelta1944ndpsc3ncw3stbc, uint8_t ldpcdecdelta1944ndpsc3ncw2stbc, uint8_t ldpcdecdelta1944ndpsc3ncw1stbc, uint8_t ldpcdecdelta1944ndpsc3ncw0stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdeltanomite << 26) & ~((uint32_t)0x7C000000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdeltamaxruncnt << 21) & ~((uint32_t)0x03E00000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdeltaendruncnt << 16) & ~((uint32_t)0x001F0000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw3stbc << 12) & ~((uint32_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw2stbc << 8) & ~((uint32_t)0x00000F00)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw1stbc << 4) & ~((uint32_t)0x000000F0)) == 0);
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw0stbc << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC3_ADDR,  ((uint32_t)ldpcdecdeltanomite << 26) | ((uint32_t)ldpcdecdeltamaxruncnt << 21) | ((uint32_t)ldpcdecdeltaendruncnt << 16) | ((uint32_t)ldpcdecdelta1944ndpsc3ncw3stbc << 12) | ((uint32_t)ldpcdecdelta1944ndpsc3ncw2stbc << 8) | ((uint32_t)ldpcdecdelta1944ndpsc3ncw1stbc << 4) | ((uint32_t)ldpcdecdelta1944ndpsc3ncw0stbc << 0));
}

/**
 * @brief Unpacks BOUF_LDPC3's fields from current value of the BOUF_LDPC3 register.
 *
 * Reads the BOUF_LDPC3 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] ldpcdecdeltanomite - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdeltamaxruncnt - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdeltaendruncnt - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc3ncw3stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc3ncw2stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc3ncw1stbc - Will be populated with the current value of this field from the register.
 * @param[out] ldpcdecdelta1944ndpsc3ncw0stbc - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_bouf_ldpc3_unpack(uint8_t* ldpcdecdeltanomite, uint8_t* ldpcdecdeltamaxruncnt, uint8_t* ldpcdecdeltaendruncnt, uint8_t* ldpcdecdelta1944ndpsc3ncw3stbc, uint8_t* ldpcdecdelta1944ndpsc3ncw2stbc, uint8_t* ldpcdecdelta1944ndpsc3ncw1stbc, uint8_t* ldpcdecdelta1944ndpsc3ncw0stbc)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC3_ADDR);

    *ldpcdecdeltanomite = (localVal & ((uint32_t)0x7C000000)) >> 26;
    *ldpcdecdeltamaxruncnt = (localVal & ((uint32_t)0x03E00000)) >> 21;
    *ldpcdecdeltaendruncnt = (localVal & ((uint32_t)0x001F0000)) >> 16;
    *ldpcdecdelta1944ndpsc3ncw3stbc = (localVal & ((uint32_t)0x0000F000)) >> 12;
    *ldpcdecdelta1944ndpsc3ncw2stbc = (localVal & ((uint32_t)0x00000F00)) >> 8;
    *ldpcdecdelta1944ndpsc3ncw1stbc = (localVal & ((uint32_t)0x000000F0)) >> 4;
    *ldpcdecdelta1944ndpsc3ncw0stbc = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

/**
 * @brief Returns the current value of the ldpcdec_deltanom_ite field in the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read and the ldpcdec_deltanom_ite field's value will be returned.
 *
 * @return The current value of the ldpcdec_deltanom_ite field in the BOUF_LDPC3 register.
 */
__INLINE uint8_t mdm_ldpcdec_deltanom_ite_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC3_ADDR);
    return ((localVal & ((uint32_t)0x7C000000)) >> 26);
}

/**
 * @brief Sets the ldpcdec_deltanom_ite field of the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdeltanomite - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_deltanom_ite_setf(uint8_t ldpcdecdeltanomite)
{
    ASSERT_ERR((((uint32_t)ldpcdecdeltanomite << 26) & ~((uint32_t)0x7C000000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC3_ADDR, (REG_PL_RD(MDM_BOUF_LDPC3_ADDR) & ~((uint32_t)0x7C000000)) | ((uint32_t)ldpcdecdeltanomite << 26));
}

/**
 * @brief Returns the current value of the ldpcdec_deltamaxruncnt field in the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read and the ldpcdec_deltamaxruncnt field's value will be returned.
 *
 * @return The current value of the ldpcdec_deltamaxruncnt field in the BOUF_LDPC3 register.
 */
__INLINE uint8_t mdm_ldpcdec_deltamaxruncnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC3_ADDR);
    return ((localVal & ((uint32_t)0x03E00000)) >> 21);
}

/**
 * @brief Sets the ldpcdec_deltamaxruncnt field of the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdeltamaxruncnt - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_deltamaxruncnt_setf(uint8_t ldpcdecdeltamaxruncnt)
{
    ASSERT_ERR((((uint32_t)ldpcdecdeltamaxruncnt << 21) & ~((uint32_t)0x03E00000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC3_ADDR, (REG_PL_RD(MDM_BOUF_LDPC3_ADDR) & ~((uint32_t)0x03E00000)) | ((uint32_t)ldpcdecdeltamaxruncnt << 21));
}

/**
 * @brief Returns the current value of the ldpcdec_deltaendruncnt field in the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read and the ldpcdec_deltaendruncnt field's value will be returned.
 *
 * @return The current value of the ldpcdec_deltaendruncnt field in the BOUF_LDPC3 register.
 */
__INLINE uint8_t mdm_ldpcdec_deltaendruncnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC3_ADDR);
    return ((localVal & ((uint32_t)0x001F0000)) >> 16);
}

/**
 * @brief Sets the ldpcdec_deltaendruncnt field of the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdeltaendruncnt - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_deltaendruncnt_setf(uint8_t ldpcdecdeltaendruncnt)
{
    ASSERT_ERR((((uint32_t)ldpcdecdeltaendruncnt << 16) & ~((uint32_t)0x001F0000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC3_ADDR, (REG_PL_RD(MDM_BOUF_LDPC3_ADDR) & ~((uint32_t)0x001F0000)) | ((uint32_t)ldpcdecdeltaendruncnt << 16));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc3_ncw3_stbc field in the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read and the ldpcdec_delta1944_ndpsc3_ncw3_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc3_ncw3_stbc field in the BOUF_LDPC3 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_3_ncw_3_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC3_ADDR);
    return ((localVal & ((uint32_t)0x0000F000)) >> 12);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc3_ncw3_stbc field of the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc3ncw3stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_3_ncw_3_stbc_setf(uint8_t ldpcdecdelta1944ndpsc3ncw3stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw3stbc << 12) & ~((uint32_t)0x0000F000)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC3_ADDR, (REG_PL_RD(MDM_BOUF_LDPC3_ADDR) & ~((uint32_t)0x0000F000)) | ((uint32_t)ldpcdecdelta1944ndpsc3ncw3stbc << 12));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc3_ncw2_stbc field in the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read and the ldpcdec_delta1944_ndpsc3_ncw2_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc3_ncw2_stbc field in the BOUF_LDPC3 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_3_ncw_2_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC3_ADDR);
    return ((localVal & ((uint32_t)0x00000F00)) >> 8);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc3_ncw2_stbc field of the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc3ncw2stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_3_ncw_2_stbc_setf(uint8_t ldpcdecdelta1944ndpsc3ncw2stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw2stbc << 8) & ~((uint32_t)0x00000F00)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC3_ADDR, (REG_PL_RD(MDM_BOUF_LDPC3_ADDR) & ~((uint32_t)0x00000F00)) | ((uint32_t)ldpcdecdelta1944ndpsc3ncw2stbc << 8));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc3_ncw1_stbc field in the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read and the ldpcdec_delta1944_ndpsc3_ncw1_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc3_ncw1_stbc field in the BOUF_LDPC3 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_3_ncw_1_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC3_ADDR);
    return ((localVal & ((uint32_t)0x000000F0)) >> 4);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc3_ncw1_stbc field of the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc3ncw1stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_3_ncw_1_stbc_setf(uint8_t ldpcdecdelta1944ndpsc3ncw1stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw1stbc << 4) & ~((uint32_t)0x000000F0)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC3_ADDR, (REG_PL_RD(MDM_BOUF_LDPC3_ADDR) & ~((uint32_t)0x000000F0)) | ((uint32_t)ldpcdecdelta1944ndpsc3ncw1stbc << 4));
}

/**
 * @brief Returns the current value of the ldpcdec_delta1944_ndpsc3_ncw0_stbc field in the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read and the ldpcdec_delta1944_ndpsc3_ncw0_stbc field's value will be returned.
 *
 * @return The current value of the ldpcdec_delta1944_ndpsc3_ncw0_stbc field in the BOUF_LDPC3 register.
 */
__INLINE uint8_t mdm_ldpcdec_delta_1944_ndpsc_3_ncw_0_stbc_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_LDPC3_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

/**
 * @brief Sets the ldpcdec_delta1944_ndpsc3_ncw0_stbc field of the BOUF_LDPC3 register.
 *
 * The BOUF_LDPC3 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] ldpcdecdelta1944ndpsc3ncw0stbc - The value to set the field to.
 */
__INLINE void mdm_ldpcdec_delta_1944_ndpsc_3_ncw_0_stbc_setf(uint8_t ldpcdecdelta1944ndpsc3ncw0stbc)
{
    ASSERT_ERR((((uint32_t)ldpcdecdelta1944ndpsc3ncw0stbc << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_PL_WR(MDM_BOUF_LDPC3_ADDR, (REG_PL_RD(MDM_BOUF_LDPC3_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)ldpcdecdelta1944ndpsc3ncw0stbc << 0));
}

#endif // RW_NX_LDPC_DEC
/// @}

#if BOUF_TXSCAL0
/**
 * @name BOUF_TXSCAL0 register definitions
 * <table>
 * <caption>BOUF_TXSCAL0 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31    <td>   pertone_scale_leg_en <td> R <td> R/W <td> 0
 * <tr><td> 30    <td>   pertone_scale_11n_en <td> R <td> R/W <td> 0
 * <tr><td> 29    <td>  pertone_scale_11ac_en <td> R <td> R/W <td> 0
 * <tr><td> 28    <td>  pertone_scale_11ax_en <td> R <td> R/W <td> 0
 * <tr><td> 27:21 <td>      pertone_scale_th0 <td> R <td> R/W <td> 0x50
 * <tr><td> 20:14 <td>      pertone_scale_th1 <td> R <td> R/W <td> 0x60
 * <tr><td> 13:07 <td>      pertone_scale_th2 <td> R <td> R/W <td> 0x70
 * <tr><td> 06:00 <td>      pertone_scale_th3 <td> R <td> R/W <td> 0x0
 * </table>
 *
 * @{
 */

/// Address of the BOUF_TXSCAL0 register
#define MDM_BOUF_TXSCAL0_ADDR   0x24C00910
/// Offset of the BOUF_TXSCAL0 register from the base address
#define MDM_BOUF_TXSCAL0_OFFSET 0x00000910
/// Index of the BOUF_TXSCAL0 register
#define MDM_BOUF_TXSCAL0_INDEX  0x00000244
/// Reset value of the BOUF_TXSCAL0 register
#define MDM_BOUF_TXSCAL0_RESET  0x0A183800

/**
 * @brief Returns the current value of the BOUF_TXSCAL0 register.
 * The BOUF_TXSCAL0 register will be read and its value returned.
 * @return The current value of the BOUF_TXSCAL0 register.
 */
__INLINE uint32_t mdm_bouf_txscal0_get(void)
{
    return REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR);
}

/**
 * @brief Sets the BOUF_TXSCAL0 register to a value.
 * The BOUF_TXSCAL0 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_bouf_txscal0_set(uint32_t value)
{
    REG_PL_WR(MDM_BOUF_TXSCAL0_ADDR, value);
}

// field definitions
/// PERTONE_SCALE_LEG_EN field bit
#define MDM_PERTONE_SCALE_LEG_EN_BIT      ((uint32_t)0x80000000)
/// PERTONE_SCALE_LEG_EN field position
#define MDM_PERTONE_SCALE_LEG_EN_POS      31
/// PERTONE_SCALE__11N_EN field bit
#define MDM_PERTONE_SCALE__11N_EN_BIT     ((uint32_t)0x40000000)
/// PERTONE_SCALE__11N_EN field position
#define MDM_PERTONE_SCALE__11N_EN_POS     30
/// PERTONE_SCALE__11AC_EN field bit
#define MDM_PERTONE_SCALE__11AC_EN_BIT    ((uint32_t)0x20000000)
/// PERTONE_SCALE__11AC_EN field position
#define MDM_PERTONE_SCALE__11AC_EN_POS    29
/// PERTONE_SCALE__11AX_EN field bit
#define MDM_PERTONE_SCALE__11AX_EN_BIT    ((uint32_t)0x10000000)
/// PERTONE_SCALE__11AX_EN field position
#define MDM_PERTONE_SCALE__11AX_EN_POS    28
/// PERTONE_SCALE_TH_0 field mask
#define MDM_PERTONE_SCALE_TH_0_MASK       ((uint32_t)0x0FE00000)
/// PERTONE_SCALE_TH_0 field LSB position
#define MDM_PERTONE_SCALE_TH_0_LSB        21
/// PERTONE_SCALE_TH_0 field width
#define MDM_PERTONE_SCALE_TH_0_WIDTH      ((uint32_t)0x00000007)
/// PERTONE_SCALE_TH_1 field mask
#define MDM_PERTONE_SCALE_TH_1_MASK       ((uint32_t)0x001FC000)
/// PERTONE_SCALE_TH_1 field LSB position
#define MDM_PERTONE_SCALE_TH_1_LSB        14
/// PERTONE_SCALE_TH_1 field width
#define MDM_PERTONE_SCALE_TH_1_WIDTH      ((uint32_t)0x00000007)
/// PERTONE_SCALE_TH_2 field mask
#define MDM_PERTONE_SCALE_TH_2_MASK       ((uint32_t)0x00003F80)
/// PERTONE_SCALE_TH_2 field LSB position
#define MDM_PERTONE_SCALE_TH_2_LSB        7
/// PERTONE_SCALE_TH_2 field width
#define MDM_PERTONE_SCALE_TH_2_WIDTH      ((uint32_t)0x00000007)
/// PERTONE_SCALE_TH_3 field mask
#define MDM_PERTONE_SCALE_TH_3_MASK       ((uint32_t)0x0000007F)
/// PERTONE_SCALE_TH_3 field LSB position
#define MDM_PERTONE_SCALE_TH_3_LSB        0
/// PERTONE_SCALE_TH_3 field width
#define MDM_PERTONE_SCALE_TH_3_WIDTH      ((uint32_t)0x00000007)

/// PERTONE_SCALE_LEG_EN field reset value
#define MDM_PERTONE_SCALE_LEG_EN_RST      0x0
/// PERTONE_SCALE__11N_EN field reset value
#define MDM_PERTONE_SCALE__11N_EN_RST     0x0
/// PERTONE_SCALE__11AC_EN field reset value
#define MDM_PERTONE_SCALE__11AC_EN_RST    0x0
/// PERTONE_SCALE__11AX_EN field reset value
#define MDM_PERTONE_SCALE__11AX_EN_RST    0x0
/// PERTONE_SCALE_TH_0 field reset value
#define MDM_PERTONE_SCALE_TH_0_RST        0x50
/// PERTONE_SCALE_TH_1 field reset value
#define MDM_PERTONE_SCALE_TH_1_RST        0x60
/// PERTONE_SCALE_TH_2 field reset value
#define MDM_PERTONE_SCALE_TH_2_RST        0x70
/// PERTONE_SCALE_TH_3 field reset value
#define MDM_PERTONE_SCALE_TH_3_RST        0x0

/**
 * @brief Constructs a value for the BOUF_TXSCAL0 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] pertonescalelegen - The value to use for the pertone_scale_leg_en field.
 * @param[in] pertonescale11nen - The value to use for the pertone_scale_11n_en field.
 * @param[in] pertonescale11acen - The value to use for the pertone_scale_11ac_en field.
 * @param[in] pertonescale11axen - The value to use for the pertone_scale_11ax_en field.
 * @param[in] pertonescaleth0 - The value to use for the pertone_scale_th0 field.
 * @param[in] pertonescaleth1 - The value to use for the pertone_scale_th1 field.
 * @param[in] pertonescaleth2 - The value to use for the pertone_scale_th2 field.
 * @param[in] pertonescaleth3 - The value to use for the pertone_scale_th3 field.
 */
__INLINE void mdm_bouf_txscal0_pack(uint8_t pertonescalelegen, uint8_t pertonescale11nen, uint8_t pertonescale11acen, uint8_t pertonescale11axen, uint8_t pertonescaleth0, uint8_t pertonescaleth1, uint8_t pertonescaleth2, uint8_t pertonescaleth3)
{
    ASSERT_ERR((((uint32_t)pertonescalelegen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescale11nen << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescale11acen << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescale11axen << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescaleth0 << 21) & ~((uint32_t)0x0FE00000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescaleth1 << 14) & ~((uint32_t)0x001FC000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescaleth2 << 7) & ~((uint32_t)0x00003F80)) == 0);
    ASSERT_ERR((((uint32_t)pertonescaleth3 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL0_ADDR,  ((uint32_t)pertonescalelegen << 31) | ((uint32_t)pertonescale11nen << 30) | ((uint32_t)pertonescale11acen << 29) | ((uint32_t)pertonescale11axen << 28) | ((uint32_t)pertonescaleth0 << 21) | ((uint32_t)pertonescaleth1 << 14) | ((uint32_t)pertonescaleth2 << 7) | ((uint32_t)pertonescaleth3 << 0));
}

/**
 * @brief Unpacks BOUF_TXSCAL0's fields from current value of the BOUF_TXSCAL0 register.
 *
 * Reads the BOUF_TXSCAL0 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] pertonescalelegen - Will be populated with the current value of this field from the register.
 * @param[out] pertonescale11nen - Will be populated with the current value of this field from the register.
 * @param[out] pertonescale11acen - Will be populated with the current value of this field from the register.
 * @param[out] pertonescale11axen - Will be populated with the current value of this field from the register.
 * @param[out] pertonescaleth0 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescaleth1 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescaleth2 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescaleth3 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_bouf_txscal0_unpack(uint8_t* pertonescalelegen, uint8_t* pertonescale11nen, uint8_t* pertonescale11acen, uint8_t* pertonescale11axen, uint8_t* pertonescaleth0, uint8_t* pertonescaleth1, uint8_t* pertonescaleth2, uint8_t* pertonescaleth3)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR);

    *pertonescalelegen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *pertonescale11nen = (localVal & ((uint32_t)0x40000000)) >> 30;
    *pertonescale11acen = (localVal & ((uint32_t)0x20000000)) >> 29;
    *pertonescale11axen = (localVal & ((uint32_t)0x10000000)) >> 28;
    *pertonescaleth0 = (localVal & ((uint32_t)0x0FE00000)) >> 21;
    *pertonescaleth1 = (localVal & ((uint32_t)0x001FC000)) >> 14;
    *pertonescaleth2 = (localVal & ((uint32_t)0x00003F80)) >> 7;
    *pertonescaleth3 = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

/**
 * @brief Returns the current value of the pertone_scale_leg_en field in the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read and the pertone_scale_leg_en field's value will be returned.
 *
 * @return The current value of the pertone_scale_leg_en field in the BOUF_TXSCAL0 register.
 */
__INLINE uint8_t mdm_pertone_scale_leg_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

/**
 * @brief Sets the pertone_scale_leg_en field of the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescalelegen - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_leg_en_setf(uint8_t pertonescalelegen)
{
    ASSERT_ERR((((uint32_t)pertonescalelegen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL0_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)pertonescalelegen << 31));
}

/**
 * @brief Returns the current value of the pertone_scale_11n_en field in the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read and the pertone_scale_11n_en field's value will be returned.
 *
 * @return The current value of the pertone_scale_11n_en field in the BOUF_TXSCAL0 register.
 */
__INLINE uint8_t mdm_pertone_scale__11n_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

/**
 * @brief Sets the pertone_scale_11n_en field of the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescale11nen - The value to set the field to.
 */
__INLINE void mdm_pertone_scale__11n_en_setf(uint8_t pertonescale11nen)
{
    ASSERT_ERR((((uint32_t)pertonescale11nen << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL0_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)pertonescale11nen << 30));
}

/**
 * @brief Returns the current value of the pertone_scale_11ac_en field in the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read and the pertone_scale_11ac_en field's value will be returned.
 *
 * @return The current value of the pertone_scale_11ac_en field in the BOUF_TXSCAL0 register.
 */
__INLINE uint8_t mdm_pertone_scale__11ac_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

/**
 * @brief Sets the pertone_scale_11ac_en field of the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescale11acen - The value to set the field to.
 */
__INLINE void mdm_pertone_scale__11ac_en_setf(uint8_t pertonescale11acen)
{
    ASSERT_ERR((((uint32_t)pertonescale11acen << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL0_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)pertonescale11acen << 29));
}

/**
 * @brief Returns the current value of the pertone_scale_11ax_en field in the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read and the pertone_scale_11ax_en field's value will be returned.
 *
 * @return The current value of the pertone_scale_11ax_en field in the BOUF_TXSCAL0 register.
 */
__INLINE uint8_t mdm_pertone_scale__11ax_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

/**
 * @brief Sets the pertone_scale_11ax_en field of the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescale11axen - The value to set the field to.
 */
__INLINE void mdm_pertone_scale__11ax_en_setf(uint8_t pertonescale11axen)
{
    ASSERT_ERR((((uint32_t)pertonescale11axen << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL0_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)pertonescale11axen << 28));
}

/**
 * @brief Returns the current value of the pertone_scale_th0 field in the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read and the pertone_scale_th0 field's value will be returned.
 *
 * @return The current value of the pertone_scale_th0 field in the BOUF_TXSCAL0 register.
 */
__INLINE uint8_t mdm_pertone_scale_th_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR);
    return ((localVal & ((uint32_t)0x0FE00000)) >> 21);
}

/**
 * @brief Sets the pertone_scale_th0 field of the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescaleth0 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_th_0_setf(uint8_t pertonescaleth0)
{
    ASSERT_ERR((((uint32_t)pertonescaleth0 << 21) & ~((uint32_t)0x0FE00000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL0_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR) & ~((uint32_t)0x0FE00000)) | ((uint32_t)pertonescaleth0 << 21));
}

/**
 * @brief Returns the current value of the pertone_scale_th1 field in the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read and the pertone_scale_th1 field's value will be returned.
 *
 * @return The current value of the pertone_scale_th1 field in the BOUF_TXSCAL0 register.
 */
__INLINE uint8_t mdm_pertone_scale_th_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR);
    return ((localVal & ((uint32_t)0x001FC000)) >> 14);
}

/**
 * @brief Sets the pertone_scale_th1 field of the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescaleth1 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_th_1_setf(uint8_t pertonescaleth1)
{
    ASSERT_ERR((((uint32_t)pertonescaleth1 << 14) & ~((uint32_t)0x001FC000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL0_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR) & ~((uint32_t)0x001FC000)) | ((uint32_t)pertonescaleth1 << 14));
}

/**
 * @brief Returns the current value of the pertone_scale_th2 field in the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read and the pertone_scale_th2 field's value will be returned.
 *
 * @return The current value of the pertone_scale_th2 field in the BOUF_TXSCAL0 register.
 */
__INLINE uint8_t mdm_pertone_scale_th_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR);
    return ((localVal & ((uint32_t)0x00003F80)) >> 7);
}

/**
 * @brief Sets the pertone_scale_th2 field of the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescaleth2 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_th_2_setf(uint8_t pertonescaleth2)
{
    ASSERT_ERR((((uint32_t)pertonescaleth2 << 7) & ~((uint32_t)0x00003F80)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL0_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR) & ~((uint32_t)0x00003F80)) | ((uint32_t)pertonescaleth2 << 7));
}

/**
 * @brief Returns the current value of the pertone_scale_th3 field in the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read and the pertone_scale_th3 field's value will be returned.
 *
 * @return The current value of the pertone_scale_th3 field in the BOUF_TXSCAL0 register.
 */
__INLINE uint8_t mdm_pertone_scale_th_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

/**
 * @brief Sets the pertone_scale_th3 field of the BOUF_TXSCAL0 register.
 *
 * The BOUF_TXSCAL0 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescaleth3 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_th_3_setf(uint8_t pertonescaleth3)
{
    ASSERT_ERR((((uint32_t)pertonescaleth3 << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL0_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL0_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)pertonescaleth3 << 0));
}

#endif // BOUF_TXSCAL0
/// @}

#if BOUF_TXSCAL1
/**
 * @name BOUF_TXSCAL1 register definitions
 * <table>
 * <caption>BOUF_TXSCAL1 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:25 <td>  pertone_scale_th4 <td> R <td> R/W <td> 0x10
 * <tr><td> 24:18 <td>  pertone_scale_th5 <td> R <td> R/W <td> 0x20
 * <tr><td> 17:11 <td>  pertone_scale_th6 <td> R <td> R/W <td> 0x30
 * <tr><td> 10:06 <td>     pertone_scale0 <td> R <td> R/W <td> 0x8
 * <tr><td> 05:01 <td>     pertone_scale1 <td> R <td> R/W <td> 0x8
 * </table>
 *
 * @{
 */

/// Address of the BOUF_TXSCAL1 register
#define MDM_BOUF_TXSCAL1_ADDR   0x24C00914
/// Offset of the BOUF_TXSCAL1 register from the base address
#define MDM_BOUF_TXSCAL1_OFFSET 0x00000914
/// Index of the BOUF_TXSCAL1 register
#define MDM_BOUF_TXSCAL1_INDEX  0x00000245
/// Reset value of the BOUF_TXSCAL1 register
#define MDM_BOUF_TXSCAL1_RESET  0x20818210

/**
 * @brief Returns the current value of the BOUF_TXSCAL1 register.
 * The BOUF_TXSCAL1 register will be read and its value returned.
 * @return The current value of the BOUF_TXSCAL1 register.
 */
__INLINE uint32_t mdm_bouf_txscal1_get(void)
{
    return REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR);
}

/**
 * @brief Sets the BOUF_TXSCAL1 register to a value.
 * The BOUF_TXSCAL1 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_bouf_txscal1_set(uint32_t value)
{
    REG_PL_WR(MDM_BOUF_TXSCAL1_ADDR, value);
}

// field definitions
/// PERTONE_SCALE_TH_4 field mask
#define MDM_PERTONE_SCALE_TH_4_MASK   ((uint32_t)0xFE000000)
/// PERTONE_SCALE_TH_4 field LSB position
#define MDM_PERTONE_SCALE_TH_4_LSB    25
/// PERTONE_SCALE_TH_4 field width
#define MDM_PERTONE_SCALE_TH_4_WIDTH  ((uint32_t)0x00000007)
/// PERTONE_SCALE_TH_5 field mask
#define MDM_PERTONE_SCALE_TH_5_MASK   ((uint32_t)0x01FC0000)
/// PERTONE_SCALE_TH_5 field LSB position
#define MDM_PERTONE_SCALE_TH_5_LSB    18
/// PERTONE_SCALE_TH_5 field width
#define MDM_PERTONE_SCALE_TH_5_WIDTH  ((uint32_t)0x00000007)
/// PERTONE_SCALE_TH_6 field mask
#define MDM_PERTONE_SCALE_TH_6_MASK   ((uint32_t)0x0003F800)
/// PERTONE_SCALE_TH_6 field LSB position
#define MDM_PERTONE_SCALE_TH_6_LSB    11
/// PERTONE_SCALE_TH_6 field width
#define MDM_PERTONE_SCALE_TH_6_WIDTH  ((uint32_t)0x00000007)
/// PERTONE_SCALE_0 field mask
#define MDM_PERTONE_SCALE_0_MASK      ((uint32_t)0x000007C0)
/// PERTONE_SCALE_0 field LSB position
#define MDM_PERTONE_SCALE_0_LSB       6
/// PERTONE_SCALE_0 field width
#define MDM_PERTONE_SCALE_0_WIDTH     ((uint32_t)0x00000005)
/// PERTONE_SCALE_1 field mask
#define MDM_PERTONE_SCALE_1_MASK      ((uint32_t)0x0000003E)
/// PERTONE_SCALE_1 field LSB position
#define MDM_PERTONE_SCALE_1_LSB       1
/// PERTONE_SCALE_1 field width
#define MDM_PERTONE_SCALE_1_WIDTH     ((uint32_t)0x00000005)

/// PERTONE_SCALE_TH_4 field reset value
#define MDM_PERTONE_SCALE_TH_4_RST    0x10
/// PERTONE_SCALE_TH_5 field reset value
#define MDM_PERTONE_SCALE_TH_5_RST    0x20
/// PERTONE_SCALE_TH_6 field reset value
#define MDM_PERTONE_SCALE_TH_6_RST    0x30
/// PERTONE_SCALE_0 field reset value
#define MDM_PERTONE_SCALE_0_RST       0x8
/// PERTONE_SCALE_1 field reset value
#define MDM_PERTONE_SCALE_1_RST       0x8

/**
 * @brief Constructs a value for the BOUF_TXSCAL1 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] pertonescaleth4 - The value to use for the pertone_scale_th4 field.
 * @param[in] pertonescaleth5 - The value to use for the pertone_scale_th5 field.
 * @param[in] pertonescaleth6 - The value to use for the pertone_scale_th6 field.
 * @param[in] pertonescale0 - The value to use for the pertone_scale0 field.
 * @param[in] pertonescale1 - The value to use for the pertone_scale1 field.
 */
__INLINE void mdm_bouf_txscal1_pack(uint8_t pertonescaleth4, uint8_t pertonescaleth5, uint8_t pertonescaleth6, uint8_t pertonescale0, uint8_t pertonescale1)
{
    ASSERT_ERR((((uint32_t)pertonescaleth4 << 25) & ~((uint32_t)0xFE000000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescaleth5 << 18) & ~((uint32_t)0x01FC0000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescaleth6 << 11) & ~((uint32_t)0x0003F800)) == 0);
    ASSERT_ERR((((uint32_t)pertonescale0 << 6) & ~((uint32_t)0x000007C0)) == 0);
    ASSERT_ERR((((uint32_t)pertonescale1 << 1) & ~((uint32_t)0x0000003E)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL1_ADDR,  ((uint32_t)pertonescaleth4 << 25) | ((uint32_t)pertonescaleth5 << 18) | ((uint32_t)pertonescaleth6 << 11) | ((uint32_t)pertonescale0 << 6) | ((uint32_t)pertonescale1 << 1));
}

/**
 * @brief Unpacks BOUF_TXSCAL1's fields from current value of the BOUF_TXSCAL1 register.
 *
 * Reads the BOUF_TXSCAL1 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] pertonescaleth4 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescaleth5 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescaleth6 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescale0 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescale1 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_bouf_txscal1_unpack(uint8_t* pertonescaleth4, uint8_t* pertonescaleth5, uint8_t* pertonescaleth6, uint8_t* pertonescale0, uint8_t* pertonescale1)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR);

    *pertonescaleth4 = (localVal & ((uint32_t)0xFE000000)) >> 25;
    *pertonescaleth5 = (localVal & ((uint32_t)0x01FC0000)) >> 18;
    *pertonescaleth6 = (localVal & ((uint32_t)0x0003F800)) >> 11;
    *pertonescale0 = (localVal & ((uint32_t)0x000007C0)) >> 6;
    *pertonescale1 = (localVal & ((uint32_t)0x0000003E)) >> 1;
}

/**
 * @brief Returns the current value of the pertone_scale_th4 field in the BOUF_TXSCAL1 register.
 *
 * The BOUF_TXSCAL1 register will be read and the pertone_scale_th4 field's value will be returned.
 *
 * @return The current value of the pertone_scale_th4 field in the BOUF_TXSCAL1 register.
 */
__INLINE uint8_t mdm_pertone_scale_th_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR);
    return ((localVal & ((uint32_t)0xFE000000)) >> 25);
}

/**
 * @brief Sets the pertone_scale_th4 field of the BOUF_TXSCAL1 register.
 *
 * The BOUF_TXSCAL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescaleth4 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_th_4_setf(uint8_t pertonescaleth4)
{
    ASSERT_ERR((((uint32_t)pertonescaleth4 << 25) & ~((uint32_t)0xFE000000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL1_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR) & ~((uint32_t)0xFE000000)) | ((uint32_t)pertonescaleth4 << 25));
}

/**
 * @brief Returns the current value of the pertone_scale_th5 field in the BOUF_TXSCAL1 register.
 *
 * The BOUF_TXSCAL1 register will be read and the pertone_scale_th5 field's value will be returned.
 *
 * @return The current value of the pertone_scale_th5 field in the BOUF_TXSCAL1 register.
 */
__INLINE uint8_t mdm_pertone_scale_th_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR);
    return ((localVal & ((uint32_t)0x01FC0000)) >> 18);
}

/**
 * @brief Sets the pertone_scale_th5 field of the BOUF_TXSCAL1 register.
 *
 * The BOUF_TXSCAL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescaleth5 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_th_5_setf(uint8_t pertonescaleth5)
{
    ASSERT_ERR((((uint32_t)pertonescaleth5 << 18) & ~((uint32_t)0x01FC0000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL1_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR) & ~((uint32_t)0x01FC0000)) | ((uint32_t)pertonescaleth5 << 18));
}

/**
 * @brief Returns the current value of the pertone_scale_th6 field in the BOUF_TXSCAL1 register.
 *
 * The BOUF_TXSCAL1 register will be read and the pertone_scale_th6 field's value will be returned.
 *
 * @return The current value of the pertone_scale_th6 field in the BOUF_TXSCAL1 register.
 */
__INLINE uint8_t mdm_pertone_scale_th_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR);
    return ((localVal & ((uint32_t)0x0003F800)) >> 11);
}

/**
 * @brief Sets the pertone_scale_th6 field of the BOUF_TXSCAL1 register.
 *
 * The BOUF_TXSCAL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescaleth6 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_th_6_setf(uint8_t pertonescaleth6)
{
    ASSERT_ERR((((uint32_t)pertonescaleth6 << 11) & ~((uint32_t)0x0003F800)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL1_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR) & ~((uint32_t)0x0003F800)) | ((uint32_t)pertonescaleth6 << 11));
}

/**
 * @brief Returns the current value of the pertone_scale0 field in the BOUF_TXSCAL1 register.
 *
 * The BOUF_TXSCAL1 register will be read and the pertone_scale0 field's value will be returned.
 *
 * @return The current value of the pertone_scale0 field in the BOUF_TXSCAL1 register.
 */
__INLINE uint8_t mdm_pertone_scale_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR);
    return ((localVal & ((uint32_t)0x000007C0)) >> 6);
}

/**
 * @brief Sets the pertone_scale0 field of the BOUF_TXSCAL1 register.
 *
 * The BOUF_TXSCAL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescale0 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_0_setf(uint8_t pertonescale0)
{
    ASSERT_ERR((((uint32_t)pertonescale0 << 6) & ~((uint32_t)0x000007C0)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL1_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR) & ~((uint32_t)0x000007C0)) | ((uint32_t)pertonescale0 << 6));
}

/**
 * @brief Returns the current value of the pertone_scale1 field in the BOUF_TXSCAL1 register.
 *
 * The BOUF_TXSCAL1 register will be read and the pertone_scale1 field's value will be returned.
 *
 * @return The current value of the pertone_scale1 field in the BOUF_TXSCAL1 register.
 */
__INLINE uint8_t mdm_pertone_scale_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR);
    return ((localVal & ((uint32_t)0x0000003E)) >> 1);
}

/**
 * @brief Sets the pertone_scale1 field of the BOUF_TXSCAL1 register.
 *
 * The BOUF_TXSCAL1 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescale1 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_1_setf(uint8_t pertonescale1)
{
    ASSERT_ERR((((uint32_t)pertonescale1 << 1) & ~((uint32_t)0x0000003E)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL1_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL1_ADDR) & ~((uint32_t)0x0000003E)) | ((uint32_t)pertonescale1 << 1));
}

#endif // BOUF_TXSCAL1
/// @}

#if BOUF_TXSCAL2
/**
 * @name BOUF_TXSCAL2 register definitions
 * <table>
 * <caption>BOUF_TXSCAL2 bitfields</caption>
 * <tr><th>Bits <th>Field Name <th>HW Access <th>SW Access <th>Reset Value
 * <tr><td> 31:27 <td>  pertone_scale2 <td> R <td> R/W <td> 0x8
 * <tr><td> 26:22 <td>  pertone_scale3 <td> R <td> R/W <td> 0x8
 * <tr><td> 21:17 <td>  pertone_scale4 <td> R <td> R/W <td> 0x8
 * <tr><td> 16:12 <td>  pertone_scale5 <td> R <td> R/W <td> 0x8
 * <tr><td> 11:07 <td>  pertone_scale6 <td> R <td> R/W <td> 0x8
 * <tr><td> 06:02 <td>  pertone_scale7 <td> R <td> R/W <td> 0x8
 * </table>
 *
 * @{
 */

/// Address of the BOUF_TXSCAL2 register
#define MDM_BOUF_TXSCAL2_ADDR   0x24C00918
/// Offset of the BOUF_TXSCAL2 register from the base address
#define MDM_BOUF_TXSCAL2_OFFSET 0x00000918
/// Index of the BOUF_TXSCAL2 register
#define MDM_BOUF_TXSCAL2_INDEX  0x00000246
/// Reset value of the BOUF_TXSCAL2 register
#define MDM_BOUF_TXSCAL2_RESET  0x42108420

/**
 * @brief Returns the current value of the BOUF_TXSCAL2 register.
 * The BOUF_TXSCAL2 register will be read and its value returned.
 * @return The current value of the BOUF_TXSCAL2 register.
 */
__INLINE uint32_t mdm_bouf_txscal2_get(void)
{
    return REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR);
}

/**
 * @brief Sets the BOUF_TXSCAL2 register to a value.
 * The BOUF_TXSCAL2 register will be written.
 * @param value - The value to write.
 */
__INLINE void mdm_bouf_txscal2_set(uint32_t value)
{
    REG_PL_WR(MDM_BOUF_TXSCAL2_ADDR, value);
}

// field definitions
/// PERTONE_SCALE_2 field mask
#define MDM_PERTONE_SCALE_2_MASK   ((uint32_t)0xF8000000)
/// PERTONE_SCALE_2 field LSB position
#define MDM_PERTONE_SCALE_2_LSB    27
/// PERTONE_SCALE_2 field width
#define MDM_PERTONE_SCALE_2_WIDTH  ((uint32_t)0x00000005)
/// PERTONE_SCALE_3 field mask
#define MDM_PERTONE_SCALE_3_MASK   ((uint32_t)0x07C00000)
/// PERTONE_SCALE_3 field LSB position
#define MDM_PERTONE_SCALE_3_LSB    22
/// PERTONE_SCALE_3 field width
#define MDM_PERTONE_SCALE_3_WIDTH  ((uint32_t)0x00000005)
/// PERTONE_SCALE_4 field mask
#define MDM_PERTONE_SCALE_4_MASK   ((uint32_t)0x003E0000)
/// PERTONE_SCALE_4 field LSB position
#define MDM_PERTONE_SCALE_4_LSB    17
/// PERTONE_SCALE_4 field width
#define MDM_PERTONE_SCALE_4_WIDTH  ((uint32_t)0x00000005)
/// PERTONE_SCALE_5 field mask
#define MDM_PERTONE_SCALE_5_MASK   ((uint32_t)0x0001F000)
/// PERTONE_SCALE_5 field LSB position
#define MDM_PERTONE_SCALE_5_LSB    12
/// PERTONE_SCALE_5 field width
#define MDM_PERTONE_SCALE_5_WIDTH  ((uint32_t)0x00000005)
/// PERTONE_SCALE_6 field mask
#define MDM_PERTONE_SCALE_6_MASK   ((uint32_t)0x00000F80)
/// PERTONE_SCALE_6 field LSB position
#define MDM_PERTONE_SCALE_6_LSB    7
/// PERTONE_SCALE_6 field width
#define MDM_PERTONE_SCALE_6_WIDTH  ((uint32_t)0x00000005)
/// PERTONE_SCALE_7 field mask
#define MDM_PERTONE_SCALE_7_MASK   ((uint32_t)0x0000007C)
/// PERTONE_SCALE_7 field LSB position
#define MDM_PERTONE_SCALE_7_LSB    2
/// PERTONE_SCALE_7 field width
#define MDM_PERTONE_SCALE_7_WIDTH  ((uint32_t)0x00000005)

/// PERTONE_SCALE_2 field reset value
#define MDM_PERTONE_SCALE_2_RST    0x8
/// PERTONE_SCALE_3 field reset value
#define MDM_PERTONE_SCALE_3_RST    0x8
/// PERTONE_SCALE_4 field reset value
#define MDM_PERTONE_SCALE_4_RST    0x8
/// PERTONE_SCALE_5 field reset value
#define MDM_PERTONE_SCALE_5_RST    0x8
/// PERTONE_SCALE_6 field reset value
#define MDM_PERTONE_SCALE_6_RST    0x8
/// PERTONE_SCALE_7 field reset value
#define MDM_PERTONE_SCALE_7_RST    0x8

/**
 * @brief Constructs a value for the BOUF_TXSCAL2 register given values for its fields
 * and writes the value to the register.
 *
 * @param[in] pertonescale2 - The value to use for the pertone_scale2 field.
 * @param[in] pertonescale3 - The value to use for the pertone_scale3 field.
 * @param[in] pertonescale4 - The value to use for the pertone_scale4 field.
 * @param[in] pertonescale5 - The value to use for the pertone_scale5 field.
 * @param[in] pertonescale6 - The value to use for the pertone_scale6 field.
 * @param[in] pertonescale7 - The value to use for the pertone_scale7 field.
 */
__INLINE void mdm_bouf_txscal2_pack(uint8_t pertonescale2, uint8_t pertonescale3, uint8_t pertonescale4, uint8_t pertonescale5, uint8_t pertonescale6, uint8_t pertonescale7)
{
    ASSERT_ERR((((uint32_t)pertonescale2 << 27) & ~((uint32_t)0xF8000000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescale3 << 22) & ~((uint32_t)0x07C00000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescale4 << 17) & ~((uint32_t)0x003E0000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescale5 << 12) & ~((uint32_t)0x0001F000)) == 0);
    ASSERT_ERR((((uint32_t)pertonescale6 << 7) & ~((uint32_t)0x00000F80)) == 0);
    ASSERT_ERR((((uint32_t)pertonescale7 << 2) & ~((uint32_t)0x0000007C)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL2_ADDR,  ((uint32_t)pertonescale2 << 27) | ((uint32_t)pertonescale3 << 22) | ((uint32_t)pertonescale4 << 17) | ((uint32_t)pertonescale5 << 12) | ((uint32_t)pertonescale6 << 7) | ((uint32_t)pertonescale7 << 2));
}

/**
 * @brief Unpacks BOUF_TXSCAL2's fields from current value of the BOUF_TXSCAL2 register.
 *
 * Reads the BOUF_TXSCAL2 register and populates all the _field variables with the corresponding
 * values from the register.
 *
 * @param[out] pertonescale2 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescale3 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescale4 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescale5 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescale6 - Will be populated with the current value of this field from the register.
 * @param[out] pertonescale7 - Will be populated with the current value of this field from the register.
 */
__INLINE void mdm_bouf_txscal2_unpack(uint8_t* pertonescale2, uint8_t* pertonescale3, uint8_t* pertonescale4, uint8_t* pertonescale5, uint8_t* pertonescale6, uint8_t* pertonescale7)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR);

    *pertonescale2 = (localVal & ((uint32_t)0xF8000000)) >> 27;
    *pertonescale3 = (localVal & ((uint32_t)0x07C00000)) >> 22;
    *pertonescale4 = (localVal & ((uint32_t)0x003E0000)) >> 17;
    *pertonescale5 = (localVal & ((uint32_t)0x0001F000)) >> 12;
    *pertonescale6 = (localVal & ((uint32_t)0x00000F80)) >> 7;
    *pertonescale7 = (localVal & ((uint32_t)0x0000007C)) >> 2;
}

/**
 * @brief Returns the current value of the pertone_scale2 field in the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read and the pertone_scale2 field's value will be returned.
 *
 * @return The current value of the pertone_scale2 field in the BOUF_TXSCAL2 register.
 */
__INLINE uint8_t mdm_pertone_scale_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR);
    return ((localVal & ((uint32_t)0xF8000000)) >> 27);
}

/**
 * @brief Sets the pertone_scale2 field of the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescale2 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_2_setf(uint8_t pertonescale2)
{
    ASSERT_ERR((((uint32_t)pertonescale2 << 27) & ~((uint32_t)0xF8000000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL2_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR) & ~((uint32_t)0xF8000000)) | ((uint32_t)pertonescale2 << 27));
}

/**
 * @brief Returns the current value of the pertone_scale3 field in the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read and the pertone_scale3 field's value will be returned.
 *
 * @return The current value of the pertone_scale3 field in the BOUF_TXSCAL2 register.
 */
__INLINE uint8_t mdm_pertone_scale_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR);
    return ((localVal & ((uint32_t)0x07C00000)) >> 22);
}

/**
 * @brief Sets the pertone_scale3 field of the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescale3 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_3_setf(uint8_t pertonescale3)
{
    ASSERT_ERR((((uint32_t)pertonescale3 << 22) & ~((uint32_t)0x07C00000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL2_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR) & ~((uint32_t)0x07C00000)) | ((uint32_t)pertonescale3 << 22));
}

/**
 * @brief Returns the current value of the pertone_scale4 field in the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read and the pertone_scale4 field's value will be returned.
 *
 * @return The current value of the pertone_scale4 field in the BOUF_TXSCAL2 register.
 */
__INLINE uint8_t mdm_pertone_scale_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR);
    return ((localVal & ((uint32_t)0x003E0000)) >> 17);
}

/**
 * @brief Sets the pertone_scale4 field of the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescale4 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_4_setf(uint8_t pertonescale4)
{
    ASSERT_ERR((((uint32_t)pertonescale4 << 17) & ~((uint32_t)0x003E0000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL2_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR) & ~((uint32_t)0x003E0000)) | ((uint32_t)pertonescale4 << 17));
}

/**
 * @brief Returns the current value of the pertone_scale5 field in the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read and the pertone_scale5 field's value will be returned.
 *
 * @return The current value of the pertone_scale5 field in the BOUF_TXSCAL2 register.
 */
__INLINE uint8_t mdm_pertone_scale_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR);
    return ((localVal & ((uint32_t)0x0001F000)) >> 12);
}

/**
 * @brief Sets the pertone_scale5 field of the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescale5 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_5_setf(uint8_t pertonescale5)
{
    ASSERT_ERR((((uint32_t)pertonescale5 << 12) & ~((uint32_t)0x0001F000)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL2_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR) & ~((uint32_t)0x0001F000)) | ((uint32_t)pertonescale5 << 12));
}

/**
 * @brief Returns the current value of the pertone_scale6 field in the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read and the pertone_scale6 field's value will be returned.
 *
 * @return The current value of the pertone_scale6 field in the BOUF_TXSCAL2 register.
 */
__INLINE uint8_t mdm_pertone_scale_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR);
    return ((localVal & ((uint32_t)0x00000F80)) >> 7);
}

/**
 * @brief Sets the pertone_scale6 field of the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescale6 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_6_setf(uint8_t pertonescale6)
{
    ASSERT_ERR((((uint32_t)pertonescale6 << 7) & ~((uint32_t)0x00000F80)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL2_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR) & ~((uint32_t)0x00000F80)) | ((uint32_t)pertonescale6 << 7));
}

/**
 * @brief Returns the current value of the pertone_scale7 field in the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read and the pertone_scale7 field's value will be returned.
 *
 * @return The current value of the pertone_scale7 field in the BOUF_TXSCAL2 register.
 */
__INLINE uint8_t mdm_pertone_scale_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR);
    return ((localVal & ((uint32_t)0x0000007C)) >> 2);
}

/**
 * @brief Sets the pertone_scale7 field of the BOUF_TXSCAL2 register.
 *
 * The BOUF_TXSCAL2 register will be read, modified to contain the new field value, and written.
 *
 * @param[in] pertonescale7 - The value to set the field to.
 */
__INLINE void mdm_pertone_scale_7_setf(uint8_t pertonescale7)
{
    ASSERT_ERR((((uint32_t)pertonescale7 << 2) & ~((uint32_t)0x0000007C)) == 0);
    REG_PL_WR(MDM_BOUF_TXSCAL2_ADDR, (REG_PL_RD(MDM_BOUF_TXSCAL2_ADDR) & ~((uint32_t)0x0000007C)) | ((uint32_t)pertonescale7 << 2));
}

#endif // BOUF_TXSCAL2
/// @}


#endif // _REG_MDM_CFG_H_

/// @}

