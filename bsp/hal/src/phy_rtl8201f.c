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

#include <string.h>
#include "hal_emac.h"
#include "platform_common.h"

#define PHY_AUTONEGOTIATE_TIMEOUT 5000

/* RTL8211x 1000BASE-T Control Register */
#define MIIM_RTL8211x_CTRL1000T_MSCE BIT(12);
#define MIIM_RTL8211x_CTRL1000T_MASTER BIT(11);

/* RTL8211x PHY Status Register */
#define MIIM_RTL8211x_PHY_STATUS       0x11
#define MIIM_RTL8211x_PHYSTAT_SPEED    0xc000
#define MIIM_RTL8211x_PHYSTAT_GBIT     0x8000
#define MIIM_RTL8211x_PHYSTAT_100      0x4000
#define MIIM_RTL8211x_PHYSTAT_DUPLEX   0x2000
#define MIIM_RTL8211x_PHYSTAT_SPDDONE  0x0800
#define MIIM_RTL8211x_PHYSTAT_LINK     0x0400

/* RTL8211x PHY Interrupt Enable Register */
#define MIIM_RTL8211x_PHY_INER         0x12
#define MIIM_RTL8211x_PHY_INTR_ENA     0x9f01
#define MIIM_RTL8211x_PHY_INTR_DIS     0x0000

/* RTL8211x PHY Interrupt Status Register */
#define MIIM_RTL8211x_PHY_INSR         0x13

/* RTL8211F PHY Status Register */
#define MIIM_RTL8211F_PHY_STATUS       0x1a
#define MIIM_RTL8211F_AUTONEG_ENABLE   0x1000
#define MIIM_RTL8211F_PHYSTAT_SPEED    0x0030
#define MIIM_RTL8211F_PHYSTAT_GBIT     0x0020
#define MIIM_RTL8211F_PHYSTAT_100      0x0010
#define MIIM_RTL8211F_PHYSTAT_DUPLEX   0x0008
#define MIIM_RTL8211F_PHYSTAT_SPDDONE  0x0800
#define MIIM_RTL8211F_PHYSTAT_LINK     0x0004

#define MIIM_RTL8211E_CONFREG		0x1c
#define MIIM_RTL8211E_CTRL_DELAY	BIT(13)
#define MIIM_RTL8211E_TX_DELAY		BIT(12)
#define MIIM_RTL8211E_RX_DELAY		BIT(11)

#define MIIM_RTL8211E_EXT_PAGE_SELECT  0x1e

#define MIIM_RTL8211F_PAGE_SELECT      0x1f
#define MIIM_RTL8211F_TX_DELAY		0x100
#define MIIM_RTL8211F_RX_DELAY		0x8
#define MIIM_RTL8211F_LCR		0x10

#define RTL8201F_RMSR			0x10

#define RMSR_RX_TIMING_SHIFT	BIT(2)
#define RMSR_RX_TIMING_MASK		(0xf << 4)
#define RMSR_RX_TIMING_VAL		0x4
#define RMSR_TX_TIMING_SHIFT	BIT(3)
#define RMSR_TX_TIMING_MASK		(0xf << 8)
#define RMSR_TX_TIMING_VAL		0x5

static emac_phy_cfg_t *phy_8720_cfg = NULL;

