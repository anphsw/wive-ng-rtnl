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
	cmm_info.c
 
    Abstract:
 
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */
 
#include	"rt_config.h"


INT	Show_SSID_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_WirelessMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_TxBurst_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_TxPreamble_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_TxPower_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_Channel_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_BGProtection_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_RTSThreshold_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_FragThreshold_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

#ifdef DOT11_N_SUPPORT
INT	Show_HtBw_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_HtMcs_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_HtGi_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_HtOpMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_HtExtcha_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_HtMpduDensity_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_HtBaWinSize_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_HtRdg_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_HtAmsdu_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_HtAutoBa_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);
#endif // DOT11_N_SUPPORT //

INT	Show_CountryRegion_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_CountryRegionABand_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_CountryCode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

#ifdef AGGREGATION_SUPPORT
INT	Show_PktAggregate_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);
#endif // AGGREGATION_SUPPORT //

#ifdef WMM_SUPPORT
INT	Show_WmmCapable_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);
#endif // WMM_SUPPORT //

INT	Show_IEEE80211H_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

#ifdef CONFIG_STA_SUPPORT
INT	Show_NetworkType_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

#ifdef WSC_STA_SUPPORT
INT	Show_WpsPbcBand_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);
#endif // WSC_STA_SUPPORT //
#endif // CONFIG_STA_SUPPORT //

INT	Show_AuthMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_EncrypType_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_DefaultKeyID_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_Key1_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_Key2_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_Key3_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_Key4_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

INT	Show_WPAPSK_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf);

extern INT	Set_AP_WscConfStatus_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

extern INT	Set_AP_AuthMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

extern INT	Set_AP_EncrypType_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

extern INT	Set_AP_SSID_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

extern INT	Set_AP_WPAPSK_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

static struct {
	PSTRING name;
	INT (*show_proc)(PRTMP_ADAPTER pAdapter, PSTRING arg);
} *PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC, RTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC[] = {
	{"SSID",					Show_SSID_Proc}, 
	{"WirelessMode",			Show_WirelessMode_Proc},       
	{"TxBurst",					Show_TxBurst_Proc},
	{"TxPreamble",				Show_TxPreamble_Proc},
	{"TxPower",					Show_TxPower_Proc},
	{"Channel",					Show_Channel_Proc},            
	{"BGProtection",			Show_BGProtection_Proc},
	{"RTSThreshold",			Show_RTSThreshold_Proc},       
	{"FragThreshold",			Show_FragThreshold_Proc},      
#ifdef DOT11_N_SUPPORT
	{"HtBw",					Show_HtBw_Proc},
	{"HtMcs",					Show_HtMcs_Proc},
	{"HtGi",					Show_HtGi_Proc},
	{"HtOpMode",				Show_HtOpMode_Proc},
	{"HtExtcha",				Show_HtExtcha_Proc},
	{"HtMpduDensity",			Show_HtMpduDensity_Proc},
	{"HtBaWinSize",		        Show_HtBaWinSize_Proc},
	{"HtRdg",		        	Show_HtRdg_Proc},
	{"HtAmsdu",		        	Show_HtAmsdu_Proc},
	{"HtAutoBa",		        Show_HtAutoBa_Proc},
#endif // DOT11_N_SUPPORT //
	{"CountryRegion",			Show_CountryRegion_Proc},
	{"CountryRegionABand",		Show_CountryRegionABand_Proc},
	{"CountryCode",				Show_CountryCode_Proc},
#ifdef AGGREGATION_SUPPORT
	{"PktAggregate",			Show_PktAggregate_Proc},       
#endif

#ifdef WMM_SUPPORT
	{"WmmCapable",				Show_WmmCapable_Proc},         
#endif         
	{"IEEE80211H",				Show_IEEE80211H_Proc},
#ifdef CONFIG_STA_SUPPORT	
	{"NetworkType",				Show_NetworkType_Proc},        
#ifdef WSC_STA_SUPPORT
	{"WpsApBand",				Show_WpsPbcBand_Proc},
#endif // WSC_STA_SUPPORT //
#endif // CONFIG_STA_SUPPORT //
	{"AuthMode",				Show_AuthMode_Proc},           
	{"EncrypType",				Show_EncrypType_Proc},         
	{"DefaultKeyID",			Show_DefaultKeyID_Proc},       
	{"Key1",					Show_Key1_Proc},               
	{"Key2",					Show_Key2_Proc},               
	{"Key3",					Show_Key3_Proc},               
	{"Key4",					Show_Key4_Proc},               
	{"WPAPSK",					Show_WPAPSK_Proc},
	{NULL, NULL}
};

/*
    ==========================================================================
    Description:
        Get Driver version.

    Return:
    ==========================================================================
*/
INT Set_DriverVersion_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		DBGPRINT(RT_DEBUG_TRACE, ("Driver version-%s\n", AP_DRIVER_VERSION));
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		DBGPRINT(RT_DEBUG_TRACE, ("Driver version-%s\n", STA_DRIVER_VERSION));
#endif // CONFIG_STA_SUPPORT //

    return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set Country Region.
        This command will not work, if the field of CountryRegion in eeprom is programmed.
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT Set_CountryRegion_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	int retval;
	
#ifdef EXT_BUILD_CHANNEL_LIST
	return -EOPNOTSUPP;
#endif // EXT_BUILD_CHANNEL_LIST //

	retval = RT_CfgSetCountryRegion(pAd, arg, BAND_24G);
	if (retval == FALSE)
		return FALSE;
	
	// if set country region, driver needs to be reset
	BuildChannelList(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_CountryRegion_Proc::(CountryRegion=%d)\n", pAd->CommonCfg.CountryRegion));
	
	return TRUE;
}


/* 
    ==========================================================================
    Description:
        Set Country Region for A band.
        This command will not work, if the field of CountryRegion in eeprom is programmed.
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT Set_CountryRegionABand_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	int retval;

#ifdef EXT_BUILD_CHANNEL_LIST
	return -EOPNOTSUPP;
#endif // EXT_BUILD_CHANNEL_LIST //

	retval = RT_CfgSetCountryRegion(pAd, arg, BAND_5G);
	if (retval == FALSE)
		return FALSE;

	// if set country region, driver needs to be reset
	BuildChannelList(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_CountryRegionABand_Proc::(CountryRegion=%d)\n", pAd->CommonCfg.CountryRegionForABand));
	
	return TRUE;
}


/* 
    ==========================================================================
    Description:
        Set Wireless Mode
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_WirelessMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	INT	success = TRUE;

	success = RT_CfgSetWirelessMode(pAd, arg);
	if (success)
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			UINT32 i;


			/* recover Wmm Capable for each BSS */
			for(i=0; i<pAd->ApCfg.BssidNum; i++)
			{
				pAd->ApCfg.MBSSID[i].bWmmCapable = \
										pAd->ApCfg.MBSSID[i].bWmmCapableOrg;
			}

			// TODO: Is the function BuildChannelList() still necessary here, due to it also been called in RTMPSetPhyMode()!
			BuildChannelList(pAd);
			RTMPSetPhyMode(pAd, pAd->CommonCfg.PhyMode);
		}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			LONG	WirelessMode = pAd->CommonCfg.PhyMode;

			/* clean up previous SCAN result */
			BssTableInit(&pAd->ScanTab);
			if (pAd->StaCfg.LastScanTime > 10 * OS_HZ)
				pAd->StaCfg.LastScanTime -= (10 * OS_HZ);
			
			RTMPSetPhyMode(pAd, WirelessMode);
#ifdef DOT11_N_SUPPORT
			if (WirelessMode >= PHY_11ABGN_MIXED)
			{
				pAd->CommonCfg.BACapability.field.AutoBA = TRUE;
				pAd->CommonCfg.REGBACapability.field.AutoBA = TRUE;
			}
			else
			{
				pAd->CommonCfg.BACapability.field.AutoBA = FALSE;
				pAd->CommonCfg.REGBACapability.field.AutoBA = FALSE;
			}
#endif // DOT11_N_SUPPORT //
			// Set AdhocMode rates
			if (pAd->StaCfg.BssType == BSS_ADHOC)
			{
				MlmeUpdateTxRates(pAd, FALSE, 0);
				MakeIbssBeacon(pAd);           // re-build BEACON frame
				AsicEnableIbssSync(pAd);       // copy to on-chip memory
			}
		}
#endif // CONFIG_STA_SUPPORT //

		// it is needed to set SSID to take effect
#ifdef DOT11_N_SUPPORT
		SetCommonHT(pAd);
#endif // DOT11_N_SUPPORT //



#ifdef CONFIG_RALINK_RT3350
	if(pAd->CommonCfg.PhyMode == PHY_11B)
	{
		USHORT i;
	        USHORT value;
		UCHAR  rf_offset;
		UCHAR  rf_value;

		RT28xx_EEPROM_READ16(pAd, 0x126, value);
		rf_value = value & 0x00FF;
                rf_offset = (value & 0xFF00) >> 8;

		if(rf_offset == 0xff)
		    rf_offset = RF_R21;
		if(rf_value == 0xff)
		    rf_value = 0x4F;
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, rf_offset, (UCHAR)rf_value);
	
		RT28xx_EEPROM_READ16(pAd, 0x12a, value);
		rf_value = value & 0x00FF;
                rf_offset = (value & 0xFF00) >> 8;

		if(rf_offset == 0xff)
		    rf_offset = RF_R29;
		if(rf_value == 0xff)
		    rf_value = 0x07;
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, rf_offset, (UCHAR)rf_value);
	

		// set RF_R24
		if(pAd->CommonCfg.RegTransmitSetting.field.BW == BW_40)
		{    
			value = 0x3F;
		}
		else
		{
			value = 0x1F;
		}
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R24, (UCHAR)value);


	}
	else
	{
		USHORT i;
	        USHORT value;
		UCHAR  rf_offset;
		UCHAR  rf_value;

		RT28xx_EEPROM_READ16(pAd, 0x124, value);
		rf_value = value & 0x00FF;
                rf_offset = (value & 0xFF00) >> 8;

		if(rf_offset == 0xff)
		    rf_offset = RF_R21;
		if(rf_value == 0xff)
		    rf_value = 0x6F;
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, rf_offset, (UCHAR)rf_value);
	
		RT28xx_EEPROM_READ16(pAd, 0x128, value);
		rf_value = value & 0x00FF;
                rf_offset = (value & 0xFF00) >> 8;

		if(rf_offset == 0xff)
		    rf_offset = RF_R29;
		if(rf_value == 0xff)
		    rf_value = 0x07;
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, rf_offset, (UCHAR)rf_value);
	
		// set RF_R24
		if(pAd->CommonCfg.RegTransmitSetting.field.BW == BW_40)
		{    
			value = 0x28;
		}
		else
		{
			value = 0x18;
		}
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R24, (UCHAR)value);

	}
#endif
		DBGPRINT(RT_DEBUG_TRACE, ("Set_WirelessMode_Proc::(=%d)\n", pAd->CommonCfg.PhyMode));
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_WirelessMode_Proc::parameters out of range\n"));
	}
	
	return success;
}

/* 
    ==========================================================================
    Description:
        Set Channel
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_Channel_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
#ifdef CONFIG_AP_SUPPORT
	INT		i;
#endif // CONFIG_AP_SUPPORT //
 	INT		success = TRUE;
	UCHAR	Channel;	

	Channel = (UCHAR) simple_strtol(arg, 0, 10);

	// check if this channel is valid
	if (ChannelSanity(pAd, Channel) == TRUE)
	{
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			pAd->CommonCfg.Channel = Channel;        

			if (MONITOR_ON(pAd))
			{
#ifdef DOT11_N_SUPPORT
				N_ChannelCheck(pAd);
				if (pAd->CommonCfg.PhyMode >= PHY_11ABGN_MIXED &&
					pAd->CommonCfg.RegTransmitSetting.field.BW == BW_40)
				{
					N_SetCenCh(pAd);
						AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
						AsicLockChannel(pAd, pAd->CommonCfg.CentralChannel);
					DBGPRINT(RT_DEBUG_TRACE, ("BW_40, control_channel(%d), CentralChannel(%d) \n", 
								pAd->CommonCfg.Channel, pAd->CommonCfg.CentralChannel));
				}
				else
#endif // DOT11_N_SUPPORT //
				{
					AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
					AsicLockChannel(pAd, pAd->CommonCfg.Channel);
					DBGPRINT(RT_DEBUG_TRACE, ("BW_20, Channel(%d)\n", pAd->CommonCfg.Channel));
				}
			}
		}
#endif // CONFIG_STA_SUPPORT //
		success = TRUE;
	}
	else
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			Channel = FirstChannel(pAd);
			DBGPRINT(RT_DEBUG_WARN,("This channel is out of channel list, set as the first channel(%d) \n ", Channel));
		}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			success = FALSE;
#endif // CONFIG_STA_SUPPORT //
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (((pAd->CommonCfg.PhyMode == PHY_11A)
#ifdef DOT11_N_SUPPORT
			|| (pAd->CommonCfg.PhyMode == PHY_11AN_MIXED)
#endif // DOT11_N_SUPPORT //
			)
			&& (pAd->CommonCfg.bIEEE80211H == TRUE))
		{
			for (i = 0; i < pAd->ChannelListNum; i++)
			{
				if (pAd->ChannelList[i].Channel == Channel)
				{
					if (pAd->ChannelList[i].RemainingTimeForUse > 0)
					{
						DBGPRINT(RT_DEBUG_ERROR, ("ERROR: previous detection of a radar on this channel(Channel=%d)\n", Channel));
						success = FALSE;
						break;
					}
					else
					{
						DBGPRINT(RT_DEBUG_TRACE, ("RemainingTimeForUse %d ,Channel %d\n", pAd->ChannelList[i].RemainingTimeForUse, Channel));
					}
				}
			}
		}

		if (success == TRUE)
		{
			pAd->CommonCfg.Channel = Channel;
#ifdef DOT11_N_SUPPORT
			N_ChannelCheck(pAd);
#endif // DOT11_N_SUPPORT //
			if ((pAd->CommonCfg.Channel > 14 )
				&& (pAd->CommonCfg.bIEEE80211H == TRUE))
			{
				if (pAd->CommonCfg.RadarDetect.RDMode == RD_SILENCE_MODE)
				{
					APStop(pAd);
					APStartUp(pAd);
				}
				else
				{
					NotifyChSwAnnToPeerAPs(pAd, ZERO_MAC_ADDR, pAd->CurrentAddress, 1, pAd->CommonCfg.Channel);
					pAd->CommonCfg.RadarDetect.RDMode = RD_SWITCHING_MODE;
					pAd->CommonCfg.RadarDetect.CSCount = 0;
				}
			}
			else
			{
				APStop(pAd);
				APStartUp(pAd);
			}
		}
	}
#endif // CONFIG_AP_SUPPORT //

	if (success == TRUE)
		DBGPRINT(RT_DEBUG_TRACE, ("Set_Channel_Proc::(Channel=%d)\n", pAd->CommonCfg.Channel));

	return success;
}


/* 
    ==========================================================================
    Description:
        Set Short Slot Time Enable or Disable
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ShortSlot_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	int retval;
	
	retval = RT_CfgSetShortSlot(pAd, arg);
	if (retval == TRUE)
		DBGPRINT(RT_DEBUG_TRACE, ("Set_ShortSlot_Proc::(ShortSlot=%d)\n", pAd->CommonCfg.bUseShortSlotTime));

	return retval;
}


/* 
    ==========================================================================
    Description:
        Set Tx power
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_TxPower_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	LONG TxPower;
	INT   success = FALSE;

	TxPower = simple_strtol(arg, 0, 10);
	if (TxPower <= 100)
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
			pAd->CommonCfg.TxPowerPercentage = TxPower;
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			pAd->CommonCfg.TxPowerDefault = TxPower;
			pAd->CommonCfg.TxPowerPercentage = pAd->CommonCfg.TxPowerDefault;
		}
#endif // CONFIG_STA_SUPPORT //
		success = TRUE;
	}
	else
		success = FALSE;

	DBGPRINT(RT_DEBUG_TRACE, ("Set_TxPower_Proc::(TxPowerPercentage=%ld)\n", pAd->CommonCfg.TxPowerPercentage));

	return success;
}

/* 
    ==========================================================================
    Description:
        Set 11B/11G Protection
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_BGProtection_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	switch (simple_strtol(arg, 0, 10))
	{
		case 0: //AUTO
			pAd->CommonCfg.UseBGProtection = 0;
			break;
		case 1: //Always On
			pAd->CommonCfg.UseBGProtection = 1;
			break;
		case 2: //Always OFF
			pAd->CommonCfg.UseBGProtection = 2;
			break;		
		default:  //Invalid argument 
			return FALSE;
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		APUpdateCapabilityAndErpIe(pAd);
#endif // CONFIG_AP_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("Set_BGProtection_Proc::(BGProtection=%ld)\n", pAd->CommonCfg.UseBGProtection));	

	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set TxPreamble
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_TxPreamble_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	RT_802_11_PREAMBLE	Preamble;

	Preamble = simple_strtol(arg, 0, 10);

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	if (Preamble == Rt802_11PreambleAuto)
		return FALSE;
#endif // CONFIG_AP_SUPPORT //

	switch (Preamble)
	{
		case Rt802_11PreambleShort:
			pAd->CommonCfg.TxPreamble = Preamble;
#ifdef CONFIG_STA_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
				MlmeSetTxPreamble(pAd, Rt802_11PreambleShort);
#endif // CONFIG_STA_SUPPORT //
			break;
		case Rt802_11PreambleLong:
#ifdef CONFIG_STA_SUPPORT
		case Rt802_11PreambleAuto:
			// if user wants AUTO, initialize to LONG here, then change according to AP's
			// capability upon association.
#endif // CONFIG_STA_SUPPORT //
			pAd->CommonCfg.TxPreamble = Preamble;
#ifdef CONFIG_STA_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
				MlmeSetTxPreamble(pAd, Rt802_11PreambleLong);
#endif // CONFIG_STA_SUPPORT //
			break;
		default: //Invalid argument 
			return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_TxPreamble_Proc::(TxPreamble=%ld)\n", pAd->CommonCfg.TxPreamble));

	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set RTS Threshold
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_RTSThreshold_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	 NDIS_802_11_RTS_THRESHOLD           RtsThresh;

	RtsThresh = simple_strtol(arg, 0, 10);

	if((RtsThresh > 0) && (RtsThresh <= MAX_RTS_THRESHOLD))
		pAd->CommonCfg.RtsThreshold  = (USHORT)RtsThresh;
#ifdef CONFIG_STA_SUPPORT
	else if (RtsThresh == 0)
		pAd->CommonCfg.RtsThreshold = MAX_RTS_THRESHOLD;
#endif // CONFIG_STA_SUPPORT //
	else
		return FALSE; //Invalid argument 

	DBGPRINT(RT_DEBUG_TRACE, ("Set_RTSThreshold_Proc::(RTSThreshold=%d)\n", pAd->CommonCfg.RtsThreshold));

	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set Fragment Threshold
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_FragThreshold_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	 NDIS_802_11_FRAGMENTATION_THRESHOLD     FragThresh;

	FragThresh = simple_strtol(arg, 0, 10);

	if (FragThresh > MAX_FRAG_THRESHOLD || FragThresh < MIN_FRAG_THRESHOLD)
	{ 
		//Illegal FragThresh so we set it to default
		pAd->CommonCfg.FragmentThreshold = MAX_FRAG_THRESHOLD;
	}
	else if (FragThresh % 2 == 1)
	{
		// The length of each fragment shall always be an even number of octets, except for the last fragment
		// of an MSDU or MMPDU, which may be either an even or an odd number of octets.
		pAd->CommonCfg.FragmentThreshold = (USHORT)(FragThresh - 1);
	}
	else
	{
		pAd->CommonCfg.FragmentThreshold = (USHORT)FragThresh;
	}

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->CommonCfg.FragmentThreshold == MAX_FRAG_THRESHOLD)
			pAd->CommonCfg.bUseZeroToDisableFragment = TRUE;
		else
			pAd->CommonCfg.bUseZeroToDisableFragment = FALSE;
	}
#endif // CONFIG_STA_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("Set_FragThreshold_Proc::(FragThreshold=%d)\n", pAd->CommonCfg.FragmentThreshold));

	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set TxBurst
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_TxBurst_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	LONG TxBurst;

	TxBurst = simple_strtol(arg, 0, 10);
	if (TxBurst == 1)
		pAd->CommonCfg.bEnableTxBurst = TRUE;
	else if (TxBurst == 0)
		pAd->CommonCfg.bEnableTxBurst = FALSE;
	else
		return FALSE;  //Invalid argument 
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_TxBurst_Proc::(TxBurst=%d)\n", pAd->CommonCfg.bEnableTxBurst));

	return TRUE;
}

INT Set_ShowRF_Proc(
	IN  PRTMP_ADAPTER		pAd,
	IN  PSTRING			arg)
{
	int ShowRF = simple_strtol(arg, 0, 10);
	
	if (ShowRF == 1)
		pAd->ShowRf = TRUE;
	else
		pAd->ShowRf = FALSE;
	
	return TRUE;
}


#ifdef AGGREGATION_SUPPORT
/* 
    ==========================================================================
    Description:
        Set TxBurst
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_PktAggregate_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	LONG aggre;

	aggre = simple_strtol(arg, 0, 10);

	if (aggre == 1)
		pAd->CommonCfg.bAggregationCapable = TRUE;
	else if (aggre == 0)
		pAd->CommonCfg.bAggregationCapable = FALSE;
	else
		return FALSE;  //Invalid argument 

#ifdef CONFIG_AP_SUPPORT
#ifdef PIGGYBACK_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		pAd->CommonCfg.bPiggyBackCapable = pAd->CommonCfg.bAggregationCapable;
		RTMPSetPiggyBack(pAd, pAd->CommonCfg.bPiggyBackCapable);
	}
#endif // PIGGYBACK_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("Set_PktAggregate_Proc::(AGGRE=%d)\n", pAd->CommonCfg.bAggregationCapable));

	return TRUE;
}
#endif


#ifdef INF_AMAZON_PPA
INT	Set_INF_AMAZON_SE_PPA_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
	ULONG aggre;
	UINT status;
	
	aggre = simple_strtol(arg, 0, 10);

	if (aggre == 1)
	{
		if(pAd->PPAEnable==TRUE)
		{
			printk("INF_AMAZON_SE_PPA already enabled \n");
		}
		else
		{
			if (ppa_hook_directpath_register_dev_fn) 
			{
				UINT32 g_if_id;
				
				if (pAd->pDirectpathCb == NULL) 
				{
					 pAd->pDirectpathCb = (PPA_DIRECTPATH_CB *) kmalloc (sizeof(PPA_DIRECTPATH_CB), GFP_ATOMIC);
				        printk("Realloc memory for  pDirectpathCb ??\n");
				}

				/* register callback */
				pAd->pDirectpathCb->rx_fn = NULL;
				pAd->pDirectpathCb->stop_tx_fn = NULL;
				pAd->pDirectpathCb->start_tx_fn = NULL;

				status = ppa_hook_directpath_register_dev_fn(&g_if_id, pAd->net_dev, pAd->pDirectpathCb, PPA_F_DIRECTPATH_ETH_IF);

				if(status==1)
				{
					pAd->g_if_id=g_if_id;
					printk("register INF_AMAZON_SE_PPA success :ret:%d id:%d:%d\n",status,pAd->g_if_id,g_if_id);
					pAd->PPAEnable=TRUE;
				}
				else
				{
					printk("register INF_AMAZON_SE_PPA fail :ret:%d\n",status);
				}

			}
			else
			{
				printk("INF_AMAZON_SE_PPA enable fail : there is no INF_AMAZON_SE_PPA module . \n");	
			}
		}

		
	}
	else if (aggre == 0)
	{
		if(pAd->PPAEnable==FALSE)
		{
			
printk("INF_AMAZON_SE_PPA already disable \n");
		}
		else
		{
			if (ppa_hook_directpath_register_dev_fn) 
			{
				UINT32 g_if_id;
				g_if_id=pAd->g_if_id;
				printk("g_if_id=%d \n",pAd->g_if_id);
				status=ppa_hook_directpath_register_dev_fn(&g_if_id, pAd->net_dev, NULL, PPA_F_DIRECTPATH_DEREGISTER);

				if(status==1)
				{
					pAd->g_if_id=0;
					printk("unregister INF_AMAZON_SE_PPA success :ret:%d\n",status);
					pAd->PPAEnable=FALSE;
				}
				else
				{
					printk("unregister INF_AMAZON_SE_PPA fail :ret:%d\n",status);
				}

			}
			else
			{
				printk("INF_AMAZON_SE_PPA enable fail : there is no INF_AMAZON_SE_PPA module . \n");	
			}
		}

	}
	else
	{
		printk("Invalid argument %d \n",aggre);
		return FALSE;  //Invalid argument 
	}	

	return TRUE;

}
#endif // INF_AMAZON_PPA //


