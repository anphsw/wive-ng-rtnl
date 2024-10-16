/*
 * New driver for Marvell Yukon 2 chipset.
 * Based on earlier sk98lin, and skge driver.
 *
 * This driver intentionally does not support all the features
 * of the original driver such as link fail-over and link management because
 * those should be done at higher levels.
 *
 * Copyright (C) 2005 Stephen Hemminger <shemminger@osdl.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/crc32.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/pci.h>
#include <linux/ip.h>
#include <net/ip.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/if_vlan.h>
#include <linux/prefetch.h>
#include <linux/mii.h>

#include <asm/irq.h>

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
#define SKY2_VLAN_TAG_USED 1
#endif

#include "sky2.h"

#define DRV_NAME		"sky2"
#define DRV_VERSION		"1.14"
#define PFX			DRV_NAME " "

/*
 * The Yukon II chipset takes 64 bit command blocks (called list elements)
 * that are organized into three (receive, transmit, status) different rings
 * similar to Tigon3.
 */

#define RX_LE_SIZE	    	1024
#define RX_LE_BYTES		(RX_LE_SIZE*sizeof(struct sky2_rx_le))
#define RX_MAX_PENDING		(RX_LE_SIZE/6 - 2)
#define RX_DEF_PENDING		RX_MAX_PENDING
#define RX_SKB_ALIGN		8
#define RX_BUF_WRITE		16

#define TX_RING_SIZE		512
#define TX_DEF_PENDING		(TX_RING_SIZE - 1)
#define TX_MIN_PENDING		64
#define MAX_SKB_TX_LE		(4 + (sizeof(dma_addr_t)/sizeof(u32))*MAX_SKB_FRAGS)

#define STATUS_RING_SIZE	2048	/* 2 ports * (TX + 2*RX) */
#define STATUS_LE_BYTES		(STATUS_RING_SIZE*sizeof(struct sky2_status_le))
#define TX_WATCHDOG		(5 * HZ)
#define NAPI_WEIGHT		64
#define PHY_RETRIES		1000

#define RING_NEXT(x,s)	(((x)+1) & ((s)-1))

static const u32 default_msg =
    NETIF_MSG_DRV | NETIF_MSG_PROBE | NETIF_MSG_LINK
    | NETIF_MSG_TIMER | NETIF_MSG_TX_ERR | NETIF_MSG_RX_ERR
    | NETIF_MSG_IFUP | NETIF_MSG_IFDOWN;

static int debug = -1;		/* defaults above */
module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "Debug level (0=none,...,16=all)");

static int copybreak __read_mostly = 128;
module_param(copybreak, int, 0);
MODULE_PARM_DESC(copybreak, "Receive copy threshold");

static int disable_msi = 0;
module_param(disable_msi, int, 0);
MODULE_PARM_DESC(disable_msi, "Disable Message Signaled Interrupt (MSI)");

static int idle_timeout = 100;
module_param(idle_timeout, int, 0);
MODULE_PARM_DESC(idle_timeout, "Watchdog timer for lost interrupts (ms)");

static const struct pci_device_id sky2_id_table[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_SYSKONNECT, 0x9000) }, /* SK-9Sxx */
	{ PCI_DEVICE(PCI_VENDOR_ID_SYSKONNECT, 0x9E00) }, /* SK-9Exx */
	{ PCI_DEVICE(PCI_VENDOR_ID_DLINK, 0x4b00) },	/* DGE-560T */
	{ PCI_DEVICE(PCI_VENDOR_ID_DLINK, 0x4001) }, 	/* DGE-550SX */
	{ PCI_DEVICE(PCI_VENDOR_ID_DLINK, 0x4B02) },	/* DGE-560SX */
	{ PCI_DEVICE(PCI_VENDOR_ID_DLINK, 0x4B03) },	/* DGE-550T */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4340) }, /* 88E8021 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4341) }, /* 88E8022 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4342) }, /* 88E8061 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4343) }, /* 88E8062 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4344) }, /* 88E8021 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4345) }, /* 88E8022 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4346) }, /* 88E8061 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4347) }, /* 88E8062 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4350) }, /* 88E8035 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4351) }, /* 88E8036 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4352) }, /* 88E8038 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4353) }, /* 88E8039 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4356) }, /* 88EC033 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4360) }, /* 88E8052 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4361) }, /* 88E8050 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4362) }, /* 88E8053 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4363) }, /* 88E8055 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4364) }, /* 88E8056 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4366) }, /* 88EC036 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4367) }, /* 88EC032 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4368) }, /* 88EC034 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x4369) }, /* 88EC042 */
	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x436A) }, /* 88E8058 */
//	{ PCI_DEVICE(PCI_VENDOR_ID_MARVELL, 0x436B) }, /* 88E8071 */
	{ 0 }
};

MODULE_DEVICE_TABLE(pci, sky2_id_table);

/* Avoid conditionals by using array */
static const unsigned txqaddr[] = { Q_XA1, Q_XA2 };
static const unsigned rxqaddr[] = { Q_R1, Q_R2 };
static const u32 portirq_msk[] = { Y2_IS_PORT_1, Y2_IS_PORT_2 };

/* This driver supports yukon2 chipset only */
static const char *yukon2_name[] = {
	"XL",		/* 0xb3 */
	"EC Ultra", 	/* 0xb4 */
	"Extreme",	/* 0xb5 */
	"EC",		/* 0xb6 */
	"FE",		/* 0xb7 */
};

/* Access to external PHY */
static int gm_phy_write(struct sky2_hw *hw, unsigned port, u16 reg, u16 val)
{
	int i;

	gma_write16(hw, port, GM_SMI_DATA, val);
	gma_write16(hw, port, GM_SMI_CTRL,
		    GM_SMI_CT_PHY_AD(PHY_ADDR_MARV) | GM_SMI_CT_REG_AD(reg));

	for (i = 0; i < PHY_RETRIES; i++) {
		if (!(gma_read16(hw, port, GM_SMI_CTRL) & GM_SMI_CT_BUSY))
			return 0;
		udelay(1);
	}

	printk(KERN_WARNING PFX "%s: phy write timeout\n", hw->dev[port]->name);
	return -ETIMEDOUT;
}

static int __gm_phy_read(struct sky2_hw *hw, unsigned port, u16 reg, u16 *val)
{
	int i;

	gma_write16(hw, port, GM_SMI_CTRL, GM_SMI_CT_PHY_AD(PHY_ADDR_MARV)
		    | GM_SMI_CT_REG_AD(reg) | GM_SMI_CT_OP_RD);

	for (i = 0; i < PHY_RETRIES; i++) {
		if (gma_read16(hw, port, GM_SMI_CTRL) & GM_SMI_CT_RD_VAL) {
			*val = gma_read16(hw, port, GM_SMI_DATA);
			return 0;
		}

		udelay(1);
	}

	return -ETIMEDOUT;
}

static u16 gm_phy_read(struct sky2_hw *hw, unsigned port, u16 reg)
{
	u16 v;

	if (__gm_phy_read(hw, port, reg, &v) != 0)
		printk(KERN_WARNING PFX "%s: phy read timeout\n", hw->dev[port]->name);
	return v;
}


static void sky2_power_on(struct sky2_hw *hw)
{
	/* switch power to VCC (WA for VAUX problem) */
	sky2_write8(hw, B0_POWER_CTRL,
		    PC_VAUX_ENA | PC_VCC_ENA | PC_VAUX_OFF | PC_VCC_ON);

	/* disable Core Clock Division, */
	sky2_write32(hw, B2_Y2_CLK_CTRL, Y2_CLK_DIV_DIS);

	if (hw->chip_id == CHIP_ID_YUKON_XL && hw->chip_rev > 1)
		/* enable bits are inverted */
		sky2_write8(hw, B2_Y2_CLK_GATE,
			    Y2_PCI_CLK_LNK1_DIS | Y2_COR_CLK_LNK1_DIS |
			    Y2_CLK_GAT_LNK1_DIS | Y2_PCI_CLK_LNK2_DIS |
			    Y2_COR_CLK_LNK2_DIS | Y2_CLK_GAT_LNK2_DIS);
	else
		sky2_write8(hw, B2_Y2_CLK_GATE, 0);

	if (hw->chip_id == CHIP_ID_YUKON_EC_U || hw->chip_id == CHIP_ID_YUKON_EX) {
		u32 reg1;

		sky2_pci_write32(hw, PCI_DEV_REG3, 0);
		reg1 = sky2_pci_read32(hw, PCI_DEV_REG4);
		reg1 &= P_ASPM_CONTROL_MSK;
		sky2_pci_write32(hw, PCI_DEV_REG4, reg1);
		sky2_pci_write32(hw, PCI_DEV_REG5, 0);
	}
}

static void sky2_power_aux(struct sky2_hw *hw)
{
	if (hw->chip_id == CHIP_ID_YUKON_XL && hw->chip_rev > 1)
		sky2_write8(hw, B2_Y2_CLK_GATE, 0);
	else
		/* enable bits are inverted */
		sky2_write8(hw, B2_Y2_CLK_GATE,
			    Y2_PCI_CLK_LNK1_DIS | Y2_COR_CLK_LNK1_DIS |
			    Y2_CLK_GAT_LNK1_DIS | Y2_PCI_CLK_LNK2_DIS |
			    Y2_COR_CLK_LNK2_DIS | Y2_CLK_GAT_LNK2_DIS);

	/* switch power to VAUX */
	if (sky2_read16(hw, B0_CTST) & Y2_VAUX_AVAIL)
		sky2_write8(hw, B0_POWER_CTRL,
			    (PC_VAUX_ENA | PC_VCC_ENA |
			     PC_VAUX_ON | PC_VCC_OFF));
}

static void sky2_gmac_reset(struct sky2_hw *hw, unsigned port)
{
	u16 reg;

	/* disable all GMAC IRQ's */
	sky2_write8(hw, SK_REG(port, GMAC_IRQ_MSK), 0);
	/* disable PHY IRQs */
	gm_phy_write(hw, port, PHY_MARV_INT_MASK, 0);

	gma_write16(hw, port, GM_MC_ADDR_H1, 0);	/* clear MC hash */
	gma_write16(hw, port, GM_MC_ADDR_H2, 0);
	gma_write16(hw, port, GM_MC_ADDR_H3, 0);
	gma_write16(hw, port, GM_MC_ADDR_H4, 0);

	reg = gma_read16(hw, port, GM_RX_CTRL);
	reg |= GM_RXCR_UCF_ENA | GM_RXCR_MCF_ENA;
	gma_write16(hw, port, GM_RX_CTRL, reg);
}

/* flow control to advertise bits */
static const u16 copper_fc_adv[] = {
	[FC_NONE]	= 0,
	[FC_TX]		= PHY_M_AN_ASP,
	[FC_RX]		= PHY_M_AN_PC,
	[FC_BOTH]	= PHY_M_AN_PC | PHY_M_AN_ASP,
};

/* flow control to advertise bits when using 1000BaseX */
static const u16 fiber_fc_adv[] = {
	[FC_BOTH] = PHY_M_P_BOTH_MD_X,
	[FC_TX]   = PHY_M_P_ASYM_MD_X,
	[FC_RX]	  = PHY_M_P_SYM_MD_X,
	[FC_NONE] = PHY_M_P_NO_PAUSE_X,
};

/* flow control to GMA disable bits */
static const u16 gm_fc_disable[] = {
	[FC_NONE] = GM_GPCR_FC_RX_DIS | GM_GPCR_FC_TX_DIS,
	[FC_TX]	  = GM_GPCR_FC_RX_DIS,
	[FC_RX]	  = GM_GPCR_FC_TX_DIS,
	[FC_BOTH] = 0,
};


static void sky2_phy_init(struct sky2_hw *hw, unsigned port)
{
	struct sky2_port *sky2 = netdev_priv(hw->dev[port]);
	u16 ctrl, ct1000, adv, pg, ledctrl, ledover, reg;

	if (sky2->autoneg == AUTONEG_ENABLE
	    && !(hw->chip_id == CHIP_ID_YUKON_XL
		 || hw->chip_id == CHIP_ID_YUKON_EC_U
		 || hw->chip_id == CHIP_ID_YUKON_EX)) {
		u16 ectrl = gm_phy_read(hw, port, PHY_MARV_EXT_CTRL);

		ectrl &= ~(PHY_M_EC_M_DSC_MSK | PHY_M_EC_S_DSC_MSK |
			   PHY_M_EC_MAC_S_MSK);
		ectrl |= PHY_M_EC_MAC_S(MAC_TX_CLK_25_MHZ);

		if (hw->chip_id == CHIP_ID_YUKON_EC)
			ectrl |= PHY_M_EC_DSC_2(2) | PHY_M_EC_DOWN_S_ENA;
		else
			ectrl |= PHY_M_EC_M_DSC(2) | PHY_M_EC_S_DSC(3);

		gm_phy_write(hw, port, PHY_MARV_EXT_CTRL, ectrl);
	}

	ctrl = gm_phy_read(hw, port, PHY_MARV_PHY_CTRL);
	if (sky2_is_copper(hw)) {
		if (hw->chip_id == CHIP_ID_YUKON_FE) {
			/* enable automatic crossover */
			ctrl |= PHY_M_PC_MDI_XMODE(PHY_M_PC_ENA_AUTO) >> 1;
		} else {
			/* disable energy detect */
			ctrl &= ~PHY_M_PC_EN_DET_MSK;

			/* enable automatic crossover */
			ctrl |= PHY_M_PC_MDI_XMODE(PHY_M_PC_ENA_AUTO);

			if (sky2->autoneg == AUTONEG_ENABLE
			    && (hw->chip_id == CHIP_ID_YUKON_XL
				|| hw->chip_id == CHIP_ID_YUKON_EC_U
				|| hw->chip_id == CHIP_ID_YUKON_EX)) {
				ctrl &= ~PHY_M_PC_DSC_MSK;
				ctrl |= PHY_M_PC_DSC(2) | PHY_M_PC_DOWN_S_ENA;
			}
		}
	} else {
		/* workaround for deviation #4.88 (CRC errors) */
		/* disable Automatic Crossover */

		ctrl &= ~PHY_M_PC_MDIX_MSK;
	}

	gm_phy_write(hw, port, PHY_MARV_PHY_CTRL, ctrl);

	/* special setup for PHY 88E1112 Fiber */
	if (hw->chip_id == CHIP_ID_YUKON_XL && !sky2_is_copper(hw)) {
		pg = gm_phy_read(hw, port, PHY_MARV_EXT_ADR);

		/* Fiber: select 1000BASE-X only mode MAC Specific Ctrl Reg. */
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, 2);
		ctrl = gm_phy_read(hw, port, PHY_MARV_PHY_CTRL);
		ctrl &= ~PHY_M_MAC_MD_MSK;
		ctrl |= PHY_M_MAC_MODE_SEL(PHY_M_MAC_MD_1000BX);
		gm_phy_write(hw, port, PHY_MARV_PHY_CTRL, ctrl);

		if (hw->pmd_type  == 'P') {
			/* select page 1 to access Fiber registers */
			gm_phy_write(hw, port, PHY_MARV_EXT_ADR, 1);

			/* for SFP-module set SIGDET polarity to low */
			ctrl = gm_phy_read(hw, port, PHY_MARV_PHY_CTRL);
			ctrl |= PHY_M_FIB_SIGD_POL;
			gm_phy_write(hw, port, PHY_MARV_CTRL, ctrl);
		}

		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, pg);
	}

	ctrl = PHY_CT_RESET;
	ct1000 = 0;
	adv = PHY_AN_CSMA;
	reg = 0;

	if (sky2->autoneg == AUTONEG_ENABLE) {
		if (sky2_is_copper(hw)) {
			if (sky2->advertising & ADVERTISED_1000baseT_Full)
				ct1000 |= PHY_M_1000C_AFD;
			if (sky2->advertising & ADVERTISED_1000baseT_Half)
				ct1000 |= PHY_M_1000C_AHD;
			if (sky2->advertising & ADVERTISED_100baseT_Full)
				adv |= PHY_M_AN_100_FD;
			if (sky2->advertising & ADVERTISED_100baseT_Half)
				adv |= PHY_M_AN_100_HD;
			if (sky2->advertising & ADVERTISED_10baseT_Full)
				adv |= PHY_M_AN_10_FD;
			if (sky2->advertising & ADVERTISED_10baseT_Half)
				adv |= PHY_M_AN_10_HD;

			adv |= copper_fc_adv[sky2->flow_mode];
		} else {	/* special defines for FIBER (88E1040S only) */
			if (sky2->advertising & ADVERTISED_1000baseT_Full)
				adv |= PHY_M_AN_1000X_AFD;
			if (sky2->advertising & ADVERTISED_1000baseT_Half)
				adv |= PHY_M_AN_1000X_AHD;

			adv |= fiber_fc_adv[sky2->flow_mode];
		}

		/* Restart Auto-negotiation */
		ctrl |= PHY_CT_ANE | PHY_CT_RE_CFG;
	} else {
		/* forced speed/duplex settings */
		ct1000 = PHY_M_1000C_MSE;

		/* Disable auto update for duplex flow control and speed */
		reg |= GM_GPCR_AU_ALL_DIS;

		switch (sky2->speed) {
		case SPEED_1000:
			ctrl |= PHY_CT_SP1000;
			reg |= GM_GPCR_SPEED_1000;
			break;
		case SPEED_100:
			ctrl |= PHY_CT_SP100;
			reg |= GM_GPCR_SPEED_100;
			break;
		}

		if (sky2->duplex == DUPLEX_FULL) {
			reg |= GM_GPCR_DUP_FULL;
			ctrl |= PHY_CT_DUP_MD;
		} else if (sky2->speed < SPEED_1000)
			sky2->flow_mode = FC_NONE;


 		reg |= gm_fc_disable[sky2->flow_mode];

		/* Forward pause packets to GMAC? */
		if (sky2->flow_mode & FC_RX)
			sky2_write8(hw, SK_REG(port, GMAC_CTRL), GMC_PAUSE_ON);
		else
			sky2_write8(hw, SK_REG(port, GMAC_CTRL), GMC_PAUSE_OFF);
	}

	gma_write16(hw, port, GM_GP_CTRL, reg);

	if (hw->chip_id != CHIP_ID_YUKON_FE)
		gm_phy_write(hw, port, PHY_MARV_1000T_CTRL, ct1000);

	gm_phy_write(hw, port, PHY_MARV_AUNE_ADV, adv);
	gm_phy_write(hw, port, PHY_MARV_CTRL, ctrl);

	/* Setup Phy LED's */
	ledctrl = PHY_M_LED_PULS_DUR(PULS_170MS);
	ledover = 0;

	switch (hw->chip_id) {
	case CHIP_ID_YUKON_FE:
		/* on 88E3082 these bits are at 11..9 (shifted left) */
		ledctrl |= PHY_M_LED_BLINK_RT(BLINK_84MS) << 1;

		ctrl = gm_phy_read(hw, port, PHY_MARV_FE_LED_PAR);

		/* delete ACT LED control bits */
		ctrl &= ~PHY_M_FELP_LED1_MSK;
		/* change ACT LED control to blink mode */
		ctrl |= PHY_M_FELP_LED1_CTRL(LED_PAR_CTRL_ACT_BL);
		gm_phy_write(hw, port, PHY_MARV_FE_LED_PAR, ctrl);
		break;

	case CHIP_ID_YUKON_XL:
		pg = gm_phy_read(hw, port, PHY_MARV_EXT_ADR);

		/* select page 3 to access LED control register */
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, 3);

		/* set LED Function Control register */
		gm_phy_write(hw, port, PHY_MARV_PHY_CTRL,
			     (PHY_M_LEDC_LOS_CTRL(1) |	/* LINK/ACT */
			      PHY_M_LEDC_INIT_CTRL(7) |	/* 10 Mbps */
			      PHY_M_LEDC_STA1_CTRL(7) |	/* 100 Mbps */
			      PHY_M_LEDC_STA0_CTRL(7)));	/* 1000 Mbps */

		/* set Polarity Control register */
		gm_phy_write(hw, port, PHY_MARV_PHY_STAT,
			     (PHY_M_POLC_LS1_P_MIX(4) |
			      PHY_M_POLC_IS0_P_MIX(4) |
			      PHY_M_POLC_LOS_CTRL(2) |
			      PHY_M_POLC_INIT_CTRL(2) |
			      PHY_M_POLC_STA1_CTRL(2) |
			      PHY_M_POLC_STA0_CTRL(2)));

		/* restore page register */
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, pg);
		break;

	case CHIP_ID_YUKON_EC_U:
	case CHIP_ID_YUKON_EX:
		pg = gm_phy_read(hw, port, PHY_MARV_EXT_ADR);

		/* select page 3 to access LED control register */
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, 3);

		/* set LED Function Control register */
		gm_phy_write(hw, port, PHY_MARV_PHY_CTRL,
			     (PHY_M_LEDC_LOS_CTRL(1) |	/* LINK/ACT */
			      PHY_M_LEDC_INIT_CTRL(8) |	/* 10 Mbps */
			      PHY_M_LEDC_STA1_CTRL(7) |	/* 100 Mbps */
			      PHY_M_LEDC_STA0_CTRL(7)));/* 1000 Mbps */

		/* set Blink Rate in LED Timer Control Register */
		gm_phy_write(hw, port, PHY_MARV_INT_MASK,
			     ledctrl | PHY_M_LED_BLINK_RT(BLINK_84MS));
		/* restore page register */
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, pg);
		break;

	default:
		/* set Tx LED (LED_TX) to blink mode on Rx OR Tx activity */
		ledctrl |= PHY_M_LED_BLINK_RT(BLINK_84MS) | PHY_M_LEDC_TX_CTRL;
		/* turn off the Rx LED (LED_RX) */
		ledover &= ~PHY_M_LED_MO_RX;
	}

	if (hw->chip_id == CHIP_ID_YUKON_EC_U &&
	    hw->chip_rev == CHIP_REV_YU_EC_U_A1) {
		/* apply fixes in PHY AFE */
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, 255);

		/* increase differential signal amplitude in 10BASE-T */
		gm_phy_write(hw, port, 0x18, 0xaa99);
		gm_phy_write(hw, port, 0x17, 0x2011);

		/* fix for IEEE A/B Symmetry failure in 1000BASE-T */
		gm_phy_write(hw, port, 0x18, 0xa204);
		gm_phy_write(hw, port, 0x17, 0x2002);

		/* set page register to 0 */
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, 0);
	} else if (hw->chip_id != CHIP_ID_YUKON_EX) {
		gm_phy_write(hw, port, PHY_MARV_LED_CTRL, ledctrl);

		if (sky2->autoneg == AUTONEG_DISABLE || sky2->speed == SPEED_100) {
			/* turn on 100 Mbps LED (LED_LINK100) */
			ledover |= PHY_M_LED_MO_100;
		}

		if (ledover)
			gm_phy_write(hw, port, PHY_MARV_LED_OVER, ledover);

	}

	/* Enable phy interrupt on auto-negotiation complete (or link up) */
	if (sky2->autoneg == AUTONEG_ENABLE)
		gm_phy_write(hw, port, PHY_MARV_INT_MASK, PHY_M_IS_AN_COMPL);
	else
		gm_phy_write(hw, port, PHY_MARV_INT_MASK, PHY_M_DEF_MSK);
}