static int genphy_config_advert(emac_phy_cfg_t *cfg)
{
	uint32_t advertise;
	uint16_t oldadv, adv, bmsr;
	int err, changed = 0;

	/* Only allow advertising what this PHY supports */
	advertise = cfg->flags;

	/* Setup standard advertisement */
	emac_phy_reg_read(MII_ADVERTISE, &adv);
	oldadv = adv;

	if (adv < 0)
		return adv;

	adv &= ~(ADVERTISE_ALL | ADVERTISE_100BASE4 | ADVERTISE_PAUSE_CAP |
		 ADVERTISE_PAUSE_ASYM);
	if (advertise & ADVERTISED_10baseT_Half)
		adv |= ADVERTISE_10HALF;
	if (advertise & ADVERTISED_10baseT_Full)
		adv |= ADVERTISE_10FULL;
#if (SPEED_SEL_MODE == 1)
	if (advertise & ADVERTISED_100baseT_Half)
		adv |= ADVERTISE_100HALF;
	if (advertise & ADVERTISED_100baseT_Full)
		adv |= ADVERTISE_100FULL;
#endif
	if (advertise & ADVERTISED_Pause)
		adv |= ADVERTISE_PAUSE_CAP;
	if (advertise & ADVERTISED_Asym_Pause)
		adv |= ADVERTISE_PAUSE_ASYM;
#if (SPEED_SEL_MODE == 1)
	if (advertise & ADVERTISED_1000baseX_Half)
		adv |= ADVERTISE_1000XHALF;
	if (advertise & ADVERTISED_1000baseX_Full)
		adv |= ADVERTISE_1000XFULL;
#endif
	if (adv != oldadv) {
		err = emac_phy_reg_write(MII_ADVERTISE, adv);

		if (err < 0)
			return err;
		changed = 1;
	}

	emac_phy_reg_read(MII_BMSR, &bmsr);
	if (bmsr < 0)
		return bmsr;

	/* Per 802.3-2008, Section 22.2.4.2.16 Extended status all
	 * 1000Mbits/sec capable PHYs shall have the BMSR_ESTATEN bit set to a
	 * logical 1.
	 */
	if (!(bmsr & BMSR_ESTATEN))
		return changed;

	/* Configure gigabit if it's supported */
	emac_phy_reg_read(MII_CTRL1000, &adv);
	oldadv = adv;

	if (adv < 0)
		return adv;

	adv &= ~(ADVERTISE_1000FULL | ADVERTISE_1000HALF);

	if (cfg->flags & (SUPPORTED_1000baseT_Half |
				SUPPORTED_1000baseT_Full)) {
		if (advertise & SUPPORTED_1000baseT_Half)
			adv |= ADVERTISE_1000HALF;
		if (advertise & SUPPORTED_1000baseT_Full)
			adv |= ADVERTISE_1000FULL;
	}

	if (adv != oldadv)
		changed = 1;

	err = emac_phy_reg_write(MII_CTRL1000, adv);
	if (err < 0)
		return err;

	return changed;
}

static int genphy_setup_forced(emac_phy_cfg_t *cfg)
{
	int err;
	int ctl = BMCR_ANRESTART;

	if (cfg->speed == 10)
		ctl |= BMCR_SPEED10;
	else if (cfg->speed == 100)
		ctl |= BMCR_SPEED100;

	if (cfg->full_duplex)
		ctl |= BMCR_FULLDPLX;

	err = emac_phy_reg_write(MII_BMCR, ctl);

	return err;
}

int genphy_restart_aneg(emac_phy_cfg_t *cfg)
{
	int ctl;

    emac_phy_reg_read(MII_BMCR, (uint16_t *)&ctl);

	if (ctl < 0)
		return ctl;

	ctl |= (BMCR_ANENABLE | BMCR_ANRESTART);

	/* Don't isolate the PHY if we're negotiating */
	ctl &= ~(BMCR_ISOLATE);

	ctl = emac_phy_reg_write(MII_BMCR, ctl);

	return ctl;
}

