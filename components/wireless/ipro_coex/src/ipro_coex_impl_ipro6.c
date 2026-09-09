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
 * @file ipro_coex_impl_ipro6.c
 * @brief IPRO6 backend for ipro_coex. Modelled on the reference's
 *        wifi_bt_coex_impl_bl808.c.
 *
 * Base address: the reference uses 0x24920400 for the PTA block, macsw and the
 * 802.15.4 HAL use 0x24920000. Verified on hardware (028HXHKN) that these are
 * the SAME registers, aliased: writing 0x50030000 to 0x24920004 reads back at
 * 0x24920404, and restoring tracks. So the reference's offset map ports
 * verbatim; only the base differs, and either would work.
 */
#include <stdio.h>
#include <stdint.h>

#include <ipro_coex.h>

#include "ipro_coex_impl.h"
#include "ipro_coex_ctx.h"

#define PTA_REG_BASE_ADDR               (0x24920000)
#define PTA_REG_REVISION                (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x00))
#define PTA_REG_CONFIG                  (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x04))
#define PTA_REG_BT_TX                   (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x08))
#define PTA_REG_BT_TX_ABORT             (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x0C))
#define PTA_REG_BT_RX                   (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x10))
#define PTA_REG_BT_RX_ABORT             (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x14))
#define PTA_REG_WLAN_TX                 (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x18))
#define PTA_REG_WLAN_TX_ABORT           (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x1C))
#define PTA_REG_WLAN_RX                 (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x20))
#define PTA_REG_WLAN_RX_ABORT           (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x24))
#define PTA_REG_CONFIG2                 (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x28))

/* PTA CONFIG fields. The reference writes these as bare shifts; naming them is
 * the point of having this file. */
#define PTA_CFG_MAGIC                   (0x50000000u)  /* fixed upper nibble the reference always ORs in */
#define PTA_CFG_PTA_ENABLE              (1u <<  0)
#define PTA_CFG_RF1_BASIC_PRIORITY      (1u <<  1)     /* 1 = WLAN, 0 = BT */
#define PTA_CFG_NO_SIM_TX               (1u <<  2)
#define PTA_CFG_NO_SIM_RX               (1u <<  3)
#define PTA_CFG_RF1_PTI_ENABLE          (1u <<  4)
#define PTA_CFG_SW_BTDM_TX_ABORT        (1u << 16)
#define PTA_CFG_SW_BTDM_RX_ABORT        (1u << 17)
#define PTA_CFG_SW_WLAN_TX_ABORT        (1u << 18)
#define PTA_CFG_SW_WLAN_RX_ABORT        (1u << 19)
#define PTA_CFG_ABORTRX_WHEN_TX         (1u << 20)

/* PTA CONFIG2. Only bit 1 is implemented on IPRO6: writing the reference value
 * 0x0A reads back 0x02. Bit 3 (Rf2_pti_enable) and bit 5 (final_pti_enable)
 * address a second RF path this part does not have and read as zero. Bit-walked
 * on 028HXHKN: 0x0A->0x02, 0x08->0x00, 0x02->0x02, 0x2A->0x02. The default
 * below keeps the reference value anyway so the block stays bit-exact. */
#define PTA_CFG2_RF1_BZ_PTI_ENABLE      (1u <<  1)
#define PTA_CFG2_DEFAULT                (0x0000000Au)

#define WLAN_COEX_REG_BASE_ADDR         (0x24B00400)
#define WLAN_COEX_REG_CoexControlReg    (*(volatile uint32_t *)(WLAN_COEX_REG_BASE_ADDR + 0x00))
#define WLAN_COEX_REG_CoexPTIReg        (*(volatile uint32_t *)(WLAN_COEX_REG_BASE_ADDR + 0x04))
#define WLAN_COEX_REG_CoexStatReg       (*(volatile uint32_t *)(WLAN_COEX_REG_BASE_ADDR + 0x08))
#define WLAN_COEX_REG_CoexIntReg        (*(volatile uint32_t *)(WLAN_COEX_REG_BASE_ADDR + 0x0C))
/* CoexControl2 is a vendor addition: macsw's own generated reg_mac_core.h
 * stops at CoexInt (0x40C), which is why it could only ever be written raw. */
#define WLAN_COEX_REG_CoexControl2Reg   (*(volatile uint32_t *)(WLAN_COEX_REG_BASE_ADDR + 0x10))