static void sky2_phy_power(struct sky2_hw *hw, unsigned port, int onoff)
{
	u32 reg1;
	static const u32 phy_power[]
		= { PCI_Y2_PHY1_POWD, PCI_Y2_PHY2_POWD };

	/* looks like this XL is back asswards .. */
	if (hw->chip_id == CHIP_ID_YUKON_XL && hw->chip_rev > 1)
		onoff = !onoff;

	sky2_write8(hw, B2_TST_CTRL1, TST_CFG_WRITE_ON);
	reg1 = sky2_pci_read32(hw, PCI_DEV_REG1);
	if (onoff)
		/* Turn off phy power saving */
		reg1 &= ~phy_power[port];
	else
		reg1 |= phy_power[port];

	sky2_pci_write32(hw, PCI_DEV_REG1, reg1);
	sky2_pci_read32(hw, PCI_DEV_REG1);
	sky2_write8(hw, B2_TST_CTRL1, TST_CFG_WRITE_OFF);
	udelay(100);
}

/* Force a renegotiation */
static void sky2_phy_reinit(struct sky2_port *sky2)
{
	spin_lock_bh(&sky2->phy_lock);
	sky2_phy_init(sky2->hw, sky2->port);
	spin_unlock_bh(&sky2->phy_lock);
}

/* Put device in state to listen for Wake On Lan */
static void sky2_wol_init(struct sky2_port *sky2)
{
	struct sky2_hw *hw = sky2->hw;
	unsigned port = sky2->port;
	enum flow_control save_mode;
	u16 ctrl;
	u32 reg1;

	/* Bring hardware out of reset */
	sky2_write16(hw, B0_CTST, CS_RST_CLR);
	sky2_write16(hw, SK_REG(port, GMAC_LINK_CTRL), GMLC_RST_CLR);

	sky2_write8(hw, SK_REG(port, GPHY_CTRL), GPC_RST_CLR);
	sky2_write8(hw, SK_REG(port, GMAC_CTRL), GMC_RST_CLR);

	/* Force to 10/100
	 * sky2_reset will re-enable on resume
	 */
	save_mode = sky2->flow_mode;
	ctrl = sky2->advertising;

	sky2->advertising &= ~(ADVERTISED_1000baseT_Half|ADVERTISED_1000baseT_Full);
	sky2->flow_mode = FC_NONE;
	sky2_phy_power(hw, port, 1);
	sky2_phy_reinit(sky2);

	sky2->flow_mode = save_mode;
	sky2->advertising = ctrl;

	/* Set GMAC to no flow control and auto update for speed/duplex */
	gma_write16(hw, port, GM_GP_CTRL,
		    GM_GPCR_FC_TX_DIS|GM_GPCR_TX_ENA|GM_GPCR_RX_ENA|
		    GM_GPCR_DUP_FULL|GM_GPCR_FC_RX_DIS|GM_GPCR_AU_FCT_DIS);

	/* Set WOL address */
	memcpy_toio(hw->regs + WOL_REGS(port, WOL_MAC_ADDR),
		    sky2->netdev->dev_addr, ETH_ALEN);

	/* Turn on appropriate WOL control bits */
	sky2_write16(hw, WOL_REGS(port, WOL_CTRL_STAT), WOL_CTL_CLEAR_RESULT);
	ctrl = 0;
	if (sky2->wol & WAKE_PHY)
		ctrl |= WOL_CTL_ENA_PME_ON_LINK_CHG|WOL_CTL_ENA_LINK_CHG_UNIT;
	else
		ctrl |= WOL_CTL_DIS_PME_ON_LINK_CHG|WOL_CTL_DIS_LINK_CHG_UNIT;

	if (sky2->wol & WAKE_MAGIC)
		ctrl |= WOL_CTL_ENA_PME_ON_MAGIC_PKT|WOL_CTL_ENA_MAGIC_PKT_UNIT;
	else
		ctrl |= WOL_CTL_DIS_PME_ON_MAGIC_PKT|WOL_CTL_DIS_MAGIC_PKT_UNIT;;

	ctrl |= WOL_CTL_DIS_PME_ON_PATTERN|WOL_CTL_DIS_PATTERN_UNIT;
	sky2_write16(hw, WOL_REGS(port, WOL_CTRL_STAT), ctrl);

	/* Turn on legacy PCI-Express PME mode */
	sky2_write8(hw, B2_TST_CTRL1, TST_CFG_WRITE_ON);
	reg1 = sky2_pci_read32(hw, PCI_DEV_REG1);
	reg1 |= PCI_Y2_PME_LEGACY;
	sky2_pci_write32(hw, PCI_DEV_REG1, reg1);
	sky2_write8(hw, B2_TST_CTRL1, TST_CFG_WRITE_OFF);

	/* block receiver */
	sky2_write8(hw, SK_REG(port, RX_GMF_CTRL_T), GMF_RST_SET);

}

static void sky2_mac_init(struct sky2_hw *hw, unsigned port)
{
	struct sky2_port *sky2 = netdev_priv(hw->dev[port]);
	u16 reg;
	int i;
	const u8 *addr = hw->dev[port]->dev_addr;

	sky2_write32(hw, SK_REG(port, GPHY_CTRL), GPC_RST_SET);
	sky2_write32(hw, SK_REG(port, GPHY_CTRL), GPC_RST_CLR|GPC_ENA_PAUSE);

	sky2_write8(hw, SK_REG(port, GMAC_CTRL), GMC_RST_CLR);

	if (hw->chip_id == CHIP_ID_YUKON_XL && hw->chip_rev == 0 && port == 1) {
		/* WA DEV_472 -- looks like crossed wires on port 2 */
		/* clear GMAC 1 Control reset */
		sky2_write8(hw, SK_REG(0, GMAC_CTRL), GMC_RST_CLR);
		do {
			sky2_write8(hw, SK_REG(1, GMAC_CTRL), GMC_RST_SET);
			sky2_write8(hw, SK_REG(1, GMAC_CTRL), GMC_RST_CLR);
		} while (gm_phy_read(hw, 1, PHY_MARV_ID0) != PHY_MARV_ID0_VAL ||
			 gm_phy_read(hw, 1, PHY_MARV_ID1) != PHY_MARV_ID1_Y2 ||
			 gm_phy_read(hw, 1, PHY_MARV_INT_MASK) != 0);
	}

	sky2_read16(hw, SK_REG(port, GMAC_IRQ_SRC));

	/* Enable Transmit FIFO Underrun */
	sky2_write8(hw, SK_REG(port, GMAC_IRQ_MSK), GMAC_DEF_MSK);

	spin_lock_bh(&sky2->phy_lock);
	sky2_phy_init(hw, port);
	spin_unlock_bh(&sky2->phy_lock);

	/* MIB clear */
	reg = gma_read16(hw, port, GM_PHY_ADDR);
	gma_write16(hw, port, GM_PHY_ADDR, reg | GM_PAR_MIB_CLR);

	for (i = GM_MIB_CNT_BASE; i <= GM_MIB_CNT_END; i += 4)
		gma_read16(hw, port, i);
	gma_write16(hw, port, GM_PHY_ADDR, reg);

	/* transmit control */
	gma_write16(hw, port, GM_TX_CTRL, TX_COL_THR(TX_COL_DEF));

	/* receive control reg: unicast + multicast + no FCS  */
	gma_write16(hw, port, GM_RX_CTRL,
		    GM_RXCR_UCF_ENA | GM_RXCR_CRC_DIS | GM_RXCR_MCF_ENA);

	/* transmit flow control */
	gma_write16(hw, port, GM_TX_FLOW_CTRL, 0xffff);

	/* transmit parameter */
	gma_write16(hw, port, GM_TX_PARAM,
		    TX_JAM_LEN_VAL(TX_JAM_LEN_DEF) |
		    TX_JAM_IPG_VAL(TX_JAM_IPG_DEF) |
		    TX_IPG_JAM_DATA(TX_IPG_JAM_DEF) |
		    TX_BACK_OFF_LIM(TX_BOF_LIM_DEF));

	/* serial mode register */
	reg = DATA_BLIND_VAL(DATA_BLIND_DEF) |
		GM_SMOD_VLAN_ENA | IPG_DATA_VAL(IPG_DATA_DEF);

	if (hw->dev[port]->mtu > ETH_DATA_LEN)
		reg |= GM_SMOD_JUMBO_ENA;

	gma_write16(hw, port, GM_SERIAL_MODE, reg);

	/* virtual address for data */
	gma_set_addr(hw, port, GM_SRC_ADDR_2L, addr);

	/* physical address: used for pause frames */
	gma_set_addr(hw, port, GM_SRC_ADDR_1L, addr);

	/* ignore counter overflows */
	gma_write16(hw, port, GM_TX_IRQ_MSK, 0);
	gma_write16(hw, port, GM_RX_IRQ_MSK, 0);
	gma_write16(hw, port, GM_TR_IRQ_MSK, 0);

	/* Configure Rx MAC FIFO */
	sky2_write8(hw, SK_REG(port, RX_GMF_CTRL_T), GMF_RST_CLR);
	sky2_write32(hw, SK_REG(port, RX_GMF_CTRL_T),
		     GMF_OPER_ON | GMF_RX_F_FL_ON);

	/* Flush Rx MAC FIFO on any flow control or error */
	sky2_write16(hw, SK_REG(port, RX_GMF_FL_MSK), GMR_FS_ANY_ERR);

	/* Set threshold to 0xa (64 bytes) + 1 to workaround pause bug  */
	sky2_write16(hw, SK_REG(port, RX_GMF_FL_THR), RX_GMF_FL_THR_DEF+1);

	/* Configure Tx MAC FIFO */
	sky2_write8(hw, SK_REG(port, TX_GMF_CTRL_T), GMF_RST_CLR);
	sky2_write16(hw, SK_REG(port, TX_GMF_CTRL_T), GMF_OPER_ON);

	if (hw->chip_id == CHIP_ID_YUKON_EC_U || hw->chip_id == CHIP_ID_YUKON_EX) {
		sky2_write8(hw, SK_REG(port, RX_GMF_LP_THR), 768/8);
		sky2_write8(hw, SK_REG(port, RX_GMF_UP_THR), 1024/8);

		/* set Tx GMAC FIFO Almost Empty Threshold */
		sky2_write32(hw, SK_REG(port, TX_GMF_AE_THR),
			     (ECU_JUMBO_WM << 16) | ECU_AE_THR);

		if (hw->dev[port]->mtu > ETH_DATA_LEN)
			sky2_write32(hw, SK_REG(port, TX_GMF_CTRL_T),
				     TX_JUMBO_ENA | TX_STFW_DIS);
		else
			sky2_write32(hw, SK_REG(port, TX_GMF_CTRL_T),
				     TX_JUMBO_DIS | TX_STFW_ENA);
	}

}

/* Assign Ram Buffer allocation to queue */
static void sky2_ramset(struct sky2_hw *hw, u16 q, u32 start, u32 space)
{
	u32 end;

	/* convert from K bytes to qwords used for hw register */
	start *= 1024/8;
	space *= 1024/8;
	end = start + space - 1;

	sky2_write8(hw, RB_ADDR(q, RB_CTRL), RB_RST_CLR);
	sky2_write32(hw, RB_ADDR(q, RB_START), start);
	sky2_write32(hw, RB_ADDR(q, RB_END), end);
	sky2_write32(hw, RB_ADDR(q, RB_WP), start);
	sky2_write32(hw, RB_ADDR(q, RB_RP), start);

	if (q == Q_R1 || q == Q_R2) {
		u32 tp = space - space/4;

		/* On receive queue's set the thresholds
		 * give receiver priority when > 3/4 full
		 * send pause when down to 2K
		 */
		sky2_write32(hw, RB_ADDR(q, RB_RX_UTHP), tp);
		sky2_write32(hw, RB_ADDR(q, RB_RX_LTHP), space/2);

		tp = space - 2048/8;
		sky2_write32(hw, RB_ADDR(q, RB_RX_UTPP), tp);
		sky2_write32(hw, RB_ADDR(q, RB_RX_LTPP), space/4);
	} else {
		/* Enable store & forward on Tx queue's because
		 * Tx FIFO is only 1K on Yukon
		 */
		sky2_write8(hw, RB_ADDR(q, RB_CTRL), RB_ENA_STFWD);
	}

	sky2_write8(hw, RB_ADDR(q, RB_CTRL), RB_ENA_OP_MD);
	sky2_read8(hw, RB_ADDR(q, RB_CTRL));
}

/* Setup Bus Memory Interface */
static void sky2_qset(struct sky2_hw *hw, u16 q)
{
	sky2_write32(hw, Q_ADDR(q, Q_CSR), BMU_CLR_RESET);
	sky2_write32(hw, Q_ADDR(q, Q_CSR), BMU_OPER_INIT);
	sky2_write32(hw, Q_ADDR(q, Q_CSR), BMU_FIFO_OP_ON);
	sky2_write32(hw, Q_ADDR(q, Q_WM),  BMU_WM_DEFAULT);
}

/* Setup prefetch unit registers. This is the interface between
 * hardware and driver list elements
 */
static void sky2_prefetch_init(struct sky2_hw *hw, u32 qaddr,
				      u64 addr, u32 last)
{
	sky2_write32(hw, Y2_QADDR(qaddr, PREF_UNIT_CTRL), PREF_UNIT_RST_SET);
	sky2_write32(hw, Y2_QADDR(qaddr, PREF_UNIT_CTRL), PREF_UNIT_RST_CLR);
	sky2_write32(hw, Y2_QADDR(qaddr, PREF_UNIT_ADDR_HI), addr >> 32);
	sky2_write32(hw, Y2_QADDR(qaddr, PREF_UNIT_ADDR_LO), (u32) addr);
	sky2_write16(hw, Y2_QADDR(qaddr, PREF_UNIT_LAST_IDX), last);
	sky2_write32(hw, Y2_QADDR(qaddr, PREF_UNIT_CTRL), PREF_UNIT_OP_ON);

	sky2_read32(hw, Y2_QADDR(qaddr, PREF_UNIT_CTRL));
}