int genphy_config_aneg(emac_phy_cfg_t *cfg)
{
	int result;

	if (!cfg->auto_negotiation)
		return genphy_setup_forced(cfg);

	result = genphy_config_advert(cfg);

	if (result < 0) /* error */
		return result;

	if (result == 0) {
		/*
		 * Advertisment hasn't changed, but maybe aneg was never on to
		 * begin with?  Or maybe phy was isolated?
		 */
		uint16_t ctl = -1;
        emac_phy_reg_read(MII_BMCR, &ctl);

		if (ctl < 0)
			return ctl;

		if (!(ctl & BMCR_ANENABLE) || (ctl & BMCR_ISOLATE))
			result = 1; /* do restart aneg */
	}
    printf("genphy_config_aneg result = %d\r\n", result);
	/*
	 * Only restart aneg if we are advertising something different
	 * than we were before.
	 */
	if (result > 0)
		result = genphy_restart_aneg(cfg);

	return result;
}
static int rtl8211f_config(emac_phy_cfg_t *cfg)
{
    uint16_t reg, phyid1, phyid2;

	emac_phy_reg_write(MII_BMCR, BMCR_RESET);
    arch_delay_ms(20);

    if (0 != emac_phy_reg_read(PHY_PHYID1, &phyid1)) {
        MSG("read emac phy id 1 error\r\n");
        return -1;
    }

    if (0 != emac_phy_reg_read(PHY_PHYID2, &phyid2)) {
        MSG("read emac phy id 2 error\r\n");
        return -1;
    }
    MSG("emac phy id 1 =%08x, phy id 2 = %08x, phy_id = %08lx\r\n",
        phyid1, phyid2, ((((uint32_t)phyid1 << 16) | phyid2) & 0x000FFFF0));
    if (cfg->phy_id != (((phyid1 << 16) | phyid2) & 0x00FFFFF0)) {
        /* ID error */
        return -1;
    } else {
        cfg->phy_id = (phyid1 << 16) | phyid2;
    }

#if 1
	if (cfg->flags & PHY_RTL8201F_S700_RMII_TIMINGS) {
        emac_phy_reg_write(MIIM_RTL8211F_PAGE_SELECT, 7);
        emac_phy_reg_read(RTL8201F_RMSR, &reg);
        printf("rmsr reg = 0x%x\r\n", reg);
        reg |= (1 << 3); // RMII
        //reg &= ~(RMSR_RX_TIMING_MASK | RMSR_TX_TIMING_MASK);
        /* Set the needed Rx/Tx Timings for proper PHY operation */
        //reg |= (RMSR_RX_TIMING_VAL << RMSR_RX_TIMING_SHIFT)
        //       | (RMSR_TX_TIMING_VAL << RMSR_TX_TIMING_SHIFT);
//        reg = 0xFFB;
        printf("set reg = 0x%x\r\n", reg);
        emac_phy_reg_write(RTL8201F_RMSR, reg);
        emac_phy_reg_write(MIIM_RTL8211F_PAGE_SELECT, 0);
    }
#endif
    genphy_config_aneg(cfg);

	return 0;
}

int genphy_parse_link(emac_phy_cfg_t *cfg)
{
    uint16_t mii_reg;
	emac_phy_reg_read(MII_BMSR, &mii_reg);

	/* We're using autonegotiation */
	if (cfg->auto_negotiation) {
		uint16_t lpa = 0, tmp = 0;
		uint16_t gblpa = 0;
		uint16_t estatus = 0;

		/* Check for gigabit capability */
		if (cfg->flags & (SUPPORTED_1000baseT_Full |
					SUPPORTED_1000baseT_Half)) {
			/* We want a list of states supported by
			 * both PHYs in the link
			 */
			emac_phy_reg_read(MII_STAT1000, &gblpa);
			if (gblpa < 0) {
				printf("Could not read MII_STAT1000. ");
				printf("Ignoring gigabit capability\r\n");
				gblpa = 0;
			}
            emac_phy_reg_read(MII_CTRL1000, &gblpa);
			gblpa &= gblpa << 2;
		}

		/* Set the baseline so we only have to set them
		 * if they're different
		 */
		cfg->speed = 10;
		cfg->full_duplex = 0;


		emac_phy_reg_read(MII_ADVERTISE, &tmp);
        emac_phy_reg_read(MII_LPA, &lpa);
		lpa = tmp & lpa;

		if (lpa & (LPA_100FULL | LPA_100HALF)) {
			cfg->speed = 100;

			if (lpa & LPA_100FULL)
				cfg->full_duplex = 1;

		} else if (lpa & LPA_10FULL) {
			cfg->full_duplex = 1;
		}
		emac_phy_reg_read(MII_BMCR, &tmp);

		/*
		 * Extended status may indicate that the PHY supports
		 * 1000BASE-T/X even though the 1000BASE-T registers
		 * are missing. In this case we can't tell whether the
		 * peer also supports it, so we only check extended
		 * status if the 1000BASE-T registers are actually
		 * missing.
		 */
		if ((mii_reg & BMSR_ESTATEN) && !(mii_reg & BMSR_ERCAP))
			emac_phy_reg_read(MII_ESTATUS, &estatus);

	} else {
	    uint16_t bmcr;
		emac_phy_reg_read(MII_BMCR, &bmcr);

		cfg->speed = 10;
		cfg->full_duplex = 0;

		if (bmcr & BMCR_FULLDPLX)
			cfg->full_duplex = 1;

		if (bmcr & BMCR_SPEED100)
            cfg->speed = 100;
	}
    emac_phy_config_full_duplex(cfg->full_duplex);

	return 0;
}


