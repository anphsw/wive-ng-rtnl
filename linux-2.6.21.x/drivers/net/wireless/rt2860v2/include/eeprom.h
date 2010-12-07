/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	eeprom.h

	Abstract:
	Miniport header file for eeprom related information

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/
#ifndef __EEPROM_H__
#define __EEPROM_H__

#ifdef CONFIG_RALINK_RT3883
#define EEPROM_SIZE			0x400
#else                                           
#define EEPROM_SIZE			0x200
#endif

#if defined(CONFIG_RALINK_RT3052)
#if defined(CONFIG_RALINK_RT3050_1T1R)
#define EEPROM_DEFAULT_PATH			"/etc/Wireless/RT3050_AP_1T1R_V1_0.bin"
#elif defined(CONFIG_RALINK_RT3051_1T2R)
#define EEPROM_DEFAULT_PATH			"/etc/Wireless/RT3051_AP_1T2R_V1_0.bin"
#elif defined(CONFIG_RALINK_RT3052_2T2R)
#define EEPROM_DEFAULT_PATH			"/etc/Wireless/RT3052_AP_2T2R_V1_1.bin"                                                      
#elif defined(CONFIG_RT2860V2_2850)
#define EEPROM_DEFAULT_PATH			"/etc/Wireless/RT2880_RT2850_AP_2T3R_V1_6.bin"
#else // RFIC 2820
#define EEPROM_DEFAULT_PATH			"/etc/Wireless/RT2880_RT2820_AP_2T3R_V1_6.bin"
#endif
#else
#if defined(CONFIG_RT2860V2_AP_2850) || defined(CONFIG_RT2860V2_STA_2850)
#define EEPROM_DEFAULT_PATH			"/etc/Wireless/RT2880_RT2850_AP_2T3R_V1_5.bin"
#else // RFIC 2820
#define EEPROM_DEFAULT_PATH			"/etc/Wireless/RT2880_RT2820_AP_2T3R_V1_5.bin"
#endif
#endif //RT3052

#ifdef RTMP_PCI_SUPPORT
/*************************************************************************
  *	Public function declarations for prom-based chipset
  ************************************************************************/
int rtmp_ee_prom_read16(
	IN PRTMP_ADAPTER	pAd,
	IN USHORT			Offset,
	OUT USHORT			*pValue);

int rtmp_ee_prom_write16(
	IN PRTMP_ADAPTER	pAd,
	IN USHORT			Offset,
	IN USHORT			value);
#endif // RTMP_PCI_SUPPORT //

#ifdef RTMP_RBUS_SUPPORT
/*************************************************************************
  *	Public function declarations for flash-based chipset
  ************************************************************************/
NDIS_STATUS rtmp_nv_init(
	IN PRTMP_ADAPTER pAd);

USHORT rtmp_ee_flash_read(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT Offset,
	OUT USHORT *pValue);

VOID rtmp_ee_flash_write(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT Offset, 
	IN USHORT Data);

VOID rtmp_ee_flash_read_all(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT *Data);

VOID rtmp_ee_flash_write_all(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT *Data);

#endif // RTMP_RBUS_SUPPORT //


/*************************************************************************
  *	Public function declarations for prom operation callback functions setting
  ************************************************************************/
INT RtmpChipOpsEepromHook(
	IN RTMP_ADAPTER *pAd,
	IN INT			infType);

#endif // __EEPROM_H__ //