static inline struct sky2_tx_le *get_tx_le(struct sky2_port *sky2)
{
	struct sky2_tx_le *le = sky2->tx_le + sky2->tx_prod;

	sky2->tx_prod = RING_NEXT(sky2->tx_prod, TX_RING_SIZE);
	le->ctrl = 0;
	return le;
}

static inline struct tx_ring_info *tx_le_re(struct sky2_port *sky2,
					    struct sky2_tx_le *le)
{
	return sky2->tx_ring + (le - sky2->tx_le);
}

/* Update chip's next pointer */
static inline void sky2_put_idx(struct sky2_hw *hw, unsigned q, u16 idx)
{
	q = Y2_QADDR(q, PREF_UNIT_PUT_IDX);
	wmb();
	sky2_write16(hw, q, idx);
	sky2_read16(hw, q);
}


static inline struct sky2_rx_le *sky2_next_rx(struct sky2_port *sky2)
{
	struct sky2_rx_le *le = sky2->rx_le + sky2->rx_put;
	sky2->rx_put = RING_NEXT(sky2->rx_put, RX_LE_SIZE);
	le->ctrl = 0;
	return le;
}

/* Return high part of DMA address (could be 32 or 64 bit) */
static inline u32 high32(dma_addr_t a)
{
	return sizeof(a) > sizeof(u32) ? (a >> 16) >> 16 : 0;
}

/* Build description to hardware for one receive segment */
static void sky2_rx_add(struct sky2_port *sky2,  u8 op,
			dma_addr_t map, unsigned len)
{
	struct sky2_rx_le *le;
	u32 hi = high32(map);

	if (sky2->rx_addr64 != hi) {
		le = sky2_next_rx(sky2);
		le->addr = cpu_to_le32(hi);
		le->opcode = OP_ADDR64 | HW_OWNER;
		sky2->rx_addr64 = high32(map + len);
	}

	le = sky2_next_rx(sky2);
	le->addr = cpu_to_le32((u32) map);
	le->length = cpu_to_le16(len);
	le->opcode = op | HW_OWNER;
}

/* Build description to hardware for one possibly fragmented skb */
static void sky2_rx_submit(struct sky2_port *sky2,
			   const struct rx_ring_info *re)
{
	int i;

	sky2_rx_add(sky2, OP_PACKET, re->data_addr, sky2->rx_data_size);

	for (i = 0; i < skb_shinfo(re->skb)->nr_frags; i++)
		sky2_rx_add(sky2, OP_BUFFER, re->frag_addr[i], PAGE_SIZE);
}


static void sky2_rx_map_skb(struct pci_dev *pdev, struct rx_ring_info *re,
			    unsigned size)
{
	struct sk_buff *skb = re->skb;
	int i;

	re->data_addr = pci_map_single(pdev, skb->data, size, PCI_DMA_FROMDEVICE);
	pci_unmap_len_set(re, data_size, size);

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++)
		re->frag_addr[i] = pci_map_page(pdev,
						skb_shinfo(skb)->frags[i].page,
						skb_shinfo(skb)->frags[i].page_offset,
						skb_shinfo(skb)->frags[i].size,
						PCI_DMA_FROMDEVICE);
}

static void sky2_rx_unmap_skb(struct pci_dev *pdev, struct rx_ring_info *re)
{
	struct sk_buff *skb = re->skb;
	int i;

	pci_unmap_single(pdev, re->data_addr, pci_unmap_len(re, data_size),
			 PCI_DMA_FROMDEVICE);

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++)
		pci_unmap_page(pdev, re->frag_addr[i],
			       skb_shinfo(skb)->frags[i].size,
			       PCI_DMA_FROMDEVICE);
}

/* Tell chip where to start receive checksum.
 * Actually has two checksums, but set both same to avoid possible byte
 * order problems.
 */
static void rx_set_checksum(struct sky2_port *sky2)
{
	struct sky2_rx_le *le;

	le = sky2_next_rx(sky2);
	le->addr = cpu_to_le32((ETH_HLEN << 16) | ETH_HLEN);
	le->ctrl = 0;
	le->opcode = OP_TCPSTART | HW_OWNER;

	sky2_write32(sky2->hw,
		     Q_ADDR(rxqaddr[sky2->port], Q_CSR),
		     sky2->rx_csum ? BMU_ENA_RX_CHKSUM : BMU_DIS_RX_CHKSUM);

}

/*
 * The RX Stop command will not work for Yukon-2 if the BMU does not
 * reach the end of packet and since we can't make sure that we have
 * incoming data, we must reset the BMU while it is not doing a DMA
 * transfer. Since it is possible that the RX path is still active,
 * the RX RAM buffer will be stopped first, so any possible incoming
 * data will not trigger a DMA. After the RAM buffer is stopped, the
 * BMU is polled until any DMA in progress is ended and only then it
 * will be reset.
 */
static void sky2_rx_stop(struct sky2_port *sky2)
{
	struct sky2_hw *hw = sky2->hw;
	unsigned rxq = rxqaddr[sky2->port];
	int i;

	/* disable the RAM Buffer receive queue */
	sky2_write8(hw, RB_ADDR(rxq, RB_CTRL), RB_DIS_OP_MD);

	for (i = 0; i < 0xffff; i++)
		if (sky2_read8(hw, RB_ADDR(rxq, Q_RSL))
		    == sky2_read8(hw, RB_ADDR(rxq, Q_RL)))
			goto stopped;

	printk(KERN_WARNING PFX "%s: receiver stop failed\n",
	       sky2->netdev->name);
stopped:
	sky2_write32(hw, Q_ADDR(rxq, Q_CSR), BMU_RST_SET | BMU_FIFO_RST);

	/* reset the Rx prefetch unit */
	sky2_write32(hw, Y2_QADDR(rxq, PREF_UNIT_CTRL), PREF_UNIT_RST_SET);
}

/* Clean out receive buffer area, assumes receiver hardware stopped */
static void sky2_rx_clean(struct sky2_port *sky2)
{
	unsigned i;

	memset(sky2->rx_le, 0, RX_LE_BYTES);
	for (i = 0; i < sky2->rx_pending; i++) {
		struct rx_ring_info *re = sky2->rx_ring + i;

		if (re->skb) {
			sky2_rx_unmap_skb(sky2->hw->pdev, re);
			kfree_skb(re->skb);
			re->skb = NULL;
		}
	}
}

/* Basic MII support */
static int sky2_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct mii_ioctl_data *data = if_mii(ifr);
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;
	int err = -EOPNOTSUPP;

	if (!netif_running(dev))
		return -ENODEV;	/* Phy still in reset */

	switch (cmd) {
	case SIOCGMIIPHY:
		data->phy_id = PHY_ADDR_MARV;

		/* fallthru */
	case SIOCGMIIREG: {
		u16 val = 0;

		spin_lock_bh(&sky2->phy_lock);
		err = __gm_phy_read(hw, sky2->port, data->reg_num & 0x1f, &val);
		spin_unlock_bh(&sky2->phy_lock);

		data->val_out = val;
		break;
	}

	case SIOCSMIIREG:
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;

		spin_lock_bh(&sky2->phy_lock);
		err = gm_phy_write(hw, sky2->port, data->reg_num & 0x1f,
				   data->val_in);
		spin_unlock_bh(&sky2->phy_lock);
		break;
	}
	return err;
}

#ifdef SKY2_VLAN_TAG_USED
static void sky2_vlan_rx_register(struct net_device *dev, struct vlan_group *grp)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;
	u16 port = sky2->port;

	netif_tx_lock_bh(dev);

	sky2_write32(hw, SK_REG(port, RX_GMF_CTRL_T), RX_VLAN_STRIP_ON);
	sky2_write32(hw, SK_REG(port, TX_GMF_CTRL_T), TX_VLAN_TAG_ON);
	sky2->vlgrp = grp;

	netif_tx_unlock_bh(dev);
}

static void sky2_vlan_rx_kill_vid(struct net_device *dev, unsigned short vid)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;
	u16 port = sky2->port;

	netif_tx_lock_bh(dev);

	sky2_write32(hw, SK_REG(port, RX_GMF_CTRL_T), RX_VLAN_STRIP_OFF);
	sky2_write32(hw, SK_REG(port, TX_GMF_CTRL_T), TX_VLAN_TAG_OFF);
	vlan_group_set_device(sky2->vlgrp, vid, NULL);

	netif_tx_unlock_bh(dev);
}
#endif

/*
 * Allocate an skb for receiving. If the MTU is large enough
 * make the skb non-linear with a fragment list of pages.
 *
 * It appears the hardware has a bug in the FIFO logic that
 * cause it to hang if the FIFO gets overrun and the receive buffer
 * is not 64 byte aligned. The buffer returned from netdev_alloc_skb is
 * aligned except if slab debugging is enabled.
 */
static struct sk_buff *sky2_rx_alloc(struct sky2_port *sky2)
{
	struct sk_buff *skb;
	unsigned long p;
	int i;

	skb = netdev_alloc_skb(sky2->netdev, sky2->rx_data_size + RX_SKB_ALIGN);
	if (!skb)
		goto nomem;

	p = (unsigned long) skb->data;
	skb_reserve(skb, ALIGN(p, RX_SKB_ALIGN) - p);

	for (i = 0; i < sky2->rx_nfrags; i++) {
		struct page *page = alloc_page(GFP_ATOMIC);

		if (!page)
			goto free_partial;
		skb_fill_page_desc(skb, i, page, 0, PAGE_SIZE);
	}

	return skb;
free_partial:
	kfree_skb(skb);
nomem:
	return NULL;
}

/*
 * Allocate and setup receiver buffer pool.
 * Normal case this ends up creating one list element for skb
 * in the receive ring. Worst case if using large MTU and each
 * allocation falls on a different 64 bit region, that results
 * in 6 list elements per ring entry.
 * One element is used for checksum enable/disable, and one
 * extra to avoid wrap.
 */
static int sky2_rx_start(struct sky2_port *sky2)
{
	struct sky2_hw *hw = sky2->hw;
	struct rx_ring_info *re;
	unsigned rxq = rxqaddr[sky2->port];
	unsigned i, size, space, thresh;

	sky2->rx_put = sky2->rx_next = 0;
	sky2_qset(hw, rxq);

	/* On PCI express lowering the watermark gives better performance */
	if (pci_find_capability(hw->pdev, PCI_CAP_ID_EXP))
		sky2_write32(hw, Q_ADDR(rxq, Q_WM), BMU_WM_PEX);

	/* These chips have no ram buffer?
	 * MAC Rx RAM Read is controlled by hardware */
	if (hw->chip_id == CHIP_ID_YUKON_EC_U &&
	    (hw->chip_rev == CHIP_REV_YU_EC_U_A1
	     || hw->chip_rev == CHIP_REV_YU_EC_U_B0))
		sky2_write32(hw, Q_ADDR(rxq, Q_F), F_M_RX_RAM_DIS);

	sky2_prefetch_init(hw, rxq, sky2->rx_le_map, RX_LE_SIZE - 1);

	rx_set_checksum(sky2);

	/* Space needed for frame data + headers rounded up */
	size = ALIGN(sky2->netdev->mtu + ETH_HLEN + VLAN_HLEN, 8)
		+ 8;

	/* Stopping point for hardware truncation */
	thresh = (size - 8) / sizeof(u32);

	/* Account for overhead of skb - to avoid order > 0 allocation */
	space = SKB_DATA_ALIGN(size) + NET_SKB_PAD
		+ sizeof(struct skb_shared_info);

	sky2->rx_nfrags = space >> PAGE_SHIFT;
	BUG_ON(sky2->rx_nfrags > ARRAY_SIZE(re->frag_addr));

	if (sky2->rx_nfrags != 0) {
		/* Compute residue after pages */
		space = sky2->rx_nfrags << PAGE_SHIFT;

		if (space < size)
			size -= space;
		else
			size = 0;

		/* Optimize to handle small packets and headers */
		if (size < copybreak)
			size = copybreak;
		if (size < ETH_HLEN)
			size = ETH_HLEN;
	}
	sky2->rx_data_size = size;

	/* Fill Rx ring */
	for (i = 0; i < sky2->rx_pending; i++) {
		re = sky2->rx_ring + i;

		re->skb = sky2_rx_alloc(sky2);
		if (!re->skb)
			goto nomem;

		sky2_rx_map_skb(hw->pdev, re, sky2->rx_data_size);
		sky2_rx_submit(sky2, re);
	}

	/*
	 * The receiver hangs if it receives frames larger than the
	 * packet buffer. As a workaround, truncate oversize frames, but
	 * the register is limited to 9 bits, so if you do frames > 2052
	 * you better get the MTU right!
	 */
	if (thresh > 0x1ff)
		sky2_write32(hw, SK_REG(sky2->port, RX_GMF_CTRL_T), RX_TRUNC_OFF);
	else {
		sky2_write16(hw, SK_REG(sky2->port, RX_GMF_TR_THR), thresh);
		sky2_write32(hw, SK_REG(sky2->port, RX_GMF_CTRL_T), RX_TRUNC_ON);
	}

	/* Tell chip about available buffers */
	sky2_write16(hw, Y2_QADDR(rxq, PREF_UNIT_PUT_IDX), sky2->rx_put);
	return 0;
nomem:
	sky2_rx_clean(sky2);
	return -ENOMEM;
}

/* Bring up network interface. */
static int sky2_up(struct net_device *dev)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;
	unsigned port = sky2->port;
	u32 ramsize, imask;
	int cap, err = -ENOMEM;
	struct net_device *otherdev = hw->dev[sky2->port^1];

	/*
 	 * On dual port PCI-X card, there is an problem where status
	 * can be received out of order due to split transactions
	 */
	if (otherdev && netif_running(otherdev) &&
 	    (cap = pci_find_capability(hw->pdev, PCI_CAP_ID_PCIX))) {
 		struct sky2_port *osky2 = netdev_priv(otherdev);
 		u16 cmd;

 		cmd = sky2_pci_read16(hw, cap + PCI_X_CMD);
 		cmd &= ~PCI_X_CMD_MAX_SPLIT;
 		sky2_pci_write16(hw, cap + PCI_X_CMD, cmd);

 		sky2->rx_csum = 0;
 		osky2->rx_csum = 0;
 	}

	if (netif_msg_ifup(sky2))
		printk(KERN_INFO PFX "%s: enabling interface\n", dev->name);

	/* must be power of 2 */
	sky2->tx_le = pci_alloc_consistent(hw->pdev,
					   TX_RING_SIZE *
					   sizeof(struct sky2_tx_le),
					   &sky2->tx_le_map);
	if (!sky2->tx_le)
		goto err_out;

	sky2->tx_ring = kcalloc(TX_RING_SIZE, sizeof(struct tx_ring_info),
				GFP_KERNEL);
	if (!sky2->tx_ring)
		goto err_out;
	sky2->tx_prod = sky2->tx_cons = 0;

	sky2->rx_le = pci_alloc_consistent(hw->pdev, RX_LE_BYTES,
					   &sky2->rx_le_map);
	if (!sky2->rx_le)
		goto err_out;
	memset(sky2->rx_le, 0, RX_LE_BYTES);

	sky2->rx_ring = kcalloc(sky2->rx_pending, sizeof(struct rx_ring_info),
				GFP_KERNEL);
	if (!sky2->rx_ring)
		goto err_out;

	sky2_phy_power(hw, port, 1);

	sky2_mac_init(hw, port);

	/* Register is number of 4K blocks on internal RAM buffer. */
	ramsize = sky2_read8(hw, B2_E_0) * 4;
	printk(KERN_INFO PFX "%s: ram buffer %dK\n", dev->name, ramsize);

	if (ramsize > 0) {
		u32 rxspace;

		if (ramsize < 16)
			rxspace = ramsize / 2;
		else
			rxspace = 8 + (2*(ramsize - 16))/3;

		sky2_ramset(hw, rxqaddr[port], 0, rxspace);
		sky2_ramset(hw, txqaddr[port], rxspace, ramsize - rxspace);

		/* Make sure SyncQ is disabled */
		sky2_write8(hw, RB_ADDR(port == 0 ? Q_XS1 : Q_XS2, RB_CTRL),
			    RB_RST_SET);
	}

	sky2_qset(hw, txqaddr[port]);

	/* Set almost empty threshold */
	if (hw->chip_id == CHIP_ID_YUKON_EC_U
	    && hw->chip_rev == CHIP_REV_YU_EC_U_A0)
		sky2_write16(hw, Q_ADDR(txqaddr[port], Q_AL), ECU_TXFF_LEV);

	sky2_prefetch_init(hw, txqaddr[port], sky2->tx_le_map,
			   TX_RING_SIZE - 1);

	err = sky2_rx_start(sky2);
	if (err)
		goto err_out;

	/* Enable interrupts from phy/mac for port */
	imask = sky2_read32(hw, B0_IMSK);
	imask |= portirq_msk[port];
	sky2_write32(hw, B0_IMSK, imask);

	return 0;

err_out:
	if (sky2->rx_le) {
		pci_free_consistent(hw->pdev, RX_LE_BYTES,
				    sky2->rx_le, sky2->rx_le_map);
		sky2->rx_le = NULL;
	}
	if (sky2->tx_le) {
		pci_free_consistent(hw->pdev,
				    TX_RING_SIZE * sizeof(struct sky2_tx_le),
				    sky2->tx_le, sky2->tx_le_map);
		sky2->tx_le = NULL;
	}
	kfree(sky2->tx_ring);
	kfree(sky2->rx_ring);

	sky2->tx_ring = NULL;
	sky2->rx_ring = NULL;
	return err;
}

/* Modular subtraction in ring */
static inline int tx_dist(unsigned tail, unsigned head)
{
	return (head - tail) & (TX_RING_SIZE - 1);
}

/* Number of list elements available for next tx */
static inline int tx_avail(const struct sky2_port *sky2)
{
	return sky2->tx_pending - tx_dist(sky2->tx_cons, sky2->tx_prod);
}