/* 
    ==========================================================================
    Description:
        Set IEEE80211H.
        This parameter is 1 when needs radar detection, otherwise 0
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_IEEE80211H_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
    LONG ieee80211h;

	ieee80211h = simple_strtol(arg, 0, 10);

	if (ieee80211h == 1)
		pAd->CommonCfg.bIEEE80211H = TRUE;
	else if (ieee80211h == 0)
		pAd->CommonCfg.bIEEE80211H = FALSE;
	else
		return FALSE;  //Invalid argument 
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_IEEE80211H_Proc::(IEEE80211H=%d)\n", pAd->CommonCfg.bIEEE80211H));

	return TRUE;
}

#ifdef WSC_INCLUDED
INT	Set_WscGenPinCode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
    PWSC_CTRL   pWscControl = NULL;
    POS_COOKIE  pObj = (POS_COOKIE) pAd->OS_Cookie;
    UCHAR	    apidx = pObj->ioctl_if;
    
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef APCLI_SUPPORT
	    if (pObj->ioctl_if_type == INT_APCLI)
	    {
	        pWscControl = &pAd->ApCfg.ApCliTab[apidx].WscControl;
	        DBGPRINT(RT_DEBUG_TRACE, ("IF(apcli%d) Set_WscGenPinCode_Proc:: This command is from apcli interface now.\n", apidx));
	    }
	    else
#endif // APCLI_SUPPORT //
	    {
			pWscControl = &pAd->ApCfg.MBSSID[apidx].WscControl;
	        DBGPRINT(RT_DEBUG_TRACE, ("IF(ra%d) Set_WscGenPinCode_Proc:: This command is from ra interface now.\n", apidx));
	    }

		pWscControl->WscEnrolleePinCode = WscRandomGeneratePinCode(pAd, apidx);
	}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
    	pWscControl = &pAd->StaCfg.WscControl;
		pWscControl->WscEnrolleePinCode = WscRandomGeneratePinCode(pAd, apidx);
	}
#endif // CONFIG_STA_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("Set_WscGenPinCode_Proc:: Enrollee PinCode\t\t%08u\n", pWscControl->WscEnrolleePinCode));

	return TRUE;
}

INT Set_WscVendorPinCode_Proc(
    IN  PRTMP_ADAPTER   pAd, 
    IN  PSTRING          arg)
{
	PWSC_CTRL   pWscControl;

#ifdef CONFIG_AP_SUPPORT
	POS_COOKIE  pObj = (POS_COOKIE) pAd->OS_Cookie;
	UCHAR       apidx = pObj->ioctl_if;

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef APCLI_SUPPORT
		if (pObj->ioctl_if_type == INT_APCLI)
		{
			pWscControl = &pAd->ApCfg.ApCliTab[apidx].WscControl;
			DBGPRINT(RT_DEBUG_TRACE, ("Set_WscVendorPinCode_Proc() for apcli(%d)\n", apidx));
		}
		else
#endif // APCLI_SUPPORT //
		{
			pWscControl = &pAd->ApCfg.MBSSID[apidx].WscControl;
			DBGPRINT(RT_DEBUG_TRACE, ("Set_WscVendorPinCode_Proc() for ra%d!\n", apidx));
		}
	}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
 	{
 		pWscControl = &pAd->StaCfg.WscControl;
 	}
#endif // CONFIG_STA_SUPPORT //

	return RT_CfgSetWscPinCode(pAd, arg, pWscControl);
}
#endif // WSC_INCLUDED //

#ifdef DBG
/* 
    ==========================================================================
    Description:
        For Debug information
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_Debug_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	DBGPRINT(RT_DEBUG_TRACE, ("==> Set_Debug_Proc *******************\n"));

    if(simple_strtol(arg, 0, 10) <= RT_DEBUG_LOUD)
        RTDebugLevel = simple_strtol(arg, 0, 10);

	DBGPRINT(RT_DEBUG_TRACE, ("<== Set_Debug_Proc(RTDebugLevel = %ld)\n", RTDebugLevel));

	return TRUE;
}
#endif

INT	Show_DescInfo_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
#ifdef RTMP_MAC_PCI
	INT i, QueIdx=0;
//  ULONG	RegValue;
        PRT28XX_RXD_STRUC pRxD;
        PTXD_STRUC pTxD;
	PRTMP_TX_RING	pTxRing = &pAd->TxRing[QueIdx];	
	PRTMP_MGMT_RING	pMgmtRing = &pAd->MgmtRing;	
	PRTMP_RX_RING	pRxRing = &pAd->RxRing;	
	
	for(i=0;i<TX_RING_SIZE;i++)
	{	
	    pTxD = (PTXD_STRUC) pTxRing->Cell[i].AllocVa;
	    DBGPRINT(RT_DEBUG_OFF, ("Desc #%d\n",i));
	    hex_dump("Tx Descriptor", (PUCHAR)pTxD, 16);
	    DBGPRINT(RT_DEBUG_OFF, ("pTxD->DMADONE = %x\n", pTxD->DMADONE));
	}    
	DBGPRINT(RT_DEBUG_OFF, ("---------------------------------------------------\n"));
	for(i=0;i<MGMT_RING_SIZE;i++)
	{	
	    pTxD = (PTXD_STRUC) pMgmtRing->Cell[i].AllocVa;
	    DBGPRINT(RT_DEBUG_OFF, ("Desc #%d\n",i));
	    hex_dump("Mgmt Descriptor", (PUCHAR)pTxD, 16);
	    DBGPRINT(RT_DEBUG_OFF, ("pMgmt->DMADONE = %x\n", pTxD->DMADONE));
	}    
	DBGPRINT(RT_DEBUG_OFF, ("---------------------------------------------------\n"));
	for(i=0;i<RX_RING_SIZE;i++)
	{	
	    pRxD = (PRT28XX_RXD_STRUC) pRxRing->Cell[i].AllocVa;
	    DBGPRINT(RT_DEBUG_OFF, ("Desc #%d\n",i));
	    hex_dump("Rx Descriptor", (PUCHAR)pRxD, 16);
	    DBGPRINT(RT_DEBUG_OFF, ("pRxD->DDONE = %x\n", pRxD->DDONE));
	}
#endif // RTMP_MAC_PCI //

	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Reset statistics counter

    Arguments:
        pAdapter            Pointer to our adapter
        arg                 

    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_ResetStatCounter_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	DBGPRINT(RT_DEBUG_TRACE, ("==>Set_ResetStatCounter_Proc\n"));

	// add the most up-to-date h/w raw counters into software counters
	NICUpdateRawCounters(pAd);
    
	NdisZeroMemory(&pAd->WlanCounters, sizeof(COUNTER_802_11));
	NdisZeroMemory(&pAd->Counters8023, sizeof(COUNTER_802_3));
	NdisZeroMemory(&pAd->RalinkCounters, sizeof(COUNTER_RALINK));

	// Reset HotSpot counter

#ifdef CONFIG_AP_SUPPORT
#endif // CONFIG_AP_SUPPORT //

#ifdef TXBF_SUPPORT
	{
	int i;
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
		NdisZeroMemory(&pAd->MacTab.Content[i].TxBFCounters, sizeof(pAd->MacTab.Content[i].TxBFCounters));
	}
#endif

	return TRUE;
}

/*
	========================================================================
	
	Routine Description:
		Add WPA key process.
		In Adhoc WPANONE, bPairwise = 0;  KeyIdx = 0;

	Arguments:
		pAd 					Pointer to our adapter
		pBuf							Pointer to the where the key stored

	Return Value:
		NDIS_SUCCESS					Add key successfully

	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/

BOOLEAN RTMPCheckStrPrintAble(
    IN  CHAR *pInPutStr, 
    IN  UCHAR strLen)
{
    UCHAR i=0;
    
    for (i=0; i<strLen; i++)
    {
        if ((pInPutStr[i] < 0x21) ||
            (pInPutStr[i] > 0x7E))
            return FALSE;
    }
    
    return TRUE;
}

/*
	========================================================================
	
	Routine Description:
		Remove WPA Key process

	Arguments:
		pAd 					Pointer to our adapter
		pBuf							Pointer to the where the key stored

	Return Value:
		NDIS_SUCCESS					Add key successfully

	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
#ifdef CONFIG_STA_SUPPORT
VOID    RTMPSetDesiredRates(
    IN  PRTMP_ADAPTER   pAdapter,
    IN  LONG            Rates)
{
    NDIS_802_11_RATES aryRates;

    memset(&aryRates, 0x00, sizeof(NDIS_802_11_RATES));
    switch (pAdapter->CommonCfg.PhyMode)
    {
        case PHY_11A: // A only
            switch (Rates)
            {
                case 6000000: //6M
                    aryRates[0] = 0x0c; // 6M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_0;
                    break;
                case 9000000: //9M
                    aryRates[0] = 0x12; // 9M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_1;
                    break;
                case 12000000: //12M
                    aryRates[0] = 0x18; // 12M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_2;
                    break;
                case 18000000: //18M
                    aryRates[0] = 0x24; // 18M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_3;
                    break;
                case 24000000: //24M
                    aryRates[0] = 0x30; // 24M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_4;
                    break;
                case 36000000: //36M
                    aryRates[0] = 0x48; // 36M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_5;
                    break;
                case 48000000: //48M
                    aryRates[0] = 0x60; // 48M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_6;
                    break;
                case 54000000: //54M
                    aryRates[0] = 0x6c; // 54M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_7;
                    break;
                case -1: //Auto
                default:
                    aryRates[0] = 0x6c; // 54Mbps
                    aryRates[1] = 0x60; // 48Mbps
                    aryRates[2] = 0x48; // 36Mbps
                    aryRates[3] = 0x30; // 24Mbps
                    aryRates[4] = 0x24; // 18M
                    aryRates[5] = 0x18; // 12M
                    aryRates[6] = 0x12; // 9M
                    aryRates[7] = 0x0c; // 6M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_AUTO;
                    break;
            }
            break;
        case PHY_11BG_MIXED: // B/G Mixed
        case PHY_11B: // B only
        case PHY_11ABG_MIXED: // A/B/G Mixed
        default:
            switch (Rates)
            {
                case 1000000: //1M
                    aryRates[0] = 0x02;
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_0;
                    break;
                case 2000000: //2M
                    aryRates[0] = 0x04;
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_1;
                    break;
                case 5000000: //5.5M
                    aryRates[0] = 0x0b; // 5.5M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_2;
                    break;
                case 11000000: //11M
                    aryRates[0] = 0x16; // 11M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_3;
                    break;
                case 6000000: //6M
                    aryRates[0] = 0x0c; // 6M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_0;
                    break;
                case 9000000: //9M
                    aryRates[0] = 0x12; // 9M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_1;
                    break;
                case 12000000: //12M
                    aryRates[0] = 0x18; // 12M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_2;
                    break;
                case 18000000: //18M
                    aryRates[0] = 0x24; // 18M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_3;
                    break;
                case 24000000: //24M
                    aryRates[0] = 0x30; // 24M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_4;
                    break;
                case 36000000: //36M
                    aryRates[0] = 0x48; // 36M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_5;
                    break;
                case 48000000: //48M
                    aryRates[0] = 0x60; // 48M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_6;
                    break;
                case 54000000: //54M
                    aryRates[0] = 0x6c; // 54M
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_7;
                    break;
                case -1: //Auto
                default:
                    if (pAdapter->CommonCfg.PhyMode == PHY_11B)
                    { //B Only
                        aryRates[0] = 0x16; // 11Mbps
                        aryRates[1] = 0x0b; // 5.5Mbps
                        aryRates[2] = 0x04; // 2Mbps
                        aryRates[3] = 0x02; // 1Mbps
                    }
                    else
                    { //(B/G) Mixed or (A/B/G) Mixed
                        aryRates[0] = 0x6c; // 54Mbps
                        aryRates[1] = 0x60; // 48Mbps
                        aryRates[2] = 0x48; // 36Mbps
                        aryRates[3] = 0x30; // 24Mbps
                        aryRates[4] = 0x16; // 11Mbps
                        aryRates[5] = 0x0b; // 5.5Mbps
                        aryRates[6] = 0x04; // 2Mbps
                        aryRates[7] = 0x02; // 1Mbps
                    }
                    pAdapter->StaCfg.DesiredTransmitSetting.field.MCS = MCS_AUTO;
                    break;
            }
            break;
    }

    NdisZeroMemory(pAdapter->CommonCfg.DesireRate, MAX_LEN_OF_SUPPORTED_RATES);
    NdisMoveMemory(pAdapter->CommonCfg.DesireRate, &aryRates, sizeof(NDIS_802_11_RATES));
    DBGPRINT(RT_DEBUG_TRACE, (" RTMPSetDesiredRates (%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)\n",
        pAdapter->CommonCfg.DesireRate[0],pAdapter->CommonCfg.DesireRate[1],
        pAdapter->CommonCfg.DesireRate[2],pAdapter->CommonCfg.DesireRate[3],
        pAdapter->CommonCfg.DesireRate[4],pAdapter->CommonCfg.DesireRate[5],
        pAdapter->CommonCfg.DesireRate[6],pAdapter->CommonCfg.DesireRate[7] ));
    // Changing DesiredRate may affect the MAX TX rate we used to TX frames out
    MlmeUpdateTxRates(pAdapter, FALSE, 0);
}

NDIS_STATUS RTMPWPARemoveKeyProc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PVOID			pBuf)
{
	PNDIS_802_11_REMOVE_KEY pKey;
	ULONG					KeyIdx;
	NDIS_STATUS 			Status = NDIS_STATUS_FAILURE;
	BOOLEAN 	bTxKey; 		// Set the key as transmit key
	BOOLEAN 	bPairwise;		// Indicate the key is pairwise key
	BOOLEAN 	bKeyRSC;		// indicate the receive  SC set by KeyRSC value.
								// Otherwise, it will set by the NIC.
	BOOLEAN 	bAuthenticator; // indicate key is set by authenticator.
	INT 		i;

	DBGPRINT(RT_DEBUG_TRACE,("---> RTMPWPARemoveKeyProc\n"));
	
	pKey = (PNDIS_802_11_REMOVE_KEY) pBuf;
	KeyIdx = pKey->KeyIndex & 0xff;
	// Bit 31 of Add-key, Tx Key
	bTxKey		   = (pKey->KeyIndex & 0x80000000) ? TRUE : FALSE;
	// Bit 30 of Add-key PairwiseKey
	bPairwise	   = (pKey->KeyIndex & 0x40000000) ? TRUE : FALSE;
	// Bit 29 of Add-key KeyRSC
	bKeyRSC 	   = (pKey->KeyIndex & 0x20000000) ? TRUE : FALSE;
	// Bit 28 of Add-key Authenticator
	bAuthenticator = (pKey->KeyIndex & 0x10000000) ? TRUE : FALSE;

	// 1. If bTx is TRUE, return failure information
	if (bTxKey == TRUE)
		return(NDIS_STATUS_INVALID_DATA);

	// 2. Check Pairwise Key
	if (bPairwise)
	{
		// a. If BSSID is broadcast, remove all pairwise keys.
		// b. If not broadcast, remove the pairwise specified by BSSID
		for (i = 0; i < SHARE_KEY_NUM; i++)
		{
			if (MAC_ADDR_EQUAL(pAd->SharedKey[BSS0][i].BssId, pKey->BSSID))
			{
				DBGPRINT(RT_DEBUG_TRACE,("RTMPWPARemoveKeyProc(KeyIdx=%d)\n", i));
				pAd->SharedKey[BSS0][i].KeyLen = 0;
				pAd->SharedKey[BSS0][i].CipherAlg = CIPHER_NONE;
				AsicRemoveSharedKeyEntry(pAd, BSS0, (UCHAR)i);
				Status = NDIS_STATUS_SUCCESS;
				break;
			}
		}
	}
	// 3. Group Key
	else
	{
		// a. If BSSID is broadcast, remove all group keys indexed
		// b. If BSSID matched, delete the group key indexed.
		DBGPRINT(RT_DEBUG_TRACE,("RTMPWPARemoveKeyProc(KeyIdx=%ld)\n", KeyIdx));
		pAd->SharedKey[BSS0][KeyIdx].KeyLen = 0;
		pAd->SharedKey[BSS0][KeyIdx].CipherAlg = CIPHER_NONE;
		AsicRemoveSharedKeyEntry(pAd, BSS0, (UCHAR)KeyIdx);
		Status = NDIS_STATUS_SUCCESS;
	}

	return (Status);
}
#endif // CONFIG_STA_SUPPORT //


#ifdef CONFIG_STA_SUPPORT
/*
	========================================================================
	
	Routine Description:
		Remove All WPA Keys

	Arguments:
		pAd 					Pointer to our adapter

	Return Value:
		None

	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
VOID	RTMPWPARemoveAllKeys(
	IN	PRTMP_ADAPTER	pAd)
{

	UCHAR 	i;
	
	DBGPRINT(RT_DEBUG_TRACE,("RTMPWPARemoveAllKeys(AuthMode=%d, WepStatus=%d)\n", pAd->StaCfg.AuthMode, pAd->StaCfg.WepStatus));
#ifdef PCIE_PS_SUPPORT
	RTMP_CLEAR_PSFLAG(pAd, fRTMP_PS_CAN_GO_SLEEP);
#endif // PCIE_PS_SUPPORT //
	// For WEP/CKIP, there is no need to remove it, since WinXP won't set it again after
	// Link up. And it will be replaced if user changed it.
	if (pAd->StaCfg.AuthMode < Ndis802_11AuthModeWPA)
		return;

	// For WPA-None, there is no need to remove it, since WinXP won't set it again after
	// Link up. And it will be replaced if user changed it.
	if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPANone)
		return;

	// set BSSID wcid entry of the Pair-wise Key table as no-security mode
	AsicRemovePairwiseKeyEntry(pAd, BSSID_WCID);

	// set all shared key mode as no-security. 
	for (i = 0; i < SHARE_KEY_NUM; i++)
    {
		DBGPRINT(RT_DEBUG_TRACE,("remove %s key #%d\n", CipherName[pAd->SharedKey[BSS0][i].CipherAlg], i));
		NdisZeroMemory(&pAd->SharedKey[BSS0][i], sizeof(CIPHER_KEY));  						

		AsicRemoveSharedKeyEntry(pAd, BSS0, i);
	}
#ifdef PCIE_PS_SUPPORT
	RTMP_SET_PSFLAG(pAd, fRTMP_PS_CAN_GO_SLEEP);
#endif // PCIE_PS_SUPPORT //

}
#endif // CONFIG_STA_SUPPORT //	


/*
	========================================================================
	
	Routine Description:
	 	As STA's BSSID is a WC too, it uses shared key table.
	 	This function write correct unicast TX key to ASIC WCID.
 	 	And we still make a copy in our MacTab.Content[BSSID_WCID].PairwiseKey.
		Caller guarantee TKIP/AES always has keyidx = 0. (pairwise key)
		Caller guarantee WEP calls this function when set Txkey,  default key index=0~3.
		
	Arguments:
		pAd 					Pointer to our adapter
		pKey							Pointer to the where the key stored

	Return Value:
		NDIS_SUCCESS					Add key successfully

	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
/*
	========================================================================
	Routine Description:
		Change NIC PHY mode. Re-association may be necessary. possible settings
		include - PHY_11B, PHY_11BG_MIXED, PHY_11A, and PHY_11ABG_MIXED 

	Arguments:
		pAd - Pointer to our adapter
		phymode  - 

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	========================================================================
*/
VOID	RTMPSetPhyMode(
	IN	PRTMP_ADAPTER	pAd,
	IN	ULONG phymode)
{
	INT i;
	// the selected phymode must be supported by the RF IC encoded in E2PROM

	// if no change, do nothing
	/* bug fix
	if (pAd->CommonCfg.PhyMode == phymode)
		return;
    */
	pAd->CommonCfg.PhyMode = (UCHAR)phymode;

	DBGPRINT(RT_DEBUG_TRACE,("RTMPSetPhyMode : PhyMode=%d, channel=%d \n", pAd->CommonCfg.PhyMode, pAd->CommonCfg.Channel));
#ifdef EXT_BUILD_CHANNEL_LIST
	BuildChannelListEx(pAd);
#else
	BuildChannelList(pAd);
#endif // EXT_BUILD_CHANNEL_LIST //

	// sanity check user setting
	for (i = 0; i < pAd->ChannelListNum; i++)
	{
		if (pAd->CommonCfg.Channel == pAd->ChannelList[i].Channel)
			break;
	}

	if (i == pAd->ChannelListNum)
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		if (pAd->CommonCfg.Channel != 0)
				pAd->CommonCfg.Channel = FirstChannel(pAd);
#endif // CONFIG_AP_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			pAd->CommonCfg.Channel = FirstChannel(pAd);
#endif // CONFIG_STA_SUPPORT //
		DBGPRINT(RT_DEBUG_ERROR, ("RTMPSetPhyMode: channel is out of range, use first channel=%d \n", pAd->CommonCfg.Channel));
	}
	
	NdisZeroMemory(pAd->CommonCfg.SupRate, MAX_LEN_OF_SUPPORTED_RATES);
	NdisZeroMemory(pAd->CommonCfg.ExtRate, MAX_LEN_OF_SUPPORTED_RATES);
	NdisZeroMemory(pAd->CommonCfg.DesireRate, MAX_LEN_OF_SUPPORTED_RATES);
	switch (phymode) {
		case PHY_11B:
			pAd->CommonCfg.SupRate[0]  = 0x82;	  // 1 mbps, in units of 0.5 Mbps, basic rate
			pAd->CommonCfg.SupRate[1]  = 0x84;	  // 2 mbps, in units of 0.5 Mbps, basic rate
			pAd->CommonCfg.SupRate[2]  = 0x8B;	  // 5.5 mbps, in units of 0.5 Mbps, basic rate
			pAd->CommonCfg.SupRate[3]  = 0x96;	  // 11 mbps, in units of 0.5 Mbps, basic rate
			pAd->CommonCfg.SupRateLen  = 4;
			pAd->CommonCfg.ExtRateLen  = 0;
			pAd->CommonCfg.DesireRate[0]  = 2;	   // 1 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[1]  = 4;	   // 2 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[2]  = 11;    // 5.5 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[3]  = 22;    // 11 mbps, in units of 0.5 Mbps
			//pAd->CommonCfg.HTPhyMode.field.MODE = MODE_CCK; // This MODE is only FYI. not use
			break;

		case PHY_11G:
		case PHY_11BG_MIXED:
		case PHY_11ABG_MIXED:
#ifdef DOT11_N_SUPPORT
		case PHY_11N_2_4G:
		case PHY_11ABGN_MIXED:
		case PHY_11BGN_MIXED:
		case PHY_11GN_MIXED:
#endif // DOT11_N_SUPPORT //
			pAd->CommonCfg.SupRate[0]  = 0x82;	  // 1 mbps, in units of 0.5 Mbps, basic rate
			pAd->CommonCfg.SupRate[1]  = 0x84;	  // 2 mbps, in units of 0.5 Mbps, basic rate
			pAd->CommonCfg.SupRate[2]  = 0x8B;	  // 5.5 mbps, in units of 0.5 Mbps, basic rate
			pAd->CommonCfg.SupRate[3]  = 0x96;	  // 11 mbps, in units of 0.5 Mbps, basic rate
			pAd->CommonCfg.SupRate[4]  = 0x12;	  // 9 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.SupRate[5]  = 0x24;	  // 18 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.SupRate[6]  = 0x48;	  // 36 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.SupRate[7]  = 0x6c;	  // 54 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.SupRateLen  = 8;
			pAd->CommonCfg.ExtRate[0]  = 0x0C;	  // 6 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.ExtRate[1]  = 0x18;	  // 12 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.ExtRate[2]  = 0x30;	  // 24 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.ExtRate[3]  = 0x60;	  // 48 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.ExtRateLen  = 4;
			pAd->CommonCfg.DesireRate[0]  = 2;	   // 1 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[1]  = 4;	   // 2 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[2]  = 11;    // 5.5 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[3]  = 22;    // 11 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[4]  = 12;    // 6 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[5]  = 18;    // 9 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[6]  = 24;    // 12 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[7]  = 36;    // 18 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[8]  = 48;    // 24 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[9]  = 72;    // 36 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[10] = 96;    // 48 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[11] = 108;   // 54 mbps, in units of 0.5 Mbps
			break;

		case PHY_11A:
#ifdef DOT11_N_SUPPORT
		case PHY_11AN_MIXED:
		case PHY_11AGN_MIXED:
		case PHY_11N_5G:
#endif // DOT11_N_SUPPORT //
			pAd->CommonCfg.SupRate[0]  = 0x8C;	  // 6 mbps, in units of 0.5 Mbps, basic rate
			pAd->CommonCfg.SupRate[1]  = 0x12;	  // 9 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.SupRate[2]  = 0x98;	  // 12 mbps, in units of 0.5 Mbps, basic rate
			pAd->CommonCfg.SupRate[3]  = 0x24;	  // 18 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.SupRate[4]  = 0xb0;	  // 24 mbps, in units of 0.5 Mbps, basic rate
			pAd->CommonCfg.SupRate[5]  = 0x48;	  // 36 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.SupRate[6]  = 0x60;	  // 48 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.SupRate[7]  = 0x6c;	  // 54 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.SupRateLen  = 8;
			pAd->CommonCfg.ExtRateLen  = 0;
			pAd->CommonCfg.DesireRate[0]  = 12;    // 6 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[1]  = 18;    // 9 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[2]  = 24;    // 12 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[3]  = 36;    // 18 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[4]  = 48;    // 24 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[5]  = 72;    // 36 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[6]  = 96;    // 48 mbps, in units of 0.5 Mbps
			pAd->CommonCfg.DesireRate[7]  = 108;   // 54 mbps, in units of 0.5 Mbps
			//pAd->CommonCfg.HTPhyMode.field.MODE = MODE_OFDM; // This MODE is only FYI. not use
			break;

		default:
			break;
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		UINT	apidx;
		
		for (apidx = 0; apidx < pAd->ApCfg.BssidNum; apidx++)
		{
			MlmeUpdateTxRates(pAd, FALSE, apidx);
		}	
#ifdef WDS_SUPPORT
		for (apidx = 0; apidx < MAX_WDS_ENTRY; apidx++)
		{				
			MlmeUpdateTxRates(pAd, FALSE, apidx + MIN_NET_DEVICE_FOR_WDS);			
		}
#endif // WDS_SUPPORT //
#ifdef APCLI_SUPPORT
		for (apidx = 0; apidx < MAX_APCLI_NUM; apidx++)
		{				
			MlmeUpdateTxRates(pAd, FALSE, apidx + MIN_NET_DEVICE_FOR_APCLI);			
		}
#endif // APCLI_SUPPORT //		
	}
#endif // CONFIG_AP_SUPPORT //

	pAd->CommonCfg.BandState = UNKNOWN_BAND;	
}


