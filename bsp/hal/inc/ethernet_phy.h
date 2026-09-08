/**
 * @file ethernet_phy.h
 * @brief
 *
 * Copyright (c) 2021 IntelPro team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#ifndef __ETHERNET_PHY_H__
#define __ETHERNET_PHY_H__

#include "misc.h"
#include "mii.h"

typedef enum emac_phy_status {
    EMAC_PHY_STAT_EEROR,
    EMAC_PHY_STAT_LINK_DOWN,
    EMAC_PHY_STAT_LINK_INIT,
    EMAC_PHY_STAT_LINK_UP,
    EMAC_PHY_STAT_100MBITS_FULLDUPLEX,
    EMAC_PHY_STAT_100MBITS_HALFDUPLEX,
    EMAC_PHY_STAT_10MBITS_FULLDUPLEX,
    EMAC_PHY_STAT_10MBITS_HALFDUPLEX,
} emac_phy_status_t;

#define PHY_RTL8211x_FORCE_MASTER BIT(1)
#define PHY_RTL8211F_FORCE_EEE_RXC_ON BIT(3)
#define PHY_RTL8201F_S700_RMII_TIMINGS BIT(4)

/* LAN8720 PHY Address*/
#define EMAC_PHY_ADDRESS          0x00U
#define PHY_LINK_TO               ((uint32_t)0x00000FFFU)
#define PHY_AUTONEGO_COMPLETED_TO ((uint32_t)0x00000FFFU)
/* Section 3: Common PHY Registers */
#define PHY_BCR                     ((uint16_t)0x00U) /*!< Transceiver Basic Control Register   */
#define PHY_BSR                     ((uint16_t)0x01U) /*!< Transceiver Basic Status Register    */
#define PHY_BSR_100BASETXFULL       (1 << 14)
#define PHY_BSR_100BASETXHALF       (1 << 13)
#define PHY_BSR_10BASETXFULL        (1 << 12)
#define PHY_BSR_10BASETXHALF        (1 << 11)
#define PHY_RESET                   ((uint16_t)0x8000U) /*!< PHY Reset */
#define PHY_LOOPBACK                ((uint16_t)0x4000U) /*!< Select loop-back mode */
#define PHY_FULLDUPLEX_100M         ((uint16_t)0x2100U) /*!< Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M         ((uint16_t)0x2000U) /*!< Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M          ((uint16_t)0x0100U) /*!< Set the full-duplex mode at 10 Mb/s  */
#define PHY_HALFDUPLEX_10M          ((uint16_t)0x0000U) /*!< Set the half-duplex mode at 10 Mb/s  */
#define PHY_AUTONEGOTIATION         ((uint16_t)0x1000U) /*!< Enable auto-negotiation function     */
#define PHY_RESTART_AUTONEGOTIATION ((uint16_t)0x0200U) /*!< Restart auto-negotiation function    */
#define PHY_POWERDOWN               ((uint16_t)0x0800U) /*!< Select the power down mode           */
#define PHY_ISOLATE                 ((uint16_t)0x0400U) /*!< Isolate PHY from MII                 */
#define PHY_AUTONEGO_COMPLETE       ((uint16_t)0x0020U) /*!< Auto-Negotiation process completed   */
#define PHY_LINKED_STATUS           ((uint16_t)0x0004U) /*!< Valid link established               */
#define PHY_JABBER_DETECTION        ((uint16_t)0x0002U) /*!< Jabber condition detected            */

#define PHY_PHYID1                  ((uint16_t)0x02U) /*!< PHY ID 1    */
#define PHY_PHYID2                  ((uint16_t)0x03U) /*!< PHY ID 2    */
#define PHY_ADVERTISE               ((uint16_t)0x04U) /*!< Auto-negotiation advertisement       */
#define PHY_ADVERTISE_100BASETXFULL (1 << 8)
#define PHY_ADVERTISE_100BASETXHALF (1 << 7)
#define PHY_ADVERTISE_10BASETXFULL  (1 << 6)
#define PHY_ADVERTISE_10BASETXHALF  (1 << 5)
#define PHY_ADVERTISE_8023          (1 << 0)
#define PHY_LPA                     ((uint16_t)0x05U) /*!< Auto-negotiation link partner base page ability    */
#define PHY_EXPANSION               ((uint16_t)0x06U) /*!< Auto-negotiation expansion           */
/* Section 4: Extended PHY Registers */
#define PHY_CTRL_STATUS           ((uint16_t)17U) /*!< PHY MODE CONTROL/STATUS REGISTER Offset     */
#define PHY_CTRL_STATUS_EDPWRDOWN (1 << 13)       /*!< EDPWRDOWN */
#define PHY_CTRL_STATUS_ENERGYON  (1 << 1)        /*!< ENERGYON */