/* Estimate of number of transmit list elements required */
static unsigned tx_le_req(const struct sk_buff *skb)
{
	unsigned count;

	count = sizeof(dma_addr_t) / sizeof(u32);
	count += skb_shinfo(skb)->nr_frags * count;

	if (skb_is_gso(skb))
		++count;

	if (skb->ip_summed == CHECKSUM_PARTIAL)
		++count;

	return count;
}

/*
 * Put one packet in ring for transmit.
 * A single packet can generate multiple list elements, and
 * the number of ring elements will probably be less than the number
 * of list elements used.
 */
static int sky2_xmit_frame(struct sk_buff *skb, struct net_device *dev)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;
	struct sky2_tx_le *le = NULL;
	struct tx_ring_info *re;
	unsigned i, len;
	dma_addr_t mapping;
	u32 addr64;
	u16 mss;
	u8 ctrl;

 	if (unlikely(tx_avail(sky2) < tx_le_req(skb)))
  		return NETDEV_TX_BUSY;

	if (unlikely(netif_msg_tx_queued(sky2)))
		printk(KERN_DEBUG "%s: tx queued, slot %u, len %d\n",
		       dev->name, sky2->tx_prod, skb->len);

	len = skb_headlen(skb);
	mapping = pci_map_single(hw->pdev, skb->data, len, PCI_DMA_TODEVICE);
	addr64 = high32(mapping);

	/* Send high bits if changed or crosses boundary */
	if (addr64 != sky2->tx_addr64 || high32(mapping + len) != sky2->tx_addr64) {
		le = get_tx_le(sky2);
		le->addr = cpu_to_le32(addr64);
		le->opcode = OP_ADDR64 | HW_OWNER;
		sky2->tx_addr64 = high32(mapping + len);
	}

	/* Check for TCP Segmentation Offload */
	mss = skb_shinfo(skb)->gso_size;
	if (mss != 0) {
		mss += ((skb->h.th->doff - 5) * 4);	/* TCP options */
		mss += ip_hdrlen(skb) + sizeof(struct tcphdr);
		mss += ETH_HLEN;

		if (mss != sky2->tx_last_mss) {
			le = get_tx_le(sky2);
			le->addr = cpu_to_le32(mss);
			le->opcode = OP_LRGLEN | HW_OWNER;
			sky2->tx_last_mss = mss;
		}
	}

	ctrl = 0;
#ifdef SKY2_VLAN_TAG_USED
	/* Add VLAN tag, can piggyback on LRGLEN or ADDR64 */
	if (sky2->vlgrp && vlan_tx_tag_present(skb)) {
		if (!le) {
			le = get_tx_le(sky2);
			le->addr = 0;
			le->opcode = OP_VLAN|HW_OWNER;
		} else
			le->opcode |= OP_VLAN;
		le->length = cpu_to_be16(vlan_tx_tag_get(skb));
		ctrl |= INS_VLAN;
	}
#endif

	/* Handle TCP checksum offload */
	if (skb->ip_summed == CHECKSUM_PARTIAL) {
		const unsigned offset = skb_transport_offset(skb);
		u32 tcpsum;

		tcpsum = offset << 16;		/* sum start */
		tcpsum |= offset + skb->csum_offset;	/* sum write */

		ctrl = CALSUM | WR_SUM | INIT_SUM | LOCK_SUM;
		if (skb->nh.iph->protocol == IPPROTO_UDP)
			ctrl |= UDPTCP;

		if (tcpsum != sky2->tx_tcpsum) {
			sky2->tx_tcpsum = tcpsum;

			le = get_tx_le(sky2);
			le->addr = cpu_to_le32(tcpsum);
			le->length = 0;	/* initial checksum value */
			le->ctrl = 1;	/* one packet */
			le->opcode = OP_TCPLISW | HW_OWNER;
		}
	}

	le = get_tx_le(sky2);
	le->addr = cpu_to_le32((u32) mapping);
	le->length = cpu_to_le16(len);
	le->ctrl = ctrl;
	le->opcode = mss ? (OP_LARGESEND | HW_OWNER) : (OP_PACKET | HW_OWNER);

	re = tx_le_re(sky2, le);
	re->skb = skb;
	pci_unmap_addr_set(re, mapaddr, mapping);
	pci_unmap_len_set(re, maplen, len);

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		const skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

		mapping = pci_map_page(hw->pdev, frag->page, frag->page_offset,
				       frag->size, PCI_DMA_TODEVICE);
		addr64 = high32(mapping);
		if (addr64 != sky2->tx_addr64) {
			le = get_tx_le(sky2);
			le->addr = cpu_to_le32(addr64);
			le->ctrl = 0;
			le->opcode = OP_ADDR64 | HW_OWNER;
			sky2->tx_addr64 = addr64;
		}

		le = get_tx_le(sky2);
		le->addr = cpu_to_le32((u32) mapping);
		le->length = cpu_to_le16(frag->size);
		le->ctrl = ctrl;
		le->opcode = OP_BUFFER | HW_OWNER;

		re = tx_le_re(sky2, le);
		re->skb = skb;
		pci_unmap_addr_set(re, mapaddr, mapping);
		pci_unmap_len_set(re, maplen, frag->size);
	}

	le->ctrl |= EOP;

	if (tx_avail(sky2) <= MAX_SKB_TX_LE)
		netif_stop_queue(dev);

	sky2_put_idx(hw, txqaddr[sky2->port], sky2->tx_prod);

	dev->trans_start = jiffies;
	return NETDEV_TX_OK;
}

/*
 * Free ring elements from starting at tx_cons until "done"
 *
 * NB: the hardware will tell us about partial completion of multi-part
 *     buffers so make sure not to free skb to early.
 */
static void sky2_tx_complete(struct sky2_port *sky2, u16 done)
{
	struct net_device *dev = sky2->netdev;
	struct pci_dev *pdev = sky2->hw->pdev;
	unsigned idx;

	BUG_ON(done >= TX_RING_SIZE);

	for (idx = sky2->tx_cons; idx != done;
	     idx = RING_NEXT(idx, TX_RING_SIZE)) {
		struct sky2_tx_le *le = sky2->tx_le + idx;
		struct tx_ring_info *re = sky2->tx_ring + idx;

		switch(le->opcode & ~HW_OWNER) {
		case OP_LARGESEND:
		case OP_PACKET:
			pci_unmap_single(pdev,
					 pci_unmap_addr(re, mapaddr),
					 pci_unmap_len(re, maplen),
					 PCI_DMA_TODEVICE);
			break;
		case OP_BUFFER:
			pci_unmap_page(pdev, pci_unmap_addr(re, mapaddr),
				       pci_unmap_len(re, maplen),
				       PCI_DMA_TODEVICE);
			break;
		}

		if (le->ctrl & EOP) {
			if (unlikely(netif_msg_tx_done(sky2)))
				printk(KERN_DEBUG "%s: tx done %u\n",
				       dev->name, idx);
			sky2->net_stats.tx_packets++;
			sky2->net_stats.tx_bytes += re->skb->len;

			dev_kfree_skb_any(re->skb);
		}

		le->opcode = 0;	/* paranoia */
	}

	sky2->tx_cons = idx;
	if (tx_avail(sky2) > MAX_SKB_TX_LE + 4)
		netif_wake_queue(dev);
}

/* Cleanup all untransmitted buffers, assume transmitter not running */
static void sky2_tx_clean(struct net_device *dev)
{
	struct sky2_port *sky2 = netdev_priv(dev);

	netif_tx_lock_bh(dev);
	sky2_tx_complete(sky2, sky2->tx_prod);
	netif_tx_unlock_bh(dev);
}

/* Network shutdown */
static int sky2_down(struct net_device *dev)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;
	unsigned port = sky2->port;
	u16 ctrl;
	u32 imask;

	/* Never really got started! */
	if (!sky2->tx_le)
		return 0;

	if (netif_msg_ifdown(sky2))
		printk(KERN_INFO PFX "%s: disabling interface\n", dev->name);

	/* Stop more packets from being queued */
	netif_stop_queue(dev);
	netif_carrier_off(dev);

	/* Disable port IRQ */
	imask = sky2_read32(hw, B0_IMSK);
	imask &= ~portirq_msk[port];
	sky2_write32(hw, B0_IMSK, imask);

	/*
	 * Both ports share the NAPI poll on port 0, so if necessary undo the
	 * the disable that is done in dev_close.
	 */
	if (sky2->port == 0 && hw->ports > 1)
		netif_poll_enable(dev);

	sky2_gmac_reset(hw, port);

	/* Stop transmitter */
	sky2_write32(hw, Q_ADDR(txqaddr[port], Q_CSR), BMU_STOP);
	sky2_read32(hw, Q_ADDR(txqaddr[port], Q_CSR));

	sky2_write32(hw, RB_ADDR(txqaddr[port], RB_CTRL),
		     RB_RST_SET | RB_DIS_OP_MD);

	ctrl = gma_read16(hw, port, GM_GP_CTRL);
	ctrl &= ~(GM_GPCR_TX_ENA | GM_GPCR_RX_ENA);
	gma_write16(hw, port, GM_GP_CTRL, ctrl);

	sky2_write8(hw, SK_REG(port, GPHY_CTRL), GPC_RST_SET);

	/* Workaround shared GMAC reset */
	if (!(hw->chip_id == CHIP_ID_YUKON_XL && hw->chip_rev == 0
	      && port == 0 && hw->dev[1] && netif_running(hw->dev[1])))
		sky2_write8(hw, SK_REG(port, GMAC_CTRL), GMC_RST_SET);

	/* Disable Force Sync bit and Enable Alloc bit */
	sky2_write8(hw, SK_REG(port, TXA_CTRL),
		    TXA_DIS_FSYNC | TXA_DIS_ALLOC | TXA_STOP_RC);

	/* Stop Interval Timer and Limit Counter of Tx Arbiter */
	sky2_write32(hw, SK_REG(port, TXA_ITI_INI), 0L);
	sky2_write32(hw, SK_REG(port, TXA_LIM_INI), 0L);

	/* Reset the PCI FIFO of the async Tx queue */
	sky2_write32(hw, Q_ADDR(txqaddr[port], Q_CSR),
		     BMU_RST_SET | BMU_FIFO_RST);

	/* Reset the Tx prefetch units */
	sky2_write32(hw, Y2_QADDR(txqaddr[port], PREF_UNIT_CTRL),
		     PREF_UNIT_RST_SET);

	sky2_write32(hw, RB_ADDR(txqaddr[port], RB_CTRL), RB_RST_SET);

	sky2_rx_stop(sky2);

	sky2_write8(hw, SK_REG(port, RX_GMF_CTRL_T), GMF_RST_SET);
	sky2_write8(hw, SK_REG(port, TX_GMF_CTRL_T), GMF_RST_SET);

	sky2_phy_power(hw, port, 0);

	/* turn off LED's */
	sky2_write16(hw, B0_Y2LED, LED_STAT_OFF);

	synchronize_irq(hw->pdev->irq);

	sky2_tx_clean(dev);
	sky2_rx_clean(sky2);

	pci_free_consistent(hw->pdev, RX_LE_BYTES,
			    sky2->rx_le, sky2->rx_le_map);
	kfree(sky2->rx_ring);

	pci_free_consistent(hw->pdev,
			    TX_RING_SIZE * sizeof(struct sky2_tx_le),
			    sky2->tx_le, sky2->tx_le_map);
	kfree(sky2->tx_ring);

	sky2->tx_le = NULL;
	sky2->rx_le = NULL;

	sky2->rx_ring = NULL;
	sky2->tx_ring = NULL;

	return 0;
}

static u16 sky2_phy_speed(const struct sky2_hw *hw, u16 aux)
{
	if (!sky2_is_copper(hw))
		return SPEED_1000;

	if (hw->chip_id == CHIP_ID_YUKON_FE)
		return (aux & PHY_M_PS_SPEED_100) ? SPEED_100 : SPEED_10;

	switch (aux & PHY_M_PS_SPEED_MSK) {
	case PHY_M_PS_SPEED_1000:
		return SPEED_1000;
	case PHY_M_PS_SPEED_100:
		return SPEED_100;
	default:
		return SPEED_10;
	}
}

static void sky2_link_up(struct sky2_port *sky2)
{
	struct sky2_hw *hw = sky2->hw;
	unsigned port = sky2->port;
	u16 reg;
	static const char *fc_name[] = {
		[FC_NONE]	= "none",
		[FC_TX]		= "tx",
		[FC_RX]		= "rx",
		[FC_BOTH]	= "both",
	};

	/* enable Rx/Tx */
	reg = gma_read16(hw, port, GM_GP_CTRL);
	reg |= GM_GPCR_RX_ENA | GM_GPCR_TX_ENA;
	gma_write16(hw, port, GM_GP_CTRL, reg);

	gm_phy_write(hw, port, PHY_MARV_INT_MASK, PHY_M_DEF_MSK);

	netif_carrier_on(sky2->netdev);
	netif_wake_queue(sky2->netdev);

	/* Turn on link LED */
	sky2_write8(hw, SK_REG(port, LNK_LED_REG),
		    LINKLED_ON | LINKLED_BLINK_OFF | LINKLED_LINKSYNC_OFF);

	if (hw->chip_id == CHIP_ID_YUKON_XL
	    || hw->chip_id == CHIP_ID_YUKON_EC_U
	    || hw->chip_id == CHIP_ID_YUKON_EX) {
		u16 pg = gm_phy_read(hw, port, PHY_MARV_EXT_ADR);
		u16 led = PHY_M_LEDC_LOS_CTRL(1);	/* link active */

		switch(sky2->speed) {
		case SPEED_10:
			led |= PHY_M_LEDC_INIT_CTRL(7);
			break;

		case SPEED_100:
			led |= PHY_M_LEDC_STA1_CTRL(7);
			break;

		case SPEED_1000:
			led |= PHY_M_LEDC_STA0_CTRL(7);
			break;
		}

		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, 3);
		gm_phy_write(hw, port, PHY_MARV_PHY_CTRL, led);
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, pg);
	}

	if (netif_msg_link(sky2))
		printk(KERN_INFO PFX
		       "%s: Link is up at %d Mbps, %s duplex, flow control %s\n",
		       sky2->netdev->name, sky2->speed,
		       sky2->duplex == DUPLEX_FULL ? "full" : "half",
		       fc_name[sky2->flow_status]);
}

static void sky2_link_down(struct sky2_port *sky2)
{
	struct sky2_hw *hw = sky2->hw;
	unsigned port = sky2->port;
	u16 reg;

	gm_phy_write(hw, port, PHY_MARV_INT_MASK, 0);

	reg = gma_read16(hw, port, GM_GP_CTRL);
	reg &= ~(GM_GPCR_RX_ENA | GM_GPCR_TX_ENA);
	gma_write16(hw, port, GM_GP_CTRL, reg);

	netif_carrier_off(sky2->netdev);
	netif_stop_queue(sky2->netdev);

	/* Turn on link LED */
	sky2_write8(hw, SK_REG(port, LNK_LED_REG), LINKLED_OFF);

	if (netif_msg_link(sky2))
		printk(KERN_INFO PFX "%s: Link is down.\n", sky2->netdev->name);

	sky2_phy_init(hw, port);
}

static enum flow_control sky2_flow(int rx, int tx)
{
	if (rx)
		return tx ? FC_BOTH : FC_RX;
	else
		return tx ? FC_TX : FC_NONE;
}

static int sky2_autoneg_done(struct sky2_port *sky2, u16 aux)
{
	struct sky2_hw *hw = sky2->hw;
	unsigned port = sky2->port;
	u16 advert, lpa;

	advert = gm_phy_read(hw, port, PHY_MARV_AUNE_ADV);
	lpa = gm_phy_read(hw, port, PHY_MARV_AUNE_LP);
	if (lpa & PHY_M_AN_RF) {
		printk(KERN_ERR PFX "%s: remote fault", sky2->netdev->name);
		return -1;
	}

	if (!(aux & PHY_M_PS_SPDUP_RES)) {
		printk(KERN_ERR PFX "%s: speed/duplex mismatch",
		       sky2->netdev->name);
		return -1;
	}

	sky2->speed = sky2_phy_speed(hw, aux);
	sky2->duplex = (aux & PHY_M_PS_FULL_DUP) ? DUPLEX_FULL : DUPLEX_HALF;

	/* Since the pause result bits seem to in different positions on
	 * different chips. look at registers.
	 */
	if (!sky2_is_copper(hw)) {
		/* Shift for bits in fiber PHY */
		advert &= ~(ADVERTISE_PAUSE_CAP|ADVERTISE_PAUSE_ASYM);
		lpa &= ~(LPA_PAUSE_CAP|LPA_PAUSE_ASYM);

		if (advert & ADVERTISE_1000XPAUSE)
			advert |= ADVERTISE_PAUSE_CAP;
		if (advert & ADVERTISE_1000XPSE_ASYM)
			advert |= ADVERTISE_PAUSE_ASYM;
		if (lpa & LPA_1000XPAUSE)
			lpa |= LPA_PAUSE_CAP;
		if (lpa & LPA_1000XPAUSE_ASYM)
			lpa |= LPA_PAUSE_ASYM;
	}

	sky2->flow_status = FC_NONE;
	if (advert & ADVERTISE_PAUSE_CAP) {
		if (lpa & LPA_PAUSE_CAP)
			sky2->flow_status = FC_BOTH;
		else if (advert & ADVERTISE_PAUSE_ASYM)
			sky2->flow_status = FC_RX;
	} else if (advert & ADVERTISE_PAUSE_ASYM) {
		if ((lpa & LPA_PAUSE_CAP) && (lpa & LPA_PAUSE_ASYM))
			sky2->flow_status = FC_TX;
	}

	if (sky2->duplex == DUPLEX_HALF && sky2->speed < SPEED_1000
	    && !(hw->chip_id == CHIP_ID_YUKON_EC_U || hw->chip_id == CHIP_ID_YUKON_EX))
		sky2->flow_status = FC_NONE;

	if (sky2->flow_status & FC_TX)
		sky2_write8(hw, SK_REG(port, GMAC_CTRL), GMC_PAUSE_ON);
	else
		sky2_write8(hw, SK_REG(port, GMAC_CTRL), GMC_PAUSE_OFF);

	return 0;
}