#ifdef DOT11_N_SUPPORT
/*
	========================================================================
	Routine Description:
		Caller ensures we has 802.11n support.
		Calls at setting HT from AP/STASetinformation

	Arguments:
		pAd - Pointer to our adapter
		phymode  - 

	========================================================================
*/
VOID	RTMPSetHT(
	IN	PRTMP_ADAPTER	pAd,
	IN	OID_SET_HT_PHYMODE *pHTPhyMode)
{
	//ULONG	*pmcs;
	UINT32	Value = 0;
	UCHAR	BBPValue = 0;
	UCHAR	BBP3Value = 0;
	UCHAR	RxStream = pAd->CommonCfg.RxStream;
#ifdef CONFIG_AP_SUPPORT
	INT		apidx;
#endif // CONFIG_AP_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("RTMPSetHT : HT_mode(%d), ExtOffset(%d), MCS(%d), BW(%d), STBC(%d), SHORTGI(%d)\n",
										pHTPhyMode->HtMode, pHTPhyMode->ExtOffset, 
										pHTPhyMode->MCS, pHTPhyMode->BW,
										pHTPhyMode->STBC, pHTPhyMode->SHORTGI));
			
	// Don't zero supportedHyPhy structure.
	RTMPZeroMemory(&pAd->CommonCfg.HtCapability, sizeof(pAd->CommonCfg.HtCapability));
	RTMPZeroMemory(&pAd->CommonCfg.AddHTInfo, sizeof(pAd->CommonCfg.AddHTInfo));
	RTMPZeroMemory(&pAd->CommonCfg.NewExtChanOffset, sizeof(pAd->CommonCfg.NewExtChanOffset));
	RTMPZeroMemory(&pAd->CommonCfg.DesiredHtPhy, sizeof(pAd->CommonCfg.DesiredHtPhy));

   	if (pAd->CommonCfg.bRdg)
	{
		pAd->CommonCfg.HtCapability.ExtHtCapInfo.PlusHTC = 1;
		pAd->CommonCfg.HtCapability.ExtHtCapInfo.RDGSupport = 1;
	}
	else
	{
		pAd->CommonCfg.HtCapability.ExtHtCapInfo.PlusHTC = 0;
		pAd->CommonCfg.HtCapability.ExtHtCapInfo.RDGSupport = 0;
	}

	pAd->CommonCfg.HtCapability.HtCapParm.MaxRAmpduFactor = 3;
	pAd->CommonCfg.DesiredHtPhy.MaxRAmpduFactor = 3;

	DBGPRINT(RT_DEBUG_TRACE, ("RTMPSetHT : RxBAWinLimit = %d\n", pAd->CommonCfg.BACapability.field.RxBAWinLimit));

	// Mimo power save, A-MSDU size, 
	pAd->CommonCfg.DesiredHtPhy.AmsduEnable = (USHORT)pAd->CommonCfg.BACapability.field.AmsduEnable;
	pAd->CommonCfg.DesiredHtPhy.AmsduSize = (UCHAR)pAd->CommonCfg.BACapability.field.AmsduSize;
	pAd->CommonCfg.DesiredHtPhy.MimoPs = (UCHAR)pAd->CommonCfg.BACapability.field.MMPSmode;
	pAd->CommonCfg.DesiredHtPhy.MpduDensity = (UCHAR)pAd->CommonCfg.BACapability.field.MpduDensity;

	pAd->CommonCfg.HtCapability.HtCapInfo.AMsduSize = (USHORT)pAd->CommonCfg.BACapability.field.AmsduSize;
	pAd->CommonCfg.HtCapability.HtCapInfo.MimoPs = (USHORT)pAd->CommonCfg.BACapability.field.MMPSmode;
	pAd->CommonCfg.HtCapability.HtCapParm.MpduDensity = (UCHAR)pAd->CommonCfg.BACapability.field.MpduDensity;
	
	DBGPRINT(RT_DEBUG_TRACE, ("RTMPSetHT : AMsduSize = %d, MimoPs = %d, MpduDensity = %d, MaxRAmpduFactor = %d\n", 
													pAd->CommonCfg.DesiredHtPhy.AmsduSize, 
													pAd->CommonCfg.DesiredHtPhy.MimoPs,
													pAd->CommonCfg.DesiredHtPhy.MpduDensity,
													pAd->CommonCfg.DesiredHtPhy.MaxRAmpduFactor));
	
	if(pHTPhyMode->HtMode == HTMODE_GF)
	{
		pAd->CommonCfg.HtCapability.HtCapInfo.GF = 1;
		pAd->CommonCfg.DesiredHtPhy.GF = 1;
	}
	else
		pAd->CommonCfg.DesiredHtPhy.GF = 0;
	
	// Decide Rx MCSSet
	switch (RxStream)
	{
		case 1:			
			pAd->CommonCfg.HtCapability.MCSSet[0] =  0xff;
			pAd->CommonCfg.HtCapability.MCSSet[1] =  0x00;
			break;

		case 2:
			pAd->CommonCfg.HtCapability.MCSSet[0] =  0xff;
			pAd->CommonCfg.HtCapability.MCSSet[1] =  0xff;
			break;

		case 3: // 3*3
			pAd->CommonCfg.HtCapability.MCSSet[0] =  0xff;
			pAd->CommonCfg.HtCapability.MCSSet[1] =  0xff;
			pAd->CommonCfg.HtCapability.MCSSet[2] =  0xff;
			break;
	}

	if (pAd->CommonCfg.bForty_Mhz_Intolerant && (pAd->CommonCfg.Channel <= 14) && (pHTPhyMode->BW == BW_40) )
	{
		pHTPhyMode->BW = BW_20;
		pAd->CommonCfg.HtCapability.HtCapInfo.Forty_Mhz_Intolerant = 1;
	}

	if(pHTPhyMode->BW == BW_40)
	{
		pAd->CommonCfg.HtCapability.MCSSet[4] = 0x1; // MCS 32
		pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth = 1;
		if (pAd->CommonCfg.Channel <= 14) 		
			pAd->CommonCfg.HtCapability.HtCapInfo.CCKmodein40 = 1;

		pAd->CommonCfg.DesiredHtPhy.ChannelWidth = 1;
		pAd->CommonCfg.AddHTInfo.AddHtInfo.RecomWidth = 1;
		pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset = (pHTPhyMode->ExtOffset == EXTCHA_BELOW)? (EXTCHA_BELOW): EXTCHA_ABOVE;
		// Set Regsiter for extension channel position.
		RTMP_IO_READ32(pAd, TX_BAND_CFG, &Value);
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BBP3Value);
		if ((pHTPhyMode->ExtOffset == EXTCHA_BELOW))
		{
			Value |= 0x1;
			BBP3Value |= (0x20);
			RTMP_IO_WRITE32(pAd, TX_BAND_CFG, Value);
		}
		else if ((pHTPhyMode->ExtOffset == EXTCHA_ABOVE))
		{
			Value &= 0xfe;
			BBP3Value &= (~0x20);
			RTMP_IO_WRITE32(pAd, TX_BAND_CFG, Value);
		}

		// Turn on BBP 40MHz mode now only as AP . 
		// Sta can turn on BBP 40MHz after connection with 40MHz AP. Sta only broadcast 40MHz capability before connection.
		if ((pAd->OpMode == OPMODE_AP) || INFRA_ON(pAd) || ADHOC_ON(pAd)
			)
		{
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BBPValue);
			BBPValue &= (~0x18);

#ifdef COC_SUPPORT
			if (pAd->CoC_sleep == 0)
#endif // COC_SUPPORT
				BBPValue |= 0x10;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BBPValue);

			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BBP3Value);
#ifdef COC_SUPPORT
			if (pAd->CoC_sleep == 1)
				pAd->CommonCfg.BBPCurrentBW = BW_20;
			else
#endif // COC_SUPPORT
				pAd->CommonCfg.BBPCurrentBW = BW_40;

#ifdef RT305x
#ifndef CONFIG_RALINK_RT3350
			RT30xxReadRFRegister(pAd, RF_R24, (PUCHAR)&Value);
#ifdef COC_SUPPORT
			if (pAd->CoC_sleep == 1)
				Value &= 0xDF;
			else
#endif // COC_SUPPORT
				Value |= 0x20;
			RT30xxWriteRFRegister(pAd, RF_R24, (UCHAR)Value);


#ifdef COC_SUPPORT
			if (pAd->CoC_sleep == 1)
				RT30xxWriteRFRegister(pAd, RF_R31, 0x0F);
			else
#endif // COC_SUPPORT
			    RT30xxWriteRFRegister(pAd, RF_R31, 0x2F);

#else //RT3350
			RT30xxReadRFRegister(pAd, RF_R24, (PUCHAR)&Value);
#ifdef COC_SUPPORT
			if (pAd->CoC_sleep == 1)
				Value &= 0xCF;
			else
#endif // COC_SUPPORT
			/*kurtis: RT3350 non CCK Mode, BW=40M  => RF_R24=0x28*/
				Value = 0x28;
			RT30xxWriteRFRegister(pAd, RF_R24, (UCHAR)Value);


#ifdef COC_SUPPORT
			if (pAd->CoC_sleep == 1)
				RT30xxWriteRFRegister(pAd, RF_R31, 0x48);
			else
#endif // COC_SUPPORT
				RT30xxWriteRFRegister(pAd, RF_R31, 0x68);



#endif// end RT3350

#endif // RT305x //

		}
	}
	else
	{
		pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth = 0;
		pAd->CommonCfg.DesiredHtPhy.ChannelWidth = 0;
		pAd->CommonCfg.AddHTInfo.AddHtInfo.RecomWidth = 0;
		pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset = EXTCHA_NONE;
		pAd->CommonCfg.CentralChannel = pAd->CommonCfg.Channel;
		// Turn on BBP 20MHz mode by request here.
		{
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BBPValue);
			BBPValue &= (~0x18);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BBPValue);
			pAd->CommonCfg.BBPCurrentBW = BW_20;
#ifdef RT305x
			#ifndef CONFIG_RALINK_RT3350
                        RT30xxReadRFRegister(pAd, RF_R24, (PUCHAR)&Value);
                        Value &= 0xDF;
                        RT30xxWriteRFRegister(pAd, RF_R24, (UCHAR)Value);
                        RT30xxWriteRFRegister(pAd, RF_R31, 0x0F);

			#else
			/*kurtis: RT3350 non CCK Mode, BW=20M  => RF_R24=0x18*/
                        Value = 0x18;
                        RT30xxWriteRFRegister(pAd, RF_R24, (UCHAR)Value);

                        RT30xxWriteRFRegister(pAd, RF_R31, 0x48);
			#endif

#endif // RT305x //

		}
	}
	if (pHTPhyMode->STBC == STBC_USE)
	{
		pAd->CommonCfg.HtCapability.HtCapInfo.TxSTBC = (pAd->Antenna.field.TxPath > 1);
		pAd->CommonCfg.DesiredHtPhy.TxSTBC = (pAd->Antenna.field.TxPath > 1);
		pAd->CommonCfg.HtCapability.HtCapInfo.RxSTBC = 1;
		pAd->CommonCfg.DesiredHtPhy.RxSTBC = 1;
	}
	else
	{
		pAd->CommonCfg.DesiredHtPhy.TxSTBC = 0;
		pAd->CommonCfg.DesiredHtPhy.RxSTBC = 0;
	}

	if(pHTPhyMode->SHORTGI == GI_400)
	{
		pAd->CommonCfg.HtCapability.HtCapInfo.ShortGIfor20 = 1;
		pAd->CommonCfg.HtCapability.HtCapInfo.ShortGIfor40 = 1;
		pAd->CommonCfg.DesiredHtPhy.ShortGIfor20 = 1;
		pAd->CommonCfg.DesiredHtPhy.ShortGIfor40 = 1;
	}
	else
	{
		pAd->CommonCfg.HtCapability.HtCapInfo.ShortGIfor20 = 0;
		pAd->CommonCfg.HtCapability.HtCapInfo.ShortGIfor40 = 0;
		pAd->CommonCfg.DesiredHtPhy.ShortGIfor20 = 0;
		pAd->CommonCfg.DesiredHtPhy.ShortGIfor40 = 0;
	}
	
	// We support link adaptation for unsolicit MCS feedback, set to 2.
	//pAd->CommonCfg.HtCapability.ExtHtCapInfo.MCSFeedback = MCSFBK_NONE; //MCSFBK_UNSOLICIT;
#if defined(CONFIG_RALINK_RT2883) || defined(CONFIG_RALINK_RT3883)
	pAd->CommonCfg.HtCapability.ExtHtCapInfo.MCSFeedback = MCSFBK_MRQ; //MCSFBK_UNSOLICIT;
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //
	pAd->CommonCfg.AddHTInfo.ControlChan = pAd->CommonCfg.Channel;
	// 1, the extension channel above the control channel. 
	
	// EDCA parameters used for AP's own transmission
	if (pAd->CommonCfg.APEdcaParm.bValid == FALSE)
	{
		pAd->CommonCfg.APEdcaParm.bValid = TRUE;
		pAd->CommonCfg.APEdcaParm.Aifsn[0] = 3;
		pAd->CommonCfg.APEdcaParm.Aifsn[1] = 7;
		pAd->CommonCfg.APEdcaParm.Aifsn[2] = 1;
		pAd->CommonCfg.APEdcaParm.Aifsn[3] = 1;
	
		pAd->CommonCfg.APEdcaParm.Cwmin[0] = 4;
		pAd->CommonCfg.APEdcaParm.Cwmin[1] = 4;
		pAd->CommonCfg.APEdcaParm.Cwmin[2] = 3;
		pAd->CommonCfg.APEdcaParm.Cwmin[3] = 2;

		pAd->CommonCfg.APEdcaParm.Cwmax[0] = 6;
		pAd->CommonCfg.APEdcaParm.Cwmax[1] = 10;
		pAd->CommonCfg.APEdcaParm.Cwmax[2] = 4;
		pAd->CommonCfg.APEdcaParm.Cwmax[3] = 3;

		pAd->CommonCfg.APEdcaParm.Txop[0]  = 0;
		pAd->CommonCfg.APEdcaParm.Txop[1]  = 0;
		pAd->CommonCfg.APEdcaParm.Txop[2]  = 94;	
		pAd->CommonCfg.APEdcaParm.Txop[3]  = 47;	
	}
	AsicSetEdcaParm(pAd, &pAd->CommonCfg.APEdcaParm);
	
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		for (apidx = 0; apidx < pAd->ApCfg.BssidNum; apidx++)
		{				
			RTMPSetIndividualHT(pAd, apidx);			
		}
#ifdef WDS_SUPPORT
		for (apidx = 0; apidx < MAX_WDS_ENTRY; apidx++)
		{				
			RTMPSetIndividualHT(pAd, apidx + MIN_NET_DEVICE_FOR_WDS);			
		}
#endif // WDS_SUPPORT //
#ifdef APCLI_SUPPORT
		for (apidx = 0; apidx < MAX_APCLI_NUM; apidx++)
		{				
			RTMPSetIndividualHT(pAd, apidx + MIN_NET_DEVICE_FOR_APCLI);			
		}
#endif // APCLI_SUPPORT //
	}
#endif // CONFIG_AP_SUPPORT //

#ifdef TXBF_SUPPORT
	pAd->CommonCfg.HtCapability.TxBFCap.TxSoundCapable =  TRUE;
#endif // TXBF_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
		RTMPSetIndividualHT(pAd, 0);
		}
#endif // CONFIG_STA_SUPPORT //

}

/*
	========================================================================
	Routine Description:
		Caller ensures we has 802.11n support.
		Calls at setting HT from AP/STASetinformation

	Arguments:
		pAd - Pointer to our adapter
		phymode  - 

	========================================================================
*/
VOID	RTMPSetIndividualHT(
	IN	PRTMP_ADAPTER		pAd,
	IN	UCHAR				apidx)
{	
	PRT_HT_PHY_INFO		pDesired_ht_phy = NULL;
	UCHAR	TxStream = pAd->CommonCfg.TxStream;		
	UCHAR	DesiredMcs	= MCS_AUTO;
	UCHAR	encrypt_mode = Ndis802_11EncryptionDisabled;
						
	do
	{

#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
#ifdef APCLI_SUPPORT	
			if (apidx >= MIN_NET_DEVICE_FOR_APCLI)
			{				
				UCHAR	idx = apidx - MIN_NET_DEVICE_FOR_APCLI;
						
				pDesired_ht_phy = &pAd->ApCfg.ApCliTab[idx].DesiredHtPhyInfo;									
				DesiredMcs = pAd->ApCfg.ApCliTab[idx].DesiredTransmitSetting.field.MCS;							
				encrypt_mode = pAd->ApCfg.ApCliTab[idx].WepStatus;
				pAd->ApCfg.ApCliTab[idx].bAutoTxRateSwitch = (DesiredMcs == MCS_AUTO) ? TRUE : FALSE;
					break;
			}
#endif // APCLI_SUPPORT //

#ifdef WDS_SUPPORT
			if (apidx >= MIN_NET_DEVICE_FOR_WDS)
			{				
				UCHAR	idx = apidx - MIN_NET_DEVICE_FOR_WDS;
						
				pDesired_ht_phy = &pAd->WdsTab.WdsEntry[idx].DesiredHtPhyInfo;									
				DesiredMcs = pAd->WdsTab.WdsEntry[idx].DesiredTransmitSetting.field.MCS;							
				//encrypt_mode = pAd->WdsTab.WdsEntry[idx].WepStatus;
				pAd->WdsTab.WdsEntry[idx].bAutoTxRateSwitch = (DesiredMcs == MCS_AUTO) ? TRUE : FALSE;
				break;
			}
#endif // WDS_SUPPORT //
			if (apidx < pAd->ApCfg.BssidNum)
			{								
				pDesired_ht_phy = &pAd->ApCfg.MBSSID[apidx].DesiredHtPhyInfo;									
				DesiredMcs = pAd->ApCfg.MBSSID[apidx].DesiredTransmitSetting.field.MCS;			
				encrypt_mode = pAd->ApCfg.MBSSID[apidx].WepStatus;
				pAd->ApCfg.MBSSID[apidx].bWmmCapable = TRUE;	
				pAd->ApCfg.MBSSID[apidx].bAutoTxRateSwitch = (DesiredMcs == MCS_AUTO) ? TRUE : FALSE;
				break;
			}

			DBGPRINT(RT_DEBUG_ERROR, ("RTMPSetIndividualHT: invalid apidx(%d)\n", apidx));
			return;
		}			
#endif // CONFIG_AP_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{		
			pDesired_ht_phy = &pAd->StaCfg.DesiredHtPhyInfo;					
			DesiredMcs = pAd->StaCfg.DesiredTransmitSetting.field.MCS;
			encrypt_mode = pAd->StaCfg.WepStatus;
			//pAd->StaCfg.bAutoTxRateSwitch = (DesiredMcs == MCS_AUTO) ? TRUE : FALSE;
				break;
		}	
#endif // CONFIG_STA_SUPPORT //
	} while (FALSE);

	if (pDesired_ht_phy == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("RTMPSetIndividualHT: invalid apidx(%d)\n", apidx));
		return;
	}
	RTMPZeroMemory(pDesired_ht_phy, sizeof(RT_HT_PHY_INFO));

	DBGPRINT(RT_DEBUG_TRACE, ("RTMPSetIndividualHT : Desired MCS = %d\n", DesiredMcs));
	// Check the validity of MCS 
	if ((TxStream == 1) && ((DesiredMcs >= MCS_8) && (DesiredMcs <= MCS_15)))
	{
		DBGPRINT(RT_DEBUG_WARN, ("RTMPSetIndividualHT: MCS(%d) is invalid in 1S, reset it as MCS_7\n", DesiredMcs));
		DesiredMcs = MCS_7;		
	}

	if ((pAd->CommonCfg.DesiredHtPhy.ChannelWidth == BW_20) && (DesiredMcs == MCS_32))
	{
		DBGPRINT(RT_DEBUG_WARN, ("RTMPSetIndividualHT: MCS_32 is only supported in 40-MHz, reset it as MCS_0\n"));
		DesiredMcs = MCS_0;		
	}
	   		
	/* 	
		WFA recommend to restrict the encryption type in 11n-HT mode.
	 	So, the WEP and TKIP are not allowed in HT rate. 
	*/
	if (pAd->CommonCfg.HT_DisallowTKIP && IS_INVALID_HT_SECURITY(encrypt_mode))
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s : Use legacy rate in WEP/TKIP encryption mode (apidx=%d)\n", 
									__FUNCTION__, apidx));
		return;
	}
			
	pDesired_ht_phy->bHtEnable = TRUE;
					 
	// Decide desired Tx MCS
	switch (TxStream)
	{
		case 1:
			if (DesiredMcs == MCS_AUTO)
			{
				pDesired_ht_phy->MCSSet[0]= 0xff;
				pDesired_ht_phy->MCSSet[1]= 0x00;
			}
			else if (DesiredMcs <= MCS_7)
			{
				pDesired_ht_phy->MCSSet[0]= 1<<DesiredMcs;
				pDesired_ht_phy->MCSSet[1]= 0x00;
			}			
			break;

		case 2:
			if (DesiredMcs == MCS_AUTO)
			{
				pDesired_ht_phy->MCSSet[0]= 0xff;
				pDesired_ht_phy->MCSSet[1]= 0xff;
			}
			else if (DesiredMcs <= MCS_15)
			{
				ULONG mode;
				
				mode = DesiredMcs / 8;
				if (mode < 2)
					pDesired_ht_phy->MCSSet[mode] = (1 << (DesiredMcs - mode * 8));
			}			
			break;

		case 3: // 3*3
			if (DesiredMcs == MCS_AUTO)
			{
				/* MCS0 ~ MCS23, 3 bytes */
				pDesired_ht_phy->MCSSet[0]= 0xff;
				pDesired_ht_phy->MCSSet[1]= 0xff;
				pDesired_ht_phy->MCSSet[2]= 0xff;
			}
			else if (DesiredMcs <= MCS_23)
			{
				ULONG mode;

				mode = DesiredMcs / 8;
				if (mode < 3)
					pDesired_ht_phy->MCSSet[mode] = (1 << (DesiredMcs - mode * 8));
			}
			break;
	}							

	if(pAd->CommonCfg.DesiredHtPhy.ChannelWidth == BW_40)
	{
		if (DesiredMcs == MCS_AUTO || DesiredMcs == MCS_32)
			pDesired_ht_phy->MCSSet[4] = 0x1;
	}

	// update HT Rate setting				
    if (pAd->OpMode == OPMODE_STA)
        MlmeUpdateHtTxRates(pAd, BSS0);
    else
	    MlmeUpdateHtTxRates(pAd, apidx);
}


/*
	========================================================================
	Routine Description:
		Update HT IE from our capability.
		
	Arguments:
		Send all HT IE in beacon/probe rsp/assoc rsp/action frame.
		
	
	========================================================================
*/
VOID	RTMPUpdateHTIE(
	IN	RT_HT_CAPABILITY	*pRtHt,
	IN		UCHAR				*pMcsSet,
	OUT		HT_CAPABILITY_IE *pHtCapability,
	OUT		ADD_HT_INFO_IE		*pAddHtInfo)
{
	RTMPZeroMemory(pHtCapability, sizeof(HT_CAPABILITY_IE));
	RTMPZeroMemory(pAddHtInfo, sizeof(ADD_HT_INFO_IE));
	
		pHtCapability->HtCapInfo.ChannelWidth = pRtHt->ChannelWidth;
		pHtCapability->HtCapInfo.MimoPs = pRtHt->MimoPs;
		pHtCapability->HtCapInfo.GF = pRtHt->GF;
		pHtCapability->HtCapInfo.ShortGIfor20 = pRtHt->ShortGIfor20;
		pHtCapability->HtCapInfo.ShortGIfor40 = pRtHt->ShortGIfor40;
		pHtCapability->HtCapInfo.TxSTBC = pRtHt->TxSTBC;
		pHtCapability->HtCapInfo.RxSTBC = pRtHt->RxSTBC;
		pHtCapability->HtCapInfo.AMsduSize = pRtHt->AmsduSize;
		pHtCapability->HtCapParm.MaxRAmpduFactor = pRtHt->MaxRAmpduFactor;
		pHtCapability->HtCapParm.MpduDensity = pRtHt->MpduDensity;

		pAddHtInfo->AddHtInfo.ExtChanOffset = pRtHt->ExtChanOffset ;
		pAddHtInfo->AddHtInfo.RecomWidth = pRtHt->RecomWidth;
		pAddHtInfo->AddHtInfo2.OperaionMode = pRtHt->OperaionMode;
		pAddHtInfo->AddHtInfo2.NonGfPresent = pRtHt->NonGfPresent;
		RTMPMoveMemory(pAddHtInfo->MCSSet, /*pRtHt->MCSSet*/pMcsSet, 4); // rt2860 only support MCS max=32, no need to copy all 16 uchar.
	
        DBGPRINT(RT_DEBUG_TRACE,("RTMPUpdateHTIE <== \n"));
}
#endif // DOT11_N_SUPPORT //

/*
	========================================================================
	Description:
		Add Client security information into ASIC WCID table and IVEIV table.
    Return:
	========================================================================
*/
VOID	RTMPAddWcidAttributeEntry(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			BssIdx,
	IN 	UCHAR		 	KeyIdx,
	IN 	UCHAR		 	CipherAlg,
	IN 	MAC_TABLE_ENTRY *pEntry)
{
	UINT32		WCIDAttri = 0;
	USHORT		offset;
	UCHAR		IVEIV = 0;
	USHORT		Wcid = 0;
#ifdef CONFIG_AP_SUPPORT
	BOOLEAN		IEEE8021X = FALSE;
#endif // CONFIG_AP_SUPPORT //

	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
#ifdef APCLI_SUPPORT
			if (BssIdx >= MIN_NET_DEVICE_FOR_APCLI)
			{
				if (pEntry)		
					BssIdx -= MIN_NET_DEVICE_FOR_APCLI;		
				else
				{
					DBGPRINT(RT_DEBUG_WARN, ("RTMPAddWcidAttributeEntry: AP-Client link doesn't need to set Group WCID Attribute. \n"));	
					return;
				}
			}	
			else 
#endif // APCLI_SUPPORT //
#ifdef WDS_SUPPORT
			if (BssIdx >= MIN_NET_DEVICE_FOR_WDS)
			{
				if (pEntry)		
					BssIdx = BSS0;		
				else
				{
					DBGPRINT(RT_DEBUG_WARN, ("RTMPAddWcidAttributeEntry: WDS link doesn't need to set Group WCID Attribute. \n"));	
					return;
				}
			}	
			else
#endif // WDS_SUPPORT //
			{
				if (BssIdx >= pAd->ApCfg.BssidNum)
				{
					DBGPRINT(RT_DEBUG_ERROR, ("RTMPAddWcidAttributeEntry: The BSS-index(%d) is out of range for MBSSID link. \n", BssIdx));	
					return;
				}
			}

			// choose wcid number
			if (pEntry)
				Wcid = pEntry->Aid;
			else
				GET_GroupKey_WCID(Wcid, BssIdx);		

			if (BssIdx < pAd->ApCfg.BssidNum)
				IEEE8021X = pAd->ApCfg.MBSSID[BssIdx].IEEE8021X;
		}
#endif // CONFIG_AP_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			if (BssIdx > BSS0)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("RTMPAddWcidAttributeEntry: The BSS-index(%d) is out of range for Infra link. \n", BssIdx));	
				return;
			}

			// 1.	In ADHOC mode, the AID is wcid number. And NO mesh link exists.
			// 2.	In Infra mode, the AID:1 MUST be wcid of infra STA. 
			//					   the AID:2~ assign to mesh link entry. 	
			if (pEntry)
				Wcid = pEntry->Aid;
			else
				Wcid = MCAST_WCID;
		}