#define COEX_CTRL2_FORCE_BK             (1u << 0)
#define COEX_CTRL2_FORCE_BE             (1u << 1)
#define COEX_CTRL2_FORCE_VI             (1u << 2)
#define COEX_CTRL2_FORCE_VO             (1u << 3)
#define COEX_CTRL2_FORCE_BCN            (1u << 4)

/* CoexStat: the only two bits that answer "is BT taking the medium from us
 * right now". Both zero through a whole stall means coex is enabled but idle,
 * and is not the reason anything failed to transmit. */
#define COEX_STAT_WLAN_TX_ABORT_STATE   (1u << 0)
#define COEX_STAT_WLAN_RX_ABORT_STATE   (1u << 1)

/* What a MAC reset leaves behind: arbitrated, PTI priority to WLAN, no
 * simultaneous tx/rx, rx aborted while we transmit. */
#define PTA_CFG_DEFAULT     (PTA_CFG_MAGIC | PTA_CFG_PTA_ENABLE |          \
                             PTA_CFG_RF1_BASIC_PRIORITY | PTA_CFG_NO_SIM_TX | \
                             PTA_CFG_NO_SIM_RX | PTA_CFG_RF1_PTI_ENABLE |  \
                             PTA_CFG_ABORTRX_WHEN_TX)

/* The MAC-side half of the same default. hal_machw_init() reaches these two
 * through the nxmac_ accessors just before it calls us; we restore them by
 * value as well so that "default" also means something after the PTI_PRIORITY
 * modes below, which overwrite both. Without this, coming back from mode 2 or
 * 3 would leave CoexControl at 0xF0000F59 and the PTI table at all-ones and
 * only a reboot would undo it.
 *
 * 0x49 is what the reference sequence converges on: CONTROL_RESET (0x68),
 * coexEnable (bit 0) set, autoPTIAdj (bit 5) cleared. */
#define COEX_CTRL_DEFAULT               (0x00000049u)
#define COEX_CTRL_DEFAULT_DISABLED      (0x00000048u)  /* same, coexEnable clear */
#define COEX_PTI_DEFAULT                (0x0024F637u)

static enum IPRO_COEX_FORCE_MODE ipro_coex_ipro6_mode = IPRO_COEX_FORCE_MODE_PTA_FORCE;