/* Interrupt from PHY */
static void sky2_phy_intr(struct sky2_hw *hw, unsigned port)
{
	struct net_device *dev = hw->dev[port];
	struct sky2_port *sky2 = netdev_priv(dev);
	u16 istatus, phystat;

	if (!netif_running(dev))
		return;

	spin_lock(&sky2->phy_lock);
	istatus = gm_phy_read(hw, port, PHY_MARV_INT_STAT);
	phystat = gm_phy_read(hw, port, PHY_MARV_PHY_STAT);

	if (netif_msg_intr(sky2))
		printk(KERN_INFO PFX "%s: phy interrupt status 0x%x 0x%x\n",
		       sky2->netdev->name, istatus, phystat);

	if (sky2->autoneg == AUTONEG_ENABLE && (istatus & PHY_M_IS_AN_COMPL)) {
		if (sky2_autoneg_done(sky2, phystat) == 0)
			sky2_link_up(sky2);
		goto out;
	}

	if (istatus & PHY_M_IS_LSP_CHANGE)
		sky2->speed = sky2_phy_speed(hw, phystat);

	if (istatus & PHY_M_IS_DUP_CHANGE)
		sky2->duplex =
		    (phystat & PHY_M_PS_FULL_DUP) ? DUPLEX_FULL : DUPLEX_HALF;

	if (istatus & PHY_M_IS_LST_CHANGE) {
		if (phystat & PHY_M_PS_LINK_UP)
			sky2_link_up(sky2);
		else
			sky2_link_down(sky2);
	}
out:
	spin_unlock(&sky2->phy_lock);
}

/* Transmit timeout is only called if we are running, carrier is up
 * and tx queue is full (stopped).
 */
static void sky2_tx_timeout(struct net_device *dev)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;

	if (netif_msg_timer(sky2))
		printk(KERN_ERR PFX "%s: tx timeout\n", dev->name);

	printk(KERN_DEBUG PFX "%s: transmit ring %u .. %u report=%u done=%u\n",
	       dev->name, sky2->tx_cons, sky2->tx_prod,
	       sky2_read16(hw, sky2->port == 0 ? STAT_TXA1_RIDX : STAT_TXA2_RIDX),
	       sky2_read16(hw, Q_ADDR(txqaddr[sky2->port], Q_DONE)));

	/* can't restart safely under softirq */
	schedule_work(&hw->restart_work);
}

static int sky2_change_mtu(struct net_device *dev, int new_mtu)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;
	unsigned port = sky2->port;
	int err;
	u16 ctl, mode;
	u32 imask;

	if (new_mtu < ETH_ZLEN || new_mtu > ETH_JUMBO_MTU)
		return -EINVAL;

	if (new_mtu > ETH_DATA_LEN && hw->chip_id == CHIP_ID_YUKON_FE)
		return -EINVAL;

	if (!netif_running(dev)) {
		dev->mtu = new_mtu;
		return 0;
	}

	imask = sky2_read32(hw, B0_IMSK);
	sky2_write32(hw, B0_IMSK, 0);

	dev->trans_start = jiffies;	/* prevent tx timeout */
	netif_stop_queue(dev);
	netif_poll_disable(hw->dev[0]);

	synchronize_irq(hw->pdev->irq);

	if (hw->chip_id == CHIP_ID_YUKON_EC_U || hw->chip_id == CHIP_ID_YUKON_EX) {
		if (new_mtu > ETH_DATA_LEN) {
			sky2_write32(hw, SK_REG(port, TX_GMF_CTRL_T),
				     TX_JUMBO_ENA | TX_STFW_DIS);
			dev->features &= NETIF_F_TSO | NETIF_F_SG | NETIF_F_IP_CSUM;
		} else
			sky2_write32(hw, SK_REG(port, TX_GMF_CTRL_T),
				     TX_JUMBO_DIS | TX_STFW_ENA);
	}

	ctl = gma_read16(hw, port, GM_GP_CTRL);
	gma_write16(hw, port, GM_GP_CTRL, ctl & ~GM_GPCR_RX_ENA);
	sky2_rx_stop(sky2);
	sky2_rx_clean(sky2);

	dev->mtu = new_mtu;

	mode = DATA_BLIND_VAL(DATA_BLIND_DEF) |
		GM_SMOD_VLAN_ENA | IPG_DATA_VAL(IPG_DATA_DEF);

	if (dev->mtu > ETH_DATA_LEN)
		mode |= GM_SMOD_JUMBO_ENA;

	gma_write16(hw, port, GM_SERIAL_MODE, mode);

	sky2_write8(hw, RB_ADDR(rxqaddr[port], RB_CTRL), RB_ENA_OP_MD);

	err = sky2_rx_start(sky2);
	sky2_write32(hw, B0_IMSK, imask);

	if (err)
		dev_close(dev);
	else {
		gma_write16(hw, port, GM_GP_CTRL, ctl);

		netif_poll_enable(hw->dev[0]);
		netif_wake_queue(dev);
	}

	return err;
}

/* For small just reuse existing skb for next receive */
static struct sk_buff *receive_copy(struct sky2_port *sky2,
				    const struct rx_ring_info *re,
				    unsigned length)
{
	struct sk_buff *skb;

	skb = netdev_alloc_skb_ip_align(sky2->netdev, length);
	if (likely(skb)) {
		pci_dma_sync_single_for_cpu(sky2->hw->pdev, re->data_addr,
					    length, PCI_DMA_FROMDEVICE);
		memcpy(skb->data, re->skb->data, length);
		skb->ip_summed = re->skb->ip_summed;
		skb->csum = re->skb->csum;
		pci_dma_sync_single_for_device(sky2->hw->pdev, re->data_addr,
					       length, PCI_DMA_FROMDEVICE);
		re->skb->ip_summed = CHECKSUM_NONE;
		skb_put(skb, length);
	}
	return skb;
}

/* Adjust length of skb with fragments to match received data */
static void skb_put_frags(struct sk_buff *skb, unsigned int hdr_space,
			  unsigned int length)
{
	int i, num_frags;
	unsigned int size;

	/* put header into skb */
	size = min(length, hdr_space);
	skb->tail += size;
	skb->len += size;
	length -= size;

	num_frags = skb_shinfo(skb)->nr_frags;
	for (i = 0; i < num_frags; i++) {
		skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

		if (length == 0) {
			/* don't need this page */
			__free_page(frag->page);
			--skb_shinfo(skb)->nr_frags;
		} else {
			size = min(length, (unsigned) PAGE_SIZE);

			frag->size = size;
			skb->data_len += size;
			skb->truesize += size;
			skb->len += size;
			length -= size;
		}
	}
}

/* Normal packet - take skb from ring element and put in a new one  */
static struct sk_buff *receive_new(struct sky2_port *sky2,
				   struct rx_ring_info *re,
				   unsigned int length)
{
	struct sk_buff *skb, *nskb;
	unsigned hdr_space = sky2->rx_data_size;

	pr_debug(PFX "receive new length=%d\n", length);

	/* Don't be tricky about reusing pages (yet) */
	nskb = sky2_rx_alloc(sky2);
	if (unlikely(!nskb))
		return NULL;

	skb = re->skb;
	sky2_rx_unmap_skb(sky2->hw->pdev, re);

	prefetch(skb->data);
	re->skb = nskb;
	sky2_rx_map_skb(sky2->hw->pdev, re, hdr_space);

	if (skb_shinfo(skb)->nr_frags)
		skb_put_frags(skb, hdr_space, length);
	else
		skb_put(skb, length);
	return skb;
}

/*
 * Receive one packet.
 * For larger packets, get new buffer.
 */
static struct sk_buff *sky2_receive(struct net_device *dev,
				    u16 length, u32 status)
{
 	struct sky2_port *sky2 = netdev_priv(dev);
	struct rx_ring_info *re = sky2->rx_ring + sky2->rx_next;
	struct sk_buff *skb = NULL;

	if (unlikely(netif_msg_rx_status(sky2)))
		printk(KERN_DEBUG PFX "%s: rx slot %u status 0x%x len %d\n",
		       dev->name, sky2->rx_next, status, length);

	sky2->rx_next = (sky2->rx_next + 1) % sky2->rx_pending;
	prefetch(sky2->rx_ring + sky2->rx_next);

	if (status & GMR_FS_ANY_ERR)
		goto error;

	if (!(status & GMR_FS_RX_OK))
		goto resubmit;

	if (length < copybreak)
		skb = receive_copy(sky2, re, length);
	else
		skb = receive_new(sky2, re, length);
resubmit:
	sky2_rx_submit(sky2, re);

	return skb;

error:
	++sky2->net_stats.rx_errors;
	if (status & GMR_FS_RX_FF_OV) {
		sky2->net_stats.rx_over_errors++;
		goto resubmit;
	}

	if (netif_msg_rx_err(sky2) && net_ratelimit())
		printk(KERN_INFO PFX "%s: rx error, status 0x%x length %d\n",
		       dev->name, status, length);

	if (status & (GMR_FS_LONG_ERR | GMR_FS_UN_SIZE))
		sky2->net_stats.rx_length_errors++;
	if (status & GMR_FS_FRAGMENT)
		sky2->net_stats.rx_frame_errors++;
	if (status & GMR_FS_CRC_ERR)
		sky2->net_stats.rx_crc_errors++;

	goto resubmit;
}

/* Transmit complete */
static inline void sky2_tx_done(struct net_device *dev, u16 last)
{
	struct sky2_port *sky2 = netdev_priv(dev);

	if (netif_running(dev)) {
		netif_tx_lock(dev);
		sky2_tx_complete(sky2, last);
		netif_tx_unlock(dev);
	}
}

/* Process status response ring */
static int sky2_status_intr(struct sky2_hw *hw, int to_do)
{
	struct sky2_port *sky2;
	int work_done = 0;
	unsigned buf_write[2] = { 0, 0 };
	u16 hwidx = sky2_read16(hw, STAT_PUT_IDX);

	rmb();

	while (hw->st_idx != hwidx) {
		struct sky2_status_le *le  = hw->st_le + hw->st_idx;
		struct net_device *dev;
		struct sk_buff *skb;
		u32 status;
		u16 length;

		hw->st_idx = RING_NEXT(hw->st_idx, STATUS_RING_SIZE);

		BUG_ON(le->link >= 2);
		dev = hw->dev[le->link];

		sky2 = netdev_priv(dev);
		length = le16_to_cpu(le->length);
		status = le32_to_cpu(le->status);

		switch (le->opcode & ~HW_OWNER) {
		case OP_RXSTAT:
			skb = sky2_receive(dev, length, status);
			if (!skb)
				goto force_update;

			skb->protocol = eth_type_trans(skb, dev);
			sky2->net_stats.rx_packets++;
			sky2->net_stats.rx_bytes += skb->len;
#ifdef SKY2_VLAN_TAG_USED
			if (sky2->vlgrp && (status & GMR_FS_VLAN)) {
				vlan_hwaccel_receive_skb(skb,
							 sky2->vlgrp,
							 be16_to_cpu(sky2->rx_tag));
			} else
#endif
				netif_receive_skb(skb);

			/* Update receiver after 16 frames */
			if (++buf_write[le->link] == RX_BUF_WRITE) {
force_update:
				sky2_put_idx(hw, rxqaddr[le->link], sky2->rx_put);
				buf_write[le->link] = 0;
			}

			/* Stop after net poll weight */
			if (++work_done >= to_do)
				goto exit_loop;
			break;

#ifdef SKY2_VLAN_TAG_USED
		case OP_RXVLAN:
			sky2->rx_tag = length;
			break;

		case OP_RXCHKSVLAN:
			sky2->rx_tag = length;
			/* fall through */
#endif
		case OP_RXCHKS:
			if (!sky2->rx_csum)
				break;

			/* Both checksum counters are programmed to start at
			 * the same offset, so unless there is a problem they
			 * should match. This failure is an early indication that
			 * hardware receive checksumming won't work.
			 */
			if (likely(status >> 16 == (status & 0xffff))) {
				skb = sky2->rx_ring[sky2->rx_next].skb;
				skb->ip_summed = CHECKSUM_COMPLETE;
				skb->csum = status & 0xffff;
			} else {
				printk(KERN_NOTICE PFX "%s: hardware receive "
				       "checksum problem (status = %#x)\n",
				       dev->name, status);
				sky2->rx_csum = 0;
				sky2_write32(sky2->hw,
					     Q_ADDR(rxqaddr[le->link], Q_CSR),
					     BMU_DIS_RX_CHKSUM);
			}
			break;

		case OP_TXINDEXLE:
			/* TX index reports status for both ports */
			BUILD_BUG_ON(TX_RING_SIZE > 0x1000);
			sky2_tx_done(hw->dev[0], status & 0xfff);
			if (hw->dev[1])
				sky2_tx_done(hw->dev[1],
				     ((status >> 24) & 0xff)
					     | (u16)(length & 0xf) << 8);
			break;

		default:
			if (net_ratelimit())
				printk(KERN_WARNING PFX
				       "unknown status opcode 0x%x\n", le->opcode);
			goto exit_loop;
		}
	}

	/* Fully processed status ring so clear irq */
	sky2_write32(hw, STAT_CTRL, SC_STAT_CLR_IRQ);

exit_loop:
	if (buf_write[0]) {
		sky2 = netdev_priv(hw->dev[0]);
		sky2_put_idx(hw, Q_R1, sky2->rx_put);
	}

	if (buf_write[1]) {
		sky2 = netdev_priv(hw->dev[1]);
		sky2_put_idx(hw, Q_R2, sky2->rx_put);
	}

	return work_done;
}

static void sky2_hw_error(struct sky2_hw *hw, unsigned port, u32 status)
{
	struct net_device *dev = hw->dev[port];

	if (net_ratelimit())
		printk(KERN_INFO PFX "%s: hw error interrupt status 0x%x\n",
		       dev->name, status);

	if (status & Y2_IS_PAR_RD1) {
		if (net_ratelimit())
			printk(KERN_ERR PFX "%s: ram data read parity error\n",
			       dev->name);
		/* Clear IRQ */
		sky2_write16(hw, RAM_BUFFER(port, B3_RI_CTRL), RI_CLR_RD_PERR);
	}

	if (status & Y2_IS_PAR_WR1) {
		if (net_ratelimit())
			printk(KERN_ERR PFX "%s: ram data write parity error\n",
			       dev->name);

		sky2_write16(hw, RAM_BUFFER(port, B3_RI_CTRL), RI_CLR_WR_PERR);
	}

	if (status & Y2_IS_PAR_MAC1) {
		if (net_ratelimit())
			printk(KERN_ERR PFX "%s: MAC parity error\n", dev->name);
		sky2_write8(hw, SK_REG(port, TX_GMF_CTRL_T), GMF_CLI_TX_PE);
	}

	if (status & Y2_IS_PAR_RX1) {
		if (net_ratelimit())
			printk(KERN_ERR PFX "%s: RX parity error\n", dev->name);
		sky2_write32(hw, Q_ADDR(rxqaddr[port], Q_CSR), BMU_CLR_IRQ_PAR);
	}

	if (status & Y2_IS_TCP_TXA1) {
		if (net_ratelimit())
			printk(KERN_ERR PFX "%s: TCP segmentation error\n",
			       dev->name);
		sky2_write32(hw, Q_ADDR(txqaddr[port], Q_CSR), BMU_CLR_IRQ_TCP);
	}
}

static void sky2_hw_intr(struct sky2_hw *hw)
{
	u32 status = sky2_read32(hw, B0_HWE_ISRC);

	if (status & Y2_IS_TIST_OV)
		sky2_write8(hw, GMAC_TI_ST_CTRL, GMT_ST_CLR_IRQ);

	if (status & (Y2_IS_MST_ERR | Y2_IS_IRQ_STAT)) {
		u16 pci_err;

		pci_err = sky2_pci_read16(hw, PCI_STATUS);
		if (net_ratelimit())
			dev_err(&hw->pdev->dev, "PCI hardware error (0x%x)\n",
			        pci_err);

		sky2_write8(hw, B2_TST_CTRL1, TST_CFG_WRITE_ON);
		sky2_pci_write16(hw, PCI_STATUS,
				 pci_err | PCI_STATUS_ERROR_BITS);
		sky2_write8(hw, B2_TST_CTRL1, TST_CFG_WRITE_OFF);
	}

	if (status & Y2_IS_PCI_EXP) {
		/* PCI-Express uncorrectable Error occurred */
		u32 pex_err;

		pex_err = sky2_pci_read32(hw, PEX_UNC_ERR_STAT);

		if (net_ratelimit())
			dev_err(&hw->pdev->dev, "PCI Express error (0x%x)\n",
				pex_err);

		/* clear the interrupt */
		sky2_write32(hw, B2_TST_CTRL1, TST_CFG_WRITE_ON);
		sky2_pci_write32(hw, PEX_UNC_ERR_STAT,
				       0xffffffffUL);
		sky2_write32(hw, B2_TST_CTRL1, TST_CFG_WRITE_OFF);

		if (pex_err & PEX_FATAL_ERRORS) {
			u32 hwmsk = sky2_read32(hw, B0_HWE_IMSK);
			hwmsk &= ~Y2_IS_PCI_EXP;
			sky2_write32(hw, B0_HWE_IMSK, hwmsk);
		}
	}

	if (status & Y2_HWE_L1_MASK)
		sky2_hw_error(hw, 0, status);
	status >>= 8;
	if (status & Y2_HWE_L1_MASK)
		sky2_hw_error(hw, 1, status);
}

static void sky2_mac_intr(struct sky2_hw *hw, unsigned port)
{
	struct net_device *dev = hw->dev[port];
	struct sky2_port *sky2 = netdev_priv(dev);
	u8 status = sky2_read8(hw, SK_REG(port, GMAC_IRQ_SRC));

	if (netif_msg_intr(sky2))
		printk(KERN_INFO PFX "%s: mac interrupt status 0x%x\n",
		       dev->name, status);

	if (status & GM_IS_RX_FF_OR) {
		++sky2->net_stats.rx_fifo_errors;
		sky2_write8(hw, SK_REG(port, RX_GMF_CTRL_T), GMF_CLI_RX_FO);
	}

	if (status & GM_IS_TX_FF_UR) {
		++sky2->net_stats.tx_fifo_errors;
		sky2_write8(hw, SK_REG(port, TX_GMF_CTRL_T), GMF_CLI_TX_FU);
	}
}