#endif // CONFIG_STA_SUPPORT //
	}

	// Update WCID attribute table
	offset = MAC_WCID_ATTRIBUTE_BASE + (Wcid * HW_WCID_ATTRI_SIZE);
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		// 1.	Wds-links and Mesh-links always use Pair-wise key table. 	
		// 2. 	When the CipherAlg is TKIP, AES or the dynamic WEP is enabled, 
		// 		it needs to set key into Pair-wise Key Table.
		// 3.	The pair-wise key security mode is set NONE, it means as no security.
		if (pEntry && (IS_ENTRY_WDS(pEntry) || IS_ENTRY_MESH(pEntry)))
			WCIDAttri = (BssIdx<<4) | (CipherAlg<<1) | PAIRWISEKEYTABLE;
		else if ((pEntry) && 
				((CipherAlg == CIPHER_TKIP) || 
				 (CipherAlg == CIPHER_AES) || 
				 (CipherAlg == CIPHER_NONE) || 
				 (IEEE8021X == TRUE)))
			WCIDAttri = (BssIdx<<4) | (CipherAlg<<1) | PAIRWISEKEYTABLE;
		else
			WCIDAttri = (BssIdx<<4) | (CipherAlg<<1) | SHAREDKEYTABLE;
	}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pEntry && IS_ENTRY_MESH(pEntry))
			WCIDAttri = (CipherAlg<<1) | PAIRWISEKEYTABLE;
#if defined(DOT11Z_TDLS_SUPPORT) || defined(QOS_DLS_SUPPORT)
		else if ((pEntry) && (IS_ENTRY_DLS(pEntry) || IS_ENTRY_TDLS(pEntry)) &&
					((CipherAlg == CIPHER_TKIP) || 
					(CipherAlg == CIPHER_AES) || 
				 	(CipherAlg == CIPHER_NONE))) 
			WCIDAttri = (CipherAlg<<1) | PAIRWISEKEYTABLE;
#endif
		else
		WCIDAttri = (CipherAlg<<1) | SHAREDKEYTABLE;
	}
#endif // CONFIG_STA_SUPPORT //
		
	RTMP_IO_WRITE32(pAd, offset, WCIDAttri);

		
	// Update IV/EIV table
	offset = MAC_IVEIV_TABLE_BASE + (Wcid * HW_IVEIV_ENTRY_SIZE);

	// WPA mode
	if ((CipherAlg == CIPHER_TKIP) || (CipherAlg == CIPHER_AES))
	{	
		// Eiv bit on. keyid always is 0 for pairwise key 			
		IVEIV = (KeyIdx <<6) | 0x20;	
	}	 
	else
	{
		// WEP KeyIdx is default tx key. 
		IVEIV = (KeyIdx << 6);	
	}

	// For key index and ext IV bit, so only need to update the position(offset+3).
#ifdef RTMP_MAC_PCI	
	RTMP_IO_WRITE8(pAd, offset+3, IVEIV);
#endif // RTMP_MAC_PCI //
	
	DBGPRINT(RT_DEBUG_TRACE,("RTMPAddWcidAttributeEntry: WCID #%d, KeyIndex #%d, Alg=%s\n",Wcid, KeyIdx, CipherName[CipherAlg]));
	DBGPRINT(RT_DEBUG_TRACE,("	WCIDAttri = 0x%x \n",  WCIDAttri));	

}

/* 
    ==========================================================================
    Description:
        Parse encryption type
Arguments:
    pAdapter                    Pointer to our adapter
    wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
    ==========================================================================
*/
PSTRING GetEncryptType(CHAR enc)
{
    if(enc == Ndis802_11WEPDisabled)
        return "NONE";
    if(enc == Ndis802_11WEPEnabled)
    	return "WEP";
    if(enc == Ndis802_11Encryption2Enabled)
    	return "TKIP";
    if(enc == Ndis802_11Encryption3Enabled)
    	return "AES";
	if(enc == Ndis802_11Encryption4Enabled)
    	return "TKIPAES";
    else
    	return "UNKNOW";
}

PSTRING GetAuthMode(CHAR auth)
{
    if(auth == Ndis802_11AuthModeOpen)
    	return "OPEN";
    if(auth == Ndis802_11AuthModeShared)
    	return "SHARED";
	if(auth == Ndis802_11AuthModeAutoSwitch)
    	return "AUTOWEP";
    if(auth == Ndis802_11AuthModeWPA)
    	return "WPA";
    if(auth == Ndis802_11AuthModeWPAPSK)
    	return "WPAPSK";
    if(auth == Ndis802_11AuthModeWPANone)
    	return "WPANONE";
    if(auth == Ndis802_11AuthModeWPA2)
    	return "WPA2";
    if(auth == Ndis802_11AuthModeWPA2PSK)
    	return "WPA2PSK";
	if(auth == Ndis802_11AuthModeWPA1WPA2)
    	return "WPA1WPA2";
	if(auth == Ndis802_11AuthModeWPA1PSKWPA2PSK)
    	return "WPA1PSKWPA2PSK";
	
    	return "UNKNOW";
}

/* 
    ==========================================================================
    Description:
        Get site survey results
	Arguments:
	    pAdapter                    Pointer to our adapter
	    wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
        Usage:
        		1.) UI needs to wait 4 seconds after issue a site survey command
        		2.) iwpriv ra0 get_site_survey
        		3.) UI needs to prepare at least 4096bytes to get the results
    ==========================================================================
*/
//#define	LINE_LEN	(4+33+20+23+9+7+7+3)	// Channel+SSID+Bssid+Security+Signal+WiressMode+ExtCh+NetworkType
#define	LINE_LEN	(3+33+18+9+16+9+8+3+3)	// Channel+SSID+Bssid+WepStatus+AuthMode+Signal+WiressMode+NetworkType+ExtCh
#ifdef CONFIG_STA_SUPPORT
#ifdef WSC_STA_SUPPORT
#define	WPS_LINE_LEN	(4+5)	// WPS+DPID
#endif // WSC_STA_SUPPORT //
#endif // CONFIG_STA_SUPPORT //

VOID	RTMPCommSiteSurveyData(
	IN  PSTRING		msg,
	IN  PBSS_ENTRY	pBss)
{
	INT         Rssi = 0;
	UINT        Rssi_Quality = 0;
	NDIS_802_11_NETWORK_TYPE    wireless_mode;
	CHAR		Ssid[MAX_LEN_OF_SSID +1];
	STRING		SecurityStr[32] = {0};

	NDIS_802_11_ENCRYPTION_STATUS	ap_cipher = Ndis802_11EncryptionDisabled;
	NDIS_802_11_AUTHENTICATION_MODE	ap_auth_mode = Ndis802_11AuthModeOpen;

	memset(Ssid, 0 ,(MAX_LEN_OF_SSID +1));
		//Channel
		sprintf(msg+strlen(msg),"%-3d", pBss->Channel);
		//SSID
		memcpy(Ssid, pBss->Ssid, pBss->SsidLen);
		Ssid[pBss->SsidLen] = '\0';
		sprintf(msg+strlen(msg),"%-33s", Ssid);      
		//BSSID
		sprintf(msg+strlen(msg),"%02X:%02X:%02X:%02X:%02X:%02X ",
			pBss->Bssid[0], 
			pBss->Bssid[1],
			pBss->Bssid[2], 
			pBss->Bssid[3], 
			pBss->Bssid[4], 
			pBss->Bssid[5]);
	//Security
	if ((Ndis802_11AuthModeWPA <= pBss->AuthMode) &&
		(pBss->AuthMode <= Ndis802_11AuthModeWPA1PSKWPA2PSK))
	{
		if (pBss->AuthMode == Ndis802_11AuthModeWPANone)
		{
			ap_auth_mode = pBss->AuthMode;
				ap_cipher = pBss->WPA.PairCipher;
		}
		else if (pBss->AuthModeAux == Ndis802_11AuthModeOpen)
		{
			ap_auth_mode = pBss->AuthMode;
			if ((ap_auth_mode == Ndis802_11AuthModeWPA) || 
				(ap_auth_mode == Ndis802_11AuthModeWPAPSK))
			{
				if (pBss->WPA.PairCipherAux == Ndis802_11WEPDisabled)
					ap_cipher = pBss->WPA.PairCipher;
				else 
					ap_cipher = Ndis802_11Encryption4Enabled;
			}
			else if ((ap_auth_mode == Ndis802_11AuthModeWPA2) || 
					 (ap_auth_mode == Ndis802_11AuthModeWPA2PSK))
			{
				if (pBss->WPA2.PairCipherAux == Ndis802_11WEPDisabled)
					ap_cipher = pBss->WPA2.PairCipher;
				else 
					ap_cipher = Ndis802_11Encryption4Enabled;
			}
		}
		else if ((pBss->AuthMode == Ndis802_11AuthModeWPAPSK) || 
				 (pBss->AuthMode == Ndis802_11AuthModeWPA2PSK))
		{
			if ((pBss->AuthModeAux == Ndis802_11AuthModeWPAPSK) ||
				(pBss->AuthModeAux == Ndis802_11AuthModeWPA2PSK))
				ap_auth_mode = Ndis802_11AuthModeWPA1PSKWPA2PSK;
			else
				ap_auth_mode = pBss->AuthMode;
			
			if (pBss->WPA.PairCipher != pBss->WPA2.PairCipher)
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux != pBss->WPA2.PairCipherAux))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux != Ndis802_11WEPDisabled))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux == Ndis802_11WEPDisabled))
				ap_cipher = pBss->WPA.PairCipher;
		}
		else if ((pBss->AuthMode == Ndis802_11AuthModeWPA) || 
				 (pBss->AuthMode == Ndis802_11AuthModeWPA2))
		{
			if ((pBss->AuthModeAux == Ndis802_11AuthModeWPA) ||
				(pBss->AuthMode == Ndis802_11AuthModeWPA2))
				ap_auth_mode = Ndis802_11AuthModeWPA1WPA2;
			else
				ap_auth_mode = pBss->AuthMode;
			
			if (pBss->WPA.PairCipher != pBss->WPA2.PairCipher)
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux != pBss->WPA2.PairCipherAux))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux != Ndis802_11WEPDisabled))
				ap_cipher = Ndis802_11Encryption4Enabled;
			else if ((pBss->WPA.PairCipher == pBss->WPA2.PairCipher) &&
					 (pBss->WPA.PairCipherAux == pBss->WPA2.PairCipherAux) &&
					 (pBss->WPA.PairCipherAux == Ndis802_11WEPDisabled))
				ap_cipher = pBss->WPA.PairCipher;
		}
		sprintf(SecurityStr+strlen(SecurityStr),"%-9s", GetEncryptType((CHAR)ap_cipher));
		sprintf(SecurityStr+strlen(SecurityStr),"%-16s", GetAuthMode((CHAR)ap_auth_mode));
	}			
	else
	{
		ap_auth_mode = pBss->AuthMode;
		ap_cipher = pBss->WepStatus;		
		if (ap_cipher == Ndis802_11WEPDisabled)
			sprintf(SecurityStr, "NONE     Open System");
		else if (ap_cipher == Ndis802_11WEPEnabled)
			sprintf(SecurityStr, "WEP      Unknown");
		else
		{
			sprintf(SecurityStr+strlen(SecurityStr),"%-9s", GetEncryptType((CHAR)ap_cipher));
			sprintf(SecurityStr+strlen(SecurityStr),"%-16s", GetAuthMode((CHAR)ap_auth_mode));
		}
	}
	sprintf(msg+strlen(msg), "%-25s", SecurityStr);
		// Rssi
		Rssi = (INT)pBss->Rssi;
		if (Rssi >= -50)
			Rssi_Quality = 100;
		else if (Rssi >= -80)    // between -50 ~ -80dbm
			Rssi_Quality = (UINT)(24 + ((Rssi + 80) * 26)/10);
		else if (Rssi >= -90)   // between -80 ~ -90dbm
			Rssi_Quality = (UINT)(((Rssi + 90) * 26)/10);
		else    // < -84 dbm
			Rssi_Quality = 0;
		sprintf(msg+strlen(msg),"%-9d", Rssi_Quality);
		// Wireless Mode
		wireless_mode = NetworkTypeInUseSanity(pBss);
		if (wireless_mode == Ndis802_11FH ||
			wireless_mode == Ndis802_11DS)
			sprintf(msg+strlen(msg),"%-8s", "11b");
		else if (wireless_mode == Ndis802_11OFDM5)
			sprintf(msg+strlen(msg),"%-8s", "11a");
		else if (wireless_mode == Ndis802_11OFDM5_N)
			sprintf(msg+strlen(msg),"%-8s", "11a/n");
		else if (wireless_mode == Ndis802_11OFDM24)
			sprintf(msg+strlen(msg),"%-8s", "11b/g");
		else if (wireless_mode == Ndis802_11OFDM24_N)
			sprintf(msg+strlen(msg),"%-8s", "11b/g/n");
		else
			sprintf(msg+strlen(msg),"%-8s", "unknown");
		// Ext Channel						// use Central Channel instead
		if (pBss->AddHtInfoLen > 0)
		{
			if (pBss->AddHtInfo.AddHtInfo.ExtChanOffset == EXTCHA_ABOVE)
				sprintf(msg+strlen(msg),"%-7s", " ABOVE");
			else if (pBss->AddHtInfo.AddHtInfo.ExtChanOffset == EXTCHA_BELOW)
				sprintf(msg+strlen(msg),"%-7s", " BELOW");
			else
				sprintf(msg+strlen(msg),"%-7s", " NONE");
		}
		else
		{
			sprintf(msg+strlen(msg),"%-7s", " NONE");
		}
		//Network Type		
		if (pBss->BssType == BSS_ADHOC)
			sprintf(msg+strlen(msg),"%-3s", "Ad ");
		else
			sprintf(msg+strlen(msg),"%-3s", "In ");
		//Central Channel
		sprintf(msg+strlen(msg),"%-2d", pBss->CentralChannel);
        sprintf(msg+strlen(msg),"\n");
	
	return;
}

VOID RTMPIoctlGetSiteSurvey(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq)
{
	PSTRING		msg;
	INT 		i=0;	 
	INT			WaitCnt;
	INT 		Status=0;	
	INT         	max_len = LINE_LEN;		
	PBSS_ENTRY	pBss;

#ifdef CONFIG_STA_SUPPORT
#ifdef WSC_STA_SUPPORT
	max_len += WPS_LINE_LEN;
#endif // WSC_STA_SUPPORT //
#endif // CONFIG_STA_SUPPORT //

	os_alloc_mem(NULL, (PUCHAR *)&msg, sizeof(CHAR)*((MAX_LEN_OF_BSS_TABLE)*max_len));

	if (msg == NULL)
	{   
		DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlGetSiteSurvey - msg memory alloc fail.\n"));
		return;
	}

	memset(msg, 0 ,(MAX_LEN_OF_BSS_TABLE)*max_len );
	sprintf(msg,"%s","\n");
	sprintf(msg+strlen(msg),"%-4s%-33s%-20s%-23s%-9s%-7s%-7s%-3s\n",
	    "Ch", "SSID", "BSSID", "Security", "Siganl(%)", "W-Mode", " ExtCH"," NT");
	
#ifdef CONFIG_STA_SUPPORT
#ifdef WSC_STA_SUPPORT
	sprintf(msg+strlen(msg)-1,"%-4s%-5s\n", " WPS", " DPID");
#endif // WSC_STA_SUPPORT //

#endif // CONFIG_STA_SUPPORT //

	WaitCnt = 0;
#ifdef CONFIG_STA_SUPPORT
	pAdapter->StaCfg.bScanReqIsFromWebUI = TRUE;
	while ((ScanRunning(pAdapter) == TRUE) && (WaitCnt++ < 200))
		OS_WAIT(500);	
#endif // CONFIG_STA_SUPPORT //

	for(i=0; i<pAdapter->ScanTab.BssNr ;i++)
	{
		pBss = &pAdapter->ScanTab.BssEntry[i];
		
		if( pBss->Channel==0)
			break;

		if((strlen(msg)+max_len ) >= IW_SCAN_MAX_DATA)
			break;


		RTMPCommSiteSurveyData(msg, pBss);
		
#ifdef CONFIG_STA_SUPPORT
#ifdef WSC_STA_SUPPORT
        //WPS
        if (pBss->bWpsAP)
			sprintf(msg+strlen(msg)-1,"%-4s", " YES");
		else
			sprintf(msg+strlen(msg)-1,"%-4s", "  NO");

		if (pBss->WscDPIDFromWpsAP == DEV_PASS_ID_PIN)
			sprintf(msg+strlen(msg),"%-5s\n", " PIN");
		else if (pBss->WscDPIDFromWpsAP == DEV_PASS_ID_PBC)
			sprintf(msg+strlen(msg),"%-5s\n", " PBC");
		else
			sprintf(msg+strlen(msg),"%-5s\n", " ");
#endif // WSC_STA_SUPPORT //
#endif // CONFIG_STA_SUPPORT //
	}

#ifdef CONFIG_STA_SUPPORT
	pAdapter->StaCfg.bScanReqIsFromWebUI = FALSE;
#endif // CONFIG_STA_SUPPORT //
	wrq->u.data.length = strlen(msg);
	Status = copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);

	DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlGetSiteSurvey - wrq->u.data.length = %d\n", wrq->u.data.length));
	os_free_mem(NULL, (PUCHAR)msg);	
}

#define	MAC_LINE_LEN	(14+4+4+10+10+10+6+6)	// Addr+aid+psm+datatime+rxbyte+txbyte+current tx rate+last tx rate

/* +++ added by Red@Ralink, 2009/09/30 */
VOID RTMPIoctlGetMacTableStaInfo(
	IN PRTMP_ADAPTER pAd, 
	IN struct iwreq *wrq)
{
	INT i;
	RT_802_11_MAC_TABLE MacTab;
	RT_802_11_MAC_TABLE *p;

	MacTab.Num = 0;
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		if (IS_ENTRY_CLIENT(&pAd->MacTab.Content[i]) && (pAd->MacTab.Content[i].Sst == SST_ASSOC))
		{
			COPY_MAC_ADDR(MacTab.Entry[MacTab.Num].Addr, &pAd->MacTab.Content[i].Addr);
			MacTab.Entry[MacTab.Num].Aid = (UCHAR)pAd->MacTab.Content[i].Aid;
			MacTab.Entry[MacTab.Num].Psm = pAd->MacTab.Content[i].PsMode;
#ifdef DOT11_N_SUPPORT
			MacTab.Entry[MacTab.Num].MimoPs = pAd->MacTab.Content[i].MmpsMode;
#endif // DOT11_N_SUPPORT //

			// Fill in RSSI per entry
			MacTab.Entry[MacTab.Num].AvgRssi0 = pAd->MacTab.Content[i].RssiSample.AvgRssi0;
			MacTab.Entry[MacTab.Num].AvgRssi1 = pAd->MacTab.Content[i].RssiSample.AvgRssi1;
			MacTab.Entry[MacTab.Num].AvgRssi2 = pAd->MacTab.Content[i].RssiSample.AvgRssi2;

			// the connected time per entry
			MacTab.Entry[MacTab.Num].ConnectedTime = pAd->MacTab.Content[i].StaConnectTime;
			MacTab.Entry[MacTab.Num].TxRate.field.MCS = pAd->MacTab.Content[i].HTPhyMode.field.MCS;
			MacTab.Entry[MacTab.Num].TxRate.field.BW = pAd->MacTab.Content[i].HTPhyMode.field.BW;
			MacTab.Entry[MacTab.Num].TxRate.field.ShortGI = pAd->MacTab.Content[i].HTPhyMode.field.ShortGI;
			MacTab.Entry[MacTab.Num].TxRate.field.STBC = pAd->MacTab.Content[i].HTPhyMode.field.STBC;
			MacTab.Entry[MacTab.Num].TxRate.field.rsv = pAd->MacTab.Content[i].HTPhyMode.field.rsv;
			MacTab.Entry[MacTab.Num].TxRate.field.MODE = pAd->MacTab.Content[i].HTPhyMode.field.MODE;
			MacTab.Entry[MacTab.Num].TxRate.word = pAd->MacTab.Content[i].HTPhyMode.word;
									
			MacTab.Num += 1;
		}
	}

	p = wrq->u.data.pointer;
	wrq->u.data.length = sizeof(RT_802_11_MAC_TABLE);
	if (copy_to_user(wrq->u.data.pointer, &MacTab, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: copy_to_user() fail\n", __FUNCTION__));
	}
}
/* +++ end of addition */

#ifdef RALINK_ATE
VOID RTMPIoctlGetATESHOW(
	IN	PRTMP_ADAPTER	pAd, 
	IN	struct iwreq	*wrq)
{
	CHAR		*msg;
	INT 		Status=0;

	os_alloc_mem(NULL, (PUCHAR *)&msg, sizeof(CHAR)*(1024));

	if (msg == NULL)
    {   
        DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlGetATESHOW - msg memory alloc fail.\n"));
		return;
    }
    
	memset(msg, 0, 1024 );

	sprintf(msg+strlen(msg), "Mode=%u\n", pAd->ate.Mode);
#ifdef CONFIG_RALINK_RT3350
	sprintf(msg+strlen(msg), "PABias=%u\n", pAd->ate.PABias);//peter
#endif // CONFIG_RALINK_RT3350 //
	sprintf(msg+strlen(msg), "TxPower0=%d\n", pAd->ate.TxPower0);
	sprintf(msg+strlen(msg), "TxPower1=%d\n", pAd->ate.TxPower1);
#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	sprintf(msg+strlen(msg), "TxPower2=%d\n", pAd->ate.TxPower2);
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //
	sprintf(msg+strlen(msg), "TxAntennaSel=%d\n", pAd->ate.TxAntennaSel);
	sprintf(msg+strlen(msg), "RxAntennaSel=%d\n", pAd->ate.RxAntennaSel);
	sprintf(msg+strlen(msg), "BBPCurrentBW=%u\n", pAd->ate.TxWI.BW);
	sprintf(msg+strlen(msg), "GI=%u\n", pAd->ate.TxWI.ShortGI);
	sprintf(msg+strlen(msg), "MCS=%u\n", pAd->ate.TxWI.MCS);
	sprintf(msg+strlen(msg), "TxMode=%u\n", pAd->ate.TxWI.PHYMODE);
	sprintf(msg+strlen(msg), "Addr1=%02x:%02x:%02x:%02x:%02x:%02x\n",
		pAd->ate.Addr1[0], pAd->ate.Addr1[1], pAd->ate.Addr1[2], pAd->ate.Addr1[3], pAd->ate.Addr1[4], pAd->ate.Addr1[5]);
	sprintf(msg+strlen(msg), "Addr2=%02x:%02x:%02x:%02x:%02x:%02x\n",
		pAd->ate.Addr2[0], pAd->ate.Addr2[1], pAd->ate.Addr2[2], pAd->ate.Addr2[3], pAd->ate.Addr2[4], pAd->ate.Addr2[5]);
	sprintf(msg+strlen(msg), "Addr3=%02x:%02x:%02x:%02x:%02x:%02x\n",
		pAd->ate.Addr3[0], pAd->ate.Addr3[1], pAd->ate.Addr3[2], pAd->ate.Addr3[3], pAd->ate.Addr3[4], pAd->ate.Addr3[5]);
	sprintf(msg+strlen(msg), "Channel=%u\n", pAd->ate.Channel);
	sprintf(msg+strlen(msg), "TxLength=%u\n", pAd->ate.TxLength);
	sprintf(msg+strlen(msg), "TxCount=%u\n", pAd->ate.TxCount);
	sprintf(msg+strlen(msg), "RFFreqOffset=%u\n", pAd->ate.RFFreqOffset);
	sprintf(msg+strlen(msg), "IPG=%u\n", pAd->ate.IPG);//peter
	sprintf(msg+strlen(msg), "Payload=0x%02x\n", pAd->ate.Payload);//peter
	sprintf(msg+strlen(msg), "Set_ATE_Show_Proc Success\n");

	wrq->u.data.length = strlen(msg);
	Status = copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);

	DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlGetATESHOW - wrq->u.data.length = %d\n", wrq->u.data.length));
	os_free_mem(NULL, (PUCHAR)msg);
}

VOID RTMPIoctlGetATEHELP(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq)
{
	CHAR		*msg;
	INT 		Status=0;

	os_alloc_mem(NULL, (PUCHAR *)&msg, sizeof(CHAR)*(1024));

	if (msg == NULL)
    {   
        DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlGetATEHELP - msg memory alloc fail.\n"));
		return;
    }
    
	memset(msg, 0, 1536 );

#ifdef	CONFIG_RT2880_ATE_CMD_NEW
	sprintf(msg+strlen(msg), "ATE=ATESTART, ATESTOP, TXCONT, TXCARR, TXFRAME, RXFRAME\n");
#else
	sprintf(msg+strlen(msg), "ATE=APSTOP, APSTART, TXCONT, TXCARR, TXFRAME, RXFRAME\n");
#endif
	sprintf(msg+strlen(msg), "ATEDA\n");
	sprintf(msg+strlen(msg), "ATESA\n");
	sprintf(msg+strlen(msg), "ATEBSSID\n");
	sprintf(msg+strlen(msg), "ATECHANNEL, range:0~14(unless A band !)\n");
	sprintf(msg+strlen(msg), "ATETXPOW0, set power level of antenna 1.\n");
	sprintf(msg+strlen(msg), "ATETXPOW1, set power level of antenna 2.\n");
#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	if (IS_RT2883(pAd) || IS_RT3883(pAd))
	{
		sprintf(msg+strlen(msg), "ATETXPOW2, set power level of antenna 3.\n");
		sprintf(msg+strlen(msg), "ATETXANT, set TX antenna. 0:all, 1:antenna one, 2:antenna two, 3:antenna three.\n");
	}
	else
	{
		sprintf(msg+strlen(msg), "ATETXANT, set TX antenna. 0:all, 1:antenna one, 2:antenna two.\n");
	}
#else
	sprintf(msg+strlen(msg), "ATETXANT, set TX antenna. 0:all, 1:antenna one, 2:antenna two.\n");
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //
	sprintf(msg+strlen(msg), "ATERXANT, set RX antenna.0:all, 1:antenna one, 2:antenna two, 3:antenna three.\n");

#ifdef CONFIG_RALINK_RT3350
	sprintf(msg+strlen(msg), "ATEPABIAS, set power amplifier bias for EVM, range 0~15\n");//peter
#endif // CONFIG_RALINK_RT3350 //
#ifdef RTMP_RF_RW_SUPPORT
	sprintf(msg+strlen(msg), "ATETXFREQOFFSET, set frequency offset, range 0~95\n");
#else
	sprintf(msg+strlen(msg), "ATETXFREQOFFSET, set frequency offset, range 0~63\n");
#endif // RTMP_RF_RW_SUPPORT //
	sprintf(msg+strlen(msg), "ATETXBW, set BandWidth, 0:20MHz, 1:40MHz.\n");
	sprintf(msg+strlen(msg), "ATETXLEN, set Frame length, range 24~%d\n", (MAX_FRAME_SIZE - 34/* == 2312 */));
	sprintf(msg+strlen(msg), "ATETXCNT, set how many frame going to transmit.\n");
	sprintf(msg+strlen(msg), "ATETXMCS, set MCS, reference to rate table.\n");
	sprintf(msg+strlen(msg), "ATETXMODE, set Mode 0:CCK, 1:OFDM, 2:HT-Mix, 3:GreenField, reference to rate table.\n");
	sprintf(msg+strlen(msg), "ATETXGI, set GI interval, 0:Long, 1:Short\n");
	sprintf(msg+strlen(msg), "ATERXFER, 0:disable Rx Frame error rate. 1:enable Rx Frame error rate.\n");
	sprintf(msg+strlen(msg), "ATERRF, show all RF registers.\n");
#ifndef RTMP_RF_RW_SUPPORT
	sprintf(msg+strlen(msg), "ATEWRF1, set RF1 register.\n");
	sprintf(msg+strlen(msg), "ATEWRF2, set RF2 register.\n");
	sprintf(msg+strlen(msg), "ATEWRF3, set RF3 register.\n");
	sprintf(msg+strlen(msg), "ATEWRF4, set RF4 register.\n");
#endif // !RTMP_RF_RW_SUPPORT //
	sprintf(msg+strlen(msg), "ATELDE2P, load EEPROM from .bin file.\n");
	sprintf(msg+strlen(msg), "ATERE2P, display all EEPROM content.\n");
	sprintf(msg+strlen(msg), "ATEIPG, set ATE Tx frame IPG.\n");//peter
	sprintf(msg+strlen(msg), "ATEPAYLOAD, set ATE payload pattern for TxFrame.\n");//peter
	sprintf(msg+strlen(msg), "ATESHOW, display all parameters of ATE.\n");
	sprintf(msg+strlen(msg), "ATEHELP, online help.\n");

	wrq->u.data.length = strlen(msg);
	Status = copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);

	DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlGetATEHELP - wrq->u.data.length = %d\n", wrq->u.data.length));
	os_free_mem(NULL, (PUCHAR)msg);
}
#endif

