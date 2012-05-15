/*
* Copyright c		  Realtek Semiconductor Corporation, 2006 
* All rights reserved.
*
* Program : Control smi connected RTL8367
* Abstract :
* Author : Yu-Mei Pan (ympan@realtek.com.cn)
*  $Id: smi.c,v 1.2 2008-04-10 03:04:19 shiehyy Exp $
*/

#include <smi.h>
#include <rtk_api.h>
#include <rtk_api_ext.h>
#include <rtl8370_asicdrv_port.h>

#include <rt_mmap.h>
#include <common.h>		/* for cpu_to_le32() and cpu_to_le32() */
#include <ralink_gpio.h>

#define DELAY_SMI			2 /* 2 microseconds */

int ralink_gpio_read_bit(int idx, int *value);
int ralink_gpio_write_bit(int idx, int value);
int ralink_gpio_direction(int idx, int dir);

const int smi_SCK = 2;	/* GPIO used for SMI Clock Generation */	/* GPIO2 */
const int smi_SDA = 1;	/* GPIO used for SMI Data signal */		/* GPIO1 */
const int smi_RST = 7;	/* GPIO used for reset swtich */		/* GPIO7, not used */

#define ack_timeout			5

#define	PHY_CONTROL_REG			0
#define	CONTROL_REG_PORT_POWER_BIT	0x800

void _smi_start(void)
{

    /* change GPIO pin to Output only */
    ralink_gpio_direction(smi_SDA, RALINK_GPIO_DIR_OUT);
    ralink_gpio_direction(smi_SCK, RALINK_GPIO_DIR_OUT);

    /* Initial state: SCK: 0, SDA: 1 */
    ralink_gpio_write_bit(smi_SCK, 0);
    ralink_gpio_write_bit(smi_SDA, 1);
    udelay(DELAY_SMI);

    /* CLK 1: 0 -> 1, 1 -> 0 */
    ralink_gpio_write_bit(smi_SCK, 1);
    udelay(DELAY_SMI);
    ralink_gpio_write_bit(smi_SCK, 0);
    udelay(DELAY_SMI);

    /* CLK 2: */
    ralink_gpio_write_bit(smi_SCK, 1);
    udelay(DELAY_SMI);
    ralink_gpio_write_bit(smi_SDA, 0);
    udelay(DELAY_SMI);
    ralink_gpio_write_bit(smi_SCK, 0);
    udelay(DELAY_SMI);
    ralink_gpio_write_bit(smi_SDA, 1);
}

void _smi_writeBit(uint16 signal, uint32 bitLen)
{
    for( ; bitLen > 0; bitLen--)
    {
	udelay(DELAY_SMI);

	/* prepare data */
	if ( signal & (1<<(bitLen-1)) ) 
	    ralink_gpio_write_bit(smi_SDA, 1);
	else
	    ralink_gpio_write_bit(smi_SDA, 0);
	udelay(DELAY_SMI);

	/* clocking */
	ralink_gpio_write_bit(smi_SCK, 1);
	udelay(DELAY_SMI);
	ralink_gpio_write_bit(smi_SCK, 0);
    }
}

void _smi_readBit(uint32 bitLen, uint32 *rData) 
{
    uint32 u;

    /* change GPIO pin to Input only */
    ralink_gpio_direction(smi_SDA, RALINK_GPIO_DIR_IN);

    for (*rData = 0; bitLen > 0; bitLen--)
    {
	udelay(DELAY_SMI);

	/* clocking */
	ralink_gpio_write_bit(smi_SCK, 1);
	udelay(DELAY_SMI);
	ralink_gpio_read_bit(smi_SDA, &u);
	ralink_gpio_write_bit(smi_SCK, 0);

	*rData |= (u << (bitLen - 1));
    }

    /* change GPIO pin to Output only */
    ralink_gpio_direction(smi_SDA, RALINK_GPIO_DIR_OUT);
}