/* This should never happen it is a bug. */
static void sky2_le_error(struct sky2_hw *hw, unsigned port,
			  u16 q, unsigned ring_size)
{
	struct net_device *dev = hw->dev[port];
	struct sky2_port *sky2 = netdev_priv(dev);
	unsigned idx;
	const u64 *le = (q == Q_R1 || q == Q_R2)
		? (u64 *) sky2->rx_le : (u64 *) sky2->tx_le;

	idx = sky2_read16(hw, Y2_QADDR(q, PREF_UNIT_GET_IDX));
	printk(KERN_ERR PFX "%s: descriptor error q=%#x get=%u [%llx] put=%u\n",
	       dev->name, (unsigned) q, idx, (unsigned long long) le[idx],
	       (unsigned) sky2_read16(hw, Y2_QADDR(q, PREF_UNIT_PUT_IDX)));

	sky2_write32(hw, Q_ADDR(q, Q_CSR), BMU_CLR_IRQ_CHK);
}

/* If idle then force a fake soft NAPI poll once a second
 * to work around cases where sharing an edge triggered interrupt.
 */
static inline void sky2_idle_start(struct sky2_hw *hw)
{
	if (idle_timeout > 0)
		mod_timer(&hw->idle_timer,
			  jiffies + msecs_to_jiffies(idle_timeout));
}

static void sky2_idle(unsigned long arg)
{
	struct sky2_hw *hw = (struct sky2_hw *) arg;
	struct net_device *dev = hw->dev[0];

	if (__netif_rx_schedule_prep(dev))
		__netif_rx_schedule(dev);

	mod_timer(&hw->idle_timer, jiffies + msecs_to_jiffies(idle_timeout));
}

/* Hardware/software error handling */
static void sky2_err_intr(struct sky2_hw *hw, u32 status)
{
	if (net_ratelimit())
		dev_warn(&hw->pdev->dev, "error interrupt status=%#x\n", status);

	if (status & Y2_IS_HW_ERR)
		sky2_hw_intr(hw);

	if (status & Y2_IS_IRQ_MAC1)
		sky2_mac_intr(hw, 0);

	if (status & Y2_IS_IRQ_MAC2)
		sky2_mac_intr(hw, 1);

	if (status & Y2_IS_CHK_RX1)
		sky2_le_error(hw, 0, Q_R1, RX_LE_SIZE);

	if (status & Y2_IS_CHK_RX2)
		sky2_le_error(hw, 1, Q_R2, RX_LE_SIZE);

	if (status & Y2_IS_CHK_TXA1)
		sky2_le_error(hw, 0, Q_XA1, TX_RING_SIZE);

	if (status & Y2_IS_CHK_TXA2)
		sky2_le_error(hw, 1, Q_XA2, TX_RING_SIZE);
}

static int sky2_poll(struct net_device *dev0, int *budget)
{
	struct sky2_hw *hw = ((struct sky2_port *) netdev_priv(dev0))->hw;
	int work_limit = min(dev0->quota, *budget);
	int work_done = 0;
	u32 status = sky2_read32(hw, B0_Y2_SP_EISR);

	if (unlikely(status & Y2_IS_ERROR))
		sky2_err_intr(hw, status);

	if (status & Y2_IS_IRQ_PHY1)
		sky2_phy_intr(hw, 0);

	if (status & Y2_IS_IRQ_PHY2)
		sky2_phy_intr(hw, 1);

	work_done = sky2_status_intr(hw, work_limit);
	if (work_done < work_limit) {
		/* Bug/Errata workaround?
		 * Need to kick the TX irq moderation timer.
		 */
		if (sky2_read8(hw, STAT_TX_TIMER_CTRL) == TIM_START) {
			sky2_write8(hw, STAT_TX_TIMER_CTRL, TIM_STOP);
			sky2_write8(hw, STAT_TX_TIMER_CTRL, TIM_START);
		}
		netif_rx_complete(dev0);

		sky2_read32(hw, B0_Y2_SP_LISR);
		return 0;
	} else {
		*budget -= work_done;
		dev0->quota -= work_done;
		return 1;
	}
}

static irqreturn_t sky2_intr(int irq, void *dev_id)
{
	struct sky2_hw *hw = dev_id;
	struct net_device *dev0 = hw->dev[0];
	u32 status;

	/* Reading this mask interrupts as side effect */
	status = sky2_read32(hw, B0_Y2_SP_ISRC2);
	if (status == 0 || status == ~0)
		return IRQ_NONE;

	prefetch(&hw->st_le[hw->st_idx]);
	if (likely(__netif_rx_schedule_prep(dev0)))
		__netif_rx_schedule(dev0);

	return IRQ_HANDLED;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void sky2_netpoll(struct net_device *dev)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct net_device *dev0 = sky2->hw->dev[0];

	if (netif_running(dev) && __netif_rx_schedule_prep(dev0))
		__netif_rx_schedule(dev0);
}
#endif

/* Chip internal frequency for clock calculations */
static inline u32 sky2_mhz(const struct sky2_hw *hw)
{
	switch (hw->chip_id) {
	case CHIP_ID_YUKON_EC:
	case CHIP_ID_YUKON_EC_U:
	case CHIP_ID_YUKON_EX:
		return 125;	/* 125 Mhz */
	case CHIP_ID_YUKON_FE:
		return 100;	/* 100 Mhz */
	default:		/* YUKON_XL */
		return 156;	/* 156 Mhz */
	}
}

static inline u32 sky2_us2clk(const struct sky2_hw *hw, u32 us)
{
	return sky2_mhz(hw) * us;
}

static inline u32 sky2_clk2us(const struct sky2_hw *hw, u32 clk)
{
	return clk / sky2_mhz(hw);
}


static int __devinit sky2_init(struct sky2_hw *hw)
{
	u8 t8;

	sky2_write8(hw, B0_CTST, CS_RST_CLR);

	hw->chip_id = sky2_read8(hw, B2_CHIP_ID);
	if (hw->chip_id < CHIP_ID_YUKON_XL || hw->chip_id > CHIP_ID_YUKON_FE) {
		dev_err(&hw->pdev->dev, "unsupported chip type 0x%x\n",
			hw->chip_id);
		return -EOPNOTSUPP;
	}

	if (hw->chip_id == CHIP_ID_YUKON_EX)
		dev_warn(&hw->pdev->dev, "this driver not yet tested on this chip type\n"
			 "Please report success or failure to <netdev@vger.kernel.org>\n");

	/* Make sure and enable all clocks */
	if (hw->chip_id == CHIP_ID_YUKON_EX || hw->chip_id == CHIP_ID_YUKON_EC_U)
		sky2_pci_write32(hw, PCI_DEV_REG3, 0);

	hw->chip_rev = (sky2_read8(hw, B2_MAC_CFG) & CFG_CHIP_R_MSK) >> 4;

	/* This rev is really old, and requires untested workarounds */
	if (hw->chip_id == CHIP_ID_YUKON_EC && hw->chip_rev == CHIP_REV_YU_EC_A1) {
		dev_err(&hw->pdev->dev, "unsupported revision Yukon-%s (0x%x) rev %d\n",
			yukon2_name[hw->chip_id - CHIP_ID_YUKON_XL],
			hw->chip_id, hw->chip_rev);
		return -EOPNOTSUPP;
	}

	hw->pmd_type = sky2_read8(hw, B2_PMD_TYP);
	hw->ports = 1;
	t8 = sky2_read8(hw, B2_Y2_HW_RES);
	if ((t8 & CFG_DUAL_MAC_MSK) == CFG_DUAL_MAC_MSK) {
		if (!(sky2_read8(hw, B2_Y2_CLK_GATE) & Y2_STATUS_LNK2_INAC))
			++hw->ports;
	}

	return 0;
}

static void sky2_reset(struct sky2_hw *hw)
{
	u16 status;
	int i;

	/* disable ASF */
	if (hw->chip_id == CHIP_ID_YUKON_EX) {
		status = sky2_read16(hw, HCU_CCSR);
		status &= ~(HCU_CCSR_AHB_RST | HCU_CCSR_CPU_RST_MODE |
			    HCU_CCSR_UC_STATE_MSK);
		sky2_write16(hw, HCU_CCSR, status);
	} else
		sky2_write8(hw, B28_Y2_ASF_STAT_CMD, Y2_ASF_RESET);
	sky2_write16(hw, B0_CTST, Y2_ASF_DISABLE);

	/* do a SW reset */
	sky2_write8(hw, B0_CTST, CS_RST_SET);
	sky2_write8(hw, B0_CTST, CS_RST_CLR);

	/* clear PCI errors, if any */
	status = sky2_pci_read16(hw, PCI_STATUS);

	sky2_write8(hw, B2_TST_CTRL1, TST_CFG_WRITE_ON);
	sky2_pci_write16(hw, PCI_STATUS, status | PCI_STATUS_ERROR_BITS);


	sky2_write8(hw, B0_CTST, CS_MRST_CLR);

	/* clear any PEX errors */
	if (pci_find_capability(hw->pdev, PCI_CAP_ID_EXP))
		sky2_pci_write32(hw, PEX_UNC_ERR_STAT, 0xffffffffUL);


	sky2_power_on(hw);

	for (i = 0; i < hw->ports; i++) {
		sky2_write8(hw, SK_REG(i, GMAC_LINK_CTRL), GMLC_RST_SET);
		sky2_write8(hw, SK_REG(i, GMAC_LINK_CTRL), GMLC_RST_CLR);
	}

	sky2_write8(hw, B2_TST_CTRL1, TST_CFG_WRITE_OFF);

	/* Clear I2C IRQ noise */
	sky2_write32(hw, B2_I2C_IRQ, 1);

	/* turn off hardware timer (unused) */
	sky2_write8(hw, B2_TI_CTRL, TIM_STOP);
	sky2_write8(hw, B2_TI_CTRL, TIM_CLR_IRQ);

	sky2_write8(hw, B0_Y2LED, LED_STAT_ON);

	/* Turn off descriptor polling */
	sky2_write32(hw, B28_DPT_CTRL, DPT_STOP);

	/* Turn off receive timestamp */
	sky2_write8(hw, GMAC_TI_ST_CTRL, GMT_ST_STOP);
	sky2_write8(hw, GMAC_TI_ST_CTRL, GMT_ST_CLR_IRQ);

	/* enable the Tx Arbiters */
	for (i = 0; i < hw->ports; i++)
		sky2_write8(hw, SK_REG(i, TXA_CTRL), TXA_ENA_ARB);

	/* Initialize ram interface */
	for (i = 0; i < hw->ports; i++) {
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_CTRL), RI_RST_CLR);

		sky2_write8(hw, RAM_BUFFER(i, B3_RI_WTO_R1), SK_RI_TO_53);
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_WTO_XA1), SK_RI_TO_53);
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_WTO_XS1), SK_RI_TO_53);
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_RTO_R1), SK_RI_TO_53);
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_RTO_XA1), SK_RI_TO_53);
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_RTO_XS1), SK_RI_TO_53);
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_WTO_R2), SK_RI_TO_53);
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_WTO_XA2), SK_RI_TO_53);
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_WTO_XS2), SK_RI_TO_53);
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_RTO_R2), SK_RI_TO_53);
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_RTO_XA2), SK_RI_TO_53);
		sky2_write8(hw, RAM_BUFFER(i, B3_RI_RTO_XS2), SK_RI_TO_53);
	}

	sky2_write32(hw, B0_HWE_IMSK, Y2_HWE_ALL_MASK);

	for (i = 0; i < hw->ports; i++)
		sky2_gmac_reset(hw, i);

	memset(hw->st_le, 0, STATUS_LE_BYTES);
	hw->st_idx = 0;

	sky2_write32(hw, STAT_CTRL, SC_STAT_RST_SET);
	sky2_write32(hw, STAT_CTRL, SC_STAT_RST_CLR);

	sky2_write32(hw, STAT_LIST_ADDR_LO, hw->st_dma);
	sky2_write32(hw, STAT_LIST_ADDR_HI, (u64) hw->st_dma >> 32);

	/* Set the list last index */
	sky2_write16(hw, STAT_LAST_IDX, STATUS_RING_SIZE - 1);

	sky2_write16(hw, STAT_TX_IDX_TH, 10);
	sky2_write8(hw, STAT_FIFO_WM, 16);

	/* set Status-FIFO ISR watermark */
	if (hw->chip_id == CHIP_ID_YUKON_XL && hw->chip_rev == 0)
		sky2_write8(hw, STAT_FIFO_ISR_WM, 4);
	else
		sky2_write8(hw, STAT_FIFO_ISR_WM, 16);

	sky2_write32(hw, STAT_TX_TIMER_INI, sky2_us2clk(hw, 1000));
	sky2_write32(hw, STAT_ISR_TIMER_INI, sky2_us2clk(hw, 20));
	sky2_write32(hw, STAT_LEV_TIMER_INI, sky2_us2clk(hw, 100));

	/* enable status unit */
	sky2_write32(hw, STAT_CTRL, SC_STAT_OP_ON);

	sky2_write8(hw, STAT_TX_TIMER_CTRL, TIM_START);
	sky2_write8(hw, STAT_LEV_TIMER_CTRL, TIM_START);
	sky2_write8(hw, STAT_ISR_TIMER_CTRL, TIM_START);
}

static void sky2_restart(struct work_struct *work)
{
	struct sky2_hw *hw = container_of(work, struct sky2_hw, restart_work);
	struct net_device *dev;
	int i, err;

	dev_dbg(&hw->pdev->dev, "restarting\n");

	del_timer_sync(&hw->idle_timer);

	rtnl_lock();
	sky2_write32(hw, B0_IMSK, 0);
	sky2_read32(hw, B0_IMSK);

	netif_poll_disable(hw->dev[0]);

	for (i = 0; i < hw->ports; i++) {
		dev = hw->dev[i];
		if (netif_running(dev))
			sky2_down(dev);
	}

	sky2_reset(hw);
	sky2_write32(hw, B0_IMSK, Y2_IS_BASE);
	netif_poll_enable(hw->dev[0]);

	for (i = 0; i < hw->ports; i++) {
		dev = hw->dev[i];
		if (netif_running(dev)) {
			err = sky2_up(dev);
			if (err) {
				printk(KERN_INFO PFX "%s: could not restart %d\n",
				       dev->name, err);
				dev_close(dev);
			}
		}
	}

	sky2_idle_start(hw);

	rtnl_unlock();
}

static inline u8 sky2_wol_supported(const struct sky2_hw *hw)
{
	return sky2_is_copper(hw) ? (WAKE_PHY | WAKE_MAGIC) : 0;
}

static void sky2_get_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	const struct sky2_port *sky2 = netdev_priv(dev);

	wol->supported = sky2_wol_supported(sky2->hw);
	wol->wolopts = sky2->wol;
}

static int sky2_set_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;

	if (wol->wolopts & ~sky2_wol_supported(sky2->hw))
		return -EOPNOTSUPP;

	sky2->wol = wol->wolopts;

	if (hw->chip_id == CHIP_ID_YUKON_EC_U)
		sky2_write32(hw, B0_CTST, sky2->wol
			     ? Y2_HW_WOL_ON : Y2_HW_WOL_OFF);

	if (!netif_running(dev))
		sky2_wol_init(sky2);
	return 0;
}

static u32 sky2_supported_modes(const struct sky2_hw *hw)
{
	if (sky2_is_copper(hw)) {
		u32 modes = SUPPORTED_10baseT_Half
			| SUPPORTED_10baseT_Full
			| SUPPORTED_100baseT_Half
			| SUPPORTED_100baseT_Full
			| SUPPORTED_Autoneg | SUPPORTED_TP;

		if (hw->chip_id != CHIP_ID_YUKON_FE)
			modes |= SUPPORTED_1000baseT_Half
				| SUPPORTED_1000baseT_Full;
		return modes;
	} else
		return  SUPPORTED_1000baseT_Half
			| SUPPORTED_1000baseT_Full
			| SUPPORTED_Autoneg
			| SUPPORTED_FIBRE;
}

static int sky2_get_settings(struct net_device *dev, struct ethtool_cmd *ecmd)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;

	ecmd->transceiver = XCVR_INTERNAL;
	ecmd->supported = sky2_supported_modes(hw);
	ecmd->phy_address = PHY_ADDR_MARV;
	if (sky2_is_copper(hw)) {
		ecmd->supported = SUPPORTED_10baseT_Half
		    | SUPPORTED_10baseT_Full
		    | SUPPORTED_100baseT_Half
		    | SUPPORTED_100baseT_Full
		    | SUPPORTED_1000baseT_Half
		    | SUPPORTED_1000baseT_Full
		    | SUPPORTED_Autoneg | SUPPORTED_TP;
		ecmd->port = PORT_TP;
		ecmd->speed = sky2->speed;
	} else {
		ecmd->speed = SPEED_1000;
		ecmd->port = PORT_FIBRE;
	}

	ecmd->advertising = sky2->advertising;
	ecmd->autoneg = sky2->autoneg;
	ecmd->duplex = sky2->duplex;
	return 0;
}

static int sky2_set_settings(struct net_device *dev, struct ethtool_cmd *ecmd)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	const struct sky2_hw *hw = sky2->hw;
	u32 supported = sky2_supported_modes(hw);

	if (ecmd->autoneg == AUTONEG_ENABLE) {
		ecmd->advertising = supported;
		sky2->duplex = -1;
		sky2->speed = -1;
	} else {
		u32 setting;

		switch (ecmd->speed) {
		case SPEED_1000:
			if (ecmd->duplex == DUPLEX_FULL)
				setting = SUPPORTED_1000baseT_Full;
			else if (ecmd->duplex == DUPLEX_HALF)
				setting = SUPPORTED_1000baseT_Half;
			else
				return -EINVAL;
			break;
		case SPEED_100:
			if (ecmd->duplex == DUPLEX_FULL)
				setting = SUPPORTED_100baseT_Full;
			else if (ecmd->duplex == DUPLEX_HALF)
				setting = SUPPORTED_100baseT_Half;
			else
				return -EINVAL;
			break;

		case SPEED_10:
			if (ecmd->duplex == DUPLEX_FULL)
				setting = SUPPORTED_10baseT_Full;
			else if (ecmd->duplex == DUPLEX_HALF)
				setting = SUPPORTED_10baseT_Half;
			else
				return -EINVAL;
			break;
		default:
			return -EINVAL;
		}

		if ((setting & supported) == 0)
			return -EINVAL;

		sky2->speed = ecmd->speed;
		sky2->duplex = ecmd->duplex;
	}

	sky2->autoneg = ecmd->autoneg;
	sky2->advertising = ecmd->advertising;

	if (netif_running(dev))
		sky2_phy_reinit(sky2);

	return 0;
}

static void sky2_get_drvinfo(struct net_device *dev,
			     struct ethtool_drvinfo *info)
{
	struct sky2_port *sky2 = netdev_priv(dev);