VOID RTMPIoctlGetMacTable(
	IN PRTMP_ADAPTER pAd, 
	IN struct iwreq *wrq)
{
	INT i;
	RT_802_11_MAC_TABLE MacTab;
	char *msg;

	MacTab.Num = 0;
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		if (IS_ENTRY_CLIENT(&pAd->MacTab.Content[i]) && (pAd->MacTab.Content[i].Sst == SST_ASSOC))
		{
			PRT_802_11_MAC_ENTRY pDst;
			MAC_TABLE_ENTRY *pEntry;
			pDst = &(MacTab.Entry[MacTab.Num]);
			pEntry = &(pAd->MacTab.Content[i]);

			COPY_MAC_ADDR(pDst->Addr, &pAd->MacTab.Content[i].Addr);
			pDst->Aid = (UCHAR)pEntry->Aid;
			pDst->Psm = pAd->MacTab.Content[i].PsMode;
#ifdef DOT11_N_SUPPORT
			pDst->MimoPs = pEntry->MmpsMode;
#endif // DOT11_N_SUPPORT //

			// Fill in RSSI per entry
			pDst->AvgRssi0 = pEntry->RssiSample.AvgRssi0;
			pDst->AvgRssi1 = pEntry->RssiSample.AvgRssi1;
			pDst->AvgRssi2 = pEntry->RssiSample.AvgRssi2;

			// the connected time per entry
			pDst->ConnectedTime = pEntry->StaConnectTime;
#if 0 // ToDo
			pDst->HSCounter.LastDataPacketTime = pEntry->HSCounter.LastDataPacketTime;
			pDst->HSCounter.TotalRxByteCount = pEntry->HSCounter.TotalRxByteCount;
			pDst->HSCounter.TotalTxByteCount = pEntry->HSCounter.TotalTxByteCount;
#endif
			pDst->TxRate.field.MCS = pEntry->HTPhyMode.field.MCS;
			pDst->TxRate.field.BW = pEntry->HTPhyMode.field.BW;
			pDst->TxRate.field.ShortGI = pEntry->HTPhyMode.field.ShortGI;
			pDst->TxRate.field.STBC = pEntry->HTPhyMode.field.STBC;
			pDst->TxRate.field.rsv = pEntry->HTPhyMode.field.rsv;
			pDst->TxRate.field.MODE = pEntry->HTPhyMode.field.MODE;
			pDst->TxRate.word = pEntry->HTPhyMode.word;

			pDst->LastRxRate = pEntry->LastRxRate;
#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
			pDst->StreamSnr[0] = pEntry->BF_SNR[0];
			pDst->StreamSnr[1] = pEntry->BF_SNR[1];
			pDst->StreamSnr[2] = pEntry->BF_SNR[2];
#ifdef TXBF_SUPPORT
			pDst->SoundingRespSnr[0] = pEntry->sndg0Snr0;
			pDst->SoundingRespSnr[1] = pEntry->sndg0Snr1;
			pDst->SoundingRespSnr[2] = pEntry->sndg0Snr2;
#endif // TXBF_SUPPORT //
#endif // defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883) //

			MacTab.Num += 1;
		}
	}

	msg = kmalloc(sizeof(CHAR)*(MAX_LEN_OF_MAC_TABLE*MAC_LINE_LEN), MEM_ALLOC_FLAG);
	if (msg == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Alloc memory failed\n", __FUNCTION__));
		return;
	}
	memset(msg, 0 ,MAX_LEN_OF_MAC_TABLE*MAC_LINE_LEN );
	sprintf(msg,"%s","\n");
	sprintf(msg+strlen(msg),"%-14s%-4s%-4s%-4s%-4s%-6s%-6s%-10s%-10s%-10s\n",
		"MAC", "AP",  "AID", "PSM", "AUTH", "CTxR", "LTxR","LDT", "RxB", "TxB");
	
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];
		if (IS_ENTRY_CLIENT(pEntry) && (pEntry->Sst == SST_ASSOC))
		{
			if((strlen(msg)+MAC_LINE_LEN ) >= (MAX_LEN_OF_MAC_TABLE*MAC_LINE_LEN) )
				break;	
			sprintf(msg+strlen(msg),"%02x%02x%02x%02x%02x%02x  ",
				pEntry->Addr[0], pEntry->Addr[1], pEntry->Addr[2],
				pEntry->Addr[3], pEntry->Addr[4], pEntry->Addr[5]);
			sprintf(msg+strlen(msg),"%-4d", (int)pEntry->apidx);
			sprintf(msg+strlen(msg),"%-4d", (int)pEntry->Aid);
			sprintf(msg+strlen(msg),"%-4d", (int)pEntry->PsMode);
			sprintf(msg+strlen(msg),"%-4d", (int)pEntry->AuthState);
			sprintf(msg+strlen(msg),"%-6d",RateIdToMbps[pAd->MacTab.Content[i].CurrTxRate]);
			sprintf(msg+strlen(msg),"%-6d",0/*RateIdToMbps[pAd->MacTab.Content[i].HTPhyMode.word]*/); // ToDo
			sprintf(msg+strlen(msg),"%-10d",0/*pAd->MacTab.Content[i].HSCounter.LastDataPacketTime*/); // ToDo
			sprintf(msg+strlen(msg),"%-10d",0/*pAd->MacTab.Content[i].HSCounter.TotalRxByteCount*/); // ToDo
			sprintf(msg+strlen(msg),"%-10d\n",0/*pAd->MacTab.Content[i].HSCounter.TotalTxByteCount*/); // ToDo

		}
	} 
	// for compatible with old API just do the printk to console
	wrq->u.data.length = strlen(msg);
	if (copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s", msg));
	}
	kfree(msg);
#if defined(TXBF_SUPPORT) && 0
	// Temporary change to display BF statistics
	{
	INT i;

	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++) {
		PMAC_TABLE_ENTRY pEntry = &(pAd->MacTab.Content[i]);

		if (IS_ENTRY_CLIENT(pEntry) && (pEntry->Sst == SST_ASSOC)) {
			COUNTER_TXBF *pCnt = &pEntry->TxBFCounters;
			ULONG totalNBF, totalEBF, totalIBF, totalTx, totalRetry, totalSuccess;

			totalNBF = pCnt->TxSuccessCount + pCnt->TxFailCount;
			totalEBF = pCnt->ETxSuccessCount + pCnt->ETxFailCount;
			totalIBF = pCnt->ITxSuccessCount + pCnt->ITxFailCount;

			totalTx = totalNBF + totalEBF + totalIBF;
			totalRetry = pCnt->TxRetryCount + pCnt->ETxRetryCount + pCnt->ITxRetryCount;
			totalSuccess = pCnt->TxSuccessCount + pCnt->ETxSuccessCount + pCnt->ITxSuccessCount;

			printk("MacTable[%d]     Success    Retry/PER    Fail/PLR\n", i);
			if (totalTx==0) {
				printk("   Total = 0\n");
				continue;
			}

			if (totalNBF!=0) {
				printk("   NonBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)\n",
					100*totalNBF/totalTx, pCnt->TxSuccessCount,
					pCnt->TxRetryCount, 100*pCnt->TxRetryCount/(pCnt->TxSuccessCount+pCnt->TxRetryCount),
					pCnt->TxFailCount, 100*pCnt->TxFailCount/totalNBF);
			}

			if (totalEBF!=0) {
				printk("   ETxBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)\n",
					 100*totalEBF/totalTx, pCnt->ETxSuccessCount,
					pCnt->ETxRetryCount, 100*pCnt->ETxRetryCount/(pCnt->ETxSuccessCount+pCnt->ETxRetryCount),
					pCnt->ETxFailCount, 100*pCnt->ETxFailCount/totalEBF);
			}

			if (totalIBF!=0) {
				printk("   ITxBF (%3lu%%): %7lu  %7lu (%2lu%%) %5lu (%1lu%%)\n",
					100*totalIBF/totalTx, pCnt->ITxSuccessCount,
					pCnt->ITxRetryCount, 100*pCnt->ITxRetryCount/(pCnt->ITxSuccessCount+pCnt->ITxRetryCount),
					pCnt->ITxFailCount, 100*pCnt->ITxFailCount/totalIBF);
			}

			printk("   Total         %7lu  %7lu (%2lu%%) %5lu (%1lu%%)\n",
				totalSuccess, totalRetry, 100*totalRetry/(totalSuccess + totalRetry),
				pCnt->TxFailCount+pCnt->ETxFailCount+pCnt->ITxFailCount,
				100*(pCnt->TxFailCount+pCnt->ETxFailCount+pCnt->ITxFailCount)/totalTx);
		}
	}
	}
#endif
}

#ifdef CONFIG_AP_SUPPORT
VOID RTMPIoctlGetSTAT2(
	IN PRTMP_ADAPTER pAd, 
	IN struct iwreq *wrq)
{
	char *msg;
	PMULTISSID_STRUCT	pMbss;
	INT apidx;


	msg = kzalloc(sizeof(CHAR)*(pAd->ApCfg.BssidNum*(14*128)), MEM_ALLOC_FLAG);
	if (msg == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Alloc memory failed\n", __FUNCTION__));
		return;
	}
	sprintf(msg,"%s","\n");
	
	for (apidx=0; apidx<pAd->ApCfg.BssidNum; apidx++)
	{
		pMbss=&pAd->ApCfg.MBSSID[apidx];
		
		sprintf(msg+strlen(msg),"ra%d\n",apidx);
		sprintf(msg+strlen(msg),"bytesTx = %ld\n",(pMbss->TransmittedByteCount));
		sprintf(msg+strlen(msg),"bytesRx = %ld\n",(pMbss->ReceivedByteCount));
		sprintf(msg+strlen(msg),"pktsTx = %ld\n",pMbss->TxCount);
		sprintf(msg+strlen(msg),"pktsRx = %ld\n",pMbss->RxCount);
		sprintf(msg+strlen(msg),"errorsTx = %ld\n",pMbss->TxErrorCount);
		sprintf(msg+strlen(msg),"errorsRx = %ld\n",pMbss->RxErrorCount);
		sprintf(msg+strlen(msg),"discardPktsTx = %ld\n",pMbss->TxDropCount);
		sprintf(msg+strlen(msg),"discardPktsRx = %ld\n",pMbss->RxDropCount);
		sprintf(msg+strlen(msg),"ucPktsTx = %ld\n",pMbss->ucPktsTx);
		sprintf(msg+strlen(msg),"ucPktsRx = %ld\n",pMbss->ucPktsRx);
		sprintf(msg+strlen(msg),"mcPktsTx = %ld\n",pMbss->mcPktsTx);
		sprintf(msg+strlen(msg),"mcPktsRx = %ld\n",pMbss->mcPktsRx);
		sprintf(msg+strlen(msg),"bcPktsTx = %ld\n",pMbss->bcPktsTx);
		sprintf(msg+strlen(msg),"bcPktsRx = %ld\n",pMbss->bcPktsRx);
		
	}

	wrq->u.data.length = strlen(msg);
	if (copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s", msg));
	}

	kfree(msg);
}


VOID RTMPIoctlGetRadioDynInfo(
	IN PRTMP_ADAPTER pAd, 
	IN struct iwreq *wrq)
{
	char *msg;
	PMULTISSID_STRUCT	pMbss;
	INT status,bandwidth,ShortGI;
	

	msg = kzalloc(sizeof(CHAR)*(4096), MEM_ALLOC_FLAG);
	if (msg == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Alloc memory failed\n", __FUNCTION__));
		return;
	}
	sprintf(msg,"%s","\n");
	

		pMbss=&pAd->ApCfg.MBSSID[0];
		if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))
			status = 0;
		else
			status = 1;

		if(pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40)
			bandwidth = 1;
		else
			bandwidth = 0;

		if(pAd->CommonCfg.RegTransmitSetting.field.ShortGI == GI_800)
			ShortGI = 1;
		else
			ShortGI = 0;

		
		sprintf(msg+strlen(msg),"status = %d\n",status);
		sprintf(msg+strlen(msg),"channelsInUse = %d\n",pAd->ChannelListNum);
		sprintf(msg+strlen(msg),"channel = %d\n",pAd->CommonCfg.Channel);
		sprintf(msg+strlen(msg),"chanWidth = %d\n",bandwidth);
		sprintf(msg+strlen(msg),"guardIntvl = %d\n",ShortGI);
		sprintf(msg+strlen(msg),"MCS = %d\n",pMbss->DesiredTransmitSetting.field.MCS);
		
	wrq->u.data.length = strlen(msg);

	if (copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length))
	{
		DBGPRINT(RT_DEBUG_OFF, ("%s", msg));
	}

	kfree(msg);
}
#endif//CONFIG_AP_SUPPORT//

#ifdef DOT11_N_SUPPORT
INT	Set_BASetup_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
    UCHAR mac[6], tid;
	PSTRING token;
	STRING sepValue[] = ":", DASH = '-';
	INT i;
    MAC_TABLE_ENTRY *pEntry;

/*
	The BASetup inupt string format should be xx:xx:xx:xx:xx:xx-d, 
		=>The six 2 digit hex-decimal number previous are the Mac address, 
		=>The seventh decimal number is the tid value.
*/
	//DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));
	
	if(strlen(arg) < 19)  //Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and tid value in decimal format.
		return FALSE;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		tid = (UCHAR) simple_strtol((token+1), 0, 10);
		if (tid > 15)
			return FALSE;
		
		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return FALSE;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return FALSE;

		DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x\n", 
								mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], tid));

	    pEntry = MacTableLookup(pAd, (PUCHAR) mac);

    	if (pEntry) {
        	DBGPRINT(RT_DEBUG_OFF, ("\nSetup BA Session: Tid = %d\n", tid));
	        BAOriSessionSetUp(pAd, pEntry, tid, 0, 100, TRUE);
    	}

		return TRUE;
	}

	return FALSE;

}

INT	Set_BADecline_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG bBADecline;

	bBADecline = simple_strtol(arg, 0, 10);

	if (bBADecline == 0)
	{
		pAd->CommonCfg.bBADecline = FALSE;
	}
	else if (bBADecline == 1)
	{
		pAd->CommonCfg.bBADecline = TRUE;
	}
	else 
	{
		return FALSE; //Invalid argument
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_BADecline_Proc::(BADecline=%d)\n", pAd->CommonCfg.bBADecline));

	return TRUE;
}

INT	Set_BAOriTearDown_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
    UCHAR mac[6], tid;
	PSTRING token;
	STRING sepValue[] = ":", DASH = '-';
	INT i;
    MAC_TABLE_ENTRY *pEntry;

    //DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));
/*
	The BAOriTearDown inupt string format should be xx:xx:xx:xx:xx:xx-d, 
		=>The six 2 digit hex-decimal number previous are the Mac address, 
		=>The seventh decimal number is the tid value.
*/
    if(strlen(arg) < 19)  //Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and tid value in decimal format.
		return FALSE;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		tid = simple_strtol((token+1), 0, 10);
		if (tid > NUM_OF_TID)
			return FALSE;
		
		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return FALSE;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return FALSE;

	    DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x", 
								mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], tid));

	    pEntry = MacTableLookup(pAd, (PUCHAR) mac);

	    if (pEntry) {
	        DBGPRINT(RT_DEBUG_OFF, ("\nTear down Ori BA Session: Tid = %d\n", tid));
	        BAOriSessionTearDown(pAd, pEntry->Aid, tid, FALSE, TRUE);
	    }

		return TRUE;
	}

	return FALSE;

}

INT	Set_BARecTearDown_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
    UCHAR mac[6], tid;
	PSTRING token;
	STRING sepValue[] = ":", DASH = '-';
	INT i;
    MAC_TABLE_ENTRY *pEntry;

    //DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));
/*
	The BARecTearDown inupt string format should be xx:xx:xx:xx:xx:xx-d, 
		=>The six 2 digit hex-decimal number previous are the Mac address, 
		=>The seventh decimal number is the tid value.
*/
    if(strlen(arg) < 19)  //Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and tid value in decimal format.
		return FALSE;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		tid = simple_strtol((token+1), 0, 10);
		if (tid > NUM_OF_TID)
			return FALSE;
		
		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return FALSE;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return FALSE;

		DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x", 
								mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], tid));

		pEntry = MacTableLookup(pAd, (PUCHAR) mac);

		if (pEntry) {
		    DBGPRINT(RT_DEBUG_OFF, ("\nTear down Rec BA Session: Tid = %d\n", tid));
		    BARecSessionTearDown(pAd, pEntry->Aid, tid, FALSE);
		}

		return TRUE;
	}

	return FALSE;

}

INT	Set_HtBw_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG HtBw;

	HtBw = simple_strtol(arg, 0, 10);
	if (HtBw == BW_40)
		pAd->CommonCfg.RegTransmitSetting.field.BW  = BW_40;
	else if (HtBw == BW_20)
		pAd->CommonCfg.RegTransmitSetting.field.BW  = BW_20;
	else
		return FALSE;  //Invalid argument 

	SetCommonHT(pAd);
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtBw_Proc::(HtBw=%d)\n", pAd->CommonCfg.RegTransmitSetting.field.BW));

	return TRUE;
}

INT	Set_HtMcs_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG HtMcs, Mcs_tmp, ValidMcs = 15;
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
#endif // CONFIG_AP_SUPPORT //	
#ifdef CONFIG_STA_SUPPORT    
    BOOLEAN bAutoRate = FALSE;
#endif // CONFIG_STA_SUPPORT //

#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	ValidMcs = 23;
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //
	Mcs_tmp = simple_strtol(arg, 0, 10);
		
	if (Mcs_tmp <= ValidMcs || Mcs_tmp == 32)			
		HtMcs = Mcs_tmp;	
	else
		HtMcs = MCS_AUTO;	

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		pAd->ApCfg.MBSSID[pObj->ioctl_if].DesiredTransmitSetting.field.MCS = HtMcs;
		DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMcs_Proc::(HtMcs=%d) for ra%d\n", 
				pAd->ApCfg.MBSSID[pObj->ioctl_if].DesiredTransmitSetting.field.MCS, pObj->ioctl_if));
	}
#endif // CONFIG_AP_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		pAd->StaCfg.DesiredTransmitSetting.field.MCS = HtMcs;
		pAd->StaCfg.bAutoTxRateSwitch = (HtMcs == MCS_AUTO) ? TRUE:FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMcs_Proc::(HtMcs=%d, bAutoTxRateSwitch = %d)\n", 
						pAd->StaCfg.DesiredTransmitSetting.field.MCS, pAd->StaCfg.bAutoTxRateSwitch));

		if ((pAd->CommonCfg.PhyMode < PHY_11ABGN_MIXED) ||
			(pAd->MacTab.Content[BSSID_WCID].HTPhyMode.field.MODE < MODE_HTMIX))
		{
	        if ((pAd->StaCfg.DesiredTransmitSetting.field.MCS != MCS_AUTO) &&
				(HtMcs >= 0 && HtMcs <= 3) &&
	            (pAd->StaCfg.DesiredTransmitSetting.field.FixedTxMode == FIXED_TXMODE_CCK))
			{
				RTMPSetDesiredRates(pAd, (LONG) (RateIdToMbps[HtMcs] * 1000000));
			}
	        else if ((pAd->StaCfg.DesiredTransmitSetting.field.MCS != MCS_AUTO) &&
					(HtMcs >= 0 && HtMcs <= 7) &&
	            	(pAd->StaCfg.DesiredTransmitSetting.field.FixedTxMode == FIXED_TXMODE_OFDM))
			{
				RTMPSetDesiredRates(pAd, (LONG) (RateIdToMbps[HtMcs+4] * 1000000));
			}
			else
				bAutoRate = TRUE;

			if (bAutoRate)
			{
	            pAd->StaCfg.DesiredTransmitSetting.field.MCS = MCS_AUTO;
				RTMPSetDesiredRates(pAd, -1);
			}
	        DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMcs_Proc::(FixedTxMode=%d)\n",pAd->StaCfg.DesiredTransmitSetting.field.FixedTxMode));
		}
        if (ADHOC_ON(pAd))
            return TRUE;
	}
#endif // CONFIG_STA_SUPPORT //

	SetCommonHT(pAd);
	
	return TRUE;
}

INT	Set_HtGi_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG HtGi;

	HtGi = simple_strtol(arg, 0, 10);
		
	if ( HtGi == GI_400)			
		pAd->CommonCfg.RegTransmitSetting.field.ShortGI = GI_400;
	else if ( HtGi == GI_800 )
		pAd->CommonCfg.RegTransmitSetting.field.ShortGI = GI_800;
	else 
		return FALSE; //Invalid argument 	

	SetCommonHT(pAd);
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtGi_Proc::(ShortGI=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.ShortGI));

	return TRUE;
}


INT	Set_HtTxBASize_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	UCHAR Size;

	Size = simple_strtol(arg, 0, 10);
		
	if (Size <=0 || Size >=64)
	{
		Size = 8;
	}
	pAd->CommonCfg.TxBASize = Size-1;
	DBGPRINT(RT_DEBUG_ERROR, ("Set_HtTxBASize ::(TxBASize= %d)\n", Size));

	return TRUE;
}

INT	Set_HtDisallowTKIP_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
		
	if (Value == 1)
	{
		pAd->CommonCfg.HT_DisallowTKIP = TRUE;
	}
	else
	{
		pAd->CommonCfg.HT_DisallowTKIP = FALSE;
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtDisallowTKIP_Proc ::%s\n", 
				(pAd->CommonCfg.HT_DisallowTKIP == TRUE) ? "enabled" : "disabled"));

	return TRUE;
}


INT	Set_HtOpMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{

	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);

	if (Value == HTMODE_GF)
		pAd->CommonCfg.RegTransmitSetting.field.HTMODE  = HTMODE_GF;
	else if ( Value == HTMODE_MM )
		pAd->CommonCfg.RegTransmitSetting.field.HTMODE  = HTMODE_MM;
	else 
		return FALSE; //Invalid argument 	

	SetCommonHT(pAd);
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtOpMode_Proc::(HtOpMode=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.HTMODE));

	return TRUE;

}	

INT	Set_HtStbc_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{

	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value == STBC_USE)
		pAd->CommonCfg.RegTransmitSetting.field.STBC = STBC_USE;
	else if ( Value == STBC_NONE )
		pAd->CommonCfg.RegTransmitSetting.field.STBC = STBC_NONE;
	else 
		return FALSE; //Invalid argument 	

	SetCommonHT(pAd);
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_Stbc_Proc::(HtStbc=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.STBC));

	return TRUE;											
}

INT	Set_HtHtc_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{

	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->HTCEnable = FALSE;
	else if ( Value ==1 )
        pAd->HTCEnable = TRUE;
	else 
		return FALSE; //Invalid argument 	
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtHtc_Proc::(HtHtc=%d)\n",pAd->HTCEnable));

	return TRUE;		
}
			
INT	Set_HtExtcha_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{

	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value == 0)			
		pAd->CommonCfg.RegTransmitSetting.field.EXTCHA  = EXTCHA_BELOW;
	else if ( Value ==1 )
        pAd->CommonCfg.RegTransmitSetting.field.EXTCHA = EXTCHA_ABOVE;
	else 
		return FALSE; //Invalid argument 	
	
	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtExtcha_Proc::(HtExtcha=%d)\n",pAd->CommonCfg.RegTransmitSetting.field.EXTCHA));

	return TRUE;			
}

INT	Set_HtMpduDensity_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value <=7 && Value >= 0)
		pAd->CommonCfg.BACapability.field.MpduDensity = Value;
	else
		pAd->CommonCfg.BACapability.field.MpduDensity = 4;

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMpduDensity_Proc::(HtMpduDensity=%d)\n",pAd->CommonCfg.BACapability.field.MpduDensity));

	return TRUE;																																	
}

INT	Set_HtBaWinSize_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);

#ifdef CONFIG_AP_SUPPORT
	// Intel IOT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		Value = 64;
#endif // CONFIG_AP_SUPPORT //

	if (Value >=1 && Value <= 64)
	{
		pAd->CommonCfg.REGBACapability.field.RxBAWinLimit = Value;
		pAd->CommonCfg.BACapability.field.RxBAWinLimit = Value;
	}
	else
	{
        pAd->CommonCfg.REGBACapability.field.RxBAWinLimit = 64;
		pAd->CommonCfg.BACapability.field.RxBAWinLimit = 64;
	}
	
	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtBaWinSize_Proc::(HtBaWinSize=%d)\n",pAd->CommonCfg.BACapability.field.RxBAWinLimit));

	return TRUE;																																	
}		