int ipro_coex_dump_all_impl(void)
{
    uint32_t val;

    printf("============IPRO6 PTA Reg Dump============\r\n");
    printf("REVISIOIN       %08lx\r\n", (unsigned long)PTA_REG_REVISION);

    val = PTA_REG_CONFIG;
    printf("CONFIG          %08lx\r\n", (unsigned long)val);
    printf("    pta_en  %lu  rf1_prio %lu  no_sim_tx %lu  no_sim_rx %lu\r\n",
           (unsigned long)!!(val & PTA_CFG_PTA_ENABLE),
           (unsigned long)!!(val & PTA_CFG_RF1_BASIC_PRIORITY),
           (unsigned long)!!(val & PTA_CFG_NO_SIM_TX),
           (unsigned long)!!(val & PTA_CFG_NO_SIM_RX));
    printf("    rf1_pti %lu  abortrx_when_tx %lu\r\n",
           (unsigned long)!!(val & PTA_CFG_RF1_PTI_ENABLE),
           (unsigned long)!!(val & PTA_CFG_ABORTRX_WHEN_TX));
    printf("    sw_abort: btdm_tx %lu btdm_rx %lu wlan_tx %lu wlan_rx %lu"
           "   (only act while pta_en=0)\r\n",
           (unsigned long)!!(val & PTA_CFG_SW_BTDM_TX_ABORT),
           (unsigned long)!!(val & PTA_CFG_SW_BTDM_RX_ABORT),
           (unsigned long)!!(val & PTA_CFG_SW_WLAN_TX_ABORT),
           (unsigned long)!!(val & PTA_CFG_SW_WLAN_RX_ABORT));

    printf("BT TX           %08lx\r\n", (unsigned long)PTA_REG_BT_TX);
    printf("BT TX Abort     %08lx\r\n", (unsigned long)PTA_REG_BT_TX_ABORT);
    printf("BT RX           %08lx\r\n", (unsigned long)PTA_REG_BT_RX);
    printf("BT RX Abort     %08lx\r\n", (unsigned long)PTA_REG_BT_RX_ABORT);
    printf("WLAN TX         %08lx\r\n", (unsigned long)PTA_REG_WLAN_TX);
    printf("WLAN TX Abort   %08lx\r\n", (unsigned long)PTA_REG_WLAN_TX_ABORT);
    printf("WLAN RX         %08lx\r\n", (unsigned long)PTA_REG_WLAN_RX);
    printf("WLAN RX Abort   %08lx\r\n", (unsigned long)PTA_REG_WLAN_RX_ABORT);
    printf("CONFIG2         %08lx   (only bit 1 exists on IPRO6)\r\n",
           (unsigned long)PTA_REG_CONFIG2);

    printf("============IPRO6 Coex (Wi-Fi) Reg Dump============\r\n");
    printf("CoexControlReg  %08lx\r\n", (unsigned long)WLAN_COEX_REG_CoexControlReg);

    val = WLAN_COEX_REG_CoexPTIReg;
    printf("CoexPTIReg      %08lx\r\n", (unsigned long)val);
    printf("    ACK     %lu\r\n", (unsigned long)((val >>  0) & 0xF));
    printf("    Cntrl   %lu\r\n", (unsigned long)((val >>  4) & 0xF));
    printf("    Mgt     %lu\r\n", (unsigned long)((val >>  8) & 0xF));
    printf("    VO      %lu\r\n", (unsigned long)((val >> 12) & 0xF));
    printf("    VI      %lu\r\n", (unsigned long)((val >> 16) & 0xF));
    printf("    BE      %lu\r\n", (unsigned long)((val >> 20) & 0xF));
    printf("    BK      %lu\r\n", (unsigned long)((val >> 24) & 0xF));
    printf("    BCN     %lu\r\n", (unsigned long)((val >> 28) & 0xF));

    val = WLAN_COEX_REG_CoexStatReg;
    printf("CoexStatReg     %08lx\r\n", (unsigned long)val);
    printf("    --> BT holding us off:  TX %lu  RX %lu\r\n",
           (unsigned long)!!(val & COEX_STAT_WLAN_TX_ABORT_STATE),
           (unsigned long)!!(val & COEX_STAT_WLAN_RX_ABORT_STATE));

    printf("CoexIntReg      %08lx\r\n", (unsigned long)WLAN_COEX_REG_CoexIntReg);

    val = WLAN_COEX_REG_CoexControl2Reg;
    printf("CoexControl2Reg %08lx\r\n", (unsigned long)val);
    printf("    Force BK    %lu\r\n", (unsigned long)!!(val & COEX_CTRL2_FORCE_BK));
    printf("    Force BE    %lu\r\n", (unsigned long)!!(val & COEX_CTRL2_FORCE_BE));
    printf("    Force VI    %lu\r\n", (unsigned long)!!(val & COEX_CTRL2_FORCE_VI));
    printf("    Force VO    %lu\r\n", (unsigned long)!!(val & COEX_CTRL2_FORCE_VO));
    printf("    Force BCN   %lu\r\n", (unsigned long)!!(val & COEX_CTRL2_FORCE_BCN));

    return 0;
}

int ipro_coex_init_default_impl(void)
{
    /* Drop coexEnable, reload the PTI table, raise it again -- same order the
     * PTI_PRIORITY modes use, so the new table latches. */
    WLAN_COEX_REG_CoexControlReg = COEX_CTRL_DEFAULT_DISABLED;
    WLAN_COEX_REG_CoexPTIReg = COEX_PTI_DEFAULT;
    WLAN_COEX_REG_CoexControlReg = COEX_CTRL_DEFAULT;

    PTA_REG_CONFIG = PTA_CFG_DEFAULT;
    WLAN_COEX_REG_CoexControl2Reg = COEX_CTRL2_FORCE_BK;
    PTA_REG_CONFIG2 = PTA_CFG2_DEFAULT;

    return 0;
}