	strcpy(info->driver, DRV_NAME);
	strcpy(info->version, DRV_VERSION);
	strcpy(info->fw_version, "N/A");
	strcpy(info->bus_info, pci_name(sky2->hw->pdev));
}

static const struct sky2_stat {
	char name[ETH_GSTRING_LEN];
	u16 offset;
} sky2_stats[] = {
	{ "tx_bytes",	   GM_TXO_OK_HI },
	{ "rx_bytes",	   GM_RXO_OK_HI },
	{ "tx_broadcast",  GM_TXF_BC_OK },
	{ "rx_broadcast",  GM_RXF_BC_OK },
	{ "tx_multicast",  GM_TXF_MC_OK },
	{ "rx_multicast",  GM_RXF_MC_OK },
	{ "tx_unicast",    GM_TXF_UC_OK },
	{ "rx_unicast",    GM_RXF_UC_OK },
	{ "tx_mac_pause",  GM_TXF_MPAUSE },
	{ "rx_mac_pause",  GM_RXF_MPAUSE },
	{ "collisions",    GM_TXF_COL },
	{ "late_collision",GM_TXF_LAT_COL },
	{ "aborted", 	   GM_TXF_ABO_COL },
	{ "single_collisions", GM_TXF_SNG_COL },
	{ "multi_collisions", GM_TXF_MUL_COL },

	{ "rx_short",      GM_RXF_SHT },
	{ "rx_runt", 	   GM_RXE_FRAG },
	{ "rx_64_byte_packets", GM_RXF_64B },
	{ "rx_65_to_127_byte_packets", GM_RXF_127B },
	{ "rx_128_to_255_byte_packets", GM_RXF_255B },
	{ "rx_256_to_511_byte_packets", GM_RXF_511B },
	{ "rx_512_to_1023_byte_packets", GM_RXF_1023B },
	{ "rx_1024_to_1518_byte_packets", GM_RXF_1518B },
	{ "rx_1518_to_max_byte_packets", GM_RXF_MAX_SZ },
	{ "rx_too_long",   GM_RXF_LNG_ERR },
	{ "rx_fifo_overflow", GM_RXE_FIFO_OV },
	{ "rx_jabber",     GM_RXF_JAB_PKT },
	{ "rx_fcs_error",   GM_RXF_FCS_ERR },

	{ "tx_64_byte_packets", GM_TXF_64B },
	{ "tx_65_to_127_byte_packets", GM_TXF_127B },
	{ "tx_128_to_255_byte_packets", GM_TXF_255B },
	{ "tx_256_to_511_byte_packets", GM_TXF_511B },
	{ "tx_512_to_1023_byte_packets", GM_TXF_1023B },
	{ "tx_1024_to_1518_byte_packets", GM_TXF_1518B },
	{ "tx_1519_to_max_byte_packets", GM_TXF_MAX_SZ },
	{ "tx_fifo_underrun", GM_TXE_FIFO_UR },
};

static u32 sky2_get_rx_csum(struct net_device *dev)
{
	struct sky2_port *sky2 = netdev_priv(dev);

	return sky2->rx_csum;
}

static int sky2_set_rx_csum(struct net_device *dev, u32 data)
{
	struct sky2_port *sky2 = netdev_priv(dev);

	sky2->rx_csum = data;

	sky2_write32(sky2->hw, Q_ADDR(rxqaddr[sky2->port], Q_CSR),
		     data ? BMU_ENA_RX_CHKSUM : BMU_DIS_RX_CHKSUM);

	return 0;
}

static u32 sky2_get_msglevel(struct net_device *netdev)
{
	struct sky2_port *sky2 = netdev_priv(netdev);
	return sky2->msg_enable;
}

static int sky2_nway_reset(struct net_device *dev)
{
	struct sky2_port *sky2 = netdev_priv(dev);

	if (!netif_running(dev) || sky2->autoneg != AUTONEG_ENABLE)
		return -EINVAL;

	sky2_phy_reinit(sky2);

	return 0;
}

static void sky2_phy_stats(struct sky2_port *sky2, u64 * data, unsigned count)
{
	struct sky2_hw *hw = sky2->hw;
	unsigned port = sky2->port;
	int i;

	data[0] = (u64) gma_read32(hw, port, GM_TXO_OK_HI) << 32
	    | (u64) gma_read32(hw, port, GM_TXO_OK_LO);
	data[1] = (u64) gma_read32(hw, port, GM_RXO_OK_HI) << 32
	    | (u64) gma_read32(hw, port, GM_RXO_OK_LO);

	for (i = 2; i < count; i++)
		data[i] = (u64) gma_read32(hw, port, sky2_stats[i].offset);
}

static void sky2_set_msglevel(struct net_device *netdev, u32 value)
{
	struct sky2_port *sky2 = netdev_priv(netdev);
	sky2->msg_enable = value;
}

static int sky2_get_stats_count(struct net_device *dev)
{
	return ARRAY_SIZE(sky2_stats);
}

static void sky2_get_ethtool_stats(struct net_device *dev,
				   struct ethtool_stats *stats, u64 * data)
{
	struct sky2_port *sky2 = netdev_priv(dev);

	sky2_phy_stats(sky2, data, ARRAY_SIZE(sky2_stats));
}

static void sky2_get_strings(struct net_device *dev, u32 stringset, u8 * data)
{
	int i;

	switch (stringset) {
	case ETH_SS_STATS:
		for (i = 0; i < ARRAY_SIZE(sky2_stats); i++)
			memcpy(data + i * ETH_GSTRING_LEN,
			       sky2_stats[i].name, ETH_GSTRING_LEN);
		break;
	}
}

static struct net_device_stats *sky2_get_stats(struct net_device *dev)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	return &sky2->net_stats;
}

static int sky2_set_mac_address(struct net_device *dev, void *p)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;
	unsigned port = sky2->port;
	const struct sockaddr *addr = p;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);
	memcpy_toio(hw->regs + B2_MAC_1 + port * 8,
		    dev->dev_addr, ETH_ALEN);
	memcpy_toio(hw->regs + B2_MAC_2 + port * 8,
		    dev->dev_addr, ETH_ALEN);

	/* virtual address for data */
	gma_set_addr(hw, port, GM_SRC_ADDR_2L, dev->dev_addr);

	/* physical address: used for pause frames */
	gma_set_addr(hw, port, GM_SRC_ADDR_1L, dev->dev_addr);

	return 0;
}

static inline void sky2_add_filter(u8 filter[8], const u8 *addr)
{
	u32 bit;

	bit = ether_crc(ETH_ALEN, addr) & 63;
	filter[bit >> 3] |= 1 << (bit & 7);
}

static void sky2_set_multicast(struct net_device *dev)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;
	unsigned port = sky2->port;
	struct dev_mc_list *list = dev->mc_list;
	u16 reg;
	u8 filter[8];
	int rx_pause;
	static const u8 pause_mc_addr[ETH_ALEN] = { 0x1, 0x80, 0xc2, 0x0, 0x0, 0x1 };

	rx_pause = (sky2->flow_status == FC_RX || sky2->flow_status == FC_BOTH);
	memset(filter, 0, sizeof(filter));

	reg = gma_read16(hw, port, GM_RX_CTRL);
	reg |= GM_RXCR_UCF_ENA;

	if (dev->flags & IFF_PROMISC)	/* promiscuous */
		reg &= ~(GM_RXCR_UCF_ENA | GM_RXCR_MCF_ENA);
	else if (dev->flags & IFF_ALLMULTI)
		memset(filter, 0xff, sizeof(filter));
	else if (dev->mc_count == 0 && !rx_pause)
		reg &= ~GM_RXCR_MCF_ENA;
	else {
		int i;
		reg |= GM_RXCR_MCF_ENA;

		if (rx_pause)
			sky2_add_filter(filter, pause_mc_addr);

		for (i = 0; list && i < dev->mc_count; i++, list = list->next)
			sky2_add_filter(filter, list->dmi_addr);
	}

	gma_write16(hw, port, GM_MC_ADDR_H1,
		    (u16) filter[0] | ((u16) filter[1] << 8));
	gma_write16(hw, port, GM_MC_ADDR_H2,
		    (u16) filter[2] | ((u16) filter[3] << 8));
	gma_write16(hw, port, GM_MC_ADDR_H3,
		    (u16) filter[4] | ((u16) filter[5] << 8));
	gma_write16(hw, port, GM_MC_ADDR_H4,
		    (u16) filter[6] | ((u16) filter[7] << 8));

	gma_write16(hw, port, GM_RX_CTRL, reg);
}

/* Can have one global because blinking is controlled by
 * ethtool and that is always under RTNL mutex
 */
static void sky2_led(struct sky2_hw *hw, unsigned port, int on)
{
	u16 pg;

	switch (hw->chip_id) {
	case CHIP_ID_YUKON_XL:
		pg = gm_phy_read(hw, port, PHY_MARV_EXT_ADR);
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, 3);
		gm_phy_write(hw, port, PHY_MARV_PHY_CTRL,
			     on ? (PHY_M_LEDC_LOS_CTRL(1) |
				   PHY_M_LEDC_INIT_CTRL(7) |
				   PHY_M_LEDC_STA1_CTRL(7) |
				   PHY_M_LEDC_STA0_CTRL(7))
			     : 0);

		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, pg);
		break;

	default:
		gm_phy_write(hw, port, PHY_MARV_LED_CTRL, 0);
		gm_phy_write(hw, port, PHY_MARV_LED_OVER, 
			     on ? PHY_M_LED_ALL : 0);
	}
}

/* blink LED's for finding board */
static int sky2_phys_id(struct net_device *dev, u32 data)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;
	unsigned port = sky2->port;
	u16 ledctrl, ledover = 0;
	long ms;
	int interrupted;
	int onoff = 1;

	if (!data || data > (u32) (MAX_SCHEDULE_TIMEOUT / HZ))
		ms = jiffies_to_msecs(MAX_SCHEDULE_TIMEOUT);
	else
		ms = data * 1000;

	/* save initial values */
	spin_lock_bh(&sky2->phy_lock);
	if (hw->chip_id == CHIP_ID_YUKON_XL) {
		u16 pg = gm_phy_read(hw, port, PHY_MARV_EXT_ADR);
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, 3);
		ledctrl = gm_phy_read(hw, port, PHY_MARV_PHY_CTRL);
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, pg);
	} else {
		ledctrl = gm_phy_read(hw, port, PHY_MARV_LED_CTRL);
		ledover = gm_phy_read(hw, port, PHY_MARV_LED_OVER);
	}

	interrupted = 0;
	while (!interrupted && ms > 0) {
		sky2_led(hw, port, onoff);
		onoff = !onoff;

		spin_unlock_bh(&sky2->phy_lock);
		interrupted = msleep_interruptible(250);
		spin_lock_bh(&sky2->phy_lock);

		ms -= 250;
	}

	/* resume regularly scheduled programming */
	if (hw->chip_id == CHIP_ID_YUKON_XL) {
		u16 pg = gm_phy_read(hw, port, PHY_MARV_EXT_ADR);
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, 3);
		gm_phy_write(hw, port, PHY_MARV_PHY_CTRL, ledctrl);
		gm_phy_write(hw, port, PHY_MARV_EXT_ADR, pg);
	} else {
		gm_phy_write(hw, port, PHY_MARV_LED_CTRL, ledctrl);
		gm_phy_write(hw, port, PHY_MARV_LED_OVER, ledover);
	}
	spin_unlock_bh(&sky2->phy_lock);

	return 0;
}

static void sky2_get_pauseparam(struct net_device *dev,
				struct ethtool_pauseparam *ecmd)
{
	struct sky2_port *sky2 = netdev_priv(dev);

	switch (sky2->flow_mode) {
	case FC_NONE:
		ecmd->tx_pause = ecmd->rx_pause = 0;
		break;
	case FC_TX:
		ecmd->tx_pause = 1, ecmd->rx_pause = 0;
		break;
	case FC_RX:
		ecmd->tx_pause = 0, ecmd->rx_pause = 1;
		break;
	case FC_BOTH:
		ecmd->tx_pause = ecmd->rx_pause = 1;
	}

	ecmd->autoneg = sky2->autoneg;
}

static int sky2_set_pauseparam(struct net_device *dev,
			       struct ethtool_pauseparam *ecmd)
{
	struct sky2_port *sky2 = netdev_priv(dev);

	sky2->autoneg = ecmd->autoneg;
	sky2->flow_mode = sky2_flow(ecmd->rx_pause, ecmd->tx_pause);

	if (netif_running(dev))
		sky2_phy_reinit(sky2);

	return 0;
}

static int sky2_get_coalesce(struct net_device *dev,
			     struct ethtool_coalesce *ecmd)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;

	if (sky2_read8(hw, STAT_TX_TIMER_CTRL) == TIM_STOP)
		ecmd->tx_coalesce_usecs = 0;
	else {
		u32 clks = sky2_read32(hw, STAT_TX_TIMER_INI);
		ecmd->tx_coalesce_usecs = sky2_clk2us(hw, clks);
	}
	ecmd->tx_max_coalesced_frames = sky2_read16(hw, STAT_TX_IDX_TH);

	if (sky2_read8(hw, STAT_LEV_TIMER_CTRL) == TIM_STOP)
		ecmd->rx_coalesce_usecs = 0;
	else {
		u32 clks = sky2_read32(hw, STAT_LEV_TIMER_INI);
		ecmd->rx_coalesce_usecs = sky2_clk2us(hw, clks);
	}
	ecmd->rx_max_coalesced_frames = sky2_read8(hw, STAT_FIFO_WM);

	if (sky2_read8(hw, STAT_ISR_TIMER_CTRL) == TIM_STOP)
		ecmd->rx_coalesce_usecs_irq = 0;
	else {
		u32 clks = sky2_read32(hw, STAT_ISR_TIMER_INI);
		ecmd->rx_coalesce_usecs_irq = sky2_clk2us(hw, clks);
	}

	ecmd->rx_max_coalesced_frames_irq = sky2_read8(hw, STAT_FIFO_ISR_WM);

	return 0;
}

/* Note: this affect both ports */
static int sky2_set_coalesce(struct net_device *dev,
			     struct ethtool_coalesce *ecmd)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	struct sky2_hw *hw = sky2->hw;
	const u32 tmax = sky2_clk2us(hw, 0x0ffffff);

	if (ecmd->tx_coalesce_usecs > tmax ||
	    ecmd->rx_coalesce_usecs > tmax ||
	    ecmd->rx_coalesce_usecs_irq > tmax)
		return -EINVAL;

	if (ecmd->tx_max_coalesced_frames >= TX_RING_SIZE-1)
		return -EINVAL;
	if (ecmd->rx_max_coalesced_frames > RX_MAX_PENDING)
		return -EINVAL;
	if (ecmd->rx_max_coalesced_frames_irq >RX_MAX_PENDING)
		return -EINVAL;

	if (ecmd->tx_coalesce_usecs == 0)
		sky2_write8(hw, STAT_TX_TIMER_CTRL, TIM_STOP);
	else {
		sky2_write32(hw, STAT_TX_TIMER_INI,
			     sky2_us2clk(hw, ecmd->tx_coalesce_usecs));
		sky2_write8(hw, STAT_TX_TIMER_CTRL, TIM_START);
	}
	sky2_write16(hw, STAT_TX_IDX_TH, ecmd->tx_max_coalesced_frames);

	if (ecmd->rx_coalesce_usecs == 0)
		sky2_write8(hw, STAT_LEV_TIMER_CTRL, TIM_STOP);
	else {
		sky2_write32(hw, STAT_LEV_TIMER_INI,
			     sky2_us2clk(hw, ecmd->rx_coalesce_usecs));
		sky2_write8(hw, STAT_LEV_TIMER_CTRL, TIM_START);
	}
	sky2_write8(hw, STAT_FIFO_WM, ecmd->rx_max_coalesced_frames);

	if (ecmd->rx_coalesce_usecs_irq == 0)
		sky2_write8(hw, STAT_ISR_TIMER_CTRL, TIM_STOP);
	else {
		sky2_write32(hw, STAT_ISR_TIMER_INI,
			     sky2_us2clk(hw, ecmd->rx_coalesce_usecs_irq));
		sky2_write8(hw, STAT_ISR_TIMER_CTRL, TIM_START);
	}
	sky2_write8(hw, STAT_FIFO_ISR_WM, ecmd->rx_max_coalesced_frames_irq);
	return 0;
}

static void sky2_get_ringparam(struct net_device *dev,
			       struct ethtool_ringparam *ering)
{
	struct sky2_port *sky2 = netdev_priv(dev);

	ering->rx_max_pending = RX_MAX_PENDING;
	ering->rx_mini_max_pending = 0;
	ering->rx_jumbo_max_pending = 0;
	ering->tx_max_pending = TX_RING_SIZE - 1;

	ering->rx_pending = sky2->rx_pending;
	ering->rx_mini_pending = 0;
	ering->rx_jumbo_pending = 0;
	ering->tx_pending = sky2->tx_pending;
}

static int sky2_set_ringparam(struct net_device *dev,
			      struct ethtool_ringparam *ering)
{
	struct sky2_port *sky2 = netdev_priv(dev);
	int err = 0;

	if (ering->rx_pending > RX_MAX_PENDING ||
	    ering->rx_pending < 8 ||
	    ering->tx_pending < MAX_SKB_TX_LE ||
	    ering->tx_pending > TX_RING_SIZE - 1)
		return -EINVAL;

	if (netif_running(dev))
		sky2_down(dev);

	sky2->rx_pending = ering->rx_pending;
	sky2->tx_pending = ering->tx_pending;

	if (netif_running(dev)) {
		err = sky2_up(dev);
		if (err)
			dev_close(dev);
		else
			sky2_set_multicast(dev);
	}

	return err;
}

static int sky2_get_regs_len(struct net_device *dev)
{
	return 0x4000;
}

/*
 * Returns copy of control register region
 * Note: access to the RAM address register set will cause timeouts.
 */
static void sky2_get_regs(struct net_device *dev, struct ethtool_regs *regs,
			  void *p)
{
	const struct sky2_port *sky2 = netdev_priv(dev);
	const void __iomem *io = sky2->hw->regs;

	BUG_ON(regs->len < B3_RI_WTO_R1);
	regs->version = 1;
	memset(p, 0, regs->len);

	memcpy_fromio(p, io, B3_RAM_ADDR);

	memcpy_fromio(p + B3_RI_WTO_R1,
		      io + B3_RI_WTO_R1,
		      regs->len - B3_RI_WTO_R1);
}

