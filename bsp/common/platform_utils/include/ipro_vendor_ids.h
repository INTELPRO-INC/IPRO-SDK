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
 * @file ipro_vendor_ids.h
 * @brief INTELPRO's registered vendor identifiers.
 *
 * These are assigned to the company by the issuing authority and are the same
 * on every product built from this SDK.  They belong in one header because the
 * cost of getting them wrong is not a bug report - a device that ships with
 * somebody else's identifier is a compliance problem, and one that ships with
 * a placeholder is refused by some hosts outright.
 *
 * Registrations
 * -------------
 *   IEEE MA-L (OUI)   C0-74-15   IntelPro Inc., registry ID 20225
 *                                https://standards-oui.ieee.org/
 *   USB-IF Vendor ID  0x3896     INTELPRO INC.
 *
 * Product IDs are per-product and stay with the product's own descriptor;
 * only the vendor half is fixed here.
 *
 * Deliberately not covered:
 *
 *   - The Bluetooth SIG Company Identifier. That is a separate registration
 *     with a separate authority and is not interchangeable with either of the
 *     above; leaving it out is better than guessing it from the USB VID.
 *
 *   - Devices that deliberately impersonate another vendor's hardware so a
 *     stock host driver will bind - ipro7_jtag_ftdi presents FTDI's 0x0403 on
 *     purpose, and changing it would leave the probe with no driver on any
 *     host. Impersonation is the feature there, so it keeps its own literal.
 */

#ifndef IPRO_VENDOR_IDS_H
#define IPRO_VENDOR_IDS_H

#include <stdbool.h>
#include <stdint.h>

/*
 * USB
 ****************************************************************************/

/** USB-IF Vendor ID assigned to INTELPRO INC. */
#define IPRO_USB_VID                0x3896

/** iManufacturer string, matching the USB-IF registration. */
#define IPRO_USB_MANUFACTURER_STR   "INTELPRO INC."

/*
 * IEEE 802 MAC / Bluetooth public device address
 ****************************************************************************/

/**
 * IEEE MA-L assignment C0-74-15, in the order a scanner prints it.
 *
 * The OUI occupies the three most significant octets of a 48-bit IEEE address.
 * Note the two byte orders in play: HCI and RivieraWaves' NVDS carry the
 * address little-endian (index 5 is the first octet a human reads), while
 * these constants are in display order.  Mixing them up produces an address
 * that looks plausible and is not ours, so the accessors below exist to keep
 * the conversion in one place.
 */
#define IPRO_IEEE_OUI_B0            0xC0
#define IPRO_IEEE_OUI_B1            0x74
#define IPRO_IEEE_OUI_B2            0x15

/** The OUI packed into the low 24 bits, MSB-first: 0x00C07415. */
#define IPRO_IEEE_OUI_U32           (((uint32_t)IPRO_IEEE_OUI_B0 << 16) | \
                                     ((uint32_t)IPRO_IEEE_OUI_B1 << 8)  | \
                                     ((uint32_t)IPRO_IEEE_OUI_B2))

/**
 * @brief Test whether a 48-bit address in display order carries our OUI.
 *
 * @param addr  addr[0] is the octet printed first (the one holding the I/G and
 *              U/L bits), addr[5] the last.
 */
static inline bool ipro_ieee_addr_is_ours_be(const uint8_t addr[6])
{
    return (addr[0] == IPRO_IEEE_OUI_B0) &&
           (addr[1] == IPRO_IEEE_OUI_B1) &&
           (addr[2] == IPRO_IEEE_OUI_B2);
}

/**
 * @brief Test whether a 48-bit address in HCI/NVDS order carries our OUI.
 *
 * @param addr  little-endian, as passed to HCI and stored in NVDS: addr[5] is
 *              the octet printed first.
 */
static inline bool ipro_ieee_addr_is_ours_le(const uint8_t addr[6])
{
    return (addr[5] == IPRO_IEEE_OUI_B0) &&
           (addr[4] == IPRO_IEEE_OUI_B1) &&
           (addr[3] == IPRO_IEEE_OUI_B2);
}

/**
 * @brief Write the OUI into the top three octets of an HCI/NVDS-order address.
 *
 * Leaves addr[0..2] - the 24 bits the manufacturer assigns per unit - alone.
 */
static inline void ipro_ieee_addr_set_oui_le(uint8_t addr[6])
{
    addr[5] = IPRO_IEEE_OUI_B0;
    addr[4] = IPRO_IEEE_OUI_B1;
    addr[3] = IPRO_IEEE_OUI_B2;
}

#endif /* IPRO_VENDOR_IDS_H */