int genphy_update_link(emac_phy_cfg_t *cfg)
{
	uint16_t reg;

	/*
	 * Wait if the link is up, and autonegotiation is in progress
	 * (ie - we're capable and it's not done)
	 */
	emac_phy_reg_read(MII_BMSR, &reg);

	/*
	 * If we already saw the link up, and it hasn't gone down, then
	 * we don't need to wait for autoneg again
	 */
	if (cfg->phy_state == PHY_STATE_UP && reg & BMSR_LSTATUS)
		return 0;

	if ((cfg->auto_negotiation) && !(reg & BMSR_ANEGCOMPLETE)) {
		int i = 0;

		printf("Waiting for PHY auto negotiation to complete\r\n");
		while (!(reg & BMSR_ANEGCOMPLETE)) {
			/*
			 * Timeout reached ?
			 */
			if (i > (PHY_ANEG_TIMEOUT)) {
				printf(" TIMEOUT !\r\n");
				cfg->phy_state = PHY_STATE_DOWN;
				return -1;
			}

			if ((i++ % 10) == 0)
				printf(".");

			emac_phy_reg_read(MII_BMSR, &reg);
			arch_delay_ms(50);	/* 50 ms */
//            printf("%s:%d, reg = %x\r\n", __func__, __LINE__, reg); 
		}
		printf(" done\r\n");
        cfg->phy_state = PHY_STATE_UP;
	} else {
		/* Read the link a second time to clear the latched state */
		emac_phy_reg_read(MII_BMSR, &reg);

		if (reg & BMSR_LSTATUS)
			cfg->phy_state = PHY_STATE_UP;
		else
			cfg->phy_state = PHY_STATE_DOWN;
	}

    emac_phy_reg_write(MIIM_RTL8211F_PAGE_SELECT, 7);
    emac_phy_reg_read(RTL8201F_RMSR, &reg);
    printf("RTL8201F_RMSR = 0x%x\r\n", reg);
    emac_phy_reg_write(MIIM_RTL8211F_PAGE_SELECT, 0);

	return 0;
}

void update_phy_timing(int tx, int rx)
{
    uint16_t reg;
    emac_phy_reg_write(MIIM_RTL8211F_PAGE_SELECT, 7);
    emac_phy_reg_read(RTL8201F_RMSR, &reg);
    reg &= ~(RMSR_RX_TIMING_MASK | RMSR_TX_TIMING_MASK);
    /* Set the needed Rx/Tx Timings for proper PHY operation */
    reg |= (rx << RMSR_RX_TIMING_SHIFT)
           | (tx << RMSR_TX_TIMING_SHIFT);
    printf("set reg = 0x%x\r\n", reg);
    emac_phy_reg_write(RTL8201F_RMSR, reg);
    emac_phy_reg_write(MIIM_RTL8211F_PAGE_SELECT, 0);


    emac_phy_reg_write(MIIM_RTL8211F_PAGE_SELECT, 7);
    emac_phy_reg_read(RTL8201F_RMSR, &reg);
    printf("RTL8201F_RMSR = 0x%x\r\n", reg);
    emac_phy_reg_write(MIIM_RTL8211F_PAGE_SELECT, 0);

}

int genphy_startup(emac_phy_cfg_t *cfg)
{
	int ret;

	ret = genphy_update_link(cfg);
	if (ret)
		return ret;

	return genphy_parse_link(cfg);
}

int phy_8720_link_up(emac_phy_cfg_t *cfg)
{
    return genphy_startup(cfg);
}

/****************************************************************************/ /**
 * @brief  Initialize EMAC PHY module
 *
 * @param  cfg: EMAC PHY configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
int emac_phy_init(emac_phy_cfg_t *cfg)
{
    uint16_t phyReg __attribute__((unused));

    CHECK_PARAM(NULL != cfg);

    phy_8720_cfg = cfg;
    rtl8211f_config(cfg);

    return phy_8720_link_up(cfg);
}

emac_phy_status_t ethernet_phy_status_get()
{
    CHECK_PARAM(NULL != phy_8720_cfg);

    if ((100 == phy_8720_cfg->speed) &&
        (phy_8720_cfg->full_duplex) &&
        (PHY_STATE_UP == phy_8720_cfg->phy_state)) {
        return EMAC_PHY_STAT_100MBITS_FULLDUPLEX;
    } else if (PHY_STATE_UP == phy_8720_cfg->phy_state) {
        return EMAC_PHY_STAT_LINK_UP;
    } else {
        return EMAC_PHY_STAT_LINK_DOWN;
    }
}
