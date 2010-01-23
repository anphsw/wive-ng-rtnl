/* vi: set sw=4 ts=4: */
#include <linux/config.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <asm/rt2880/rt_mmap.h>

#if defined (CONFIG_RALINK_RT3052) 
#define PHY_CONTROL_0		0xC0
#define PHY_CONTROL_1		0xC4
#define MDIO_PHY_CONTROL_0	le32_to_cpu(*(volatile u_long *)(RALINK_ETH_SW_BASE + PHY_CONTROL_0))
#define MDIO_PHY_CONTROL_1 	le32_to_cpu(*(volatile u_long *)(RALINK_ETH_SW_BASE + PHY_CONTROL_1))
#else 
#define PHY_CONTROL_0		0x00
#define PHY_CONTROL_1		0x04
#define MDIO_PHY_CONTROL_0	le32_to_cpu(*(volatile u_long *)(RALINK_FRAME_ENGINE_BASE + PHY_CONTROL_0))
#define MDIO_PHY_CONTROL_1	le32_to_cpu(*(volatile u_long *)(RALINK_FRAME_ENGINE_BASE + PHY_CONTROL_1))
#endif

u32 mii_mgr_read(u32 phy_addr, u32 phy_register, u32 *read_data)
{
	u32 volatile  status=0;
	u32 volatile  t_start=jiffies;
	u32 volatile  data;

	// make sure previous read operation is complete
	while(1)
	{
#if defined (CONFIG_RALINK_RT3052) 
		// rd_rdy: read operation is complete
		if(!(MDIO_PHY_CONTROL_1 & (0x1 << 1))) 
#else
		// 0 : Read/write operation complet
		if(!(MDIO_PHY_CONTROL_0 & (0x1 << 31))) 
#endif
		{
			break;
		}
		else if(time_after(jiffies, t_start + 5*HZ))
		{
			printk("\n MDIO Read operation is ongoing !!\n");
			return 0;
		}
	}
	
#if defined (CONFIG_RALINK_RT3052) 
	MDIO_PHY_CONTROL_0 = (1<<14) | (phy_register << 8) | (phy_addr); 
#else
	data  = (phy_addr << 24) | (phy_register << 16);
	MDIO_PHY_CONTROL_0 = data;
	data |= (1<<31);
	MDIO_PHY_CONTROL_0 =data;
#endif
	//printk("\n Set Command [0x%08X] to PHY !!\n",MDIO_PHY_CONTROL_0);

	// make sure read operation is complete
	t_start = jiffies;
	while(1)
	{
#if defined (CONFIG_RALINK_RT3052) 
		if(MDIO_PHY_CONTROL_1 & (0x1 << 1))
		{
			status = MDIO_PHY_CONTROL_1;
			*read_data = (u32)(status >>16);
			return 1;
		}
#else
		if(!(MDIO_PHY_CONTROL_0 & (0x1 << 31)))
		{
			status = MDIO_PHY_CONTROL_0;
			*read_data = (u32)(status & 0x0000FFFF);
			return 1;
		}
#endif
		else if(time_after(jiffies, t_start+5*HZ))
		{
			printk("\n MDIO Read operation is ongoing and Time Out!!\n");
			return 0;
		}
	}
}

u32 mii_mgr_write(u32 phy_addr, u32 phy_register, u32 write_data)
{
	u32 volatile  status;
	u32 volatile  t_start=jiffies;
	u32 volatile  data;

	// make sure previous write operation is complete
	while(1)
	{
#if defined (CONFIG_RALINK_RT3052) 
		if(!(MDIO_PHY_CONTROL_1 & (0x1 << 0)))
#else
		if (!(MDIO_PHY_CONTROL_0 & (0x1 << 31))) 
#endif
		{
			break;
		}
		else if(time_after(jiffies, t_start+5*HZ))
		{
			printk("\n MDIO Write operation is ongoing !!\n");
			return 0;
		}
	}

#if defined (CONFIG_RALINK_RT3052) 
	MDIO_PHY_CONTROL_0 = ((write_data & 0xFFFF)<<16);
  	MDIO_PHY_CONTROL_0 |= (phy_register << 8) | (phy_addr);
  	MDIO_PHY_CONTROL_0 |= (1<<13); // start write operation
#else
	data = (1<<30) | (phy_addr << 24) | (phy_register << 16) | write_data;
	MDIO_PHY_CONTROL_0 = data;
	data |= (1<<31);
	MDIO_PHY_CONTROL_0 = data; //start operation
#endif
	//printk("\n Set Command [0x%08X] to PHY !!\n",MDIO_PHY_CONTROL_0);

	t_start = jiffies;

	// make sure write operation is complete
	while(1)
	{
#if defined (CONFIG_RALINK_RT3052) 
		if(MDIO_PHY_CONTROL_1 & (0x1 << 0)) //wt_done ?= 1
#else
		if(!(MDIO_PHY_CONTROL_0 & (0x1 << 31))) // 0 : Read/write operation complete
#endif
		{
			return 1;
		}
		else if(time_after(jiffies, t_start+5*HZ))
		{
			printk("\n MDIO Write operation is ongoing and Time Out!!\n");
			return 0;
		}
	}
}

EXPORT_SYMBOL(mii_mgr_write);
EXPORT_SYMBOL(mii_mgr_read);
