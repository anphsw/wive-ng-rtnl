/*
* Copyright c		  Realtek Semiconductor Corporation, 2006 
* All rights reserved.
*
* Program : Control  smi connected RTL8366
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

//#define DELAY			10000
#define DELAY			166 * 2
#define CLK_DURATION(clk)	{ int i; for(i=0; i<clk; i++); }
#define _SMI_ACK_RESPONSE(ok)	{ /*if (!(ok)) return RT_ERR_FAILED; */}

typedef int gpioID;

int ralink_gpio_write_bit(int idx, int value);
int ralink_gpio_read_bit(int idx, int *value);
int ralink_initGpioPin(unsigned int idx, int dir);

extern int input_txDelay, input_rxDelay;
extern int rtl8367m_switch_inited;

gpioID smi_SCK = 2;	/* GPIO used for SMI Clock Generation */	/* GPIO2 */
gpioID smi_SDA = 1;	/* GPIO used for SMI Data signal */		/* GPIO1 */
gpioID smi_RST = 7;	/* GPIO used for reset swtich */		/* GPIO7, not used */

#define ack_timer	5
#define max_register	0x018A 

#define	PHY_CONTROL_REG			0
#define	CONTROL_REG_PORT_POWER_BIT	0x800

void _smi_start(void)
{

    /* change GPIO pin to Output only */
    ralink_initGpioPin(smi_SDA, RALINK_GPIO_DIR_OUT);
    ralink_initGpioPin(smi_SCK, RALINK_GPIO_DIR_OUT);

    /* Initial state: SCK: 0, SDA: 1 */
    ralink_gpio_write_bit(smi_SCK, 0);
    ralink_gpio_write_bit(smi_SDA, 1);
    CLK_DURATION(DELAY);

    /* CLK 1: 0 -> 1, 1 -> 0 */
    ralink_gpio_write_bit(smi_SCK, 1);
    CLK_DURATION(DELAY);
    ralink_gpio_write_bit(smi_SCK, 0);
    CLK_DURATION(DELAY);

    /* CLK 2: */
    ralink_gpio_write_bit(smi_SCK, 1);
    CLK_DURATION(DELAY);
    ralink_gpio_write_bit(smi_SDA, 0);
    CLK_DURATION(DELAY);
    ralink_gpio_write_bit(smi_SCK, 0);
    CLK_DURATION(DELAY);
    ralink_gpio_write_bit(smi_SDA, 1);

}



void _smi_writeBit(uint16 signal, uint32 bitLen)
{
    for( ; bitLen > 0; bitLen--)
    {
	CLK_DURATION(DELAY);

	/* prepare data */
	if ( signal & (1<<(bitLen-1)) ) 
	    ralink_gpio_write_bit(smi_SDA, 1);
	else
	    ralink_gpio_write_bit(smi_SDA, 0);
	CLK_DURATION(DELAY);

	/* clocking */
	ralink_gpio_write_bit(smi_SCK, 1);
	CLK_DURATION(DELAY);
	ralink_gpio_write_bit(smi_SCK, 0);
    }
}



void _smi_readBit(uint32 bitLen, uint32 *rData) 
{
    uint32 u;

    /* change GPIO pin to Input only */
    ralink_initGpioPin(smi_SDA, RALINK_GPIO_DIR_IN);

    for (*rData = 0; bitLen > 0; bitLen--)
    {
	CLK_DURATION(DELAY);

	/* clocking */
	ralink_gpio_write_bit(smi_SCK, 1);
	CLK_DURATION(DELAY);
	ralink_gpio_read_bit(smi_SDA, &u);
	ralink_gpio_write_bit(smi_SCK, 0);

	*rData |= (u << (bitLen - 1));
    }

    /* change GPIO pin to Output only */
    ralink_initGpioPin(smi_SDA, RALINK_GPIO_DIR_OUT);
}