INT	Set_HtRdg_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value == 0)			
		pAd->CommonCfg.bRdg = FALSE;
	else if ( Value ==1 )
	{
		pAd->HTCEnable = TRUE;
    		pAd->CommonCfg.bRdg = TRUE;
	}
	else 
		return FALSE; //Invalid argument
	
	SetCommonHT(pAd);	
		
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtRdg_Proc::(HtRdg=%d)\n",pAd->CommonCfg.bRdg));

	return TRUE;																																	
}		

INT	Set_HtLinkAdapt_Proc(																																																																																																																																																																																																																																																																																																																			
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->bLinkAdapt = FALSE;
	else if ( Value ==1 )
	{
			pAd->HTCEnable = TRUE;
			pAd->bLinkAdapt = TRUE;
	}
	else
		return FALSE; //Invalid argument
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtLinkAdapt_Proc::(HtLinkAdapt=%d)\n",pAd->bLinkAdapt));

	return TRUE;																																	
}		

INT	Set_HtAmsdu_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->CommonCfg.BACapability.field.AmsduEnable = FALSE;
	else if ( Value == 1 )
        pAd->CommonCfg.BACapability.field.AmsduEnable = TRUE;
	else
		return FALSE; //Invalid argument
	
	SetCommonHT(pAd);	
		
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtAmsdu_Proc::(HtAmsdu=%d)\n",pAd->CommonCfg.BACapability.field.AmsduEnable));

	return TRUE;																																	
}			

INT	Set_HtAutoBa_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
	{
		pAd->CommonCfg.BACapability.field.AutoBA = FALSE;
		pAd->CommonCfg.BACapability.field.Policy = BA_NOTUSE;
	}
    else if (Value == 1)	
    {
		pAd->CommonCfg.BACapability.field.AutoBA = TRUE;
		pAd->CommonCfg.BACapability.field.Policy = IMMED_BA;
    }
	else
		return FALSE; //Invalid argument
	
    pAd->CommonCfg.REGBACapability.field.AutoBA = pAd->CommonCfg.BACapability.field.AutoBA;
	pAd->CommonCfg.REGBACapability.field.Policy = pAd->CommonCfg.BACapability.field.Policy;
	SetCommonHT(pAd);	
		
	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtAutoBa_Proc::(HtAutoBa=%d)\n",pAd->CommonCfg.BACapability.field.AutoBA));

	return TRUE;				
		
}		
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																						
INT	Set_HtProtect_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->CommonCfg.bHTProtect = FALSE;
    else if (Value == 1)	
		pAd->CommonCfg.bHTProtect = TRUE;
	else
		return FALSE; //Invalid argument

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtProtect_Proc::(HtProtect=%d)\n",pAd->CommonCfg.bHTProtect));

	return TRUE;
}

INT	Set_SendPSMPAction_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
    UCHAR mac[6], mode;
	PSTRING token;
	STRING sepValue[] = ":", DASH = '-';
	INT i;
    MAC_TABLE_ENTRY *pEntry;

    //DBGPRINT(RT_DEBUG_TRACE,("\n%s\n", arg));
/*
	The BARecTearDown inupt string format should be xx:xx:xx:xx:xx:xx-d, 
		=>The six 2 digit hex-decimal number previous are the Mac address, 
		=>The seventh decimal number is the mode value.
*/
    if(strlen(arg) < 19)  //Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and mode value in decimal format.
		return FALSE;

   	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		mode = simple_strtol((token+1), 0, 10);
		if (mode > MMPS_ENABLE)
			return FALSE;
		
		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return FALSE;
			AtoH(token, (&mac[i]), 1);
		}
		if(i != 6)
			return FALSE;

		DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x", 
								mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mode));

		pEntry = MacTableLookup(pAd, mac);

		if (pEntry) {
		    DBGPRINT(RT_DEBUG_OFF, ("\nSendPSMPAction MIPS mode = %d\n", mode));
		    SendPSMPAction(pAd, pEntry->Aid, mode);
		}

		return TRUE;
	}

	return FALSE;


}

INT	Set_HtMIMOPSmode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;
	
	Value = simple_strtol(arg, 0, 10);
	
	if (Value <=3 && Value >= 0)
		pAd->CommonCfg.BACapability.field.MMPSmode = Value;
	else
		pAd->CommonCfg.BACapability.field.MMPSmode = 3;

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMIMOPSmode_Proc::(MIMOPS mode=%d)\n",pAd->CommonCfg.BACapability.field.MMPSmode));

	return TRUE;																																	
}