#define PHY_SPECIAL_MODES              ((uint16_t)18U) /*!< PHY SPECIAL MODES REGISTER Offset     */
#define PHY_SPECIAL_MODES_MODE         (7 << 5)        /*!< Transceiver mode of operation */
#define PHY_SPECIAL_MODES_MODE_PWRDOWN (6 << 5)        /*!< All capable. Auto-negotiation enabled */
#define PHY_SPECIAL_MODES_MODE_ALL     (7 << 5)        /*!< All capable. Auto-negotiation enabled */
#define PHY_SPECIAL_MODES_PHYAD        (0x1F << 0)     /*!< PHY Address */

#define PHY_SR                                  ((uint16_t)0x1FU) /*!< PHY special control/ status register Offset     */
#define PHY_SR_SPEED_OFFSET                     (2)
#define PHY_SR_SPEED_MASK                       (0x7 << PHY_SR_SPEED_OFFSET)
#define PHY_SR_SPEED_10BASETXHALF               (0x1 << PHY_SR_SPEED_OFFSET)
#define PHY_SR_SPEED_10BASETXFULL               (0x5 << PHY_SR_SPEED_OFFSET)
#define PHY_SR_SPEED_100BASETXHALF              (0x2 << PHY_SR_SPEED_OFFSET)
#define PHY_SR_SPEED_100BASETXFULL              (0x6 << PHY_SR_SPEED_OFFSET)
#define PHY_SR_SPEED_MODE_COMPARE(status, mode) (!!(mode == (status & PHY_SR_SPEED_MASK)))
#define PHY_SPEED_STATUS                        ((uint16_t)0x0004U) /*!< PHY Speed mask                                  */
#define PHY_DUPLEX_STATUS                       ((uint16_t)0x0010U) /*!< PHY Duplex mask                                 */
#define PHY_ISFR                                ((uint16_t)0x1DU)   /*!< PHY Interrupt Source Flag register Offset       */
#define PHY_ISFR_INT4                           ((uint16_t)0x0010U) /*!< PHY Link down inturrupt                         */

#define PHY_GMII2RGMII_ID	0x5a5a5a5a

#define PHY_MAX_ADDR 32

#define PHY_FLAG_BROKEN_RESET	(1 << 0) /* soft reset not supported */

#define SUPPORTED_10baseT_Half		(1 << 0)
#define SUPPORTED_10baseT_Full		(1 << 1)
#define SUPPORTED_100baseT_Half		(1 << 2)
#define SUPPORTED_100baseT_Full		(1 << 3)
#define SUPPORTED_1000baseT_Half	(1 << 4)
#define SUPPORTED_1000baseT_Full	(1 << 5)
#define SUPPORTED_Autoneg		(1 << 6)
#define SUPPORTED_TP			(1 << 7)
#define SUPPORTED_AUI			(1 << 8)
#define SUPPORTED_MII			(1 << 9)
#define SUPPORTED_FIBRE			(1 << 10)
#define SUPPORTED_BNC			(1 << 11)
#define SUPPORTED_10000baseT_Full	(1 << 12)
#define SUPPORTED_Pause			(1 << 13)
#define SUPPORTED_Asym_Pause		(1 << 14)
#define SUPPORTED_2500baseX_Full	(1 << 15)
#define SUPPORTED_Backplane		(1 << 16)
#define SUPPORTED_1000baseKX_Full	(1 << 17)
#define SUPPORTED_10000baseKX4_Full	(1 << 18)
#define SUPPORTED_10000baseKR_Full	(1 << 19)
#define SUPPORTED_10000baseR_FEC	(1 << 20)
#define SUPPORTED_1000baseX_Half	(1 << 21)
#define SUPPORTED_1000baseX_Full	(1 << 22)
                 
                 /* Indicates what features are advertised by the interface. */