/* In order to do Jumbo packets on these chips, need to turn off the
 * transmit store/forward. Therefore checksum offload won't work.
 */
static int no_tx_offload(struct net_device *dev)
{
	const struct sky2_port *sky2 = netdev_priv(dev);
	const struct sky2_hw *hw = sky2->hw;

	return dev->mtu > ETH_DATA_LEN &&
		(hw->chip_id == CHIP_ID_YUKON_EX
		 || hw->chip_id == CHIP_ID_YUKON_EC_U);
}

static int sky2_set_tx_csum(struct net_device *dev, u32 data)
{
	if (data && no_tx_offload(dev))
		return -EINVAL;

	return ethtool_op_set_tx_csum(dev, data);
}


static int sky2_set_tso(struct net_device *dev, u32 data)
{
	if (data && no_tx_offload(dev))
		return -EINVAL;

	return ethtool_op_set_tso(dev, data);
}

static const struct ethtool_ops sky2_ethtool_ops = {
	.get_settings = sky2_get_settings,
	.set_settings = sky2_set_settings,
	.get_drvinfo  = sky2_get_drvinfo,
	.get_wol      = sky2_get_wol,
	.set_wol      = sky2_set_wol,
	.get_msglevel = sky2_get_msglevel,
	.set_msglevel = sky2_set_msglevel,
	.nway_reset   = sky2_nway_reset,
	.get_regs_len = sky2_get_regs_len,
	.get_regs = sky2_get_regs,
	.get_link = ethtool_op_get_link,
	.get_sg = ethtool_op_get_sg,
	.set_sg = ethtool_op_set_sg,
	.get_tx_csum = ethtool_op_get_tx_csum,
	.set_tx_csum = sky2_set_tx_csum,
	.get_tso = ethtool_op_get_tso,
	.set_tso = sky2_set_tso,
	.get_rx_csum = sky2_get_rx_csum,
	.set_rx_csum = sky2_set_rx_csum,
	.get_strings = sky2_get_strings,
	.get_coalesce = sky2_get_coalesce,
	.set_coalesce = sky2_set_coalesce,
	.get_ringparam = sky2_get_ringparam,
	.set_ringparam = sky2_set_ringparam,
	.get_pauseparam = sky2_get_pauseparam,
	.set_pauseparam = sky2_set_pauseparam,
	.phys_id = sky2_phys_id,
	.get_stats_count = sky2_get_stats_count,
	.get_ethtool_stats = sky2_get_ethtool_stats,
	.get_perm_addr	= ethtool_op_get_perm_addr,
};

/* Initialize network device */
static __devinit struct net_device *sky2_init_netdev(struct sky2_hw *hw,
						     unsigned port,
						     int highmem, int wol)
{
	struct sky2_port *sky2;
	struct net_device *dev = alloc_etherdev(sizeof(*sky2));

	if (!dev) {
		dev_err(&hw->pdev->dev, "etherdev alloc failed");
		return NULL;
	}

	SET_NETDEV_DEV(dev, &hw->pdev->dev);
	dev->irq = hw->pdev->irq;
	dev->open = sky2_up;
	dev->stop = sky2_down;
	dev->do_ioctl = sky2_ioctl;
	dev->hard_start_xmit = sky2_xmit_frame;
	dev->get_stats = sky2_get_stats;
	dev->set_multicast_list = sky2_set_multicast;
	dev->set_mac_address = sky2_set_mac_address;
	dev->change_mtu = sky2_change_mtu;
	SET_ETHTOOL_OPS(dev, &sky2_ethtool_ops);
	dev->tx_timeout = sky2_tx_timeout;
	dev->watchdog_timeo = TX_WATCHDOG;
	if (port == 0)
		dev->poll = sky2_poll;
	dev->weight = NAPI_WEIGHT;
#ifdef CONFIG_NET_POLL_CONTROLLER
	/* Network console (only works on port 0)
	 * because netpoll makes assumptions about NAPI
	 */
	if (port == 0)
		dev->poll_controller = sky2_netpoll;
#endif

	sky2 = netdev_priv(dev);
	sky2->netdev = dev;
	sky2->hw = hw;
	sky2->msg_enable = netif_msg_init(debug, default_msg);

	/* Auto speed and flow control */
	sky2->autoneg = AUTONEG_ENABLE;
	sky2->flow_mode = FC_BOTH;

	sky2->duplex = -1;
	sky2->speed = -1;
	sky2->advertising = sky2_supported_modes(hw);
	sky2->rx_csum = 1;
	sky2->wol = wol;

	spin_lock_init(&sky2->phy_lock);
	sky2->tx_pending = TX_DEF_PENDING;
	sky2->rx_pending = RX_DEF_PENDING;

	hw->dev[port] = dev;

	sky2->port = port;

	dev->features |= NETIF_F_TSO | NETIF_F_IP_CSUM | NETIF_F_SG;
	if (highmem)
		dev->features |= NETIF_F_HIGHDMA;

#ifdef SKY2_VLAN_TAG_USED
	dev->features |= NETIF_F_HW_VLAN_TX | NETIF_F_HW_VLAN_RX;
	dev->vlan_rx_register = sky2_vlan_rx_register;
	dev->vlan_rx_kill_vid = sky2_vlan_rx_kill_vid;
#endif

	/* read the mac address */
	memcpy_fromio(dev->dev_addr, hw->regs + B2_MAC_1 + port * 8, ETH_ALEN);
	memcpy(dev->perm_addr, dev->dev_addr, dev->addr_len);

	/* device is off until link detection */
	netif_carrier_off(dev);
	netif_stop_queue(dev);

	return dev;
}

static void __devinit sky2_show_addr(struct net_device *dev)
{
	const struct sky2_port *sky2 = netdev_priv(dev);
	DECLARE_MAC_BUF(mac);

	if (netif_msg_probe(sky2))
		printk(KERN_INFO PFX "%s: addr %s\n",
		       dev->name, print_mac(mac, dev->dev_addr));
}

/* Handle software interrupt used during MSI test */
static irqreturn_t __devinit sky2_test_intr(int irq, void *dev_id)
{
	struct sky2_hw *hw = dev_id;
	u32 status = sky2_read32(hw, B0_Y2_SP_ISRC2);

	if (status == 0)
		return IRQ_NONE;

	if (status & Y2_IS_IRQ_SW) {
		hw->msi = 1;
		wake_up(&hw->msi_wait);
		sky2_write8(hw, B0_CTST, CS_CL_SW_IRQ);
	}
	sky2_write32(hw, B0_Y2_SP_ICR, 2);

	return IRQ_HANDLED;
}

/* Test interrupt path by forcing a a software IRQ */
static int __devinit sky2_test_msi(struct sky2_hw *hw)
{
	struct pci_dev *pdev = hw->pdev;
	int err;

	init_waitqueue_head (&hw->msi_wait);

	sky2_write32(hw, B0_IMSK, Y2_IS_IRQ_SW);

	err = request_irq(pdev->irq, sky2_test_intr, 0, DRV_NAME, hw);
	if (err) {
		dev_err(&pdev->dev, "cannot assign irq %d\n", pdev->irq);
		return err;
	}

	sky2_write8(hw, B0_CTST, CS_ST_SW_IRQ);
	sky2_read8(hw, B0_CTST);

	wait_event_timeout(hw->msi_wait, hw->msi, HZ/10);

	if (!hw->msi) {
		/* MSI test failed, go back to INTx mode */
		dev_info(&pdev->dev, "No interrupt generated using MSI, "
			 "switching to INTx mode.\n");

		err = -EOPNOTSUPP;
		sky2_write8(hw, B0_CTST, CS_CL_SW_IRQ);
	}

	sky2_write32(hw, B0_IMSK, 0);
	sky2_read32(hw, B0_IMSK);

	free_irq(pdev->irq, hw);

	return err;
}

static int __devinit pci_wake_enabled(struct pci_dev *dev)
{
	int pm  = pci_find_capability(dev, PCI_CAP_ID_PM);
	u16 value;

	if (!pm)
		return 0;
	if (pci_read_config_word(dev, pm + PCI_PM_CTRL, &value))
		return 0;
	return value & PCI_PM_CTRL_PME_ENABLE;
}

static int __devinit sky2_probe(struct pci_dev *pdev,
				const struct pci_device_id *ent)
{
	struct net_device *dev;
	struct sky2_hw *hw;
	int err, using_dac = 0, wol_default;

	err = pci_enable_device(pdev);
	if (err) {
		dev_err(&pdev->dev, "cannot enable PCI device\n");
		goto err_out;
	}

	err = pci_request_regions(pdev, DRV_NAME);
	if (err) {
		dev_err(&pdev->dev, "cannot obtain PCI resources\n");
		goto err_out;
	}

	pci_set_master(pdev);

	if (sizeof(dma_addr_t) > sizeof(u32) &&
	    !(err = pci_set_dma_mask(pdev, DMA_64BIT_MASK))) {
		using_dac = 1;
		err = pci_set_consistent_dma_mask(pdev, DMA_64BIT_MASK);
		if (err < 0) {
			dev_err(&pdev->dev, "unable to obtain 64 bit DMA "
				"for consistent allocations\n");
			goto err_out_free_regions;
		}
	} else {
		err = pci_set_dma_mask(pdev, DMA_32BIT_MASK);
		if (err) {
			dev_err(&pdev->dev, "no usable DMA configuration\n");
			goto err_out_free_regions;
		}
	}

	wol_default = pci_wake_enabled(pdev) ? WAKE_MAGIC : 0;

	err = -ENOMEM;
	hw = kzalloc(sizeof(*hw), GFP_KERNEL);
	if (!hw) {
		dev_err(&pdev->dev, "cannot allocate hardware struct\n");
		goto err_out_free_regions;
	}

	hw->pdev = pdev;

	hw->regs = ioremap_nocache(pci_resource_start(pdev, 0), 0x4000);
	if (!hw->regs) {
		dev_err(&pdev->dev, "cannot map device registers\n");
		goto err_out_free_hw;
	}

#ifdef __BIG_ENDIAN
	/* The sk98lin vendor driver uses hardware byte swapping but
	 * this driver uses software swapping.
	 */
	{
		u32 reg;
		reg = sky2_pci_read32(hw, PCI_DEV_REG2);
		reg &= ~PCI_REV_DESC;
		sky2_pci_write32(hw, PCI_DEV_REG2, reg);
	}
#endif

	/* ring for status responses */
	hw->st_le = pci_alloc_consistent(hw->pdev, STATUS_LE_BYTES,
					 &hw->st_dma);
	if (!hw->st_le)
		goto err_out_iounmap;

	err = sky2_init(hw);
	if (err)
		goto err_out_iounmap;

	dev_info(&pdev->dev, "v%s addr 0x%llx irq %d Yukon-%s (0x%x) rev %d\n",
	       DRV_VERSION, (unsigned long long)pci_resource_start(pdev, 0),
	       pdev->irq, yukon2_name[hw->chip_id - CHIP_ID_YUKON_XL],
	       hw->chip_id, hw->chip_rev);

	sky2_reset(hw);

	dev = sky2_init_netdev(hw, 0, using_dac, wol_default);
	if (!dev) {
		err = -ENOMEM;
		goto err_out_free_pci;
	}

	if (!disable_msi && pci_enable_msi(pdev) == 0) {
		err = sky2_test_msi(hw);
		if (err == -EOPNOTSUPP)
 			pci_disable_msi(pdev);
		else if (err)
			goto err_out_free_netdev;
 	}

	err = register_netdev(dev);
	if (err) {
		dev_err(&pdev->dev, "cannot register net device\n");
		goto err_out_free_netdev;
	}

	err = request_irq(pdev->irq,  sky2_intr, hw->msi ? 0 : IRQF_SHARED,
			  dev->name, hw);
	if (err) {
		dev_err(&pdev->dev, "cannot assign irq %d\n", pdev->irq);
		goto err_out_unregister;
	}
	sky2_write32(hw, B0_IMSK, Y2_IS_BASE);

	sky2_show_addr(dev);

	if (hw->ports > 1) {
		struct net_device *dev1;

		dev1 = sky2_init_netdev(hw, 1, using_dac, wol_default);
		if (!dev1)
			dev_warn(&pdev->dev, "allocation for second device failed\n");
		else if ((err = register_netdev(dev1))) {
			dev_warn(&pdev->dev,
				 "register of second port failed (%d)\n", err);
			hw->dev[1] = NULL;
			free_netdev(dev1);
		} else
			sky2_show_addr(dev1);
	}

	setup_timer(&hw->idle_timer, sky2_idle, (unsigned long) hw);
	INIT_WORK(&hw->restart_work, sky2_restart);

	sky2_idle_start(hw);

	pci_set_drvdata(pdev, hw);

	return 0;

err_out_unregister:
	if (hw->msi)
		pci_disable_msi(pdev);
	unregister_netdev(dev);
err_out_free_netdev:
	free_netdev(dev);
err_out_free_pci:
	sky2_write8(hw, B0_CTST, CS_RST_SET);
	pci_free_consistent(hw->pdev, STATUS_LE_BYTES, hw->st_le, hw->st_dma);
err_out_iounmap:
	iounmap(hw->regs);
err_out_free_hw:
	kfree(hw);
err_out_free_regions:
	pci_release_regions(pdev);
	pci_disable_device(pdev);
err_out:
	pci_set_drvdata(pdev, NULL);
	return err;
}

static void __devexit sky2_remove(struct pci_dev *pdev)
{
	struct sky2_hw *hw = pci_get_drvdata(pdev);
	struct net_device *dev0, *dev1;

	if (!hw)
		return;

	del_timer_sync(&hw->idle_timer);

	flush_scheduled_work();

	sky2_write32(hw, B0_IMSK, 0);
	synchronize_irq(hw->pdev->irq);

	dev0 = hw->dev[0];
	dev1 = hw->dev[1];
	if (dev1)
		unregister_netdev(dev1);
	unregister_netdev(dev0);

	sky2_power_aux(hw);

	sky2_write16(hw, B0_Y2LED, LED_STAT_OFF);
	sky2_write8(hw, B0_CTST, CS_RST_SET);
	sky2_read8(hw, B0_CTST);

	free_irq(pdev->irq, hw);
	if (hw->msi)
		pci_disable_msi(pdev);
	pci_free_consistent(pdev, STATUS_LE_BYTES, hw->st_le, hw->st_dma);
	pci_release_regions(pdev);
	pci_disable_device(pdev);

	if (dev1)
		free_netdev(dev1);
	free_netdev(dev0);
	iounmap(hw->regs);
	kfree(hw);

	pci_set_drvdata(pdev, NULL);
}

#ifdef CONFIG_PM
static int sky2_suspend(struct pci_dev *pdev, pm_message_t state)
{
	struct sky2_hw *hw = pci_get_drvdata(pdev);
	int i, wol = 0;

	if (!hw)
		return 0;

	del_timer_sync(&hw->idle_timer);
	netif_poll_disable(hw->dev[0]);

	for (i = 0; i < hw->ports; i++) {
		struct net_device *dev = hw->dev[i];
		struct sky2_port *sky2 = netdev_priv(dev);

		if (netif_running(dev))
			sky2_down(dev);

		if (sky2->wol)
			sky2_wol_init(sky2);

		wol |= sky2->wol;
	}

	sky2_write32(hw, B0_IMSK, 0);
	sky2_power_aux(hw);

	pci_save_state(pdev);
	pci_enable_wake(pdev, pci_choose_state(pdev, state), wol);
	pci_set_power_state(pdev, pci_choose_state(pdev, state));

	return 0;
}

static int sky2_resume(struct pci_dev *pdev)
{
	struct sky2_hw *hw = pci_get_drvdata(pdev);
	int i, err;

	if (!hw)
		return 0;

	err = pci_set_power_state(pdev, PCI_D0);
	if (err)
		goto out;

	err = pci_restore_state(pdev);
	if (err)
		goto out;

	pci_enable_wake(pdev, PCI_D0, 0);

	/* Re-enable all clocks */
	if (hw->chip_id == CHIP_ID_YUKON_EX || hw->chip_id == CHIP_ID_YUKON_EC_U)
		sky2_pci_write32(hw, PCI_DEV_REG3, 0);

	sky2_reset(hw);

	sky2_write32(hw, B0_IMSK, Y2_IS_BASE);

	for (i = 0; i < hw->ports; i++) {
		struct net_device *dev = hw->dev[i];
		if (netif_running(dev)) {
			err = sky2_up(dev);
			if (err) {
				printk(KERN_ERR PFX "%s: could not up: %d\n",
				       dev->name, err);
				dev_close(dev);
				goto out;
			}
		}
	}

	netif_poll_enable(hw->dev[0]);
	sky2_idle_start(hw);
	return 0;
out:
	dev_err(&pdev->dev, "resume failed (%d)\n", err);
	pci_disable_device(pdev);
	return err;
}
#endif

static void sky2_shutdown(struct pci_dev *pdev)
{
	struct sky2_hw *hw = pci_get_drvdata(pdev);
	int i, wol = 0;

	if (!hw)
		return;

	del_timer_sync(&hw->idle_timer);
	netif_poll_disable(hw->dev[0]);

	for (i = 0; i < hw->ports; i++) {
		struct net_device *dev = hw->dev[i];
		struct sky2_port *sky2 = netdev_priv(dev);

		if (sky2->wol) {
			wol = 1;
			sky2_wol_init(sky2);
		}
	}

	if (wol)
		sky2_power_aux(hw);

	pci_enable_wake(pdev, PCI_D3hot, wol);
	pci_enable_wake(pdev, PCI_D3cold, wol);

	pci_disable_device(pdev);
	pci_set_power_state(pdev, PCI_D3hot);

}

static struct pci_driver sky2_driver = {
	.name = DRV_NAME,
	.id_table = sky2_id_table,
	.probe = sky2_probe,
	.remove = __devexit_p(sky2_remove),
#ifdef CONFIG_PM
	.suspend = sky2_suspend,
	.resume = sky2_resume,
#endif
	.shutdown = sky2_shutdown,
};

static int __init sky2_init_module(void)
{
	return pci_register_driver(&sky2_driver);
}

static void __exit sky2_cleanup_module(void)
{
	pci_unregister_driver(&sky2_driver);
}

module_init(sky2_init_module);
module_exit(sky2_cleanup_module);

MODULE_DESCRIPTION("Marvell Yukon 2 Gigabit Ethernet driver");
MODULE_AUTHOR("Stephen Hemminger <shemminger@linux-foundation.org>");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