void _smi_stop(void)
{

    CLK_DURATION(DELAY);
    ralink_gpio_write_bit(smi_SDA, 0);
    ralink_gpio_write_bit(smi_SCK, 1);
    CLK_DURATION(DELAY);
    ralink_gpio_write_bit(smi_SDA, 1);
    CLK_DURATION(DELAY);
    ralink_gpio_write_bit(smi_SCK, 1);
    CLK_DURATION(DELAY);
    ralink_gpio_write_bit(smi_SCK, 0);
    CLK_DURATION(DELAY);
    ralink_gpio_write_bit(smi_SCK, 1);

    /* add a click */
    CLK_DURATION(DELAY);
    ralink_gpio_write_bit(smi_SCK, 0);
    CLK_DURATION(DELAY);
    ralink_gpio_write_bit(smi_SCK, 1);


    /* change GPIO pin to Output only */
    ralink_initGpioPin(smi_SDA, RALINK_GPIO_DIR_IN);
    ralink_initGpioPin(smi_SCK, RALINK_GPIO_DIR_IN);


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
    } while ((ACK != 0) && (con < ack_timer));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);	     /* Set reg_addr[7:0] */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK for setting reg_addr[7:0] */    
    } while ((ACK != 0) && (con < ack_timer));

    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);		 /* Set reg_addr[15:8] */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK by RTL8369 */
    } while ((ACK != 0) && (con < ack_timer));
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
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs&0xff), 8);	     /* Set reg_addr[7:0] */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK for setting reg_addr[7:0] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit((mAddrs>>8), 8);		 /* Set reg_addr[15:8] */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK for setting reg_addr[15:8] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData&0xff, 8);		/* Write Data [7:0] out */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);		    /* ACK for writting data [7:0] */
    } while ((ACK != 0) && (con < ack_timer));
    if (ACK != 0) ret = RT_ERR_FAILED;

    _smi_writeBit(rData>>8, 8);		    /* Write Data [15:8] out */

    con = 0;
    do {
	con++;
	_smi_readBit(1, &ACK);			/* ACK for writting data [15:8] */
    } while ((ACK != 0) && (con < ack_timer));
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
	unsigned int req;
	value &= 1;

	if (0L <= idx && idx < RALINK_GPIO_NUMBER)
		req = RALINK_GPIO_WRITE_BIT;
	else
		return -1;

	return ralink_gpio_ioctl(req, idx, value);
}

int ralink_gpio_read_bit(int idx, int *value)
{
	unsigned int req;
	*value = 0;

	if (0L <= idx && idx < RALINK_GPIO_NUMBER)
		req = RALINK_GPIO_READ_BIT;
	else
		return -1;

	if ((*value = ralink_gpio_ioctl(req, idx, value)) < 0)
		return -1;

	return *value;
}