int ipro_coex_force_wlan_impl(void)
{
    switch (ipro_coex_ipro6_mode) {
    case IPRO_COEX_FORCE_MODE_PTA_FORCE:
        /* Arbiter off. The sw_*_abort bits are only honoured while pta_enable
         * is clear, so this mutes BT outright rather than arbitrating. */
        PTA_REG_CONFIG2 = 0;
        PTA_REG_CONFIG = PTA_CFG_MAGIC | PTA_CFG_SW_BTDM_TX_ABORT |
                         PTA_CFG_SW_BTDM_RX_ABORT;
        break;

    case IPRO_COEX_FORCE_MODE_PTI_DEFAULT_FORCE:
        /* Arbiter on, PTI off: WLAN wins on the fixed basic priority. */
        PTA_REG_CONFIG2 = 0;
        PTA_REG_CONFIG = PTA_CFG_MAGIC | PTA_CFG_PTA_ENABLE |
                         PTA_CFG_RF1_BASIC_PRIORITY;
        break;

    case IPRO_COEX_FORCE_MODE_PTI_PRIORITY_FORCE:
        /* Arbiter on with PTI, and every WLAN traffic class pinned to the top
         * PTI value. CoexControl is written twice around the PTI update: the
         * low bit is coexEnable, so it is dropped and re-raised to make the
         * new PTI table take effect atomically. */
        PTA_REG_CONFIG2 = 0;
        PTA_REG_CONFIG = PTA_CFG_MAGIC | PTA_CFG_PTA_ENABLE |
                         PTA_CFG_RF1_BASIC_PRIORITY | PTA_CFG_RF1_PTI_ENABLE;
        WLAN_COEX_REG_CoexControlReg = 0xF48;
        WLAN_COEX_REG_CoexPTIReg = 0xFFFFFFFF;
        WLAN_COEX_REG_CoexControlReg = 0xF49;
        break;

    case IPRO_COEX_FORCE_MODE_PTI_PRIORITY_FORCE2:
        /* As above but driven from CoexControl's forceWlanPTI field
         * (bits 31:28) instead of the PTI table. */
        PTA_REG_CONFIG2 = 0;
        WLAN_COEX_REG_CoexControlReg = 0xF0000F58;
        PTA_REG_CONFIG = PTA_CFG_MAGIC | PTA_CFG_PTA_ENABLE |
                         PTA_CFG_RF1_BASIC_PRIORITY | PTA_CFG_RF1_PTI_ENABLE;
        WLAN_COEX_REG_CoexControlReg = 0xF0000F59;
        break;

    default:
        return -1;
    }

    return 0;
}

int ipro_coex_force_bt_impl(void)
{
    switch (ipro_coex_ipro6_mode) {
    case IPRO_COEX_FORCE_MODE_PTA_FORCE:
        /* Mutes WiFi outright -- the link will drop. */
        PTA_REG_CONFIG2 = 0;
        PTA_REG_CONFIG = PTA_CFG_MAGIC | PTA_CFG_SW_WLAN_TX_ABORT |
                         PTA_CFG_SW_WLAN_RX_ABORT;
        break;

    case IPRO_COEX_FORCE_MODE_PTI_DEFAULT_FORCE:
        PTA_REG_CONFIG2 = 0;
        PTA_REG_CONFIG = PTA_CFG_MAGIC | PTA_CFG_PTA_ENABLE;
        break;

    case IPRO_COEX_FORCE_MODE_PTI_PRIORITY_FORCE:
        /* Note the asymmetry with the WLAN side: rf1_basic_priority stays set
         * here too, exactly as in the reference. It does not matter, because
         * with PTI enabled the zeroed PTI table is what decides, and basic
         * priority is only the fallback. Kept verbatim rather than "fixed". */
        PTA_REG_CONFIG2 = 0;
        PTA_REG_CONFIG = PTA_CFG_MAGIC | PTA_CFG_PTA_ENABLE |
                         PTA_CFG_RF1_BASIC_PRIORITY | PTA_CFG_RF1_PTI_ENABLE;
        WLAN_COEX_REG_CoexControlReg = 0x48;
        WLAN_COEX_REG_CoexPTIReg = 0x00;
        WLAN_COEX_REG_CoexControlReg = 0x49;
        break;

    case IPRO_COEX_FORCE_MODE_PTI_PRIORITY_FORCE2:
        PTA_REG_CONFIG2 = 0;
        WLAN_COEX_REG_CoexControlReg = 0x048;
        PTA_REG_CONFIG = PTA_CFG_MAGIC | PTA_CFG_PTA_ENABLE |
                         PTA_CFG_RF1_BASIC_PRIORITY | PTA_CFG_RF1_PTI_ENABLE;
        WLAN_COEX_REG_CoexControlReg = 0x049;
        break;

    default:
        return -1;
    }

    return 0;
}

int ipro_coex_force_mode_impl(enum IPRO_COEX_FORCE_MODE mode)
{
    ipro_coex_ipro6_mode = mode;

    return 0;
}

int ipro_coex_event_handler_impl(enum IPRO_COEX_EVENT event, void *event_arg)
{
    (void)event_arg;

    switch (event) {
    case IPRO_COEX_EVENT_BT_A2DP_UNDERRUN:
        ipro_coex_bt_inc(&ipro_coex_timing_control_ctx, 1);
        break;

    default:
        break;
    }

    return 0;
}