void _smi_stop(void)
{
    udelay(DELAY_SMI);
    ralink_gpio_write_bit(smi_SDA, 0);
    ralink_gpio_write_bit(smi_SCK, 1);
    udelay(DELAY_SMI);
    ralink_gpio_write_bit(smi_SDA, 1);
    udelay(DELAY_SMI);
    ralink_gpio_write_bit(smi_SCK, 1);
    udelay(DELAY_SMI);
    ralink_gpio_write_bit(smi_SCK, 0);
    udelay(DELAY_SMI);
    ralink_gpio_write_bit(smi_SCK, 1);

    /* add a click */
    udelay(DELAY_SMI);
    ralink_gpio_write_bit(smi_SCK, 0);
    udelay(DELAY_SMI);
    ralink_gpio_write_bit(smi_SCK, 1);

    /* change GPIO pin to Output only */
    ralink_gpio_direction(smi_SDA, RALINK_GPIO_DIR_IN);
    ralink_gpio_direction(smi_SCK, RALINK_GPIO_DIR_IN);
}



int32 smi_read(uint32 mAddrs, uint32 *rData)
{
    uint32 rawData=0, ACK;
    uint8  con;
    uint32 ret = RT_ERR_OK;

    /*Disable CPU interrupt to ensure that the SMI operation is atomic. 
      The API is based on RTL865X, rewrite the API if porting to other platform.*/

    _smi_start();				/* Start SMI */

    _smi_writeBit(0x0b, 4);		     /* CTRL code: 4'b1011 for RTL8370 */

    _smi_writeBit(0x4, 3);			/* CTRL code: 3'b100 */

    _smi_writeBit(0x1, 1);			/* 1: issue READ command */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK for issuing READ command*/
    } while ((ACK != 0) && (con < ack_timeout));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);	     /* Set reg_addr[7:0] */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK for setting reg_addr[7:0] */    
    } while ((ACK != 0) && (con < ack_timeout));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);		 /* Set reg_addr[15:8] */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK by RTL8369 */
    } while ((ACK != 0) && (con < ack_timeout));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_readBit(8, &rawData);		    /* Read DATA [7:0] */
    *rData = rawData&0xff; 

    _smi_writeBit(0x00, 1);			/* ACK by CPU */

    _smi_readBit(8, &rawData);		    /* Read DATA [15: 8] */

    _smi_writeBit(0x01, 1);			/* ACK by CPU */
    *rData |= (rawData<<8);

    _smi_stop();
    return ret;
}

int32 smi_write(uint32 mAddrs, uint32 rData)
{
    int8 con;
    uint32 ACK;
    uint32 ret = RT_ERR_OK;

    /*Disable CPU interrupt to ensure that the SMI operation is atomic.
      The API is based on RTL865X, rewrite the API if porting to other platform.*/
    _smi_start();				/* Start SMI */

    _smi_writeBit(0x0b, 4);		     /* CTRL code: 4'b1011 for RTL8370*/

    _smi_writeBit(0x4, 3);			/* CTRL code: 3'b100 */

    _smi_writeBit(0x0, 1);			/* 0: issue WRITE command */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK for issuing WRITE command*/
    } while ((ACK != 0) && (con < ack_timeout));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);	     /* Set reg_addr[7:0] */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK for setting reg_addr[7:0] */
    } while ((ACK != 0) && (con < ack_timeout));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);		 /* Set reg_addr[15:8] */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK for setting reg_addr[15:8] */
    } while ((ACK != 0) && (con < ack_timeout));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData&0xff, 8);		/* Write Data [7:0] out */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK for writting data [7:0] */
    } while ((ACK != 0) && (con < ack_timeout));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData>>8, 8);		    /* Write Data [15:8] out */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);			/* ACK for writting data [15:8] */
    } while ((ACK != 0) && (con < ack_timeout));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_stop();
    return ret;
}