int ralink_initGpioPin(unsigned int idx, int dir)
{
	unsigned long tmp;

	if (idx < 0 || RALINK_GPIO_NUMBER <= idx)
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

void LANWANPartition_8367m()
{
	rtk_portmask_t fwd_mask;

	/* LAN */
	fwd_mask.bits[0] = 0x10F;
	rtk_port_isolation_set(0, fwd_mask);
	rtk_port_isolation_set(1, fwd_mask);
	rtk_port_isolation_set(2, fwd_mask);
	rtk_port_isolation_set(3, fwd_mask);
	rtk_port_isolation_set(8, fwd_mask);

	/* WAN */
	fwd_mask.bits[0] = 0x210;
	rtk_port_isolation_set(4, fwd_mask);
	rtk_port_isolation_set(9, fwd_mask);

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

void asus_led_init(void)
{
	printf("PWR LED on\n");
	ralink_initGpioPin(0, RALINK_GPIO_DIR_OUT);
	ralink_gpio_write_bit(0, 1);	// PWR LED
	ralink_gpio_write_bit(0, 0);	// PWR LED
/*
	printf("USB LED on\n");
	ralink_initGpioPin(24, RALINK_GPIO_DIR_OUT);
	ralink_gpio_write_bit(24, 1);	// USB LED
	ralink_gpio_write_bit(24, 0);	// USB LED

	printf("WAN LED on\n");
	ralink_initGpioPin(27, RALINK_GPIO_DIR_OUT);
	ralink_gpio_write_bit(27, 1);	// WAN LED
	ralink_gpio_write_bit(27, 0);	// WAN LED

	printf("LAN LED on\n");
	ralink_initGpioPin(31, RALINK_GPIO_DIR_OUT);
	ralink_gpio_write_bit(31, 1);	// LAN LED
	ralink_gpio_write_bit(31, 0);	// LAN LED
*/
}

unsigned long gpiomode_backup;

int rtl8367m_switch_init_pre()
{
	rtk_api_ret_t retVal;
	unsigned long gpiomode;

	gpiomode_backup = le32_to_cpu(*(volatile u32 *)(RALINK_REG_GPIOMODE));
	printf("GPIOMODE current: %x\n",  gpiomode_backup);
	gpiomode = gpiomode_backup;
	gpiomode &= ~RALINK_GPIOMODE_I2C;
	gpiomode |= RALINK_GPIOMODE_I2C;
	printf("GPIOMODE writing: %x\n", gpiomode);
	*(volatile u32 *)(RALINK_REG_GPIOMODE) = cpu_to_le32(gpiomode);
	gpiomode = le32_to_cpu(*(volatile u32 *)(RALINK_REG_GPIOMODE));
	printf("GPIOMODE current: %x\n",  gpiomode);

	retVal = rtk_switch_init();
	printf("rtk_switch_init(): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;

	rtk_port_mac_ability_t mac_cfg;
	mac_cfg.forcemode	= MAC_FORCE;
	mac_cfg.speed		= SPD_1000M;
	mac_cfg.duplex		= FULL_DUPLEX;
	mac_cfg.link		= 1;
	mac_cfg.nway		= 0;
	mac_cfg.rxpause		= 1;
	mac_cfg.txpause		= 1;

	retVal = rtk_port_macForceLinkExt1_set(MODE_EXT_RGMII, &mac_cfg);
	printf("rtk_port_macForceLinkExt1_set(): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;

	/* power down all ports */
	rtk_port_phy_data_t pData;
	rtk_port_phyReg_get(0, PHY_CONTROL_REG, &pData);
	pData |= CONTROL_REG_PORT_POWER_BIT;
	rtk_port_phyReg_set(0, PHY_CONTROL_REG, pData);

	rtk_port_phyReg_get(1, PHY_CONTROL_REG, &pData);
	pData |= CONTROL_REG_PORT_POWER_BIT;
	rtk_port_phyReg_set(1, PHY_CONTROL_REG, pData);

	rtk_port_phyReg_get(2, PHY_CONTROL_REG, &pData);
	pData |= CONTROL_REG_PORT_POWER_BIT;
	rtk_port_phyReg_set(2, PHY_CONTROL_REG, pData);

	rtk_port_phyReg_get(3, PHY_CONTROL_REG, &pData);
	pData |= CONTROL_REG_PORT_POWER_BIT;
	rtk_port_phyReg_set(3, PHY_CONTROL_REG, pData);

	rtk_port_phyReg_get(4, PHY_CONTROL_REG, &pData);
	pData |= CONTROL_REG_PORT_POWER_BIT;
	rtk_port_phyReg_set(4, PHY_CONTROL_REG, pData);

	rtk_data_t txDelay_ro, rxDelay_ro;
	rtk_port_rgmiiDelayExt1_get(&txDelay_ro, &rxDelay_ro);
	printf("org Ext1 txDelay: %d, rxDelay: %d\n", txDelay_ro, rxDelay_ro);

	rtk_data_t txDelay = 0;
	rtk_data_t rxDelay = 7;
	printf("new Ext txDelay: %d, rxDelay: %d\n", txDelay, rxDelay);
	retVal = rtk_port_rgmiiDelayExt1_set(txDelay, rxDelay);
	printf("rtk_port_rgmiiDelayExt1_set(): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;

	LANWANPartition_8367m();

	rtk_portmask_t portmask;
	portmask.bits[0] = 0x1F;

	retVal = rtk_led_enable_set(LED_GROUP_0, portmask);
	printf("rtk_led_enable_set(LED_GROUP_0...): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;
	retVal = rtk_led_enable_set(LED_GROUP_1, portmask);
	printf("rtk_led_enable_set(LED_GROUP_1...): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;

	retVal = rtk_led_operation_set(LED_OP_PARALLEL);
	printf("rtk_led_operation_set(): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;
	retVal = rtk_led_groupConfig_set(LED_GROUP_0, LED_CONFIG_SPD10010ACT);
	printf("rtk_led_groupConfig_set(LED_GROUP_0...): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;
	retVal = rtk_led_groupConfig_set(LED_GROUP_1, LED_CONFIG_SPD1000ACT);
	printf("rtk_led_groupConfig_set(LED_GROUP_1...): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;

	rtl8367m_switch_inited = 1;
	return RT_ERR_OK;
}

int
rtl8367m_switch_init()
{
	rtk_api_ret_t retVal;

	printf("software reset RTL8367M...\n");
	rtl8370_setAsicReg(0x1322, 1);	// software reset
	udelay(1000);

	retVal = rtk_switch_init();
	printf("rtk_switch_init(): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;

	rtk_port_mac_ability_t mac_cfg;
	mac_cfg.forcemode	= MAC_FORCE;
	mac_cfg.speed		= SPD_1000M;
	mac_cfg.duplex		= FULL_DUPLEX;
	mac_cfg.link		= 1;
	mac_cfg.nway		= 0;
	mac_cfg.rxpause		= 1;
	mac_cfg.txpause		= 1;

	retVal = rtk_port_macForceLinkExt1_set(MODE_EXT_RGMII, &mac_cfg);
	printf("rtk_port_macForceLinkExt1_set(): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;

	/* power down default wan port */
	rtk_port_phy_data_t pData;
	rtk_port_phyReg_get(4, PHY_CONTROL_REG, &pData);
	pData |= CONTROL_REG_PORT_POWER_BIT;
	rtk_port_phyReg_set(4, PHY_CONTROL_REG, pData);

	rtk_data_t txDelay_ro, rxDelay_ro;
	rtk_port_rgmiiDelayExt1_get(&txDelay_ro, &rxDelay_ro);
	printf("org Ext1 txDelay: %d, rxDelay: %d\n", txDelay_ro, rxDelay_ro);

	rtk_data_t txDelay = 1;
	rtk_data_t rxDelay = 0;
	printf("new Ext txDelay: %d, rxDelay: %d\n", txDelay, rxDelay);
	retVal = rtk_port_rgmiiDelayExt1_set(txDelay, rxDelay);
	printf("rtk_port_rgmiiDelayExt1_set(): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;

	LANWANPartition_8367m();

	rtk_portmask_t portmask;
	portmask.bits[0] = 0x1F;

	retVal = rtk_led_enable_set(LED_GROUP_0, portmask);
	printf("rtk_led_enable_set(LED_GROUP_0...): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;
	retVal = rtk_led_enable_set(LED_GROUP_1, portmask);
	printf("rtk_led_enable_set(LED_GROUP_1...): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;
	retVal = rtk_led_operation_set(LED_OP_PARALLEL);
	printf("rtk_led_operation_set(): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;
	retVal = rtk_led_groupConfig_set(LED_GROUP_0, LED_CONFIG_SPD10010ACT);
	printf("rtk_led_groupConfig_set(LED_GROUP_0...): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;
	retVal = rtk_led_groupConfig_set(LED_GROUP_1, LED_CONFIG_SPD1000ACT);
	printf("rtk_led_groupConfig_set(LED_GROUP_1...): return %d\n", retVal);
	if (retVal !=RT_ERR_OK) return retVal;

	rtl8367m_switch_inited = 1;
	return RT_ERR_OK;
}
