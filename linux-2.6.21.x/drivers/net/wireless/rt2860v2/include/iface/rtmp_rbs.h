/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
	rtmp_rbs.h
 
    Abstract:
 	Ralink SoC Internal Bus related definitions and data dtructures
 	
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */
 
#ifndef __RTMP_RBUS_H__
#define __RTMP_RBUS_H__

#ifdef LINUX
#include <linux/skbuff.h>
#endif // LINUX //


/*************************************************************************
  *
  *	Device hardware/ Interface related definitions.
  *
  ************************************************************************/  

#define RTMP_MAC_IRQ_NUM		4


/*************************************************************************
  *
  *	EEPROM Related definitions
  *
  ************************************************************************/
#if defined(CONFIG_RALINK_RT3050_1T1R)
#define EEPROM_DEFAULT_FILE_PATH                     "/etc_ro/wlan/RT3050_AP_1T1R_V1_0.bin"
#elif defined(CONFIG_RALINK_RT3051_1T2R)
#define EEPROM_DEFAULT_FILE_PATH                     "/etc_ro/wlan/RT3051_AP_1T2R_V1_0.bin"
#elif defined(CONFIG_RALINK_RT3052_2T2R)
#define EEPROM_DEFAULT_FILE_PATH                     "/etc_ro/wlan/RT3052_AP_2T2R_V1_1.bin"
#elif defined(CONFIG_RT2860V2_2850)
#define EEPROM_DEFAULT_FILE_PATH                     "/etc_ro/wlan/RT2880_RT2850_AP_2T3R_V1_6.bin"
#else // RFIC 2820
#define EEPROM_DEFAULT_FILE_PATH                     "/etc_ro/wlan/RT2880_RT2820_AP_2T3R_V1_6.bin"
#endif

#if defined (CONFIG_RT2880_FLASH_32M) && defined (CONFIG_RALINK_RT3052_MP2)
#define MTD_NUM_FACTORY 5
#else
#define MTD_NUM_FACTORY 2
#endif


#define EEPROM_SIZE					0x200
#define NVRAM_OFFSET					0x30000
#define RF_OFFSET					0x40000



#ifdef LINUX
/*************************************************************************
  *
  *	Device Tx/Rx related definitions.
  *
  ************************************************************************/
#if !defined(CONFIG_RA_NAT_NONE)
/* bruce+ */
extern int (*ra_sw_nat_hook_tx)(struct sk_buff *skb);
extern int (*ra_sw_nat_hook_rx)(struct sk_buff *skb);
#endif

#ifdef DFS_SUPPORT
// TODO: Check these functions.
extern void unregister_tmr_service(void);
extern void request_tmr_service(int, void *, void *);

#endif // DFS_SUPPORT //


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#define SA_SHIRQ IRQF_SHARED
#endif

#ifdef PCI_MSI_SUPPORT
#define RTMP_MSI_ENABLE(_pAd) \
{ 	POS_COOKIE _pObj = (POS_COOKIE)(_pAd->OS_Cookie); \
	(_pAd)->HaveMsi =	pci_enable_msi(_pObj->pci_dev) == 0 ? TRUE : FALSE; }

#define RTMP_MSI_DISABLE(_pAd) \
{ 	POS_COOKIE _pObj = (POS_COOKIE)(_pAd->OS_Cookie); \
	if (_pAd->HaveMsi == TRUE) \
		pci_disable_msi(_pObj->pci_dev); \
	_pAd->HaveMsi = FALSE;	}
#else
#define RTMP_MSI_ENABLE(_pAd)
#define RTMP_MSI_DISABLE(_pAd)
#endif // PCI_MSI_SUPPORT //


#define RTMP_IRQ_REQUEST(net_dev)							\
{	PRTMP_ADAPTER _pAd = (PRTMP_ADAPTER)((net_dev)->priv);	\
	POS_COOKIE _pObj = (POS_COOKIE)(_pAd->OS_Cookie);		\
	RTMP_MSI_ENABLE(_pAd);									\
	if ((retval = request_irq(net_dev->irq, 		\
							rt2860_interrupt, SA_INTERRUPT,		\
							(net_dev)->name, (net_dev)))) {	\
		DBGPRINT(RT_DEBUG_ERROR, ("request_irq  error(%d)\n", retval));	\
	return retval; } }


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
#define RTMP_IRQ_RELEASE(net_dev)								\
{	PRTMP_ADAPTER _pAd = (PRTMP_ADAPTER)((net_dev)->priv);		\
	synchronize_irq(net_dev->irq);						\
	free_irq(net_dev->irq, (net_dev));					\
	RTMP_MSI_DISABLE(_pAd); }
#else
#define RTMP_IRQ_RELEASE(net_dev)								\
{	PRTMP_ADAPTER _pAd = (PRTMP_ADAPTER)((net_dev)->priv);		\
	free_irq(net_dev->irq, (net_dev));					\
	RTMP_MSI_DISABLE(_pAd); }
#endif

#endif // LINUX //


#endif // __RTMP_RBUS_H__ //