#ifdef CONFIG_AP_SUPPORT
/* 
    ==========================================================================
    Description:
        Set Tx Stream number
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_HtTxStream_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG 	Value;	
		
	Value = simple_strtol(arg, 0, 10);

	if ((Value <= 3) && (Value >= 1) && (Value <= pAd->Antenna.field.TxPath)) // 3*3
		pAd->CommonCfg.TxStream = Value;
	else
		pAd->CommonCfg.TxStream = pAd->Antenna.field.TxPath;

	if ((pAd->MACVersion < RALINK_2883_VERSION) &&
		(pAd->CommonCfg.TxStream > 3))
	{
		pAd->CommonCfg.TxStream = 2; // only 2 TX streams for RT2860 series
	}

	SetCommonHT(pAd);

	APStop(pAd);
	APStartUp(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtTxStream_Proc::(Tx Stream=%d)\n",pAd->CommonCfg.TxStream));
		
	return TRUE;
}

/* 
    ==========================================================================
    Description:
        Set Rx Stream number
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_HtRxStream_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG 	Value;	
		
	Value = simple_strtol(arg, 0, 10);

	if ((Value <= 3) && (Value >= 1) && (Value <= pAd->Antenna.field.RxPath))
		pAd->CommonCfg.RxStream = Value;
	else
		pAd->CommonCfg.RxStream = pAd->Antenna.field.RxPath;

	if ((pAd->MACVersion < RALINK_2883_VERSION) &&
		(pAd->CommonCfg.RxStream > 2)) // 3*3
	{
		pAd->CommonCfg.RxStream = 2; // only 2 RX streams for RT2860 series
	}

	SetCommonHT(pAd);

	APStop(pAd);
	APStartUp(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtRxStream_Proc::(Rx Stream=%d)\n",pAd->CommonCfg.RxStream));
		
	return TRUE;
}

#ifdef DOT11_N_SUPPORT
#ifdef GREENAP_SUPPORT
INT	Set_GreenAP_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
	{
		pAd->ApCfg.bBlockAntDivforGreenAP=FALSE;
		pAd->ApCfg.bGreenAPEnable = FALSE;
	}
	else if (Value == 1)	
		pAd->ApCfg.bGreenAPEnable = TRUE;
	else
		return FALSE; //Invalid argument

	DBGPRINT(RT_DEBUG_TRACE, ("Set_GreenAP_Proc::(bGreenAPEnable=%d)\n",pAd->ApCfg.bGreenAPEnable));

	return TRUE;
}
#endif // GREENAP_SUPPORT //
#endif // DOT11_N_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

INT	Set_ForceShortGI_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->WIFItestbed.bShortGI = FALSE;
	else if (Value == 1)	
		pAd->WIFItestbed.bShortGI = TRUE;
	else
		return FALSE; //Invalid argument

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ForceShortGI_Proc::(ForceShortGI=%d)\n", pAd->WIFItestbed.bShortGI));

	return TRUE;
}



INT	Set_ForceGF_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->WIFItestbed.bGreenField = FALSE;
	else if (Value == 1)	
		pAd->WIFItestbed.bGreenField = TRUE;
	else
		return FALSE; //Invalid argument

	SetCommonHT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ForceGF_Proc::(ForceGF=%d)\n", pAd->WIFItestbed.bGreenField));

	return TRUE;
}

INT	Set_HtMimoPs_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);
	if (Value == 0)
		pAd->CommonCfg.bMIMOPSEnable = FALSE;
	else if (Value == 1)	
		pAd->CommonCfg.bMIMOPSEnable = TRUE;
	else
		return FALSE; //Invalid argument

	DBGPRINT(RT_DEBUG_TRACE, ("Set_HtMimoPs_Proc::(HtMimoPs=%d)\n",pAd->CommonCfg.bMIMOPSEnable));

	return TRUE;
}
#endif // DOT11_N_SUPPORT //


#ifdef DOT11_N_SUPPORT
INT	SetCommonHT(
	IN	PRTMP_ADAPTER	pAd)
{
	OID_SET_HT_PHYMODE		SetHT;
	
	if (pAd->CommonCfg.PhyMode < PHY_11ABGN_MIXED)
		return FALSE;
				
	SetHT.PhyMode = pAd->CommonCfg.PhyMode;
	SetHT.TransmitNo = ((UCHAR)pAd->Antenna.field.TxPath);
	SetHT.HtMode = (UCHAR)pAd->CommonCfg.RegTransmitSetting.field.HTMODE;
	SetHT.ExtOffset = (UCHAR)pAd->CommonCfg.RegTransmitSetting.field.EXTCHA;
	SetHT.MCS = MCS_AUTO;
	SetHT.BW = (UCHAR)pAd->CommonCfg.RegTransmitSetting.field.BW;
	SetHT.STBC = (UCHAR)pAd->CommonCfg.RegTransmitSetting.field.STBC;
	SetHT.SHORTGI = (UCHAR)pAd->CommonCfg.RegTransmitSetting.field.ShortGI;		

	RTMPSetHT(pAd, &SetHT);

	return TRUE;
}
#endif // DOT11_N_SUPPORT //

INT	Set_FixedTxMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
#endif // CONFIG_AP_SUPPORT //		
	INT	fix_tx_mode = FIXED_TXMODE_HT;

	fix_tx_mode = RT_CfgSetFixedTxPhyMode(arg);
	
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		pAd->ApCfg.MBSSID[pObj->ioctl_if].DesiredTransmitSetting.field.FixedTxMode = fix_tx_mode;
#endif // CONFIG_AP_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		pAd->StaCfg.DesiredTransmitSetting.field.FixedTxMode = fix_tx_mode;
#endif // CONFIG_STA_SUPPORT //
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_FixedTxMode_Proc::(FixedTxMode=%d)\n", fix_tx_mode));

	return TRUE;
}

#ifdef CONFIG_APSTA_MIXED_SUPPORT
INT	Set_OpMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG Value;

	Value = simple_strtol(arg, 0, 10);

#ifdef RTMP_MAC_PCI
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
#endif // RTMP_MAC_PCI //
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Can not switch operate mode on interface up !! \n"));
		return FALSE;
	}

	if (Value == 0)
		pAd->OpMode = OPMODE_STA;
	else if (Value == 1)	
		pAd->OpMode = OPMODE_AP;
	else
		return FALSE; //Invalid argument

	DBGPRINT(RT_DEBUG_TRACE, ("Set_OpMode_Proc::(OpMode=%s)\n", pAd->OpMode == 1 ? "AP Mode" : "STA Mode"));

	return TRUE;
}
#endif // CONFIG_APSTA_MIXED_SUPPORT //

#ifdef TXBF_SUPPORT

#define IMP_MAX_BYTES		14		// Implicit: 14 bytes per subcarrier
#define IMP_MAX_BYTES_ONE_COL	7	// Implicit: 7 bytes per subcarrier, when reading first column
#define EXP_MAX_BYTES		18		// Explicit: 18 bytes per subcarrier
#define IMP_COEFF_SIZE		 9		// 9 bits/coeff
#define IMP_COEFF_MASK		0x1FF

#define PROFILE_MAX_CARRIERS_20		56		// Number of subcarriers in 20 MHz mode
#define PROFILE_MAX_CARRIERS_40		114		// Number of subcarriers in 40 MHz mode

// Indices of valid rows in Implicit and Explicit profiles for 20 and 40 MHz
typedef
struct {
	int lwb1, upb1;
	int lwb2, upb2;
} SC_TABLE_ENTRY;

SC_TABLE_ENTRY impSubCarrierTable[2] = { {36, 63, 1, 28}, {70, 126, 2, 58} };
SC_TABLE_ENTRY expSubCarrierTable[2] = { {100, 127, 1, 28}, {70, 126, 2, 58} };

typedef
struct {
	BOOLEAN impProfile;
	BOOLEAN fortyMHz;
	int rows, columns;
	int grouping;
	UCHAR tag[EXP_MAX_BYTES];
	UCHAR data[PROFILE_MAX_CARRIERS_40][EXP_MAX_BYTES];
} PROFILE_DATA;

PROFILE_DATA profData;

// Read_TagField - read a profile tagfield
void Read_TagField(
	IN	PRTMP_ADAPTER	pAd, 
	IN  UCHAR	*row,
	IN  int		profileNum)
{
	int byteIndex;

	// Assume R179 has already been set to select Explicit or Implicit profiles
	
	// Read a tagfield
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R181, 0x80);
	for (byteIndex=0; byteIndex<EXP_MAX_BYTES; byteIndex++ ) {
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R180, (profileNum<<5) | byteIndex);
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R182, &row[byteIndex]);
	}
}
// Write_TagField - write a profile tagfield
void Write_TagField(
	IN	PRTMP_ADAPTER	pAd, 
	IN  UCHAR	*row,
	IN  int		profileNum)
{
	int byteIndex;

	// Assume R179 has already been set to select Explicit or Implicit profiles
	
	// Write a tagfield
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R181, 0x80);
	for (byteIndex=0; byteIndex<EXP_MAX_BYTES; byteIndex++ ) {
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R180, (profileNum<<5) | byteIndex);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R182, row[byteIndex]);
	}
}

#if 1
// Unpack an ITxBF matrix element from a row of bytes
int Unpack_IBFValue(
	IN UCHAR *row,
	IN int elemNum)
{
	int bitNum, byteOffset, bitOffset;
	int val;

	bitNum = elemNum*IMP_COEFF_SIZE;
	byteOffset = bitNum/8;
	bitOffset = bitNum - byteOffset*8;

	val = row[byteOffset] | (row[byteOffset+1]<<8);
	val = (val>>bitOffset) & IMP_COEFF_MASK;
	if (val >= 1<<(IMP_COEFF_SIZE-1) )
		val -= 1<<IMP_COEFF_SIZE;

	return val;
}

// Pack an ITxBF matrix element into a row of bytes
void Pack_IBFValue(
	IN	UCHAR	*row,
	IN	int		elemNum,
	IN	int		value)
{
	int bitNum, byteOffset, bitOffset;
	int rowBytes;

	bitNum = elemNum*IMP_COEFF_SIZE;
	byteOffset = bitNum/8;
	bitOffset = bitNum - byteOffset*8;

	rowBytes = row[byteOffset] | (row[byteOffset+1]<<8);

	rowBytes &= ~(IMP_COEFF_MASK<<bitOffset);
	rowBytes |= (value & IMP_COEFF_MASK)<<bitOffset;

	row[byteOffset] = rowBytes & 0xFF;
	row[byteOffset+1] = (rowBytes >> 8) & 0xFF;
}

// Read_BFRow - read a row from a BF profile
void Read_BFRow(
	IN	PRTMP_ADAPTER	pAd, 
	IN	UCHAR	*row,
	IN	int		profileNum,
	IN	int		rowIndex,
	IN	int		bytesPerRow)
{
	int byteIndex;

	// Assume R179 has already been set to select Explicit or Implicit profiles
	
	// Read a row of data
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R181, rowIndex);
			
	for (byteIndex=0; byteIndex <bytesPerRow; byteIndex++) {
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R180, (profileNum<<5) | byteIndex);
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R182, &row[byteIndex]);
	}

}

// Write_BFRow - write a row for a BF profile
void Write_BFRow(
	IN	PRTMP_ADAPTER	pAd,
	IN	int		profileNum,
	IN	int		rowIndex, 
	IN	PROFILE_DATA	*pExp,
	IN	int		carrierIndex)
{
	int byteIndex, bytesPerRow;
	UCHAR *row = pExp->data[carrierIndex];

	// Optimize the number of bytes written
	if (pExp->impProfile)
		bytesPerRow = pExp->columns==1? IMP_MAX_BYTES_ONE_COL: IMP_MAX_BYTES;
	else
		bytesPerRow = EXP_MAX_BYTES;

	// Assume R179 has already been set to select Explicit or Implicit profiles
	
	// Write a row of data
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R181, rowIndex);
			
	for (byteIndex=0; byteIndex <bytesPerRow; byteIndex++) {
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R180, (profileNum<<5) | byteIndex);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R182, row[byteIndex]);
	}
}

void Read_TxBfProfile(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PROFILE_DATA	*prof,
	IN	int				profileNum,
	IN	BOOLEAN			implicitProfile)
{
	int carrierIndex, scIndex;
	int maxBytes;
	SC_TABLE_ENTRY *pTab;
	int j, c;


	// Select Implicit/Explicit profile
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, implicitProfile? 0: 0x04);
	
	// Read tag and set up profile data
	Read_TagField(pAd, prof->tag, profileNum);

	if (implicitProfile) {
		prof->impProfile = TRUE;
		prof->fortyMHz = (prof->tag[7] & 0x7)==6;
		prof->rows = 3;	// min(# of TX, # of RX)
		prof->columns = (prof->tag[7] & 0x70)==0x30? 2: 1;
		prof->grouping = 1;

		// Read subcarrier data
		pTab = &impSubCarrierTable[prof->fortyMHz];
		maxBytes = prof->columns==1? IMP_MAX_BYTES_ONE_COL: IMP_MAX_BYTES;

		// Negative subcarriers
		carrierIndex = 0;
		for (scIndex=pTab->lwb1; scIndex <= pTab->upb1; scIndex++)
			Read_BFRow(pAd, prof->data[carrierIndex++], profileNum, scIndex, maxBytes);

		// Positive subcarriers
		for (scIndex=pTab->lwb2; scIndex <= pTab->upb2; scIndex++)
			Read_BFRow(pAd, prof->data[carrierIndex++], profileNum, scIndex, maxBytes);
	}
	else {
		prof->impProfile = FALSE;
		prof->fortyMHz = (prof->tag[7] & 0x10)!=0;
		prof->rows = 1 + (prof->tag[7]>>2 & 0x3);
		prof->columns = 1 + (prof->tag[7] & 0x03);

		switch (prof->tag[7] & 0x60) {
		case 0x20:
			prof->grouping = 2;
			break;
		case 0x40:
			prof->grouping = 4;
			break;
		default:	// 1 or invalid values
			prof->grouping = 1;
			break;
		}

		// Read subcarrier data
		pTab = &expSubCarrierTable[prof->fortyMHz];
		carrierIndex = 0;

		// Negative subcarriers
		for (scIndex=pTab->lwb1; scIndex < pTab->upb1; scIndex += prof->grouping) {
			c = carrierIndex;
			Read_BFRow(pAd, prof->data[carrierIndex++], profileNum, scIndex, EXP_MAX_BYTES);

			// Replicate data if subcarriers are grouped. For 20Mhz the last carrier requires special handling to make sure it
			//	isn't overwritten when replicating the data
			for (j=1; j<prof->grouping; j++) {
				if (!prof->fortyMHz && carrierIndex==(PROFILE_MAX_CARRIERS_20/2 - 1))
					break;
				memcpy(prof->data[carrierIndex++], prof->data[c], sizeof(prof->data[c]));
			}
		}
		Read_BFRow(pAd, prof->data[carrierIndex++], profileNum, pTab->upb1, EXP_MAX_BYTES);

		// Positive subcarriers
		for (scIndex=pTab->lwb2; scIndex < pTab->upb2; scIndex += prof->grouping) {
			c = carrierIndex;
			Read_BFRow(pAd, prof->data[carrierIndex++], profileNum, scIndex, EXP_MAX_BYTES);

			// Replicate data if subcarriers are grouped. For 20Mhz the last carrier requires special handling to make sure it
			//	isn't overwritten when replicating the data
			for (j=1; j<prof->grouping; j++) {
				if (!prof->fortyMHz && carrierIndex==(PROFILE_MAX_CARRIERS_20-1))
					break;
				memcpy(prof->data[carrierIndex++], prof->data[c], sizeof(prof->data[c]));
			}
		}
		Read_BFRow(pAd, prof->data[carrierIndex++], profileNum, pTab->upb2, EXP_MAX_BYTES);
	}
}

void Write_TxBfProfile(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PROFILE_DATA	*prof,
	IN	int				profileNum)
{
	int carrierIndex, scIndex;
	SC_TABLE_ENTRY *pTab;
	int maxBytes;

	// Select Implicit/Explicit profile
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, prof->impProfile? 0: 0x04);
	
	// Write Tagfield format byte so it matches the profile
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R181, 0x80);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R180, (profileNum<<5) | 0x7);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R182, profData.tag[7]);
	
	// Write Implicit or Explicit profile
	if (prof->impProfile) {
		// Write subcarrier data
		pTab = &impSubCarrierTable[profData.fortyMHz];
		maxBytes = profData.columns==1? 7: IMP_MAX_BYTES;

		carrierIndex = 0;
		for (scIndex=pTab->lwb1; scIndex <= pTab->upb1; scIndex++)
			Write_BFRow(pAd, profileNum, scIndex, &profData, carrierIndex++);

		for (scIndex=pTab->lwb2; scIndex <= pTab->upb2; scIndex++)
			Write_BFRow(pAd, profileNum, scIndex, &profData, carrierIndex++);
	}
	else {
		// Write subcarrier data. If data is grouped then just write every n-th subcarrier
		pTab = &expSubCarrierTable[profData.fortyMHz];
		carrierIndex = 0;

		// Negative subcarriers
		for (scIndex=pTab->lwb1; scIndex<pTab->upb1; scIndex += profData.grouping) {
			Write_BFRow(pAd, profileNum, scIndex, &profData, carrierIndex);
			carrierIndex += profData.grouping;
		}
		//  In 20MHz mode the last carrier in the group is a special case
		if (!profData.fortyMHz)
			carrierIndex--;
		Write_BFRow(pAd, profileNum, pTab->upb1, &profData, carrierIndex++);

		// Positive subcarriers
		for (scIndex=pTab->lwb2; scIndex<pTab->upb2; scIndex += profData.grouping) {
			Write_BFRow(pAd, profileNum, scIndex, &profData, carrierIndex);
			carrierIndex += profData.grouping;
		}
		if (!profData.fortyMHz)
			carrierIndex--;
		Write_BFRow(pAd, profileNum, pTab->upb2, &profData, carrierIndex);
	}
}


// Set_ReadITxBf_Proc - Read Implicit BF profile and display it
//		iwpriv ra0 set ReadITxBf=<profile number>
INT	Set_ReadITxBf_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	int profileNum = simple_strtol(arg, 0, 10);
	int scIndex, i, maxCarriers;

	Read_TxBfProfile(pAd, &profData, profileNum, TRUE);

	// Display profile. Note: each column is displayed as a row. This shortens the display
	DBGPRINT(RT_DEBUG_OFF, ("---ITxBF Profile: %d - %dx%d, %dMHz\n",
		profileNum, profData.rows, profData.columns, profData.fortyMHz? 40: 20));

	maxCarriers = profData.fortyMHz? PROFILE_MAX_CARRIERS_40: PROFILE_MAX_CARRIERS_20;

	for (scIndex=0; scIndex<maxCarriers; scIndex++) {
		for (i=0; i<profData.rows; i++) {
			DBGPRINT(RT_DEBUG_OFF, ("%d %d    ", Unpack_IBFValue(profData.data[scIndex], 2*i+1), Unpack_IBFValue(profData.data[scIndex], 2*i)));
		}
		DBGPRINT(RT_DEBUG_OFF, ("\n"));

		if (profData.columns>1) {
			for (i=0; i<profData.rows; i++) {
				DBGPRINT(RT_DEBUG_OFF, ("%d %d    ", Unpack_IBFValue(profData.data[scIndex], 2*i+7), Unpack_IBFValue(profData.data[scIndex], 2*i+6)));
			}
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		}
	}

	return TRUE;
}

// Set_ReadETxBf_Proc - Read Explicit BF profile and display it
//		usage: iwpriv ra0 set ReadETxBf=<profile number>
INT	Set_ReadETxBf_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	int profileNum = simple_strtol(arg, 0, 10);
	int scIndex, i, maxCarriers;

	Read_TxBfProfile(pAd, &profData, profileNum, FALSE);

	// Dump ETxBF profile values. Note: each column is displayed as a row. This shortens the display
	DBGPRINT(RT_DEBUG_OFF, ("---ETxBF Profile: %d - %dx%d, %dMHz, grp=%d\n",
		profileNum, profData.rows, profData.columns, profData.fortyMHz? 40: 20, profData.grouping));

	maxCarriers = profData.fortyMHz? PROFILE_MAX_CARRIERS_40: PROFILE_MAX_CARRIERS_20;

	for (scIndex=0; scIndex<maxCarriers; scIndex++) {
		for (i=0; i<profData.rows; i++) {
			DBGPRINT(RT_DEBUG_OFF, ("%d %d\t", (CHAR)(profData.data[scIndex][6*i]), (CHAR)(profData.data[scIndex][6*i+1]) ));
		}
		DBGPRINT(RT_DEBUG_OFF, ("\n"));

		if (profData.columns>1) {
			for (i=0; i<profData.rows; i++) {
				DBGPRINT(RT_DEBUG_OFF, ("%d %d    ", (CHAR)(profData.data[scIndex][6*i+2]), (CHAR)(profData.data[scIndex][6*i+3]) ));
			}
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		}

		if (profData.columns>2) {
			for (i=0; i<profData.rows; i++) {
				DBGPRINT(RT_DEBUG_OFF, ("%d %d    ", (CHAR)(profData.data[scIndex][6*i+4]), (CHAR)(profData.data[scIndex][6*i+5]) ));
			}
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		}
	}

	return TRUE;
}


// Set_WriteITxBf_Proc - Write Implicit BF matrix
//		usage: iwpriv ra0 set WriteITxBf=<profile number>
//		Assumes profData contains a valid Implicit Profile
INT	Set_WriteITxBf_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	int profileNum = simple_strtol(arg, 0, 10);

	if (!profData.impProfile)
		return FALSE;

	Write_TxBfProfile(pAd, &profData, profileNum);

	return TRUE;
}


// Set_WriteETxBf_Proc - Write Explicit BF matrix
//		usage: iwpriv ra0 set WriteETxBf=<profile number>
//		Assumes profData contains a valid Explicit Profile
INT	Set_WriteETxBf_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	int profileNum = simple_strtol(arg, 0, 10);

	if (profData.impProfile)
		return FALSE;

	Write_TxBfProfile(pAd, &profData, profileNum);

	return TRUE;
}


// Set_StatITxBf_Proc - Compute power of each chain in Implicit BF matrix
//		usage: iwpriv ra0 set StatITxBf=<profile number>
INT	Set_StatITxBf_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	int scIndex, maxCarriers, i;
	unsigned long totalPower[3] = {0,0,0};
	int profileNum = simple_strtol(arg, 0, 10);

	Read_TxBfProfile(pAd, &profData, profileNum, TRUE);

	maxCarriers = profData.fortyMHz? PROFILE_MAX_CARRIERS_40: PROFILE_MAX_CARRIERS_20;

	for (scIndex=0; scIndex<maxCarriers; scIndex++) {
		for (i=0; i<profData.rows; i++) {
			int ival = Unpack_IBFValue(profData.data[scIndex], 2*i+1);
			int qval = Unpack_IBFValue(profData.data[scIndex], 2*i);
			totalPower[i] += ival*ival+qval*qval;

			if (profData.columns==2) {
				ival = Unpack_IBFValue(profData.data[scIndex], 2*i+7);
				qval = Unpack_IBFValue(profData.data[scIndex], 2*i+6);
				totalPower[i] += ival*ival+qval*qval;
			}
		}
	}

	// Remove implied scale factor of 2^-16. Convert to thousandths
	for (i=0; i<profData.rows; i++) {
		totalPower[i] >>= 12;
		totalPower[i] = (totalPower[i]*1000)/(maxCarriers*profData.columns);
		totalPower[i] >>= 4;
	}

	// Display stats
	DBGPRINT(RT_DEBUG_OFF, ("---ITxBF Stats: 0.%03lu 0.%03lu, 0.%03lu\n", totalPower[0], totalPower[1], totalPower[2]));

	return TRUE;
}

// Set_StatETxBf_Proc - Compute power of each chain in Explicit BF matrix
//		usage: iwpriv ra0 set StatETxBf=<profile number>
INT	Set_StatETxBf_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	int scIndex, maxCarriers, i;
	unsigned long totalPower[3] = {0,0,0};
	int profileNum = simple_strtol(arg, 0, 10);

	Read_TxBfProfile(pAd, &profData, profileNum, FALSE);

	maxCarriers = profData.fortyMHz? PROFILE_MAX_CARRIERS_40: PROFILE_MAX_CARRIERS_20;

	for (scIndex=0; scIndex<maxCarriers; scIndex++) {
		for (i=0; i<profData.rows; i++) {
			int ival = (CHAR)(profData.data[scIndex][6*i]);
			int qval = (CHAR)(profData.data[scIndex][6*i+1]);
			totalPower[i] += ival*ival+qval*qval;

			if (profData.columns>1) {
				ival = (CHAR)(profData.data[scIndex][6*i+2]);
				qval = (CHAR)(profData.data[scIndex][6*i+3]);
				totalPower[i] += ival*ival+qval*qval;
			}

			if (profData.columns>2) {
				ival = (CHAR)(profData.data[scIndex][6*i+4]);
				qval = (CHAR)(profData.data[scIndex][6*i+5]);
				totalPower[i] += ival*ival+qval*qval;
			}
		}
	}

	// Remove implied scale factor of 2^-14. Convert to thousandths
	for (i=0; i<profData.rows; i++) {
		totalPower[i] >>= 10;
		totalPower[i] = (totalPower[i]*1000)/(maxCarriers*profData.columns);
		totalPower[i] >>= 4;
	}

	// Display stats
	DBGPRINT(RT_DEBUG_OFF, ("---ETxBF Stats: 0.%03lu 0.%03lu, 0.%03lu\n", totalPower[0], totalPower[1], totalPower[2]));

	return TRUE;
}
#endif


// displayTagfield - display one tagfield
static void displayTagfield(
	IN	PRTMP_ADAPTER	pAd, 
	IN int profileNum)
{
	int byteIndex;
	UCHAR row[EXP_MAX_BYTES];;

	// Print a row of Tagfield data
	DBGPRINT(RT_DEBUG_OFF, ("%d: ", profileNum));

	Read_TagField(pAd, row, profileNum);
	for (byteIndex=EXP_MAX_BYTES; --byteIndex >= 0; )
		DBGPRINT(RT_DEBUG_OFF, ("%02X ", row[byteIndex]));
	DBGPRINT(RT_DEBUG_OFF, ("\n"));
}

// Set_TxBfTag_Proc - Display BF Profile Tags
//	usage: "iwpriv ra0 set TxBfTag=n"
INT	Set_TxBfTag_Proc(
	IN	PRTMP_ADAPTER	pAd, 
    IN  PSTRING         arg)
{
	int profileNum;

	// Display Implicit tagfield
	DBGPRINT(RT_DEBUG_OFF, ("---Implicit TxBfTag:\n"));
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, 0);
	for (profileNum=0; profileNum<4; profileNum++)
		displayTagfield(pAd, profileNum);

	// Display Explicit tagfield
	DBGPRINT(RT_DEBUG_OFF, ("---Explicit TxBfTag:\n"));
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, 4);
	for (profileNum=0; profileNum<4; profileNum++)
		displayTagfield(pAd, profileNum);

	return TRUE;
}

// Set_InvTxBfTag_Proc - Invalidate BF Profile Tags
//	usage: "iwpriv ra0 set InvTxBfTag=n"
//		Reset Valid bit and zero out MAC address of each profile. The next profile will be stored in profile 0
INT	Set_InvTxBfTag_Proc(
	IN	PRTMP_ADAPTER	pAd, 
    IN  PSTRING         arg)
{
	int profileNum;
	UCHAR row[EXP_MAX_BYTES];;

	// Invalidate Implicit tags
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, 0);
	for (profileNum=0; profileNum<4; profileNum++) {
		Read_TagField(pAd, row, profileNum);
		row[0] &= 0x7F;
		row[1] = row[2] = row[3] = row[4] = row[5] = row[6] = 0x00;
		Write_TagField(pAd, row, profileNum);
	}

	// Invalidate Explicit tags
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, 4);
	for (profileNum=0; profileNum<4; profileNum++) {
		Read_TagField(pAd, row, profileNum);
		row[0] &= 0x7F;
		row[1] = row[2] = row[3] = row[4] = row[5] = row[6] = 0x00;
		Write_TagField(pAd, row, profileNum);
	}

	return TRUE;
}

// Set_ITxBfTimeout_Proc - Set ITxBF timeout value
//		usage: iwpriv ra0 set ITxBfTimeout=<decimal timeout in units of 25 microsecs>
INT Set_ITxBfTimeout_Proc(
    IN  PRTMP_ADAPTER   pAd, 
    IN  PSTRING         arg)
{
	ULONG t = simple_strtol(arg, 0, 10);

	if (t > 65535) {
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ITxBfTimeout_Proc: value > 65535!\n"));
		return FALSE;
	}

    pAd->CommonCfg.ITxBfTimeout = t;
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, 0x02);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R180, 0);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R182, pAd->CommonCfg.ITxBfTimeout & 0xFF);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R180, 1);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R182, (pAd->CommonCfg.ITxBfTimeout>>8) & 0xFF);
	
    DBGPRINT(RT_DEBUG_TRACE, ("Set_ITxBfTimeout_Proc::(ITxBfTimeout=%d)\n", (int)pAd->CommonCfg.ITxBfTimeout));
	return TRUE;
}

// Set_ETxBfTimeout_Proc - Set ITxBF timeout value
//		usage: iwpriv ra0 set ETxBfTimeout=<decimal timeout in units of 25 microsecs>
INT Set_ETxBfTimeout_Proc(
    IN  PRTMP_ADAPTER   pAd, 
    IN  PSTRING         arg)
{
	ULONG t = simple_strtol(arg, 0, 10);

	if (t > 65535) {
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ETxBfTimeout_Proc: value > 65535!\n"));
		return FALSE;
	}

    pAd->CommonCfg.ETxBfTimeout = t;
	RTMP_IO_WRITE32(pAd, TX_TXBF_CFG_3, pAd->CommonCfg.ETxBfTimeout)	
    DBGPRINT(RT_DEBUG_TRACE, ("Set_ETxBfTimeout_Proc::(ETxBfTimeout=%d)\n", (int)pAd->CommonCfg.ETxBfTimeout));
	return TRUE;
}

// isqrt - fixed point sqrt
//	x - unsigned value
UINT32 isqrt (UINT32 x) 
{ 
	UINT32 base, y;

	if (x &      0xF0000000)
		base = 1<<15;
	else if (x & 0x0F000000)
		base = 1<<13;
	else if (x & 0x00F00000)
		base = 1<<11;
	else if (x & 0x000F0000)
		base = 1<<9;
	else
		base = 1<<7;

    y = 0; 
    while (base) { 
		y += base; 
		if  ((y * y) > x)
			y -= base;
		base >>= 1;
    }
    return y; 
} 

// icexp - fixed point complex exponential
//	phase - 0 to 255 representing 0 to 2pi
//		return cos and sin in 1p10 format
void icexp(short c[2], int phase)
{
	// cosine table generated with Matlab: round(1024*cos(2*pi*[0:255]/256)
	static short cosTable[256] = {
		1024, 1024, 1023, 1021, 1019, 1016, 1013, 1009,
		1004, 999, 993, 987, 980, 972, 964, 955,
		946, 936, 926, 915, 903, 891, 878, 865,
		851, 837, 822, 807, 792, 775, 759, 742,
		724, 706, 688, 669, 650, 630, 610, 590,
		569, 548, 526, 505, 483, 460, 438, 415,
		392, 369, 345, 321, 297, 273, 249, 224,
		200, 175, 150, 125, 100, 75, 50, 25,
		0, -25, -50, -75, -100, -125, -150, -175,
		-200, -224, -249, -273, -297, -321, -345, -369,
		-392, -415, -438, -460, -483, -505, -526, -548,
		-569, -590, -610, -630, -650, -669, -688, -706,
		-724, -742, -759, -775, -792, -807, -822, -837,
		-851, -865, -878, -891, -903, -915, -926, -936,
		-946, -955, -964, -972, -980, -987, -993, -999,
		-1004, -1009, -1013, -1016, -1019, -1021, -1023, -1024,
		-1024, -1024, -1023, -1021, -1019, -1016, -1013, -1009,
		-1004, -999, -993, -987, -980, -972, -964, -955,
		-946, -936, -926, -915, -903, -891, -878, -865,
		-851, -837, -822, -807, -792, -775, -759, -742,
		-724, -706, -688, -669, -650, -630, -610, -590,
		-569, -548, -526, -505, -483, -460, -438, -415,
		-392, -369, -345, -321, -297, -273, -249, -224,
		-200, -175, -150, -125, -100, -75, -50, -25,
		0, 25, 50, 75, 100, 125, 150, 175,
		200, 224, 249, 273, 297, 321, 345, 369,
		392, 415, 438, 460, 483, 505, 526, 548,
		569, 590, 610, 630, 650, 669, 688, 706,
		724, 742, 759, 775, 792, 807, 822, 837,
		851, 865, 878, 891, 903, 915, 926, 936,
		946, 955, 964, 972, 980, 987, 993, 999,
		1004, 1009, 1013, 1016, 1019, 1021, 1023, 1024};
	c[0] = cosTable[phase & 0xFF];
	c[1] = cosTable[(phase-64) & 0xFF];
}

// icMult - fixed point complex multiply
//		r = a*b
void icMult(INT32 r[2], INT32 a[2], INT32 b0, INT32 b1)
{
	INT32 t;
	t = a[0]*b0 - a[1]*b1;
	r[1] = a[0]*b1 + a[1]*b0;
	r[0] = t;
}


#define P_RESOLUTION	256		// Resolution of phase calculation: 2pi/256

PROFILE_DATA	expData, impData;

INT32 ei0[PROFILE_MAX_CARRIERS_40][2];
INT32 ei1[PROFILE_MAX_CARRIERS_40][2];
INT32 ei2[PROFILE_MAX_CARRIERS_40][2];

// iCalcCalibration - calculate calibration parameters
//	Returns 0 if successful, -1 if profiles are invalid
int iCalcCalibration(PRTMP_ADAPTER pAd, int calParams[2], int profileNum)
{
	//struct timeval tval1, tval2;
	int		pi, maxCarriers, ii;

	short rot[2], rot1[2];
	INT32 c0[2], c1[2];

	INT32 minSum=0;
	int	  di1=0, di2=0;

	// Read Implicit and Explicit data
	Read_TxBfProfile(pAd, &impData, profileNum, TRUE);
	Read_TxBfProfile(pAd, &expData, profileNum, FALSE);

	// Quit if MAC addresses don't match
	for (ii=1; ii<7; ii++) {
		if (impData.tag[ii]!=expData.tag[ii])
			return -2;
	}

	// Quit if profiles cannot be used
	if (impData.fortyMHz!=expData.fortyMHz || impData.rows<2 || expData.rows<2) {
		return -1;
	}

	// If Implicit profile is legacy then zero out the unused carriers so they don't
	//	affect the calculation
	if ((impData.tag[7] & 0x70)==0x10) {
	   memset(impData.data[0], 0x00, sizeof(impData.data[0]));
	   memset(impData.data[1], 0x00, sizeof(impData.data[0]));
	   memset(impData.data[PROFILE_MAX_CARRIERS_20-2], 0x00, sizeof(impData.data[0]));
	   memset(impData.data[PROFILE_MAX_CARRIERS_20-1], 0x00, sizeof(impData.data[0]));
	}

	//do_gettimeofday(&tval1);

	maxCarriers = impData.fortyMHz? PROFILE_MAX_CARRIERS_40: PROFILE_MAX_CARRIERS_20;

	// Compute Exp .* conj(Imp). ei0 is 2p25, ei1 and ei2 are are 2p15
	for (pi=0; pi<maxCarriers; pi++) {
		INT32 ed[2];
		ed[0] = ((CHAR)expData.data[pi][0])<<10;
		ed[1] = ((CHAR)expData.data[pi][1])<<10;
		icMult(ei0[pi], ed, Unpack_IBFValue(impData.data[pi], 1), -Unpack_IBFValue(impData.data[pi], 0));
		ed[0] = (CHAR)expData.data[pi][6];
		ed[1] = (CHAR)expData.data[pi][7];
		icMult(ei1[pi], ed, Unpack_IBFValue(impData.data[pi], 3), -Unpack_IBFValue(impData.data[pi], 2));
		ed[0] = (CHAR)expData.data[pi][12];
		ed[1] = (CHAR)expData.data[pi][13];
		icMult(ei2[pi], ed, Unpack_IBFValue(impData.data[pi], 5), -Unpack_IBFValue(impData.data[pi], 4));
	}

	// Search for best Phase 1
	for (ii=0; ii<P_RESOLUTION; ii++) {
		INT32 sum = 0;

		icexp(rot, ii*256/P_RESOLUTION);

		for (pi=0; pi<maxCarriers; pi++) {
			icMult(c1, ei1[pi], rot[0], rot[1]);
			// Sum as 1p25 and scale 1p25 => 1p13
			c0[0] = (ei0[pi][0] + c1[0])>>12;
			c0[1] = (ei0[pi][1] + c1[1])>>12;
			sum -= isqrt(c0[0]*c0[0] + c0[1]*c0[1]);
		}

		// Record minimum
		if (ii==0 || minSum>sum) {
			di1 = ii;
			minSum = sum;
		}
	}

	// Search for best Phase 2
	if (impData.rows==2 || expData.rows==2) {
		di2 = 0;
	}
	else {
		icexp(rot1, di1);
		// ei0 = ei0 + rot1*ei1
		for (pi=0; pi<maxCarriers; pi++) {
			icMult(c1, ei1[pi], rot1[0], rot1[1]);
			ei0[pi][0] += c1[0];
			ei0[pi][1] += c1[1];
		}

		for (ii=0; ii<P_RESOLUTION; ii++) {
			INT32 sum = 0;

			icexp(rot, ii*256/P_RESOLUTION);

			for (pi=0; pi<maxCarriers; pi++) {
				// Compute ei0 + ei2*rot. Scale 1p25 => 1p13
				icMult(c1, ei2[pi], rot[0], rot[1]);
				c0[0] = (ei0[pi][0] + c1[0]) >> 12;
				c0[1] = (ei0[pi][1] + c1[1]) >> 12;
				sum -= isqrt(c0[0]*c0[0] + c0[1]*c0[1]);
			}

			// Record minimum
			if (ii==0 || minSum>sum) {
				di2 = ii;
				minSum = sum;
			}
		}
	}

	// Convert to calibration parameters
	calParams[0] = -di1 & 0xFF;
	calParams[1] = -(di1-di2) & 0xFF;

	return 0;
}


// Set_ITxBfCal_Proc - Calculate ITxBf Calibration parameters
//	usage: "iwpriv ra0 set ITxBfCal=<0 | 1> 0=>calculate values, 1=>update BBP and EEPROM"
INT	Set_ITxBfCal_Proc(
	IN	PRTMP_ADAPTER	pAd, 
    IN  PSTRING         arg)
{
	int calFunction = simple_strtol(arg, 0, 10);
	int calParams[2];
	int ret;

	ret = iCalcCalibration(pAd, calParams, 0);
	if (ret == -2) {
		DBGPRINT(RT_DEBUG_OFF, ("Set_ITxBfCal_Proc: MAC Address mismatch\n"));
		return FALSE;
	}
	else if (ret < 0) {
		DBGPRINT(RT_DEBUG_OFF, ("Set_ITxBfCal_Proc: Invalid profiles\n"));
		return FALSE;
	}

	// Display result
	DBGPRINT(RT_DEBUG_OFF, ("ITxBfCal R176 = [0x%02x 0x%02x]\n", calParams[0], calParams[1]));


	// Update BBP R176 and EEPROM for Ant 0 and 2
	if (calFunction == 1) {
		UCHAR BbpValue;
		USHORT value;

		// Select Ant 0
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R27, &BbpValue);
		BbpValue &= ~0x60;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, BbpValue);

		// Update R176 and update EEPROM
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R176, &BbpValue);
		BbpValue += calParams[0];
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R176, BbpValue);

		RT28xx_EEPROM_READ16(pAd, 0x1a2, value);
		value = (value & 0xFF00) | BbpValue;
		RT28xx_EEPROM_WRITE16(pAd, 0x1a2, value);

		// Select Ant 2
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R27, &BbpValue);
		BbpValue &= ~0x60;
		BbpValue |= 0x40;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, BbpValue);

		// Update R176 and update EEPROM
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R176, &BbpValue);
		BbpValue += calParams[1];
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R176, BbpValue);

		RT28xx_EEPROM_READ16(pAd, 0x1aa, value);
		value = (value & 0xFF00) | BbpValue;
		RT28xx_EEPROM_WRITE16(pAd, 0x1aa, value);

		DBGPRINT(RT_DEBUG_OFF, ("Set_ITxBfCal_Proc: Calibration Parameters updated\n"));
	}

	return TRUE;
}

#endif	// TXBF_SUPPORT //

#ifdef INCLUDE_DEBUG_QUEUE
// ---------------------- Debug Queue ------------------------

#define DBQ_LENGTH	512
#define DBQ_DATA_LENGTH	8


typedef
struct {
	UCHAR type;					// type of data
	ULONG timestamp;			// sec/usec timestamp from gettimeofday
	UCHAR data[DBQ_DATA_LENGTH];	// data
} DBQUEUE_ENTRY;

// Type field definitions
#define DBQ_TYPE_EMPTY	0
#define DBQ_TYPE_TXWI	0x70		// TXWI
#define DBQ_TYPE_TXHDR	0x72		// TX Header
#define DBQ_TYPE_TXFIFO	0x73		// TX Stat FIFO
#define DBQ_TYPE_RXWI	0x78		// RXWI uses 0x78 to 0x7A for 5 longs
#define DBQ_TYPE_RXHDR	0x7B		// RX Header

#define DBQ_INIT_SIG	0x4442484E	// 'DBIN' - dbqInit initialized flag
#define DBQ_ENA_SIG		0x4442454E	// 'DBEN' - dbqEnable enabled flag

static DBQUEUE_ENTRY dbQueue[DBQ_LENGTH];
static ULONG dbqTail=0;
static ULONG dbqEnable=0;
static ULONG dbqInit=0;

// dbQueueInit - initialize Debug Queue variables and clear the queue
void dbQueueInit(void)
{
	int i;

	for (i=0; i<DBQ_LENGTH; i++)
		dbQueue[i].type = DBQ_TYPE_EMPTY;
	dbqTail = 0;
	dbqInit = DBQ_INIT_SIG;
}

// dbQueueEnqueue - enqueue data
void dbQueueEnqueue(UCHAR type, UCHAR *data)
{
	DBQUEUE_ENTRY *oldTail;
	struct timeval tval;

	if (dbqEnable!=DBQ_ENA_SIG || data==NULL)
		return;

	if (dbqInit!=DBQ_INIT_SIG || dbqTail>=DBQ_LENGTH)
		dbQueueInit();

	oldTail = &dbQueue[dbqTail];

	// Advance tail and mark as empty
	if (dbqTail >= DBQ_LENGTH-1)
		dbqTail = 0;
	else
		dbqTail++;
	dbQueue[dbqTail].type = DBQ_TYPE_EMPTY;

	// Enqueue data
	oldTail->type = type;
	do_gettimeofday(&tval);
	oldTail->timestamp = tval.tv_sec*1000000L + tval.tv_usec;
	memcpy(oldTail->data, data, DBQ_DATA_LENGTH);
}

// dbQueueDump - dump contents of debug queue
void dbQueueDump(void)
{
	DBQUEUE_ENTRY *oldTail;
	int i;
	ULONG t, dt, lastTimestamp=0;

	if (dbqInit!=DBQ_INIT_SIG || dbqTail>=DBQ_LENGTH)
		return;

	oldTail = &dbQueue[dbqTail];

	for (i=0; i<DBQ_LENGTH; i++) {
		if (++oldTail >= &dbQueue[DBQ_LENGTH])
			oldTail = dbQueue;

		// Skip empty entries
		if (oldTail->type == DBQ_TYPE_EMPTY)
			continue;

		switch (oldTail->type) {
		case 0x70:	// TXWI - 2 longs, MSB to LSB
		case 0x78:	// RXWI - 2 longs, MSB to LSB
			DBGPRINT(RT_DEBUG_OFF, ("%cxWI %02X%02X %02X%02X-%02X%02X %02X%02X---", 
					oldTail->type==0x70? 'T': 'R',
					oldTail->data[3], oldTail->data[2], oldTail->data[1], oldTail->data[0],
					oldTail->data[7], oldTail->data[6], oldTail->data[5], oldTail->data[4]) );
			break;
		case 0x79:	// RXWI - next 2 longs, MSB to LSB
		case 0x7a:	// RXWI - next 2 longs, MSB to LSB
			DBGPRINT(RT_DEBUG_OFF, ("%02X   %02X%02X %02X%02X-%02X%02X %02X%02X   ", oldTail->type, 
					oldTail->data[3], oldTail->data[2], oldTail->data[1], oldTail->data[0],
					oldTail->data[7], oldTail->data[6], oldTail->data[5], oldTail->data[4]) );
			break;
		case 0x72:	// Tx 802.11 header, MSB to LSB, translate type/subtype
		case 0x7b:	// Rx
			{
			UCHAR tCode;
			struct _typeTableEntry {
				UCHAR code;	// Type/subtype
				CHAR  str[4];
			} *pTab, typeTable[] = {
				{0x00, "mARq"}, {0x01, "mARp"}, {0x02, "mRRq"}, {0x03, "mRRp"},
				{0x04, "mPRq"}, {0x05, "mRRp"}, {0x08, "mBcn"}, {0x09, "mATI"},
				{0x0a, "mDis"}, {0x0b, "mAut"}, {0x0c, "mDAu"}, {0x0d, "mAct"},
				{0x0e, "mANA"},
				{0x17, "cCWr"}, {0x18, "cBAR"}, {0x19, "cBAc"}, {0x1a, "cPSP"},
				{0x1b, "cRTS"}, {0x1c, "cCTS"}, {0x1d, "cACK"}, {0x1e, "cCFE"},
				{0x1f, "cCEA"},
				{0x20, "dDat"}, {0x21, "dDCA"}, {0x22, "dDCP"}, {0x23, "dDAP"},
				{0x24, "dNul"}, {0x25, "dCFA"}, {0x26, "dCFP"}, {0x27, "dCAP"},
				{0x28, "dQDa"}, {0x29, "dQCA"}, {0x2a, "dQCP"}, {0x2b, "dQAP"},
				{0x2c, "dQNu"}, {0x2e, "dQNP"}, {0x2f, "dQNA"},
				{0xFF, "RESV"}};

			tCode = ((oldTail->data[0]<<2) & 0x30) | ((oldTail->data[0]>>4) & 0xF);
			for (pTab=typeTable; pTab->code!=0xFF; pTab++) {
				if (pTab->code == tCode)
					break;
			}

			DBGPRINT(RT_DEBUG_OFF, ("%cxH  %c%c%c%c ", oldTail->type==0x72? 'T': 'R',
					pTab->str[0], pTab->str[1], pTab->str[2], pTab->str[3]) );

			DBGPRINT(RT_DEBUG_OFF, ("[%02X%02X %02X%02X]      ", 
					oldTail->data[3], oldTail->data[2], oldTail->data[1], oldTail->data[0]) );

			}
			break;
		case 0x73:	// TX STAT FIFO
			DBGPRINT(RT_DEBUG_OFF, ("TxFI %02X%02X%02X%02X=%c%c%2s M%02d/%02d%c%c ", 
					oldTail->data[3], oldTail->data[2], oldTail->data[1], oldTail->data[0],
					(oldTail->data[3] & 0x20)? 'I': ((oldTail->data[3] & 0x08)? 'E': '_'),	// Beamforming:  E or I or _
					(oldTail->data[3] & 0x01)? 'S': 'L',									// Guard Int:    S or L
					(oldTail->data[3] & 0x02)? "st": "__",									// STBC:         st or __
					(oldTail->data[2] & 0x7F), (oldTail->data[0]>>1) & 0xF,					// MCS:          <Final>/<orig>
					(oldTail->data[2] & 0x7F)==((oldTail->data[0]>>1) & 0xF)? ' ': '*',		// Retry:        * if MCS doesn't match
					(oldTail->data[0] & 0x20)? ' ': 'F') );									// Success/Fail  _ or F
			break;
		default:
			DBGPRINT(RT_DEBUG_OFF, ("%02X   %02X%02X %02X%02X %02X%02X %02X%02X   ", oldTail->type,
					oldTail->data[0], oldTail->data[1], oldTail->data[2], oldTail->data[3], 
					oldTail->data[4], oldTail->data[5], oldTail->data[6], oldTail->data[7]) );
			break;
		}

		t = oldTail->timestamp;
		dt = oldTail->timestamp-lastTimestamp;

		DBGPRINT(RT_DEBUG_OFF, ("%lu.%06lu ", t/1000000L, t % 1000000L) );

		if (dt>999999L) {
			DBGPRINT(RT_DEBUG_OFF, ("+%lu.%06lu s\n", dt/1000000L, dt % 1000000L) );
		}
		else {
			DBGPRINT(RT_DEBUG_OFF, ("+%lu us\n", dt) );
		}
		lastTimestamp = oldTail->timestamp;
	}
}

// Set_DebugQueue_Proc - Control DBQueue
//	iwpriv ra0 set DBQueue=dd.
//		dd: 0=>disable, 1=>enable, 2=>dump, 3=>clear
INT Set_DebugQueue_Proc(
    IN  PRTMP_ADAPTER   pAd, 
    IN  PSTRING         arg)
{
    ULONG argValue = simple_strtol(arg, 0, 10);

	switch (argValue) {
	case 0:
		dbqEnable = 0;
		break;
	case 1:
		dbqEnable = DBQ_ENA_SIG;
		break;
	case 2:
		dbQueueDump();
		break;
	case 3:
		dbQueueInit();
		break;
	default:
		break;
	}

	return TRUE;
}
#endif


#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
INT Set_PreAntSwitch_Proc(
    IN  PRTMP_ADAPTER   pAd, 
    IN  PSTRING         arg)
{
    pAd->CommonCfg.PreAntSwitch = simple_strtol(arg, 0, 10)!=0;
    DBGPRINT(RT_DEBUG_TRACE, ("Set_PreAntSwitch_Proc::(PreAntSwitch=%d)\n", pAd->CommonCfg.PreAntSwitch));
	return TRUE;
}

INT Set_StreamMode_Proc(
    IN  PRTMP_ADAPTER   pAd, 
    IN  PSTRING         arg)
{
    pAd->CommonCfg.StreamMode = simple_strtol(arg, 0, 10)!=0;
    DBGPRINT(RT_DEBUG_TRACE, ("Set_StreamMode_Proc::(StreamMode=%d)\n", pAd->CommonCfg.StreamMode));
	return TRUE;
}

// Set_PhyRateLimit_Proc - limit max PHY rate
//		usage: iwpriv ra0 set PhyRateLimit=<PHY rate in Mbps>
INT Set_PhyRateLimit_Proc(
    IN  PRTMP_ADAPTER   pAd, 
    IN  PSTRING         arg)
{
    pAd->CommonCfg.PhyRateLimit = simple_strtol(arg, 0, 10);
    DBGPRINT(RT_DEBUG_TRACE, ("Set_PhyRateLimit_Proc::(PhyRateLimit=%ld)\n", pAd->CommonCfg.PhyRateLimit));
	return TRUE;
}
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //


INT Set_DebugFlags_Proc(
    IN  PRTMP_ADAPTER   pAd, 
    IN  PSTRING         arg)
{
    pAd->CommonCfg.DebugFlags = simple_strtol(arg, 0, 16);
    DBGPRINT(RT_DEBUG_TRACE, ("Set_DebugFlags_Proc::(DebugFlags=%02lX)\n", pAd->CommonCfg.DebugFlags));
	return TRUE;
}

#if defined(RT305x)||defined(RT3070)
INT Set_HiPower_Proc(
    IN  PRTMP_ADAPTER   pAdapter, 
    IN  PSTRING          arg)
{
	pAdapter->CommonCfg.HighPowerPatchDisabled = !(simple_strtol(arg, 0, 10));

	if (pAdapter->CommonCfg.HighPowerPatchDisabled != 0)
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAdapter, BBP_R82, 0x62);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAdapter, BBP_R67, 0x20);
		RT30xxWriteRFRegister(pAdapter, RF_R27, 0x23); 
	}
	return TRUE;
}
#endif

INT Set_LongRetryLimit_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PSTRING			arg)
{
	TX_RTY_CFG_STRUC	tx_rty_cfg;
	UCHAR				LongRetryLimit = (UCHAR)simple_strtol(arg, 0, 10);

	RTMP_IO_READ32(pAdapter, TX_RTY_CFG, &tx_rty_cfg.word);
	tx_rty_cfg.field.LongRtyLimit = LongRetryLimit;
	RTMP_IO_WRITE32(pAdapter, TX_RTY_CFG, tx_rty_cfg.word);
	DBGPRINT(RT_DEBUG_TRACE, ("IF Set_LongRetryLimit_Proc::(tx_rty_cfg=0x%x)\n", tx_rty_cfg.word));
	return TRUE;
}

INT Set_ShortRetryLimit_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PSTRING			arg)
{
	TX_RTY_CFG_STRUC	tx_rty_cfg;
	UCHAR				ShortRetryLimit = (UCHAR)simple_strtol(arg, 0, 10);

	RTMP_IO_READ32(pAdapter, TX_RTY_CFG, &tx_rty_cfg.word);
	tx_rty_cfg.field.ShortRtyLimit = ShortRetryLimit;
	RTMP_IO_WRITE32(pAdapter, TX_RTY_CFG, tx_rty_cfg.word);	
	DBGPRINT(RT_DEBUG_TRACE, ("IF Set_ShortRetryLimit_Proc::(tx_rty_cfg=0x%x)\n", tx_rty_cfg.word));
	return TRUE;
}

INT Set_AutoFallBack_Proc(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PSTRING			arg)
{
	return RT_CfgSetAutoFallBack(pAdapter, arg);
}



/////////////////////////////////////////////////////////////////////////
PSTRING RTMPGetRalinkAuthModeStr(
    IN  NDIS_802_11_AUTHENTICATION_MODE authMode)
{
	switch(authMode)
	{
		case Ndis802_11AuthModeOpen:
			return "OPEN";
		case Ndis802_11AuthModeWPAPSK:
			return "WPAPSK";
		case Ndis802_11AuthModeShared:
			return "SHARED";
		case Ndis802_11AuthModeWPA:
			return "WPA";
		case Ndis802_11AuthModeWPA2:
			return "WPA2";
		case Ndis802_11AuthModeWPA2PSK:
			return "WPA2PSK";
        case Ndis802_11AuthModeWPA1PSKWPA2PSK:
			return "WPAPSKWPA2PSK";
        case Ndis802_11AuthModeWPA1WPA2:
			return "WPA1WPA2";
		case Ndis802_11AuthModeWPANone:
			return "WPANONE";
		default:
			return "UNKNOW";
	}
}

PSTRING RTMPGetRalinkEncryModeStr(
    IN  USHORT encryMode)
{
	switch(encryMode)
	{
		case Ndis802_11WEPDisabled:
			return "NONE";
		case Ndis802_11WEPEnabled:
			return "WEP";        
		case Ndis802_11Encryption2Enabled:
			return "TKIP";
		case Ndis802_11Encryption3Enabled:
			return "AES";
        case Ndis802_11Encryption4Enabled:
			return "TKIPAES";
		default:
			return "UNKNOW";
	}
}

INT RTMPShowCfgValue(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			pName,
	IN	PSTRING			pBuf)
{
	INT	Status = 0;	
	
	for (PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC = RTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC; PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->name; PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC++)
	{
		if (!strcmp(pName, PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->name)) 
		{						
			if(PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->show_proc(pAd, pBuf))
				Status = -EINVAL;
			break;  //Exit for loop.
		}
	}

	if(PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->name == NULL)
	{
		sprintf(pBuf, "\n");
		for (PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC = RTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC; PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->name; PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC++)
			sprintf(pBuf, "%s%s\n", pBuf, PRTMP_PRIVATE_STA_SHOW_CFG_VALUE_PROC->name);
	}
	
	return Status;
}

INT	Show_SSID_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
#ifdef CONFIG_AP_SUPPORT
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		sprintf(pBuf, "\t%s", pAd->ApCfg.MBSSID[pObj->ioctl_if].Ssid);
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		sprintf(pBuf, "\t%s", pAd->CommonCfg.Ssid);
#endif // CONFIG_STA_SUPPORT //
	return 0;
}

INT	Show_WirelessMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	switch(pAd->CommonCfg.PhyMode)
	{
		case PHY_11BG_MIXED:
			sprintf(pBuf, "\t11B/G");
			break;
		case PHY_11B:
			sprintf(pBuf, "\t11B");
			break;
		case PHY_11A:
			sprintf(pBuf, "\t11A");
			break;
		case PHY_11ABG_MIXED:
			sprintf(pBuf, "\t11A/B/G");
			break;
		case PHY_11G:
			sprintf(pBuf, "\t11G");
			break;
#ifdef DOT11_N_SUPPORT
		case PHY_11ABGN_MIXED:
			sprintf(pBuf, "\t11A/B/G/N");
			break;
		case PHY_11N_2_4G:
			sprintf(pBuf, "\t11N only with 2.4G");
			break;
		case PHY_11GN_MIXED:
			sprintf(pBuf, "\t11G/N");
			break;
		case PHY_11AN_MIXED:
			sprintf(pBuf, "\t11A/N");
			break;
		case PHY_11BGN_MIXED:
			sprintf(pBuf, "\t11B/G/N");
			break;
		case PHY_11AGN_MIXED:
			sprintf(pBuf, "\t11A/G/N");
			break;
		case PHY_11N_5G:
			sprintf(pBuf, "\t11N only with 5G");
			break;
#endif // DOT11_N_SUPPORT //
		default:
			sprintf(pBuf, "\tUnknow Value(%d)", pAd->CommonCfg.PhyMode);
			break;
	}
	return 0;
}


INT	Show_TxBurst_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%s", pAd->CommonCfg.bEnableTxBurst ? "TRUE":"FALSE");
	return 0;
}

INT	Show_TxPreamble_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	switch(pAd->CommonCfg.TxPreamble)
	{
		case Rt802_11PreambleShort:
			sprintf(pBuf, "\tShort");
			break;
		case Rt802_11PreambleLong:
			sprintf(pBuf, "\tLong");
			break;
		case Rt802_11PreambleAuto:
			sprintf(pBuf, "\tAuto");
			break;
		default:
			sprintf(pBuf, "\tUnknown Value(%lu)", pAd->CommonCfg.TxPreamble);
			break;
	}
	
	return 0;
}

INT	Show_TxPower_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%lu", pAd->CommonCfg.TxPowerPercentage);
	return 0;
}

INT	Show_Channel_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%d", pAd->CommonCfg.Channel);
	return 0;
}

INT	Show_BGProtection_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	switch(pAd->CommonCfg.UseBGProtection)
	{
		case 1: //Always On
			sprintf(pBuf, "\tON");
			break;
		case 2: //Always OFF
			sprintf(pBuf, "\tOFF");
			break;
		case 0: //AUTO
			sprintf(pBuf, "\tAuto");
			break;
		default:
			sprintf(pBuf, "\tUnknow Value(%lu)", pAd->CommonCfg.UseBGProtection);
			break;
	}
	return 0;
}

INT	Show_RTSThreshold_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%u", pAd->CommonCfg.RtsThreshold);
	return 0;
}

INT	Show_FragThreshold_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%u", pAd->CommonCfg.FragmentThreshold);
	return 0;
}

#ifdef DOT11_N_SUPPORT
INT	Show_HtBw_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	if (pAd->CommonCfg.RegTransmitSetting.field.BW == BW_40)
	{
		sprintf(pBuf, "\t40 MHz");
	}
	else
	{
        sprintf(pBuf, "\t20 MHz");
	}
	return 0;
}

INT	Show_HtMcs_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		sprintf(pBuf, "\t%u", pAd->ApCfg.MBSSID[pObj->ioctl_if].DesiredTransmitSetting.field.MCS);
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		sprintf(pBuf, "\t%u", pAd->StaCfg.DesiredTransmitSetting.field.MCS);
#endif // CONFIG_STA_SUPPORT //
	return 0;
}

INT	Show_HtGi_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	switch(pAd->CommonCfg.RegTransmitSetting.field.ShortGI)
	{
		case GI_400:
			sprintf(pBuf, "\tGI_400");
			break;
		case GI_800:
			sprintf(pBuf, "\tGI_800");
			break;
		default:
			sprintf(pBuf, "\tUnknow Value(%u)", pAd->CommonCfg.RegTransmitSetting.field.ShortGI);
			break;
	}
	return 0;
}

INT	Show_HtOpMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	switch(pAd->CommonCfg.RegTransmitSetting.field.HTMODE)
	{
		case HTMODE_GF:
			sprintf(pBuf, "\tGF");
			break;
		case HTMODE_MM:
			sprintf(pBuf, "\tMM");
			break;
		default:
			sprintf(pBuf, "\tUnknow Value(%u)", pAd->CommonCfg.RegTransmitSetting.field.HTMODE);
			break;
	}
	return 0;
}

INT	Show_HtExtcha_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	switch(pAd->CommonCfg.RegTransmitSetting.field.EXTCHA)
	{
		case EXTCHA_BELOW:
			sprintf(pBuf, "\tBelow");
			break;
		case EXTCHA_ABOVE:
			sprintf(pBuf, "\tAbove");
			break;
		default:
			sprintf(pBuf, "\tUnknow Value(%u)", pAd->CommonCfg.RegTransmitSetting.field.EXTCHA);
			break;
	}
	return 0;
}


INT	Show_HtMpduDensity_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%u", pAd->CommonCfg.BACapability.field.MpduDensity);
	return 0;
}

INT	Show_HtBaWinSize_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%u", pAd->CommonCfg.BACapability.field.RxBAWinLimit);
	return 0;
}

INT	Show_HtRdg_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%s", pAd->CommonCfg.bRdg ? "TRUE":"FALSE");
	return 0;
}

INT	Show_HtAmsdu_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%s", pAd->CommonCfg.BACapability.field.AmsduEnable ? "TRUE":"FALSE");
	return 0;
}

INT	Show_HtAutoBa_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%s", pAd->CommonCfg.BACapability.field.AutoBA ? "TRUE":"FALSE");
	return 0;
}
#endif // DOT11_N_SUPPORT //

INT	Show_CountryRegion_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%d", pAd->CommonCfg.CountryRegion);
	return 0;
}

INT	Show_CountryRegionABand_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%d", pAd->CommonCfg.CountryRegionForABand);
	return 0;
}

INT	Show_CountryCode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%s", pAd->CommonCfg.CountryCode);
	return 0;
}

#ifdef AGGREGATION_SUPPORT
INT	Show_PktAggregate_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%s", pAd->CommonCfg.bAggregationCapable ? "TRUE":"FALSE");
	return 0;
}
#endif // AGGREGATION_SUPPORT //

#ifdef WMM_SUPPORT
INT	Show_WmmCapable_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		sprintf(pBuf, "\t%s", pAd->ApCfg.MBSSID[pObj->ioctl_if].bWmmCapable ? "TRUE":"FALSE");
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		sprintf(pBuf, "\t%s", pAd->CommonCfg.bWmmCapable ? "TRUE":"FALSE");
#endif // CONFIG_STA_SUPPORT //
	
	return 0;
}
#endif // WMM_SUPPORT //

INT	Show_IEEE80211H_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	sprintf(pBuf, "\t%s", pAd->CommonCfg.bIEEE80211H ? "TRUE":"FALSE");
	return 0;
}

#ifdef CONFIG_STA_SUPPORT
INT	Show_NetworkType_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	switch(pAd->StaCfg.BssType)
	{
		case BSS_ADHOC:
			sprintf(pBuf, "\tAdhoc");
			break;
		case BSS_INFRA:
			sprintf(pBuf, "\tInfra");
			break;
		case BSS_ANY:
			sprintf(pBuf, "\tAny");
			break;
		case BSS_MONITOR:
			sprintf(pBuf, "\tMonitor");
			break;
		default:
			sprintf(pBuf, "\tUnknow Value(%d)", pAd->StaCfg.BssType);
			break;
	}
	return 0;
}

#ifdef WSC_STA_SUPPORT
INT	Show_WpsPbcBand_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	switch(pAd->StaCfg.WscControl.WpsApBand)
	{
		case PREFERRED_WPS_AP_PHY_TYPE_2DOT4_G_FIRST:
			sprintf(pBuf, "\t2.4G");
			break;
		case PREFERRED_WPS_AP_PHY_TYPE_5_G_FIRST:
			sprintf(pBuf, "\t5G");
			break;
		case PREFERRED_WPS_AP_PHY_TYPE_AUTO_SELECTION:
			sprintf(pBuf, "\tAuto");
			break;
		default:
			sprintf(pBuf, "\tUnknow Value(%d)", pAd->StaCfg.WscControl.WpsApBand);
			break;
	}
	return 0;
}
#endif // WSC_STA_SUPPORT //

#endif // CONFIG_STA_SUPPORT //

INT	Show_AuthMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	NDIS_802_11_AUTHENTICATION_MODE	AuthMode = Ndis802_11AuthModeOpen; 
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		AuthMode = pAd->ApCfg.MBSSID[pObj->ioctl_if].AuthMode;
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		AuthMode = pAd->StaCfg.AuthMode;
#endif // CONFIG_STA_SUPPORT //

	if ((AuthMode >= Ndis802_11AuthModeOpen) && 
		(AuthMode <= Ndis802_11AuthModeWPA1PSKWPA2PSK))
		sprintf(pBuf, "\t%s", RTMPGetRalinkAuthModeStr(AuthMode));
	else
		sprintf(pBuf, "\tUnknow Value(%d)", AuthMode);
	
	return 0;
}

INT	Show_EncrypType_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	NDIS_802_11_WEP_STATUS	WepStatus = Ndis802_11WEPDisabled;
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		WepStatus = pAd->ApCfg.MBSSID[pObj->ioctl_if].WepStatus;
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		WepStatus = pAd->StaCfg.WepStatus;
#endif // CONFIG_STA_SUPPORT //

	if ((WepStatus >= Ndis802_11WEPEnabled) && 
		(WepStatus <= Ndis802_11Encryption4KeyAbsent))
		sprintf(pBuf, "\t%s", RTMPGetRalinkEncryModeStr(WepStatus));
	else
		sprintf(pBuf, "\tUnknow Value(%d)", WepStatus);
	
	return 0;
}

INT	Show_DefaultKeyID_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	UCHAR DefaultKeyId = 0;
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		DefaultKeyId = pAd->ApCfg.MBSSID[pObj->ioctl_if].DefaultKeyId;
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		DefaultKeyId = pAd->StaCfg.DefaultKeyId;
#endif // CONFIG_STA_SUPPORT //

	sprintf(pBuf, "\t%d", DefaultKeyId);

	return 0;
}

INT	Show_WepKey_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN  INT				KeyIdx,
	OUT	PSTRING			pBuf)
{
	UCHAR   Key[16] = {0}, KeyLength = 0;
	INT		index = BSS0;
#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		index = pObj->ioctl_if;
#endif // CONFIG_AP_SUPPORT //

	KeyLength = pAd->SharedKey[index][KeyIdx].KeyLen;
	NdisMoveMemory(Key, pAd->SharedKey[index][KeyIdx].Key, KeyLength);		
		
	//check key string is ASCII or not
    if (RTMPCheckStrPrintAble((PCHAR)Key, KeyLength))
        sprintf(pBuf, "\t%s", Key);
    else
    {
        int idx;
        sprintf(pBuf, "\t");
        for (idx = 0; idx < KeyLength; idx++)
            sprintf(pBuf+strlen(pBuf), "%02X", Key[idx]);
    }
	return 0;
}

INT	Show_Key1_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	Show_WepKey_Proc(pAd, 0, pBuf);
	return 0;
}

INT	Show_Key2_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	Show_WepKey_Proc(pAd, 1, pBuf);
	return 0;
}

INT	Show_Key3_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	Show_WepKey_Proc(pAd, 2, pBuf);
	return 0;
}

INT	Show_Key4_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	Show_WepKey_Proc(pAd, 3, pBuf);
	return 0;
}

INT	Show_WPAPSK_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	OUT	PSTRING			pBuf)
{
	INT 	idx;
	UCHAR	PMK[32] = {0};

#ifdef CONFIG_AP_SUPPORT    
    POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		NdisMoveMemory(PMK, pAd->ApCfg.MBSSID[pObj->ioctl_if].PMK, 32);
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		NdisMoveMemory(PMK, pAd->StaCfg.PMK, 32);
#endif // CONFIG_STA_SUPPORT //
	
    sprintf(pBuf, "\tPMK = ");
    for (idx = 0; idx < 32; idx++)
        sprintf(pBuf+strlen(pBuf), "%02X", PMK[idx]);

	return 0;
}

#ifdef CONFIG_RALINK_RT3883
/* 
    ==========================================================================
    Description:
        Set VCO Re-Calibration threshold
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	Set_VCORecalibrationThreshold_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	ULONG 	Value;	
		
	Value = simple_strtol(arg, 0, 10);
	
	pAd->CommonCfg.VCORecalibrationThreshold = Value;

	DBGPRINT(RT_DEBUG_TRACE, ("Set_VCORecalibrationThreshold_Proc: Threshold=%d)\n", pAd->CommonCfg.VCORecalibrationThreshold));
		
	return TRUE;
}
#endif // CONFIG_RALINK_RT3883 //



#ifdef TXBF_SUPPORT
INT	Set_ETxBfEnCond_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	// insert code here
	UCHAR i, byteValue;
	MAC_TABLE_ENTRY		*pEntry = NULL;	

	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++){
		pEntry =	&pAd->MacTab.Content[i];
#if 1
		pEntry->eTxBfEnCond = simple_strtol(arg, 0, 10);
		pEntry->HTPhyMode.field.eTxBF = pEntry->eTxBfEnCond!=0;		//temporary fix. need to check Client BF capabilities
		pEntry->bfState = READY_FOR_SNDG0;				//temporary fix. sometimes Snding not sent
		pEntry->noSndgCnt = 0;
#else
		pEntry->eTxBfEnCond = simple_strtol(arg, &arg, 10);
		pEntry->HTPhyMode.field.eTxBF = pEntry->eTxBfEnCond!=0;	//temporary fix. need to check Client BF capabilities
		pEntry->bfState = READY_FOR_SNDG0;						//temporary fix. sometimes Snding not sent
		pEntry->noSndgCnt = 0;
		while (*arg<'0' || *arg>'9')
			arg++;
		if (*arg == '\0')
			break;
#endif
	}


	pEntry = &pAd->MacTab.Content[0];
	if ((pEntry->HTPhyMode.field.iTxBF) || (pEntry->HTPhyMode.field.eTxBF))
	{
		RT30xxReadRFRegister(pAd, RF_R39, (PUCHAR)&byteValue);
		byteValue |= 0x40;
		RT30xxWriteRFRegister(pAd, RF_R39, (UCHAR)byteValue);

		RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)&byteValue);
		byteValue |= 0x20;
		RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)byteValue);
	}
	else
	{
		RT30xxReadRFRegister(pAd, RF_R39, (PUCHAR)&byteValue);
		byteValue &= ~0x40;
		RT30xxWriteRFRegister(pAd, RF_R39, (UCHAR)byteValue);

		RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)&byteValue);
		byteValue &= ~0x20;
		RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)byteValue);
	}


	return TRUE;	
}
INT	Set_NoSndgCntThrd_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	// insert code here
	UCHAR i;
	MAC_TABLE_ENTRY		*pEntry = NULL;	
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++){
		pEntry =	&pAd->MacTab.Content[i];
		pEntry->noSndgCntThrd = simple_strtol(arg, 0, 10);
	}
	return TRUE;	
}

INT	Set_NdpSndgStreams_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	// insert code here
	UCHAR i;
	MAC_TABLE_ENTRY		*pEntry = NULL;	
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++){
		pEntry =	&pAd->MacTab.Content[i];
		pEntry->ndpSndgStreams = simple_strtol(arg, 0, 10);
	}
	return TRUE;	
}


INT	Set_Trigger_Sounding_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	UCHAR					macAddr[MAC_ADDR_LEN];
	CHAR					*value;
	INT						i;
	//UCHAR HashIdx;
	MAC_TABLE_ENTRY *pEntry = NULL;

	if(strlen(arg) != 17)  //Mac address acceptable format 01:02:03:04:05:06 length 17
		return FALSE;

	for (i=0, value = rstrtok(arg,":"); value; value = rstrtok(NULL,":")) 
	{
		if((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))) ) 
			return FALSE;  //Invalid

		AtoH(value, &macAddr[i++], 1);
	}

	//DBGPRINT(RT_DEBUG_TRACE, ("TriggerSounding=%02x:%02x:%02x:%02x:%02x:%02x\n",
	//		macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5], macAddr[6]) );
	pEntry = MacTableLookup(pAd, macAddr);
	if (pEntry==NULL)
		return FALSE;

	Trigger_Sounding_Packet(pAd, SNDG_TYPE_SOUNGING, 0, pEntry->sndgMcs, pEntry);

	return TRUE;
}

INT	Set_ITxBfEn_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	UCHAR i, byteValue;
	MAC_TABLE_ENTRY		*pEntry = NULL;	
	BOOLEAN tempB;
	tempB = simple_strtol(arg, 0, 10);
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_REG_BF, &byteValue);
	if (tempB == 1)
		byteValue |= 0x20;
	else
		byteValue &= (~0x20);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_REG_BF, byteValue);

	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		pEntry = &pAd->MacTab.Content[i];		

		pEntry->HTPhyMode.field.iTxBF = tempB;		
	}

	pEntry = &pAd->MacTab.Content[0];
	if ((pEntry->HTPhyMode.field.iTxBF) || (pEntry->HTPhyMode.field.eTxBF))
	{
		RT30xxReadRFRegister(pAd, RF_R39, (PUCHAR)&byteValue);
		byteValue |= 0x40;
		RT30xxWriteRFRegister(pAd, RF_R39, (UCHAR)byteValue);

		RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)&byteValue);
		byteValue |= 0x20;
		RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)byteValue);
	}
	else
	{
		RT30xxReadRFRegister(pAd, RF_R39, (PUCHAR)&byteValue);
		byteValue &= ~0x40;
		RT30xxWriteRFRegister(pAd, RF_R39, (UCHAR)byteValue);

		RT30xxReadRFRegister(pAd, RF_R49, (PUCHAR)&byteValue);
		byteValue &= ~0x20;
		RT30xxWriteRFRegister(pAd, RF_R49, (UCHAR)byteValue);
	}
	
	return TRUE;	
}

#endif // TXBF_SUPPORT //

INT	Set_PerThrdAdj_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	// insert code here
	UCHAR i;
	MAC_TABLE_ENTRY		*pEntry = NULL;	
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++){
		pEntry =	&pAd->MacTab.Content[i];
		pEntry->perThrdAdj = simple_strtol(arg, 0, 10);
	}
	return TRUE;	
}