int ralink_gpio_ioctl(unsigned int req, int idx, unsigned long arg)
{
	unsigned long tmp;

	req &= RALINK_GPIO_DATA_MASK;

	switch(req) {
	case RALINK_GPIO_READ_BIT:
		if (idx <= 23)
			tmp = le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIODATA));
		else if (idx <= 39)
			tmp = le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIO3924DATA));
		else
			tmp = le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIO5140DATA));

		if (idx <= 23)
			tmp = (tmp >> idx) & 1L;
		else if (idx <= 39)
			tmp = (tmp >> (idx-24)) & 1L;
		else
			tmp = (tmp >> (idx-40)) & 1L;
		return tmp;

		break;
	case RALINK_GPIO_WRITE_BIT:
		if (idx <= 23) {
			tmp =le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIODATA));
			if (arg & 1L)
				tmp |= (1L << idx);
			else
				tmp &= ~(1L << idx);
			*(volatile u32 *)(RALINK_REG_PIODATA)= cpu_to_le32(tmp);
		}
		else if (idx <= 39) {
			tmp =le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIO3924DATA));
			if (arg & 1L)
				tmp |= (1L << (idx-24));
			else
				tmp &= ~(1L << (idx-24));
			*(volatile u32 *)(RALINK_REG_PIO3924DATA)= cpu_to_le32(tmp);
		}
		else {
			tmp =le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIO5140DATA));
			if (arg & 1L)
				tmp |= (1L << (idx-40));
			else
				tmp &= ~(1L << (idx-40));
			*(volatile u32 *)(RALINK_REG_PIO5140DATA)= cpu_to_le32(tmp);
		}

		break;
	default:
		return -1;
	}
	return 0;
}

int ralink_gpio_write_bit(int idx, int value)
{
	value &= 1;

	if (idx < 0 || idx >= RALINK_GPIO_NUMBER)
		return -1;

	return ralink_gpio_ioctl(RALINK_GPIO_WRITE_BIT, idx, value);
}

int ralink_gpio_read_bit(int idx, int *value)
{
	*value = 0;

	if (idx < 0 || idx >= RALINK_GPIO_NUMBER)
		return -1;

	if ((*value = ralink_gpio_ioctl(RALINK_GPIO_READ_BIT, idx, 0)) < 0)
		return -1;

	return *value;
}

int ralink_gpio_direction(int idx, int dir)
{
	unsigned long tmp;

	if (idx < 0 || idx >= RALINK_GPIO_NUMBER)
		return -1;

	if (dir == RALINK_GPIO_DIR_OUT)
	{
		if (idx <= 23) {
			tmp = le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIODIR));
			tmp |= (1L << idx);
		}
		else if (idx <= 39) {
			tmp = le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIO3924DIR));
			tmp |= (1L << (idx-24));
		}
		else {
			tmp = le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIO5140DIR));
			tmp |= (1L << (idx-40));
		}
	}
	else if (dir == RALINK_GPIO_DIR_IN)
	{
		if (idx <= 23) {
			tmp = le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIODIR));
			tmp &= ~(1L << idx);
		}
		else if (idx <= 39) {
			tmp = le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIO3924DIR));
			tmp &= ~(1L << (idx-24));
		}
		else {
			tmp = le32_to_cpu(*(volatile u32 *)(RALINK_REG_PIO5140DIR));
			tmp &= ~(1L << (idx-40));
		}
	}
	else
		return -1;

	if (idx <= 23) {
		*(volatile u32 *)(RALINK_REG_PIODIR) = cpu_to_le32(tmp);
	}
	else if (idx <= 39) {
		*(volatile u32 *)(RALINK_REG_PIO3924DIR) = cpu_to_le32(tmp);
	}
	else {
		*(volatile u32 *)(RALINK_REG_PIO5140DIR) = cpu_to_le32(tmp);
	}

	return 0;
}

void partition_bridge_default(void)
{
	rtk_portmask_t fwd_mask;

	/* LAN */
	fwd_mask.bits[0] = 0x10F;
	rtk_port_isolation_set(0, fwd_mask);
	rtk_port_isolation_set(1, fwd_mask);
	rtk_port_isolation_set(2, fwd_mask);
	rtk_port_isolation_set(3, fwd_mask);
	rtk_port_isolation_set(8, fwd_mask); // CPU LAN

	/* WAN */
	fwd_mask.bits[0] = 0x210;
	rtk_port_isolation_set(4, fwd_mask);
	rtk_port_isolation_set(9, fwd_mask); // CPU WAN

	/* EFID setting LAN */
	rtk_port_efid_set(0, 0);
	rtk_port_efid_set(1, 0);
	rtk_port_efid_set(2, 0);
	rtk_port_efid_set(3, 0);
	rtk_port_efid_set(8, 0);

	/* EFID setting WAN */
	rtk_port_efid_set(4, 1);
	rtk_port_efid_set(9, 1);
}