#define ADVERTISED_10baseT_Half		(1 << 0)
#define ADVERTISED_10baseT_Full		(1 << 1)
#define ADVERTISED_100baseT_Half	(1 << 2)
#define ADVERTISED_100baseT_Full	(1 << 3)
#define ADVERTISED_1000baseT_Half	(1 << 4)
#define ADVERTISED_1000baseT_Full	(1 << 5)
#define ADVERTISED_Autoneg		(1 << 6)
#define ADVERTISED_TP			(1 << 7)
#define ADVERTISED_AUI			(1 << 8)
#define ADVERTISED_MII			(1 << 9)
#define ADVERTISED_FIBRE		(1 << 10)
#define ADVERTISED_BNC			(1 << 11)
#define ADVERTISED_10000baseT_Full	(1 << 12)
#define ADVERTISED_Pause		(1 << 13)
#define ADVERTISED_Asym_Pause		(1 << 14)
#define ADVERTISED_2500baseX_Full	(1 << 15)
#define ADVERTISED_Backplane		(1 << 16)
#define ADVERTISED_1000baseKX_Full	(1 << 17)
#define ADVERTISED_10000baseKX4_Full	(1 << 18)
#define ADVERTISED_10000baseKR_Full	(1 << 19)
#define ADVERTISED_10000baseR_FEC	(1 << 20)
#define ADVERTISED_1000baseX_Half	(1 << 21)
#define ADVERTISED_1000baseX_Full	(1 << 22)

#define PHY_DEFAULT_FEATURES	(SUPPORTED_Autoneg | \
				 SUPPORTED_TP | \
				 SUPPORTED_MII)

#define PHY_10BT_FEATURES	(SUPPORTED_10baseT_Half | \
				 SUPPORTED_10baseT_Full)

#define PHY_100BT_FEATURES	(SUPPORTED_100baseT_Half | \
				 SUPPORTED_100baseT_Full)

#define PHY_1000BT_FEATURES	(SUPPORTED_1000baseT_Half | \
				 SUPPORTED_1000baseT_Full)

#define PHY_BASIC_FEATURES	(PHY_10BT_FEATURES | \
				 PHY_100BT_FEATURES | \
				 PHY_DEFAULT_FEATURES)

#define PHY_GBIT_FEATURES	(PHY_BASIC_FEATURES | \
				 PHY_1000BT_FEATURES)

#define PHY_10G_FEATURES	(PHY_GBIT_FEATURES | \
				SUPPORTED_10000baseT_Full)

#ifndef PHY_ANEG_TIMEOUT
#define PHY_ANEG_TIMEOUT	4000
#endif

typedef struct
{
    uint8_t auto_negotiation; /*!< Speed and mode auto negotiation */
    uint8_t full_duplex;      /*!< Duplex mode */
#define PHY_STATE_DOWN    (0) /* PHY is not usable */
#define PHY_STATE_READY   (1) /* PHY is OK, wait for controller */
#define PHY_STATE_UP      (2) /* Network is ready for TX/RX */
#define PHY_STATE_RUNNING (3) /* working */
#define PHY_STATE_NOLINK  (4) /* no cable connected */
#define PHY_STATE_STOPPED (5) /* PHY has been stopped */
#define PHY_STATE_TESTING (6) /* in test mode */
    uint8_t phy_state;        /*!< down,ready,up,running,nolink,halted */
    uint8_t use_irq;          /*!< 0: no IRQ used */
    uint16_t speed;           /*!< Speed mode */
    uint16_t phy_address;     /*!< PHY address */
    uint32_t phy_id;          /*!< PHY OUI */
    uint32_t flags;
} emac_phy_cfg_t;

emac_phy_status_t ethernet_phy_status_get();
int emac_phy_init(emac_phy_cfg_t *cfg);

#endif