int rtl8367m_switch_init_pre(void)
{
	printf("Init RTL8367M external switch...\n");
	
	rtk_api_ret_t retVal;
	rtk_port_t port;
	rtk_portmask_t portmask;
	rtk_port_mac_ability_t mac_cfg;
	rtk_port_phy_data_t pData = 0;
	rtk_data_t txDelay, rxDelay;
	unsigned long gpiomode;

	/* set RT3883 GPIO mode: disable I2C and JTAG (JTAG for LED LAN control) */
	gpiomode = le32_to_cpu(*(volatile u32 *)(RALINK_REG_GPIOMODE));
	gpiomode |= RALINK_GPIOMODE_I2C;
	gpiomode |= RALINK_GPIOMODE_JTAG;
	*(volatile u32 *)(RALINK_REG_GPIOMODE) = cpu_to_le32(gpiomode);

	/* delay 125ms after power-on-reset */
	udelay(125000);

	/* main switch init */
	retVal = rtk_switch_init();
	if (retVal != RT_ERR_OK) {
		printf("Init RTL8367M: FAILED! (%d)\n", retVal);
		return retVal;
	}

	/* power down LAN4, LAN3, LAN2, LAN1, WAN ports */
	for (port = 0; port <= 4; port++) {
		retVal = rtk_port_phyReg_get(port, PHY_CONTROL_REG, &pData);
		if (retVal == RT_ERR_OK) {
			pData |= CONTROL_REG_PORT_POWER_BIT;
			rtk_port_phyReg_set(port, PHY_CONTROL_REG, pData);
		}
	}

	/* configure both cpu RGMII to fixed gigabit mode w/o autoneg */
	mac_cfg.forcemode	= MAC_FORCE;
	mac_cfg.speed		= SPD_1000M;
	mac_cfg.duplex		= FULL_DUPLEX;
	mac_cfg.link		= 1;
	mac_cfg.nway		= 0;
	mac_cfg.rxpause		= 1;
	mac_cfg.txpause		= 1;
	rtk_port_macForceLinkExt0_set(MODE_EXT_RGMII, &mac_cfg); // Ext0 -> RT3883 GMAC WAN
	rtk_port_macForceLinkExt1_set(MODE_EXT_RGMII, &mac_cfg); // Ext1 -> RT3883 GMAC LAN

	/* configure both cpu RGMII delay for RT3883 */
	txDelay = 1; // 0..1
	rxDelay = 0; // 0..7
	rtk_port_rgmiiDelayExt0_set(txDelay, rxDelay); // Ext0 -> RT3883 GMAC WAN
	rtk_port_rgmiiDelayExt1_set(txDelay, rxDelay); // Ext1 -> RT3883 GMAC LAN

	/* configure bridge isolation [LAN4 LAN3 LAN2 LAN1] [WAN] */
	partition_bridge_default();

	/* configure PHY leds */
	portmask.bits[0] = 0x1F;	// LAN4, LAN3, LAN2, LAN1, WAN
	rtk_led_enable_set(LED_GROUP_0, portmask);
	rtk_led_enable_set(LED_GROUP_1, portmask);
	rtk_led_operation_set(LED_OP_PARALLEL);
	rtk_led_groupConfig_set(LED_GROUP_0, LED_CONFIG_SPD10010ACT);	// group 0 - RJ45 green LED
	rtk_led_groupConfig_set(LED_GROUP_1, LED_CONFIG_SPD1000ACT);	// group 1 - RJ45 yellow LED

	printf("Init RTL8367M: SUCCESS!\n");

	return RT_ERR_OK;
}

int rtl8367m_switch_init_post(void)
{
	rtk_api_ret_t retVal;
	rtk_port_t port;
	rtk_port_phy_data_t pData = 0;

	/* power up LAN4, LAN3, LAN2, LAN1 ports */
	for (port = 0; port <= 3; port++) {
		retVal = rtk_port_phyReg_get(port, PHY_CONTROL_REG, &pData);
		if (retVal == RT_ERR_OK) {
			pData &= ~CONTROL_REG_PORT_POWER_BIT;
			rtk_port_phyReg_set(port, PHY_CONTROL_REG, pData);
		}
	}

	return RT_ERR_OK;
}
