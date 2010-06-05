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
    soft_ap.c
 
    Abstract:
    Access Point specific routines and MAC table maintenance routines
 
    Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    John Chang  08-04-2003    created for 11g soft-AP

 */

#include "rt_config.h"

BOOLEAN ApCheckLongPreambleSTA(
    IN PRTMP_ADAPTER pAd);

VOID EnableAPMIMOPS(
	IN PRTMP_ADAPTER pAd);
VOID DisableAPMIMOPS(
	IN PRTMP_ADAPTER pAd);


char const *pEventText[EVENT_MAX_EVENT_TYPE] = {
	"restart access point",
	"successfully associated",
	"has disassociated",
	"has been aged-out and disassociated" ,    
	"active countermeasures",
	"has disassociated with invalid PSK password"};

/*
	==========================================================================
	Description:
		Initialize AP specific data especially the NDIS packet pool that's
		used for wireless client bridging.
	==========================================================================
 */
NDIS_STATUS APInitialize(
	IN  PRTMP_ADAPTER   pAd)
{
	NDIS_STATUS     Status = NDIS_STATUS_SUCCESS;
	INT				i;			

	DBGPRINT(RT_DEBUG_TRACE, ("---> APInitialize\n"));

	// Init Group key update timer, and countermeasures timer
	for (i = 0; i < MAX_MBSSID_NUM; i++)
		RTMPInitTimer(pAd, &pAd->ApCfg.MBSSID[i].REKEYTimer, GET_TIMER_FUNCTION(GREKEYPeriodicExec), pAd,  TRUE); 
	
	RTMPInitTimer(pAd, &pAd->ApCfg.CounterMeasureTimer, GET_TIMER_FUNCTION(CMTimerExec), pAd, FALSE);

#ifdef IDS_SUPPORT
	// Init intrusion detection timer
	RTMPInitTimer(pAd, &pAd->ApCfg.IDSTimer, GET_TIMER_FUNCTION(RTMPIdsPeriodicExec), pAd, FALSE);
	pAd->ApCfg.IDSTimerRunning = FALSE;
#endif // IDS_SUPPORT //

#ifdef WAPI_SUPPORT
	// Init WAPI rekey timer
	RTMPInitWapiRekeyTimerAction(pAd, NULL);
#endif // WAPI_SUPPORT //

#ifdef WDS_SUPPORT
	APWdsInitialize(pAd);
#endif // WDS_SUPPORT //

#ifdef IGMP_SNOOP_SUPPORT
	MulticastFilterTableInit(&pAd->pMulticastFilterTable);
#endif // IGMP_SNOOP_SUPPORT //

	NdisAllocateSpinLock(&pAd->WdsTabLock);

	DBGPRINT(RT_DEBUG_TRACE, ("<--- APInitialize\n"));
	return Status;
}

/*
	==========================================================================
	Description:
		Shutdown AP and free AP specific resources
	==========================================================================
 */
VOID APShutdown(
	IN PRTMP_ADAPTER pAd)
{
	DBGPRINT(RT_DEBUG_TRACE, ("---> APShutdown\n"));
//	if (pAd->OpMode == OPMODE_AP)
		APStop(pAd);

	MlmeRadioOff(pAd);
/* mark by wy, this is done by firmware in MlmeRadioOff.
#ifdef RTMP_MAC_PCI
	// reset DMA Index
	RTMP_IO_WRITE32(pAd, WPDMA_RST_IDX , 0xFFFFFFFF);
	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0xe1f);
	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0xe00);
#endif // RTMP_MAC_PCI //
*/

#ifdef IGMP_SNOOP_SUPPORT
	MultiCastFilterTableReset(&pAd->pMulticastFilterTable);
#endif // IGMP_SNOOP_SUPPORT //

	NdisFreeSpinLock(&pAd->MacTabLock);
	NdisFreeSpinLock(&pAd->WdsTabLock);
	DBGPRINT(RT_DEBUG_TRACE, ("<--- APShutdown\n"));
}

/*
	==========================================================================
	Description:
		Start AP service. If any vital AP parameter is changed, a STOP-START
		sequence is required to disassociate all STAs.

	IRQL = DISPATCH_LEVEL.(from SetInformationHandler)
	IRQL = PASSIVE_LEVEL. (from InitializeHandler)  

	Note:
		Can't call NdisMIndicateStatus on this routine.

		RT61 is a serialized driver on Win2KXP and is a deserialized on Win9X
		Serialized callers of NdisMIndicateStatus must run at IRQL = DISPATCH_LEVEL.

	==========================================================================
 */
VOID APStartUp(
	IN PRTMP_ADAPTER pAd) 
{
	//UCHAR         GTK[TKIP_GTK_LENGTH];
//	UCHAR		BCASTADDR[6]={0x1, 0x0, 0x0, 0x0, 0x0, 0x0};
	ULONG		offset, i;
	UINT32		Value = 0;
	BOOLEAN		bWmmCapable = FALSE;
	UCHAR		apidx;
	BOOLEAN		TxPreamble, SpectrumMgmt;
	UCHAR		BBPR1 = 0, BBPR3 = 0, byteValue = 0;
	UCHAR		phy_mode = pAd->CommonCfg.DesiredPhyMode;
	BOOLEAN		bLegacyPhyMode = FALSE;
//2008/10/28: KH add to support Antenna power-saving of AP<--
#ifdef DOT11_N_SUPPORT
#ifdef CONFIG_AP_SUPPORT
#endif // CONFIG_AP_SUPPORT //
#endif // DOT11_N_SUPPORT //
//2008/10/28: KH add to support Antenna power-saving of AP-->
	
	DBGPRINT(RT_DEBUG_TRACE, ("===> APStartUp\n"));

#ifdef COC_SUPPORT
#ifdef RALINK_ATE
	if (ATE_ON(pAd))
		pAd->CoC_sleep = 0;
	else
#endif // RALINK_ATE //
	{
	
		/* Only set TxRx stream and core power here
		 * BW control values will be initialized at the later functions
		 */
		pAd->CoC_sleep = 1;
		pAd->Antenna.field.RxPath = 1;
		pAd->Antenna.field.TxPath = 1;
#ifdef RT305x
 	       /* Set Core Power to 1.1V */
		RT30xxWriteRFRegister(pAd, RF_R26, 0xC5);
		RT30xxWriteRFRegister(pAd, RF_R28, 0x16);
#endif // RT305x //
	}
#endif // COC_SUPPORT
		
	AsicDisableSync(pAd);

	TxPreamble = (pAd->CommonCfg.TxPreamble == Rt802_11PreambleLong ? 0 : 1);

	// Decide the Capability information field
	// In IEEE Std 802.1h-2003, the spectrum management bit is enabled in the 5 GHz band 
	if ((pAd->CommonCfg.Channel > 14) && pAd->CommonCfg.bIEEE80211H == TRUE)
		SpectrumMgmt = TRUE;
	else
		SpectrumMgmt = FALSE;	
			
	for (apidx=0; apidx<pAd->ApCfg.BssidNum; apidx++)
	{
		if ((pAd->ApCfg.MBSSID[apidx].SsidLen <= 0) || (pAd->ApCfg.MBSSID[apidx].SsidLen > MAX_LEN_OF_SSID))
		{
			NdisMoveMemory(pAd->ApCfg.MBSSID[apidx].Ssid, "HT_AP", 5);
			pAd->ApCfg.MBSSID[apidx].Ssid[5] = '0'+apidx;
			pAd->ApCfg.MBSSID[apidx].SsidLen = 6;			
		}

		/* re-copy the MAC to virtual interface to avoid these MAC = all zero,
		   when re-open the ra0,
		   i.e. ifconfig ra0 down, ifconfig ra0 up, ifconfig ra0 down, ifconfig up ... */
		COPY_MAC_ADDR(pAd->ApCfg.MBSSID[apidx].Bssid, pAd->CurrentAddress);
		pAd->ApCfg.MBSSID[apidx].Bssid[5] += apidx;

		if (pAd->ApCfg.MBSSID[apidx].MSSIDDev != NULL)
		{
			NdisMoveMemory(RTMP_OS_NETDEV_GET_PHYADDR(pAd->ApCfg.MBSSID[apidx].MSSIDDev), 
								pAd->ApCfg.MBSSID[apidx].Bssid,
								MAC_ADDR_LEN);
		}

		if (pAd->ApCfg.MBSSID[apidx].bWmmCapable)
		{
        	bWmmCapable = TRUE;
		}
		
		pAd->ApCfg.MBSSID[apidx].CapabilityInfo =
			CAP_GENERATE(1, 0, (pAd->ApCfg.MBSSID[apidx].WepStatus != Ndis802_11EncryptionDisabled), TxPreamble, pAd->CommonCfg.bUseShortSlotTime, SpectrumMgmt);

		
		if (bWmmCapable == TRUE)
		{
			/* In page 38, QoS = CF-Pollable = CF-Poll Request = 0 means
			   no PC (PCF) function, dont need to set the bit */
//			pAd->ApCfg.MBSSID[apidx].CapabilityInfo |= 0x0200;
		} /* End of if */

#ifdef UAPSD_AP_SUPPORT
        if (pAd->CommonCfg.bAPSDCapable == TRUE)
		{
			/* QAPs set the APSD subfield to 1 within the Capability Information
			   field when the MIB attribute dot11APSDOptionImplemented is true
			   and set it to 0 otherwise. STAs always set this subfield to 0. */
            pAd->ApCfg.MBSSID[apidx].CapabilityInfo |= 0x0800;
        } /* End of if */
#endif // UAPSD_AP_SUPPORT //

		
		// decide the mixed WPA cipher combination  
		if (pAd->ApCfg.MBSSID[apidx].WepStatus == Ndis802_11Encryption4Enabled)
		{
			switch ((UCHAR)pAd->ApCfg.MBSSID[apidx].AuthMode)
			{
				// WPA mode
				case Ndis802_11AuthModeWPA:
				case Ndis802_11AuthModeWPAPSK:
					pAd->ApCfg.MBSSID[apidx].WpaMixPairCipher = WPA_TKIPAES_WPA2_NONE;
					break;	

				// WPA2 mode
				case Ndis802_11AuthModeWPA2:
				case Ndis802_11AuthModeWPA2PSK:
					pAd->ApCfg.MBSSID[apidx].WpaMixPairCipher = WPA_NONE_WPA2_TKIPAES;
					break;

				// WPA and WPA2 both mode
				case Ndis802_11AuthModeWPA1WPA2:
				case Ndis802_11AuthModeWPA1PSKWPA2PSK:	

					// In WPA-WPA2 and TKIP-AES mixed mode, it shall use the maximum 
					// cipher capability unless users assign the desired setting.
					if (pAd->ApCfg.MBSSID[apidx].WpaMixPairCipher == MIX_CIPHER_NOTUSE)
						pAd->ApCfg.MBSSID[apidx].WpaMixPairCipher = WPA_TKIPAES_WPA2_TKIPAES;										
					break;				
			}
											
		}
		else
			pAd->ApCfg.MBSSID[apidx].WpaMixPairCipher = MIX_CIPHER_NOTUSE;

		/* 	
			WFA recommend to restrict the encryption type in 11n-HT mode.
		 	So, the WEP and TKIP are not allowed in HT rate. 
	 	*/
		if (pAd->CommonCfg.HT_DisallowTKIP && (pAd->CommonCfg.PhyMode > PHY_11G))
		{
			if ((pAd->ApCfg.MBSSID[apidx].WepStatus == Ndis802_11Encryption1Enabled) || 
				(pAd->ApCfg.MBSSID[apidx].WepStatus == Ndis802_11Encryption2Enabled) ||
					 (pAd->ApCfg.MBSSID[apidx].WepStatus == Ndis802_11Encryption4Enabled))
			{
				bLegacyPhyMode = TRUE;					
			}			
		}
				
		// Generate the corresponding RSNIE
		RTMPMakeRSNIE(pAd, pAd->ApCfg.MBSSID[apidx].AuthMode, pAd->ApCfg.MBSSID[apidx].WepStatus, apidx);

	}

	if (bLegacyPhyMode)
	{
		if (pAd->CommonCfg.PhyMode == PHY_11AN_MIXED ||
			pAd->CommonCfg.PhyMode == PHY_11N_5G)
			phy_mode = PHY_11A;
		else
			phy_mode = PHY_11BG_MIXED;
	}

	if (phy_mode != pAd->CommonCfg.PhyMode)
		RTMPSetPhyMode(pAd, phy_mode);
#ifdef DOT11_N_SUPPORT
	SetCommonHT(pAd);
#endif // DOT11_N_SUPPORT //
	
	COPY_MAC_ADDR(pAd->CommonCfg.Bssid, pAd->CurrentAddress);

	// Select DAC according to HT or Legacy, write to BBP R1(bit4:3)
	// In HT mode and two stream mode, both DACs are selected.
	// In legacy mode or one stream mode, DAC-0 is selected.
	if (pAd->MACVersion >= 0x28830300 && pAd->MACVersion < RALINK_3070_VERSION) // 3*3
	{
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BBPR1);
		BBPR1 &= (~0x18);

#ifdef DOT11_N_SUPPORT
		if ((pAd->CommonCfg.PhyMode >= PHY_11ABGN_MIXED) &&
			(pAd->Antenna.field.TxPath > 0))
		{
			/* bit1:0 tx num = 0, 0x00; tx num = 1, 0x01; tx num = 2, 0x02 */
			BBPR1 |= ((pAd->Antenna.field.TxPath - 1)<<3);
		}
#endif // DOT11_N_SUPPORT //
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BBPR1);

		// reset Tx beamforming bit
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &byteValue);
		byteValue &= (~0x01);
		byteValue |= pAd->CommonCfg.RegTransmitSetting.field.TxBF;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, byteValue);
	}
	else
	{
#ifdef DOT11_N_SUPPORT
	if ((pAd->CommonCfg.PhyMode >= PHY_11ABGN_MIXED) && (pAd->Antenna.field.TxPath == 2))
	{
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BBPR1);
		BBPR1 &= (~0x18);
		BBPR1 |= 0x10;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BBPR1);
	}
	else
#endif // DOT11_N_SUPPORT //
	{
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BBPR1);
		BBPR1 &= (~0x18);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BBPR1);
	}
	}

	// Receiver Antenna selection, write to BBP R3(bit4:3)
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BBPR3);
	BBPR3 &= (~0x18);
	if(pAd->Antenna.field.RxPath == 3)
	{
		BBPR3 |= (0x10);
	}
	else if(pAd->Antenna.field.RxPath == 2)
	{
		BBPR3 |= (0x8);
	}
	else if(pAd->Antenna.field.RxPath == 1)
	{
		BBPR3 |= (0x0);
	}
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BBPR3);
//2008/11/05: KH add to support Antenna power-saving of AP<--
#ifdef DOT11_N_SUPPORT
#ifdef CONFIG_AP_SUPPORT		

			if (pAd->CommonCfg.bGreenAPEnable==TRUE
				&&(pAd->Antenna.field.RxPath>1||pAd->Antenna.field.TxPath>1))
			{
				
				EnableAPMIMOPS(pAd);
			}

#endif // CONFIG_AP_SUPPORT //

#endif // DOT11_N_SUPPORT //
//2008/11/05: KH add to support Antenna power-saving of AP-->
	
	//if ((pAd->CommonCfg.PhyMode >= PHY_11ABGN_MIXED) || bWmmCapable)
	if ((pAd->CommonCfg.PhyMode > PHY_11G) || bWmmCapable)  // edit by Benson, fix the bug of no 11n support
	{
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
			pAd->CommonCfg.APEdcaParm.Txop[2]  = 94;	//96;
			pAd->CommonCfg.APEdcaParm.Txop[3]  = 47;	//48;
		}
		AsicSetEdcaParm(pAd, &pAd->CommonCfg.APEdcaParm);

		// EDCA parameters to be annouced in outgoing BEACON, used by WMM STA
		if (pAd->ApCfg.BssEdcaParm.bValid == FALSE)
		{
			pAd->ApCfg.BssEdcaParm.bValid = TRUE;
			pAd->ApCfg.BssEdcaParm.Aifsn[0] = 3;
			pAd->ApCfg.BssEdcaParm.Aifsn[1] = 7;
			pAd->ApCfg.BssEdcaParm.Aifsn[2] = 2;
			pAd->ApCfg.BssEdcaParm.Aifsn[3] = 2;

			pAd->ApCfg.BssEdcaParm.Cwmin[0] = 4;
			pAd->ApCfg.BssEdcaParm.Cwmin[1] = 4;
			pAd->ApCfg.BssEdcaParm.Cwmin[2] = 3;
			pAd->ApCfg.BssEdcaParm.Cwmin[3] = 2;

			pAd->ApCfg.BssEdcaParm.Cwmax[0] = 10;
			pAd->ApCfg.BssEdcaParm.Cwmax[1] = 10;
			pAd->ApCfg.BssEdcaParm.Cwmax[2] = 4;
			pAd->ApCfg.BssEdcaParm.Cwmax[3] = 3;

			pAd->ApCfg.BssEdcaParm.Txop[0]  = 0;
			pAd->ApCfg.BssEdcaParm.Txop[1]  = 0;
			pAd->ApCfg.BssEdcaParm.Txop[2]  = 94;	//96;
			pAd->ApCfg.BssEdcaParm.Txop[3]  = 47;	//48;
		}
	}
	else
		AsicSetEdcaParm(pAd, NULL);

#ifdef DOT11_N_SUPPORT
	if (pAd->CommonCfg.PhyMode < PHY_11ABGN_MIXED)
	{
		// Patch UI
		pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth = BW_20;
	}

	// init
	if (pAd->CommonCfg.bRdg)
	{	
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE);
		AsicEnableRDG(pAd);
	}
	else	
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE);
		AsicDisableRDG(pAd);
	}	
#endif // DOT11_N_SUPPORT //

	COPY_MAC_ADDR(pAd->ApCfg.MBSSID[BSS0].Bssid, pAd->CurrentAddress);
	AsicSetBssid(pAd, pAd->CurrentAddress); 
	AsicSetMcastWC(pAd);
	// In AP mode,  First WCID Table in ASIC will never be used. To prevent it's 0xff-ff-ff-ff-ff-ff, Write 0 here.
	// p.s ASIC use all 0xff as termination of WCID table search.
	RTMP_IO_WRITE32(pAd, MAC_WCID_BASE, 0x00);
	RTMP_IO_WRITE32(pAd, MAC_WCID_BASE+4, 0x0);

#ifdef DFS_SUPPORT
#ifndef NEW_DFS
	RTMPPrepareRadarDetectParams(pAd);
#endif
#endif // DFS_SUPPORT //

	// reset WCID table 
	for (i=1; i<255; i++)
	{
		offset = MAC_WCID_BASE + (i * HW_WCID_ENTRY_SIZE);	
		RTMP_IO_WRITE32(pAd, offset, 0x0);
		RTMP_IO_WRITE32(pAd, offset+4, 0x0);
	}

	
	pAd->MacTab.Content[0].Addr[0] = 0x01;
	pAd->MacTab.Content[0].HTPhyMode.field.MODE = MODE_OFDM;
	pAd->MacTab.Content[0].HTPhyMode.field.MCS = 3;
	pAd->CommonCfg.CentralChannel = pAd->CommonCfg.Channel;
	
#ifdef DOT11_N_SUPPORT	
	if ((pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40) && (pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset == EXTCHA_ABOVE))
	{
#ifdef COC_SUPPORT
		if (pAd->CoC_sleep == 1)
			pAd->CommonCfg.BBPCurrentBW = BW_20;
		else
#endif // COC_SUPPORT
			pAd->CommonCfg.BBPCurrentBW = BW_40;

#ifdef COC_SUPPORT
		if (pAd->CoC_sleep == 1)
			pAd->CommonCfg.CentralChannel = pAd->CommonCfg.Channel;
		else
#endif // COC_SUPPORT
			pAd->CommonCfg.CentralChannel = pAd->CommonCfg.Channel + 2;
		
		//  TX : control channel at lower 
		RTMP_IO_READ32(pAd, TX_BAND_CFG, &Value);
		Value &= (~0x1);
		RTMP_IO_WRITE32(pAd, TX_BAND_CFG, Value);

		//  RX : control channel at lower 
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &byteValue);
		byteValue &= (~0x20);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, byteValue);

		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &byteValue);
		byteValue &= (~0x18);
#ifdef COC_SUPPORT
		if (pAd->CoC_sleep == 0)
#endif // COC_SUPPORT
			byteValue |= 0x10;

		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, byteValue);
		if (pAd->CommonCfg.Channel > 14)
		{ 	// request by Gary 20070208 for middle and long range A Band
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x48);
		}
		else
		{	// request by Gary 20070208 for middle and long range G Band
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x38);
		}	
		// 
		if (pAd->MACVersion == 0x28600100)
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x1A);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, 0x0A);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x16);
		}
		else
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x12);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, 0x0A);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x10);
		}	

		DBGPRINT(RT_DEBUG_TRACE, ("ApStartUp : ExtAbove, ChannelWidth=%d, Channel=%d, ExtChanOffset=%d \n",
			pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth, pAd->CommonCfg.Channel, pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset));
	}
	else if ((pAd->CommonCfg.Channel > 2) && (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40) && (pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset == EXTCHA_BELOW))
	{
#ifdef COC_SUPPORT
		if (pAd->CoC_sleep == 1)
			pAd->CommonCfg.BBPCurrentBW = BW_20;
		else
#endif // COC_SUPPORT
			pAd->CommonCfg.BBPCurrentBW = BW_40;

		if (pAd->CommonCfg.Channel == 14)
			pAd->CommonCfg.CentralChannel = pAd->CommonCfg.Channel - 1;
		else
			pAd->CommonCfg.CentralChannel = pAd->CommonCfg.Channel - 2;

#ifdef COC_SUPPORT
		if (pAd->CoC_sleep == 1)
			pAd->CommonCfg.CentralChannel = pAd->CommonCfg.Channel;
#endif // COC_SUPPORT
		
		//  TX : control channel at upper 
		RTMP_IO_READ32(pAd, TX_BAND_CFG, &Value);
		Value |= (0x1);		
		RTMP_IO_WRITE32(pAd, TX_BAND_CFG, Value);

		//  RX : control channel at upper 
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &byteValue);
		byteValue |= (0x20);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, byteValue);

		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &byteValue);
		byteValue &= (~0x18);
#ifdef COC_SUPPORT
		if (pAd->CoC_sleep == 0)
#endif // COC_SUPPORT
			byteValue |= 0x10;

		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, byteValue);
		
		if (pAd->CommonCfg.Channel > 14)
		{ 	// request by Gary 20070208 for middle and long range A Band
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x48);
		}
		else
		{ 	// request by Gary 20070208 for middle and long range G band
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x38);
		}	
	
		
		if (pAd->MACVersion == 0x28600100)
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x1A);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, 0x0A);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x16);
		}
		else
		{	
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x12);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, 0x0A);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x10);
		}
		DBGPRINT(RT_DEBUG_TRACE, ("ApStartUp : ExtBlow, ChannelWidth=%d, Channel=%d, ExtChanOffset=%d \n",
			pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth, pAd->CommonCfg.Channel, pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset));
	}
	else
#endif // DOT11_N_SUPPORT //
	{
		pAd->CommonCfg.BBPCurrentBW = BW_20;
		pAd->CommonCfg.CentralChannel = pAd->CommonCfg.Channel;
		
		//  TX : control channel at lower 
		RTMP_IO_READ32(pAd, TX_BAND_CFG, &Value);
		Value &= (~0x1);
		RTMP_IO_WRITE32(pAd, TX_BAND_CFG, Value);

		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &byteValue);
		byteValue &= (~0x18);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, byteValue);
		
		// 20 MHz bandwidth
		if (pAd->CommonCfg.Channel > 14)
		{	 // request by Gary 20070208
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x40);
		}	
		else
		{	// request by Gary 20070208
			//RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x30);
			// request by Brian 20070306
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x38);
		}	
				 
		if (pAd->MACVersion == 0x28600100)
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x16);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, 0x08);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x11);
		}
		else
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x12);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, 0x0a);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x10);
		}

#ifdef DOT11_N_SUPPORT
		DBGPRINT(RT_DEBUG_TRACE, ("ApStartUp : 20MHz, ChannelWidth=%d, Channel=%d, ExtChanOffset=%d \n",
			pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth, pAd->CommonCfg.Channel, pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset));
#endif // DOT11_N_SUPPORT //
	}
	
	if (pAd->CommonCfg.Channel > 14)
	{	// request by Gary 20070208 for middle and long range A Band
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R62, 0x1D);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R63, 0x1D);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R64, 0x1D);
		//RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R86, 0x1D);
	}
	else
	{ 	// request by Gary 20070208 for middle and long range G band
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R62, 0x2D);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R63, 0x2D);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R64, 0x2D);
			//RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R86, 0x2D);
	}	

	// Clear BG-Protection flag
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED);	
	AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
	AsicLockChannel(pAd, pAd->CommonCfg.CentralChannel);
 	MlmeSetTxPreamble(pAd, (USHORT)pAd->CommonCfg.TxPreamble);
	
	for (apidx = 0; apidx < pAd->ApCfg.BssidNum; apidx++)
	{
		MlmeUpdateTxRates(pAd, FALSE, apidx);
#ifdef DOT11_N_SUPPORT
		MlmeUpdateHtTxRates(pAd, apidx);
#endif // DOT11_N_SUPPORT //
	}
	
	// Set the RadarDetect Mode as Normal, bc the APUpdateAllBeaconFram() will refer this parameter.
	pAd->CommonCfg.RadarDetect.RDMode = RD_NORMAL_MODE;
	// start sending BEACON out
	APMakeAllBssBeacon(pAd);
	APUpdateAllBeaconFrame(pAd);

	// Disable Protection first.
	AsicUpdateProtect(pAd, 0, (ALLN_SETPROTECT|CCKSETPROTECT|OFDMSETPROTECT), TRUE, FALSE);
	
	APUpdateCapabilityAndErpIe(pAd);
#ifdef DOT11_N_SUPPORT
	APUpdateOperationMode(pAd);
#endif // DOT11_N_SUPPORT //

	if ( (pAd->CommonCfg.Channel > 14)
		&& (pAd->CommonCfg.bIEEE80211H == 1)
		&& RadarChannelCheck(pAd, pAd->CommonCfg.Channel))
	{
		pAd->CommonCfg.RadarDetect.RDMode = RD_SILENCE_MODE;
		pAd->CommonCfg.RadarDetect.RDCount = 0;
		pAd->CommonCfg.RadarDetect.InServiceMonitorCount = 0;
#ifdef DFS_SUPPORT
#ifdef RTMP_RBUS_SUPPORT
#ifdef NEW_DFS
		if ((pAd->MACVersion == 0x28720200) && (pAd->CommonCfg.CID == 0x200))
		{
			NewRadarDetectionStart(pAd);
		}
#endif // NEW_DFS //
#endif // RTMP_RBUS_SUPPORT //
		BbpRadarDetectionStart(pAd); // start Radar detection.
#endif // DFS_SUPPORT //
	}
	else
	{
		pAd->CommonCfg.RadarDetect.RDMode = RD_NORMAL_MODE;
		AsicEnableBssSync(pAd);
#ifdef CONFIG_AP_SUPPORT
#ifdef CARRIER_DETECTION_SUPPORT
#ifdef MERGE_ARCH_TEAM
		if (pAd->CommonCfg.Channel <= 14)
		{
			if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40)
			{
				if ((pAd->CommonCfg.CarrierDetect.Enable == 0) && ((pAd->CommonCfg.RadarDetect.RDDurRegion == JAP) || (pAd->CommonCfg.RadarDetect.RDDurRegion == JAP_W53) || (pAd->CommonCfg.RadarDetect.RDDurRegion == JAP_W56)))
					pAd->CommonCfg.CarrierDetect.Enable = 1;
			}
		}
		else
		{
			if ((pAd->CommonCfg.CarrierDetect.Enable == 0) && ((pAd->CommonCfg.RadarDetect.RDDurRegion == JAP) || (pAd->CommonCfg.RadarDetect.RDDurRegion == JAP_W53) || (pAd->CommonCfg.RadarDetect.RDDurRegion == JAP_W56)))
				pAd->CommonCfg.CarrierDetect.Enable = 1;
		}
#endif // MERGE_ARCH_TEAM //

		if (pAd->CommonCfg.CarrierDetect.Enable == TRUE)
		{
#ifdef RT305x
			NewCarrierDetectionStart(pAd);
#else // original RT28xx source code
			// trun on Carrier-Detection. (Carrier-Detect with CTS protection).
			CarrierDetectionStart(pAd, 1);
#endif // RT305x //
		}
#endif // CARRIER_DETECTION_SUPPORT //
#endif // CONFIG_AP_SUPPORT //
	}

	// Pre-tbtt interrupt setting.
	RTMP_IO_READ32(pAd, INT_TIMER_CFG, &Value);
	Value &= 0xffff0000;
	Value |= 6 << 4; // Pre-TBTT is 6ms before TBTT interrupt. 1~10 ms is reasonable.
	RTMP_IO_WRITE32(pAd, INT_TIMER_CFG, Value);
	// Enable pre-tbtt interrupt
	RTMP_IO_READ32(pAd, INT_TIMER_EN, &Value);
	Value |=0x1;
	RTMP_IO_WRITE32(pAd, INT_TIMER_EN, Value);

	// Set LED
	RTMPSetLED(pAd, LED_LINK_UP);

#ifdef WAPI_SUPPORT
	RTMPStartWapiRekeyTimerAction(pAd, NULL);
#endif // WAPI_SUPPORT //

	// Init some variable
	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		if (pAd->MacTab.Content[i].ValidAsCLI)
		{
			pAd->MacTab.Content[i].PortSecured  = WPA_802_1X_PORT_NOT_SECURED;
		}
	}

	// Init pairwise key table, re-set all WCID entry as NO-security mode.
	for (i = 0; i < MAX_LEN_OF_MAC_TABLE; i++)
		AsicRemovePairwiseKeyEntry(pAd, BSS0, (UCHAR)i);
		
	// Init Security variables
	for (apidx = 0; apidx < pAd->ApCfg.BssidNum; apidx++)
	{
		USHORT		Wcid = 0;	
		PMULTISSID_STRUCT	pMbss = &pAd->ApCfg.MBSSID[apidx];

		pMbss->PortSecured = WPA_802_1X_PORT_NOT_SECURED;

		if (IS_WPA_CAPABILITY(pMbss->AuthMode))
		{   
			pMbss->DefaultKeyId = 1;
		}

		// initialize IVEIV in Asic			  
		GET_GroupKey_WCID(Wcid, apidx);
		AsicUpdateWCIDIVEIV(pAd, Wcid, 1, 0);

		// When WEP, TKIP or AES is enabled, set group key info to Asic
		if (pMbss->WepStatus == Ndis802_11WEPEnabled)
		{
    			UCHAR	CipherAlg;
			UCHAR	idx;
    			PUCHAR	Key;    		   			

			for (idx=0; idx < SHARE_KEY_NUM; idx++)
			{
				CipherAlg = pAd->SharedKey[apidx][idx].CipherAlg;
    				Key = pAd->SharedKey[apidx][idx].Key;

				if (pAd->SharedKey[apidx][idx].KeyLen > 0)
				{
					// Set key material to Asic
    					AsicAddSharedKeyEntry(pAd, apidx, idx, CipherAlg, Key, NULL, NULL);	
		
					if (idx == pMbss->DefaultKeyId)
					{
						// Update WCID attribute table and IVEIV table for this group key table  
						RTMPAddWcidAttributeEntry(pAd, apidx, idx, CipherAlg, NULL);
					}
				}
			}
    		}
		else if ((pMbss->WepStatus == Ndis802_11Encryption2Enabled) ||
				 (pMbss->WepStatus == Ndis802_11Encryption3Enabled) ||
				 (pMbss->WepStatus == Ndis802_11Encryption4Enabled))
		{
			// Init Group-Key-related variables
			GenRandom(pAd, pMbss->Bssid, pMbss->GMK);
			GenRandom(pAd, pMbss->Bssid, pMbss->GNonce);		

			// Group rekey related
			if ((pMbss->WPAREKEY.ReKeyInterval != 0) 
				&& ((pMbss->WPAREKEY.ReKeyMethod == TIME_REKEY) || (
					pMbss->WPAREKEY.ReKeyMethod == PKT_REKEY))) 
			{
				// Regularly check the timer
				if (pMbss->REKEYTimerRunning == FALSE)
				{
					RTMPSetTimer(&pMbss->REKEYTimer, GROUP_KEY_UPDATE_EXEC_INTV);

					pMbss->REKEYTimerRunning = TRUE;
					pMbss->REKEYCOUNTER = 0;
				}
				DBGPRINT(RT_DEBUG_TRACE, (" %s : Group rekey method= %ld , interval = 0x%lx\n",
											__FUNCTION__, pMbss->WPAREKEY.ReKeyMethod,
											pMbss->WPAREKEY.ReKeyInterval));
			}
			else
				pMbss->REKEYTimerRunning = FALSE;

			// Count GTK for this BSSID
			WpaDeriveGTK(pMbss->GMK, (UCHAR*)pMbss->GNonce, pMbss->Bssid, pMbss->GTK, TKIP_GTK_LENGTH);
			pAd->SharedKey[apidx][pMbss->DefaultKeyId].KeyLen = LEN_TKIP_EK;
			NdisMoveMemory(pAd->SharedKey[apidx][pMbss->DefaultKeyId].Key, pMbss->GTK, LEN_TKIP_EK);
			NdisMoveMemory(pAd->SharedKey[apidx][pMbss->DefaultKeyId].TxMic, &pMbss->GTK[16], LEN_TKIP_TXMICK);
			NdisMoveMemory(pAd->SharedKey[apidx][pMbss->DefaultKeyId].RxMic, &pMbss->GTK[24], LEN_TKIP_RXMICK);            

			if (pMbss->GroupKeyWepStatus == Ndis802_11Encryption2Enabled)
				pAd->SharedKey[apidx][pMbss->DefaultKeyId].CipherAlg = CIPHER_TKIP;
			else if (pMbss->GroupKeyWepStatus == Ndis802_11Encryption3Enabled)
				pAd->SharedKey[apidx][pMbss->DefaultKeyId].CipherAlg = CIPHER_AES;
	        	else
				pAd->SharedKey[apidx][pMbss->DefaultKeyId].CipherAlg = CIPHER_NONE;
            
        		// install Group Key to MAC ASIC
		        AsicAddSharedKeyEntry(
							pAd, 
							apidx, 
							pMbss->DefaultKeyId, 
							pAd->SharedKey[apidx][pMbss->DefaultKeyId].CipherAlg, 
							pAd->SharedKey[apidx][pMbss->DefaultKeyId].Key, 
							pAd->SharedKey[apidx][pMbss->DefaultKeyId].TxMic, 
							pAd->SharedKey[apidx][pMbss->DefaultKeyId].RxMic);
        
			// update Group key information to ASIC
			RTMPAddWcidAttributeEntry(
				pAd, 
				apidx, 
				pMbss->DefaultKeyId, 
				pAd->SharedKey[apidx][pMbss->DefaultKeyId].CipherAlg,
				NULL);
		
		}
#ifdef WAPI_SUPPORT
		else if (pMbss->WepStatus == Ndis802_11EncryptionSMS4Enabled)
		{	
			INT	cnt;
		
			// Initial the related variables
			pMbss->DefaultKeyId = 0;
			NdisMoveMemory(pMbss->key_announce_flag, AE_BCAST_PN, LEN_TX_IV);
			NdisMoveMemory(pMbss->tx_iv, AE_BCAST_PN, LEN_TX_IV);			

			// Generate NMK randomly
			for (cnt = 0; cnt < 16; cnt++)
				pMbss->NMK[cnt] = RandomByte(pAd);
			
			RTMPCalculateWapiGTK(pAd, pMbss->NMK, pMbss->GTK);
		}
#endif // WAPI_SUPPORT //

		// Send singal to daemon to indicate driver had restarted
		if ((pMbss->AuthMode == Ndis802_11AuthModeWPA) || (pMbss->AuthMode == Ndis802_11AuthModeWPA2)
        		|| (pMbss->AuthMode == Ndis802_11AuthModeWPA1WPA2) || (pMbss->IEEE8021X == TRUE))
		{
			POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
		
	        	SendSingalToDaemon(SIGUSR1, pObj->apd_pid);
    		}

#ifdef CONFIG_IS_ASUS
		DBGPRINT(RT_DEBUG_TRACE, ("### BSS(%d) AuthMode(%d)=%s, WepStatus(%d)=%s , AccessControlList.Policy=%ld\n", apidx, pMbss->AuthMode, GetAuthMode(pMbss->AuthMode), 
																  pMbss->WepStatus, GetEncryptType(pMbss->WepStatus), pMbss->AccessControlList.Policy));
#endif
	}

	// Disable Protection first.
	//AsicUpdateProtect(pAd, 0, (ALLN_SETPROTECT|CCKSETPROTECT|OFDMSETPROTECT), TRUE, FALSE);
#ifdef PIGGYBACK_SUPPORT
	RTMPSetPiggyBack(pAd, pAd->CommonCfg.bPiggyBackCapable);
#endif // PIGGYBACK_SUPPORT //

	ApLogEvent(pAd, pAd->CurrentAddress, EVENT_RESET_ACCESS_POINT);
	pAd->Mlme.PeriodicRound = 0;
	pAd->Mlme.OneSecPeriodicRound = 0;

	OPSTATUS_SET_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED);

	RTMP_IndicateMediaState(pAd);


	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);

#ifdef WDS_SUPPORT
	// Prepare WEP key
	WdsPrepareWepKeyFromMainBss(pAd);

	// Add wds key infomation to ASIC	
	AsicUpdateWdsRxWCIDTable(pAd);
#endif // WDS_SUPPORT //

#ifdef IDS_SUPPORT
	// Start IDS timer
	if (pAd->ApCfg.IdsEnable)
	{
		if (pAd->CommonCfg.bWirelessEvent == FALSE)
			DBGPRINT(RT_DEBUG_WARN, ("!!! WARNING !!! The WirelessEvent parameter doesn't be enabled \n"));
		
		RTMPIdsStart(pAd);
	}
#endif // IDS_SUPPORT //


#ifdef MESH_SUPPORT
	if (MESH_ON(pAd))
		MeshUp(pAd);
#endif // MESH_SUPPORT //


	DBGPRINT(RT_DEBUG_TRACE, ("<=== APStartUp\n"));
}

/*
	==========================================================================
	Description:
		disassociate all STAs and stop AP service.
	Note:
	==========================================================================
 */
VOID APStop(
	IN PRTMP_ADAPTER pAd) 
{
	BOOLEAN     Cancelled;
	UINT32		Value;
	INT			apidx;
	
	DBGPRINT(RT_DEBUG_TRACE, ("!!! APStop !!!\n"));

#ifdef DFS_SUPPORT
#ifdef RTMP_RBUS_SUPPORT
#ifdef NEW_DFS
	if ((pAd->MACVersion == 0x28720200) && (pAd->CommonCfg.CID == 0x200))
		NewRadarDetectionStop(pAd);
	else
#endif // NEW_DFS //
#endif // RTMP_RBUS_SUPPORT //
	{
		RadarDetectionStop(pAd);
		BbpRadarDetectionStop(pAd);
	}
#endif // DFS_SUPPORT //

#ifdef MESH_SUPPORT
	if (MESH_ON(pAd))
		MeshDown(pAd, TRUE);
#endif // MESH_SUPPORT //


#ifdef RT3XXX_ANTENNA_DIVERSITY_SUPPORT
	RT3XXX_AntDiversity_Fini(pAd);
#endif // RT3XXX_ANTENNA_DIVERSITY_SUPPORT //


#ifdef WDS_SUPPORT
	WdsDown(pAd);
#endif // WDS_SUPPORT //

	MacTableReset(pAd);

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);

	// Disable pre-tbtt interrupt
	RTMP_IO_READ32(pAd, INT_TIMER_EN, &Value);
	Value &=0xe;
	RTMP_IO_WRITE32(pAd, INT_TIMER_EN, Value);
	// Disable piggyback
	RTMPSetPiggyBack(pAd, FALSE);

   	AsicUpdateProtect(pAd, 0,  (ALLN_SETPROTECT|CCKSETPROTECT|OFDMSETPROTECT), TRUE, FALSE);

	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		//RTMP_ASIC_INTERRUPT_DISABLE(pAd);
		AsicDisableSync(pAd);
		// Set LED
		RTMPSetLED(pAd, LED_LINK_DOWN);
	}



	for (apidx = 0; apidx < MAX_MBSSID_NUM; apidx++)
	{
		if (pAd->ApCfg.MBSSID[apidx].REKEYTimerRunning == TRUE)
		{
			RTMPCancelTimer(&pAd->ApCfg.MBSSID[apidx].REKEYTimer, &Cancelled);
			pAd->ApCfg.MBSSID[apidx].REKEYTimerRunning = FALSE;
		}
	}

	if (pAd->ApCfg.CMTimerRunning == TRUE)
	{
		RTMPCancelTimer(&pAd->ApCfg.CounterMeasureTimer, &Cancelled);
		pAd->ApCfg.CMTimerRunning = FALSE;
	}
	
#ifdef WAPI_SUPPORT
	RTMPCancelWapiRekeyTimerAction(pAd, NULL);
#endif // WAPI_SUPPORT //
	
	//
	// Cancel the Timer, to make sure the timer was not queued.
	//
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED);

#ifdef IDS_SUPPORT
	// if necessary, cancel IDS timer
	RTMPIdsStop(pAd);
#endif // IDS_SUPPORT //


}

/*
	==========================================================================
	Description:
		This routine is used to clean up a specified power-saving queue. It's
		used whenever a wireless client is deleted.
	==========================================================================
 */
VOID APCleanupPsQueue(
	IN  PRTMP_ADAPTER   pAd,
	IN  PQUEUE_HEADER   pQueue)
{
	PQUEUE_ENTRY pEntry;
	PNDIS_PACKET pPacket;

	DBGPRINT(RT_DEBUG_TRACE, ("APCleanupPsQueue (0x%08lx)...\n", (ULONG)pQueue));

	while (pQueue->Head)
	{
		DBGPRINT(RT_DEBUG_TRACE,
					("APCleanupPsQueue %ld...\n",pQueue->Number));

		pEntry = RemoveHeadQueue(pQueue);
		//pPacket = CONTAINING_RECORD(pEntry, NDIS_PACKET, MiniportReservedEx);
		pPacket = QUEUE_ENTRY_TO_PACKET(pEntry);
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
	}
}

/*
	==========================================================================
	Description:
		This routine is called by APMlmePeriodicExec() every second to check if
		1. any associated client in PSM. If yes, then TX MCAST/BCAST should be
		   out in DTIM only
		2. any client being idle for too long and should be aged-out from MAC table
		3. garbage collect PSQ
	==========================================================================
*/
VOID MacTableMaintenance(
	IN PRTMP_ADAPTER pAd)
{
	int i;
#ifdef DOT11_N_SUPPORT
	BOOLEAN	bRdgActive;
#endif // DOT11_N_SUPPORT //
#ifdef RTMP_MAC_PCI
	unsigned long	IrqFlags;
#endif // RTMP_MAC_PCI //
	UINT	fAnyStationPortSecured[MAX_MBSSID_NUM];
 	UINT 	bss_index;
//2008/10/28: KH add to support Antenna power-saving of AP<--
#ifdef DOT11_N_SUPPORT
#endif // DOT11_N_SUPPORT //
//2008/10/28: KH add to support Antenna power-saving of AP-->

	for (bss_index = BSS0; bss_index < MAX_MBSSID_NUM; bss_index++)
		fAnyStationPortSecured[bss_index] = 0;

	pAd->MacTab.fAnyStationInPsm = FALSE;
	pAd->MacTab.fAnyStationBadAtheros = FALSE;
	pAd->MacTab.fAnyTxOPForceDisable = FALSE;
	pAd->MacTab.fAllStationAsRalink = TRUE;
#ifdef DOT11_N_SUPPORT
	pAd->MacTab.fAnyStationNonGF = FALSE;
	pAd->MacTab.fAnyStation20Only = FALSE;
	pAd->MacTab.fAnyStationIsLegacy = FALSE;
	pAd->MacTab.fAnyStationMIMOPSDynamic = FALSE;
//2008/10/28: KH add to support Antenna power-saving of AP<--
	pAd->MacTab.fAnyStationIsHT=FALSE;
//2008/10/28: KH add to support Antenna power-saving of AP-->

#endif // DOT11_N_SUPPORT //

#ifdef WAPI_SUPPORT
	pAd->MacTab.fAnyWapiStation = FALSE;
#endif // WAPI_SUPPORT //

	for (i = 1; i < MAX_LEN_OF_MAC_TABLE; i++) 
	{
		MAC_TABLE_ENTRY *pEntry = &pAd->MacTab.Content[i];
		BOOLEAN bDisconnectSta = FALSE;

		if (pEntry->ValidAsCLI == FALSE)
			continue;

		if (pEntry->NoDataIdleCount == 0)
			pEntry->StationKeepAliveCount = 0;

		pEntry->NoDataIdleCount ++;  
		pEntry->StaConnectTime ++;

		// 0. STA failed to complete association should be removed to save MAC table space.
		if ((pEntry->Sst != SST_ASSOC) && (pEntry->NoDataIdleCount >= pEntry->AssocDeadLine))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%02x:%02x:%02x:%02x:%02x:%02x fail to complete ASSOC in %d sec\n",
					pEntry->Addr[0],pEntry->Addr[1],pEntry->Addr[2],pEntry->Addr[3],
					pEntry->Addr[4],pEntry->Addr[5],MAC_TABLE_ASSOC_TIMEOUT));
			MacTableDeleteEntry(pAd, pEntry->Aid, pEntry->Addr);
			continue;
		}

		// 1. check if there's any associated STA in power-save mode. this affects outgoing
		//    MCAST/BCAST frames should be stored in PSQ till DtimCount=0
		if (pEntry->PsMode == PWR_SAVE)
			pAd->MacTab.fAnyStationInPsm = TRUE;

#ifdef DOT11_N_SUPPORT
		if (pEntry->MmpsMode == MMPS_DYNAMIC)
		{
			pAd->MacTab.fAnyStationMIMOPSDynamic = TRUE;
		}

		if (pEntry->MaxHTPhyMode.field.BW == BW_20)
			pAd->MacTab.fAnyStation20Only = TRUE;

		if (pEntry->MaxHTPhyMode.field.MODE != MODE_HTGREENFIELD)
			pAd->MacTab.fAnyStationNonGF = TRUE;

		if ((pEntry->MaxHTPhyMode.field.MODE == MODE_OFDM) || (pEntry->MaxHTPhyMode.field.MODE == MODE_CCK))
		{
			pAd->MacTab.fAnyStationIsLegacy = TRUE;
		}
//2008/10/28: KH add to support Antenna power-saving of AP<--
#ifdef CONFIG_AP_SUPPORT
		else
			pAd->MacTab.fAnyStationIsHT=TRUE;
#endif // CONFIG_AP_SUPPORT //
//2008/10/28: KH add to support Antenna power-saving of AP-->

#endif // DOT11_N_SUPPORT //
		
		if (pEntry->bIAmBadAtheros)
		{
			pAd->MacTab.fAnyStationBadAtheros = TRUE;
#ifdef DOT11_N_SUPPORT
			if (pAd->CommonCfg.IOTestParm.bRTSLongProtOn == FALSE)
				AsicUpdateProtect(pAd, 8, ALLN_SETPROTECT, FALSE, pAd->MacTab.fAnyStationNonGF);
#endif // DOT11_N_SUPPORT //
			if (pEntry->WepStatus != Ndis802_11EncryptionDisabled)
			{
				pAd->MacTab.fAnyTxOPForceDisable = TRUE;
			}
		}

		// detect the station alive status
		// detect the station alive status
		if ((pAd->ApCfg.MBSSID[pEntry->apidx].StationKeepAliveTime > 0) &&
			(pEntry->NoDataIdleCount >= pAd->ApCfg.MBSSID[pEntry->apidx].StationKeepAliveTime))
		{
			MULTISSID_STRUCT *pMbss = &pAd->ApCfg.MBSSID[pEntry->apidx];

			// if no any data success between ap and the station for StationKeepAliveTime,
			// try to detect whether the station is still alive
			// just only keepalive station function, no disassociation function
			// if too many no response
			if (pEntry->StationKeepAliveCount++ == 0)
			{
				if (pEntry->PsMode == PWR_SAVE)
				{
					// use TIM bit to detect the PS station
					WLAN_MR_TIM_BIT_SET(pAd, pEntry->apidx, pEntry->Aid);
				}
				else
				{
					// use Null or QoS Null to detect the ACTIVE station
					BOOLEAN bQosNull = FALSE;
	
					if (CLIENT_STATUS_TEST_FLAG(pEntry, fCLIENT_STATUS_WMM_CAPABLE))
						bQosNull = TRUE;

		            ApEnqueueNullFrame(pAd, pEntry->Addr, pEntry->CurrTxRate,
	    	                           pEntry->Aid, pEntry->apidx, bQosNull, TRUE, 0);
				}
			}
			else
			{
				if (pEntry->StationKeepAliveCount >= pMbss->StationKeepAliveTime)
					pEntry->StationKeepAliveCount = 0;
			}
		}

		// 2. delete those MAC entry that has been idle for a long time
		if (pEntry->PsMode == PWR_SAVE)
		{
			pEntry->ContinueTxFailCnt = 0;
		}

		if (pEntry->NoDataIdleCount >= MAC_TABLE_AGEOUT_TIME)
		{
			bDisconnectSta = TRUE;
			DBGPRINT(RT_DEBUG_WARN, ("ageout %02x:%02x:%02x:%02x:%02x:%02x after %d-sec silence\n",
					pEntry->Addr[0],pEntry->Addr[1],pEntry->Addr[2],pEntry->Addr[3],
					pEntry->Addr[4],pEntry->Addr[5],MAC_TABLE_AGEOUT_TIME));
			ApLogEvent(pAd, pEntry->Addr, EVENT_AGED_OUT);
		}
		else if (pEntry->ContinueTxFailCnt >= pAd->ApCfg.EntryLifeCheck)
		{
			bDisconnectSta = TRUE;
			DBGPRINT(RT_DEBUG_WARN, ("STA-%02x:%02x:%02x:%02x:%02x:%02x had left\n",
					pEntry->Addr[0],pEntry->Addr[1],pEntry->Addr[2],pEntry->Addr[3],
					pEntry->Addr[4],pEntry->Addr[5]));			
		}

		if (bDisconnectSta)
		{
			// send wireless event - for ageout 
			if (pAd->CommonCfg.bWirelessEvent)
				RTMPSendWirelessEvent(pAd, IW_AGEOUT_EVENT_FLAG, pEntry->Addr, 0, 0); 

			if (pEntry->Sst == SST_ASSOC)
			{
				PUCHAR      pOutBuffer = NULL;
				NDIS_STATUS NStatus;
				ULONG       FrameLen = 0;
				HEADER_802_11 DeAuthHdr;
				USHORT      Reason;

				//  send out a DISASSOC request frame
				NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);
				if (NStatus != NDIS_STATUS_SUCCESS) 
				{
					DBGPRINT(RT_DEBUG_TRACE, (" MlmeAllocateMemory fail  ..\n"));
					//NdisReleaseSpinLock(&pAd->MacTabLock);
					continue;
				}
				Reason = REASON_DEAUTH_STA_LEAVING;
				DBGPRINT(RT_DEBUG_WARN, ("Send DEAUTH - Reason = %d frame  TO %x %x %x %x %x %x \n",Reason,pEntry->Addr[0],
										pEntry->Addr[1],pEntry->Addr[2],pEntry->Addr[3],pEntry->Addr[4],pEntry->Addr[5]));
				MgtMacHeaderInit(pAd, &DeAuthHdr, SUBTYPE_DEAUTH, 0, pEntry->Addr, pAd->ApCfg.MBSSID[pEntry->apidx].Bssid);				
		    	MakeOutgoingFrame(pOutBuffer,            &FrameLen, 
		    	                  sizeof(HEADER_802_11), &DeAuthHdr, 
		    	                  2,                     &Reason, 
		    	                  END_OF_ARGS);				
		    	MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);
		    	MlmeFreeMemory(pAd, pOutBuffer);
			}

			MacTableDeleteEntry(pAd, pEntry->Aid, pEntry->Addr);
			continue;
		}

		// 3. garbage collect the PsQueue if the STA has being idle for a while
		if (pEntry->PsQueue.Head)
		{
			pEntry->PsQIdleCount ++;  
			if (pEntry->PsQIdleCount > 2) 
			{
				NdisAcquireSpinLock(&pAd->irq_lock);
				APCleanupPsQueue(pAd, &pEntry->PsQueue);
				NdisReleaseSpinLock(&pAd->irq_lock);
				pEntry->PsQIdleCount = 0;
				WLAN_MR_TIM_BIT_CLEAR(pAd, pEntry->apidx, pEntry->Aid);
			}
		}
		else
			pEntry->PsQIdleCount = 0;
	
#ifdef UAPSD_AP_SUPPORT
        UAPSD_QueueMaintenance(pAd, pEntry);
#endif // UAPSD_AP_SUPPORT //

		// check if this STA is Ralink-chipset 
		if (!CLIENT_STATUS_TEST_FLAG(pEntry, fCLIENT_STATUS_RALINK_CHIPSET))
			pAd->MacTab.fAllStationAsRalink = FALSE;

		// Check if the port is secured
		if (pEntry->PortSecured == WPA_802_1X_PORT_SECURED)
			fAnyStationPortSecured[pEntry->apidx]++;
#ifdef DOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
		if ((pEntry->BSS2040CoexistenceMgmtSupport) && 
			(pAd->CommonCfg.Bss2040CoexistFlag & BSS_2040_COEXIST_INFO_NOTIFY))
		{
			SendNotifyBWActionFrame(pAd, pEntry->Aid, pEntry->apidx);
	}
#endif // DOT11N_DRAFT3 //
#endif // DOT11_N_SUPPORT //
#ifdef WAPI_SUPPORT
		if (pEntry->WepStatus == Ndis802_11EncryptionSMS4Enabled)
			pAd->MacTab.fAnyWapiStation = TRUE;
#endif // WAPI_SUPPORT //

	}

	// Update the state of port per MBSS
	for (bss_index = BSS0; bss_index < MAX_MBSSID_NUM; bss_index++)
	{
		if (fAnyStationPortSecured[bss_index] > 0)
			pAd->ApCfg.MBSSID[bss_index].PortSecured = WPA_802_1X_PORT_SECURED;
		else
			pAd->ApCfg.MBSSID[bss_index].PortSecured = WPA_802_1X_PORT_NOT_SECURED;
	}

#ifdef DOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
	if (pAd->CommonCfg.Bss2040CoexistFlag & BSS_2040_COEXIST_INFO_NOTIFY)
		pAd->CommonCfg.Bss2040CoexistFlag &= (~BSS_2040_COEXIST_INFO_NOTIFY);
#endif // DOT11N_DRAFT3 //

	// If all associated STAs are Ralink-chipset, AP shall enable RDG.
	if (pAd->CommonCfg.bRdg && pAd->MacTab.fAllStationAsRalink)
	{
		bRdgActive = TRUE;
	}
	else
	{
		bRdgActive = FALSE;
	}
//2008/11/05: KH add to support Antenna power-saving of AP<--
#ifdef DOT11_N_SUPPORT
			if (pAd->Antenna.field.RxPath>1||pAd->Antenna.field.TxPath>1)
			{

				if(pAd->MacTab.fAnyStationIsHT==FALSE
					&&pAd->CommonCfg.bGreenAPEnable==TRUE)
					EnableAPMIMOPS(pAd);
				else
					DisableAPMIMOPS(pAd);

			}

#endif // DOT11_N_SUPPORT //
//2008/11/05: KH add to support Antenna power-saving of AP-->
	if (bRdgActive != RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE))
	{
		if (bRdgActive)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE);
			AsicEnableRDG(pAd);
		}
		else
		{
			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE);
			AsicDisableRDG(pAd);
		}
	}

	if ((pAd->MacTab.fAnyStationBadAtheros == FALSE) && (pAd->CommonCfg.IOTestParm.bRTSLongProtOn == TRUE))
	{
		AsicUpdateProtect(pAd, pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode, ALLN_SETPROTECT, FALSE, pAd->MacTab.fAnyStationNonGF);
	}
#endif // DOT11_N_SUPPORT //

#ifdef RTMP_MAC_PCI
	RTMP_IRQ_LOCK(&pAd->irq_lock, IrqFlags);
#endif // RTMP_MAC_PCI //
	// 4. garbage collect pAd->MacTab.McastPsQueue if backlogged MCAST/BCAST frames
	//    stale in queue. Since MCAST/BCAST frames always been sent out whenever 
	//    DtimCount==0, the only case to let them stale is surprise removal of the NIC,
	//    so that ASIC-based Tbcn interrupt stops and DtimCount dead.
	if (pAd->MacTab.McastPsQueue.Head)
	{
		UINT bss_index;

		pAd->MacTab.PsQIdleCount ++;
		if (pAd->MacTab.PsQIdleCount > 1)
		{
			/* Normally, should not be here;
			   because bc/mc packets will be moved to SwQueue when DTIM = 0 and
			   DTIM period < 2 seconds;
			   If enter here, it is the kernel bug or driver bug */

			//NdisAcquireSpinLock(&pAd->MacTabLock);
			APCleanupPsQueue(pAd, &pAd->MacTab.McastPsQueue);
			//NdisReleaseSpinLock(&pAd->MacTabLock);
			pAd->MacTab.PsQIdleCount = 0;

		        /* sanity check */
			if (pAd->ApCfg.BssidNum > MAX_MBSSID_NUM)
				pAd->ApCfg.BssidNum = MAX_MBSSID_NUM;
		        /* End of if */
	        
		        /* clear MCAST/BCAST backlog bit for all BSS */
			for(bss_index=BSS0; bss_index<pAd->ApCfg.BssidNum; bss_index++)
				WLAN_MR_TIM_BCMC_CLEAR(bss_index);
		        /* End of for */
		}
	}
	else
		pAd->MacTab.PsQIdleCount = 0;
#ifdef RTMP_MAC_PCI
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, IrqFlags);
#endif // RTMP_MAC_PCI //
}


UINT32 MacTableAssocStaNumGet(
	IN PRTMP_ADAPTER pAd)
{
	UINT32 num = 0;
	UINT32 i;


	for (i = 1; i < MAX_LEN_OF_MAC_TABLE; i++) 
	{
		MAC_TABLE_ENTRY *pEntry = &pAd->MacTab.Content[i];

		if (pEntry->ValidAsCLI == FALSE)
			continue;

		if (pEntry->Sst == SST_ASSOC)
			num ++;
	}

	return num;
}


/*
	==========================================================================
	Description:
		Look up a STA MAC table. Return its Sst to decide if an incoming
		frame from this STA or an outgoing frame to this STA is permitted.
	Return:
	==========================================================================
*/
MAC_TABLE_ENTRY *APSsPsInquiry(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PUCHAR pAddr, 
	OUT SST   *Sst, 
	OUT USHORT *Aid,
	OUT UCHAR *PsMode,
	OUT UCHAR *Rate) 
{
	MAC_TABLE_ENTRY *pEntry = NULL;
	
	if (MAC_ADDR_IS_GROUP(pAddr)) // mcast & broadcast address
	{
		*Sst        = SST_ASSOC;
		*Aid        = MCAST_WCID;	// Softap supports 1 BSSID and use WCID=0 as multicast Wcid index
		*PsMode     = PWR_ACTIVE;
		*Rate       = pAd->CommonCfg.MlmeRate; 
	} 
	else // unicast address
	{
		pEntry = MacTableLookup(pAd, pAddr);
		if (pEntry) 
		{
			*Sst        = pEntry->Sst;
			*Aid        = pEntry->Aid;
			*PsMode     = pEntry->PsMode;
			if ((pEntry->AuthMode >= Ndis802_11AuthModeWPA) && (pEntry->GTKState != REKEY_ESTABLISHED))
				*Rate   = pAd->CommonCfg.MlmeRate;
			else
			*Rate       = pEntry->CurrTxRate;
		} 
		else 
		{
			*Sst        = SST_NOT_AUTH;
			*Aid        = MCAST_WCID;
			*PsMode     = PWR_ACTIVE;
			*Rate       = pAd->CommonCfg.MlmeRate; 
		}
	}
	return pEntry;
}

/*
	==========================================================================
	Description:
		Update the station current power save mode. Calling this routine also
		prove the specified client is still alive. Otherwise AP will age-out
		this client once IdleCount exceeds a threshold.
	==========================================================================
 */
BOOLEAN APPsIndicate(
	IN PRTMP_ADAPTER pAd, 
	IN PUCHAR pAddr, 
	IN ULONG Wcid, 
	IN UCHAR Psm) 
{
	MAC_TABLE_ENTRY *pEntry;
    UCHAR old_psmode;

	if (Wcid >= MAX_LEN_OF_MAC_TABLE)
	{
		return PWR_ACTIVE;	
	}

	pEntry = &pAd->MacTab.Content[Wcid];
	old_psmode = pEntry->PsMode;
	//if (RTMPEqualMemory(pEntry->Addr, pAddr, MAC_ADDR_LEN)) 
	if (pEntry)
	{
		/*
			Change power save mode first because we will call
			RTMPDeQueuePacket() in APHandleRxPsPoll().

			Or when Psm = PWR_ACTIVE, we will not do Aggregation in
			RTMPDeQueuePacket().
		*/
		pEntry->NoDataIdleCount = 0;
		pEntry->PsMode = Psm;

		//if ((pEntry->PsMode == PWR_SAVE) && (Psm == PWR_ACTIVE))
		if ((old_psmode == PWR_SAVE) && (Psm == PWR_ACTIVE))
		{
			// TODO: For RT2870, how to handle about the BA when STA in PS mode????
#ifdef RTMP_MAC_PCI
#ifdef DOT11_N_SUPPORT
			// When sta wake up, we send BAR to refresh the BA sequence.
			SendRefreshBAR(pAd, pEntry);
#endif // DOT11_N_SUPPORT //
#endif // RTMP_MAC_PCI //
			DBGPRINT(RT_DEBUG_INFO, ("APPsIndicate - %02x:%02x:%02x:%02x:%02x:%02x wakes up, act like rx PS-POLL\n", pAddr[0],pAddr[1],pAddr[2],pAddr[3],pAddr[4],pAddr[5]));
			// sleep station awakes, move all pending frames from PSQ to TXQ if any
			APHandleRxPsPoll(pAd, pAddr, pEntry->Aid, TRUE);
		}
		else if ((pEntry->PsMode != PWR_SAVE) && (Psm == PWR_SAVE))
		{
			DBGPRINT(RT_DEBUG_INFO, ("APPsIndicate - %02x:%02x:%02x:%02x:%02x:%02x sleeps\n", pAddr[0],pAddr[1],pAddr[2],pAddr[3],pAddr[4],pAddr[5]));
		}

		/* move to above section */
//		pEntry->NoDataIdleCount = 0;
//		pEntry->PsMode = Psm;
	} 
	else 
	{
		// not in table, try to learn it ???? why bother?
	}
	return old_psmode;
}

/*
	==========================================================================
	Description:
		This routine is called to log a specific event into the event table.
		The table is a QUERY-n-CLEAR array that stop at full.
	==========================================================================
 */
VOID ApLogEvent(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR   pAddr,
	IN USHORT   Event)
{
	if (pAd->EventTab.Num < MAX_NUM_OF_EVENT)
	{
		RT_802_11_EVENT_LOG *pLog = &pAd->EventTab.Log[pAd->EventTab.Num];
		RTMP_GetCurrentSystemTime(&pLog->SystemTime);
		COPY_MAC_ADDR(pLog->Addr, pAddr);
		pLog->Event = Event;
		DBGPRINT_RAW(RT_DEBUG_TRACE,("LOG#%ld %02x:%02x:%02x:%02x:%02x:%02x %s\n",
			pAd->EventTab.Num, pAddr[0], pAddr[1], pAddr[2], 
			pAddr[3], pAddr[4], pAddr[5], pEventText[Event]));
		pAd->EventTab.Num += 1;
	}
}


#ifdef DOT11_N_SUPPORT
/*
	==========================================================================
	Description:
		Operationg mode is as defined at 802.11n for how proteciton in this BSS operates. 
		Ap broadcast the operation mode at Additional HT Infroamtion Element Operating Mode fields.
		802.11n D1.0 might has bugs so this operating mode use  EWC MAC 1.24 definition first.

		Called when receiving my bssid beacon or beaconAtJoin to update protection mode.
		40MHz or 20MHz protection mode in HT 40/20 capabale BSS.
		As STA, this obeys the operation mode in ADDHT IE.
		As AP, update protection when setting ADDHT IE and after new STA joined.
	==========================================================================
*/
VOID APUpdateOperationMode(
	IN PRTMP_ADAPTER pAd)
{
	pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode = 0;

	if ((pAd->ApCfg.LastNoneHTOLBCDetectTime + (5 * OS_HZ)) > pAd->Mlme.Now32) // non HT BSS exist within 5 sec
	{
		pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode = 1;
    	AsicUpdateProtect(pAd, pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode, ALLN_SETPROTECT, FALSE, TRUE);
	}

   	// If I am 40MHz BSS, and there exist HT-20MHz station. 
	// Update to 2 when it's zero.  Because OperaionMode = 1 or 3 has more protection.
	if ((pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode == 0) && (pAd->MacTab.fAnyStation20Only) && (pAd->CommonCfg.DesiredHtPhy.ChannelWidth == 1))
	{
		pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode = 2;
		AsicUpdateProtect(pAd, pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode, (ALLN_SETPROTECT), TRUE, pAd->MacTab.fAnyStationNonGF);
	}
		
	if (pAd->MacTab.fAnyStationIsLegacy)
	{
		pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode = 3;
		AsicUpdateProtect(pAd, pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode, (ALLN_SETPROTECT), TRUE, pAd->MacTab.fAnyStationNonGF);
	}
	
	pAd->CommonCfg.AddHTInfo.AddHtInfo2.NonGfPresent = pAd->MacTab.fAnyStationNonGF;
}
#endif // DOT11_N_SUPPORT //

/*
	==========================================================================
	Description:
		Update ERP IE and CapabilityInfo based on STA association status.
		The result will be auto updated into the next outgoing BEACON in next
		TBTT interrupt service routine
	==========================================================================
 */
VOID APUpdateCapabilityAndErpIe(
	IN PRTMP_ADAPTER pAd)
{
	UCHAR  i, ErpIeContent = 0;
	BOOLEAN ShortSlotCapable = pAd->CommonCfg.bUseShortSlotTime;
	UCHAR	apidx;
	BOOLEAN	bUseBGProtection;
	BOOLEAN	LegacyBssExist;


	if (pAd->CommonCfg.PhyMode == PHY_11B)
		return;

	for (i=1; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];
		if ((pEntry->ValidAsCLI == FALSE) || (pEntry->Sst != SST_ASSOC))
			continue;

		// at least one 11b client associated, turn on ERP.NonERPPresent bit
		// almost all 11b client won't support "Short Slot" time, turn off for maximum compatibility
		if (pEntry->MaxSupportedRate < RATE_FIRST_OFDM_RATE)
		{
			ShortSlotCapable = FALSE;
			ErpIeContent |= 0x01;
		}

		// at least one client can't support short slot
		if ((pEntry->CapabilityInfo & 0x0400) == 0)
			ShortSlotCapable = FALSE;
	}

	// legacy BSS exist within 5 sec
	if ((pAd->ApCfg.LastOLBCDetectTime + (5 * OS_HZ)) > pAd->Mlme.Now32) 
	{
		LegacyBssExist = TRUE;
	}
	else
	{
		LegacyBssExist = FALSE;
	}
	
	// decide ErpIR.UseProtection bit, depending on pAd->CommonCfg.UseBGProtection
	//    AUTO (0): UseProtection = 1 if any 11b STA associated
	//    ON (1): always USE protection
	//    OFF (2): always NOT USE protection
	if (pAd->CommonCfg.UseBGProtection == 0)
	{
		ErpIeContent = (ErpIeContent)? 0x03 : 0x00;
		//if ((pAd->ApCfg.LastOLBCDetectTime + (5 * OS_HZ)) > pAd->Mlme.Now32) // legacy BSS exist within 5 sec
		if (LegacyBssExist)
		{
			ErpIeContent |= 0x02;                                     // set Use_Protection bit
		}
	}
	else if (pAd->CommonCfg.UseBGProtection == 1)   
		ErpIeContent |= 0x02;
	else
		;

	bUseBGProtection = (pAd->CommonCfg.UseBGProtection == 1) ||    // always use
						((pAd->CommonCfg.UseBGProtection == 0) && ERP_IS_USE_PROTECTION(ErpIeContent));

	// always no BG protection in A-band. falsely happened when switching A/G band to a dual-band AP
	if (pAd->CommonCfg.Channel > 14) 
		bUseBGProtection = FALSE;

	if (bUseBGProtection != OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED))
	{
		USHORT OperationMode = 0;
		BOOLEAN	bNonGFExist = 0;

#ifdef DOT11_N_SUPPORT
		OperationMode = pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode;
		bNonGFExist = pAd->MacTab.fAnyStationNonGF;
#endif // DOT11_N_SUPPORT //
		if (bUseBGProtection)
		{
			OPSTATUS_SET_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED);
			AsicUpdateProtect(pAd, OperationMode, (OFDMSETPROTECT), FALSE, bNonGFExist);
		}
		else
		{
			OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED);
			AsicUpdateProtect(pAd, OperationMode, (OFDMSETPROTECT), TRUE, bNonGFExist);
		}
	}

	// Decide Barker Preamble bit of ERP IE
	if ((pAd->CommonCfg.TxPreamble == Rt802_11PreambleLong) || (ApCheckLongPreambleSTA(pAd) == TRUE))
		pAd->ApCfg.ErpIeContent = (ErpIeContent | 0x04);
	else
		pAd->ApCfg.ErpIeContent = ErpIeContent;

	// Force to use ShortSlotTime at A-band
	if (pAd->CommonCfg.Channel > 14)
		ShortSlotCapable = TRUE;
	
	//
	// deicide CapabilityInfo.ShortSlotTime bit
	//
    for (apidx=0; apidx<pAd->ApCfg.BssidNum; apidx++)
    {
		// In A-band, the ShortSlotTime bit should be ignored. 
		if (ShortSlotCapable && (pAd->CommonCfg.Channel <= 14))
    		pAd->ApCfg.MBSSID[apidx].CapabilityInfo |= 0x0400;
		else
    		pAd->ApCfg.MBSSID[apidx].CapabilityInfo &= 0xfbff;


   		if (pAd->CommonCfg.TxPreamble == Rt802_11PreambleLong)
			pAd->ApCfg.MBSSID[apidx].CapabilityInfo &= (~0x020);
		else
			pAd->ApCfg.MBSSID[apidx].CapabilityInfo |= 0x020;

	}

	AsicSetSlotTime(pAd, ShortSlotCapable);

}

/*
	==========================================================================
	Description:
        Check to see the exist of long preamble STA in associated list
    ==========================================================================
 */
BOOLEAN ApCheckLongPreambleSTA(
    IN PRTMP_ADAPTER pAd)
{
    UCHAR   i;
    
    for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
    {
		PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];
		if ((pEntry->ValidAsCLI == FALSE) || (pEntry->Sst != SST_ASSOC))
			continue;
	            
        if (!CAP_IS_SHORT_PREAMBLE_ON(pEntry->CapabilityInfo))
        {
            return TRUE;
        }
    }

    return FALSE;
}    

/*
	==========================================================================
	Description:
		Check if the specified STA pass the Access Control List checking.
		If fails to pass the checking, then no authentication nor association 
		is allowed
	Return:
		MLME_SUCCESS - this STA passes ACL checking

	==========================================================================
*/
BOOLEAN ApCheckAccessControlList(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR        pAddr,
	IN UCHAR         Apidx)
{
	BOOLEAN Result = TRUE;

    if (pAd->ApCfg.MBSSID[Apidx].AccessControlList.Policy == 0)       // ACL is disabled
        Result = TRUE;
    else
    {
        ULONG i;
        if (pAd->ApCfg.MBSSID[Apidx].AccessControlList.Policy == 1)   // ACL is a positive list
            Result = FALSE;
        else                                              // ACL is a negative list
            Result = TRUE;
        for (i=0; i<pAd->ApCfg.MBSSID[Apidx].AccessControlList.Num; i++)
        {
            if (MAC_ADDR_EQUAL(pAddr, pAd->ApCfg.MBSSID[Apidx].AccessControlList.Entry[i].Addr))
            {
                Result = !Result;
                break;
            }
        }
    }

    if (Result == FALSE)
    {
        DBGPRINT(RT_DEBUG_TRACE, ("%02x:%02x:%02x:%02x:%02x:%02x failed in ACL checking\n",
        pAddr[0],pAddr[1],pAddr[2],pAddr[3],pAddr[4],pAddr[5]));
    }

    return Result;
}

/*
	==========================================================================
	Description:
		This routine update the current MAC table based on the current ACL.
		If ACL change causing an associated STA become un-authorized. This STA
		will be kicked out immediately.
	==========================================================================
*/
VOID ApUpdateAccessControlList(
    IN PRTMP_ADAPTER pAd,
    IN UCHAR         Apidx)
{
	USHORT   AclIdx, MacIdx;
	BOOLEAN  Matched;

	PUCHAR      pOutBuffer = NULL;
	NDIS_STATUS NStatus;
	ULONG       FrameLen = 0;
	HEADER_802_11 DisassocHdr;
	USHORT      Reason;

	
	//Apidx = pObj->ioctl_if;
	ASSERT(Apidx <= MAX_MBSSID_NUM);
	DBGPRINT(RT_DEBUG_TRACE, ("ApUpdateAccessControlList : Apidx = %d\n", Apidx));
	
    // ACL is disabled. Do nothing about the MAC table.
    if (pAd->ApCfg.MBSSID[Apidx].AccessControlList.Policy == 0)
		return;

	for (MacIdx=0; MacIdx < MAX_LEN_OF_MAC_TABLE; MacIdx++)
	{
		if (! pAd->MacTab.Content[MacIdx].ValidAsCLI) 
			continue;

		//
		// We only need to update associations related to ACL of MBSSID[Apidx].
		//
        if (pAd->MacTab.Content[MacIdx].apidx != Apidx) 
            continue;
    
		Matched = FALSE;
        for (AclIdx = 0; AclIdx < pAd->ApCfg.MBSSID[Apidx].AccessControlList.Num; AclIdx++)
		{
            if (MAC_ADDR_EQUAL(&pAd->MacTab.Content[MacIdx].Addr, pAd->ApCfg.MBSSID[Apidx].AccessControlList.Entry[AclIdx].Addr))
			{
				Matched = TRUE;
				break;
			}
		}

        if ((Matched == FALSE) && (pAd->ApCfg.MBSSID[Apidx].AccessControlList.Policy == 1))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Apidx = %d\n", Apidx));
			DBGPRINT(RT_DEBUG_TRACE, ("pAd->ApCfg.MBSSID[%d].AccessControlList.Policy = %ld\n", Apidx,
				pAd->ApCfg.MBSSID[Apidx].AccessControlList.Policy));
			DBGPRINT(RT_DEBUG_TRACE, ("STA not on positive ACL. remove it...\n"));
			
			// Before delete the entry from MacTable, send disassociation packet to client.
			if (pAd->MacTab.Content[MacIdx].Sst == SST_ASSOC)
			{
				//  send out a DISASSOC frame
				NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);
				if (NStatus != NDIS_STATUS_SUCCESS) 
				{
					DBGPRINT(RT_DEBUG_TRACE, (" MlmeAllocateMemory fail  ..\n"));
					return;
				}

				Reason = REASON_DECLINED;
				DBGPRINT(RT_DEBUG_ERROR, ("ASSOC - Send DISASSOC  Reason = %d frame  TO %x %x %x %x %x %x \n",Reason,pAd->MacTab.Content[MacIdx].Addr[0],
					pAd->MacTab.Content[MacIdx].Addr[1],pAd->MacTab.Content[MacIdx].Addr[2],pAd->MacTab.Content[MacIdx].Addr[3],pAd->MacTab.Content[MacIdx].Addr[4],pAd->MacTab.Content[MacIdx].Addr[5]));
				MgtMacHeaderInit(pAd, &DisassocHdr, SUBTYPE_DISASSOC, 0, pAd->MacTab.Content[MacIdx].Addr, pAd->ApCfg.MBSSID[pAd->MacTab.Content[MacIdx].apidx].Bssid);
				MakeOutgoingFrame(pOutBuffer, &FrameLen, sizeof(HEADER_802_11), &DisassocHdr, 2, &Reason, END_OF_ARGS);
				MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);
				MlmeFreeMemory(pAd, pOutBuffer);

				RTMPusecDelay(5000);
			}
			MacTableDeleteEntry(pAd, pAd->MacTab.Content[MacIdx].Aid, pAd->MacTab.Content[MacIdx].Addr);
		}
        else if ((Matched == TRUE) && (pAd->ApCfg.MBSSID[Apidx].AccessControlList.Policy == 2))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Apidx = %d\n", Apidx));
			DBGPRINT(RT_DEBUG_TRACE, ("pAd->ApCfg.MBSSID[%d].AccessControlList.Policy = %ld\n", Apidx,
				pAd->ApCfg.MBSSID[Apidx].AccessControlList.Policy));
			DBGPRINT(RT_DEBUG_TRACE, ("STA on negative ACL. remove it...\n"));
			
			// Before delete the entry from MacTable, send disassociation packet to client.
			if (pAd->MacTab.Content[MacIdx].Sst == SST_ASSOC)
			{
				// send out a DISASSOC frame
				NStatus = MlmeAllocateMemory(pAd, &pOutBuffer);
				if (NStatus != NDIS_STATUS_SUCCESS) 
				{
					DBGPRINT(RT_DEBUG_TRACE, (" MlmeAllocateMemory fail  ..\n"));
					return;
				}

				Reason = REASON_DECLINED;
				DBGPRINT(RT_DEBUG_ERROR, ("ASSOC - Send DISASSOC  Reason = %d frame  TO %x %x %x %x %x %x \n",Reason,pAd->MacTab.Content[MacIdx].Addr[0],
					pAd->MacTab.Content[MacIdx].Addr[1],pAd->MacTab.Content[MacIdx].Addr[2],pAd->MacTab.Content[MacIdx].Addr[3],pAd->MacTab.Content[MacIdx].Addr[4],pAd->MacTab.Content[MacIdx].Addr[5]));
				MgtMacHeaderInit(pAd, &DisassocHdr, SUBTYPE_DISASSOC, 0, pAd->MacTab.Content[MacIdx].Addr, pAd->ApCfg.MBSSID[pAd->MacTab.Content[MacIdx].apidx].Bssid);
				MakeOutgoingFrame(pOutBuffer, &FrameLen, sizeof(HEADER_802_11), &DisassocHdr, 2, &Reason, END_OF_ARGS);
				MiniportMMRequest(pAd, 0, pOutBuffer, FrameLen);
				MlmeFreeMemory(pAd, pOutBuffer);

				RTMPusecDelay(5000);
			}
			MacTableDeleteEntry(pAd, pAd->MacTab.Content[MacIdx].Aid, pAd->MacTab.Content[MacIdx].Addr);
		}
	}
}

/* 
	==========================================================================
	Description:
		Send out a NULL frame to a specified STA at a higher TX rate. The 
		purpose is to ensure the designated client is okay to received at this
		rate.
	==========================================================================
 */
VOID ApEnqueueNullFrame(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR        pAddr,
	IN UCHAR         TxRate,
	IN UCHAR         PID,
	IN UCHAR         apidx,
    IN BOOLEAN       bQosNull,
    IN BOOLEAN       bEOSP,
    IN UCHAR         OldUP)
{
	NDIS_STATUS    NState;
	PHEADER_802_11 pNullFr;
	PUCHAR pFrame;
    ULONG		   Length;


	// since TxRate may change, we have to change Duration each time
	NState = MlmeAllocateMemory(pAd, (PUCHAR *)&pFrame);
	pNullFr = (PHEADER_802_11) pFrame;
    Length = sizeof(HEADER_802_11);

	if (NState == NDIS_STATUS_SUCCESS) 
	{
//		if ((PID & 0x3f) < WDS_PAIRWISE_KEY_OFFSET) // send to client
		{
			MgtMacHeaderInit(pAd, pNullFr, SUBTYPE_NULL_FUNC, 0, pAddr, pAd->ApCfg.MBSSID[apidx].Bssid);
			pNullFr->FC.Type = BTYPE_DATA;
			pNullFr->FC.FrDs = 1;
			pNullFr->Duration = RTMPCalcDuration(pAd, TxRate, 14);

#ifdef UAPSD_AP_SUPPORT
            if (bQosNull)
			{
                UCHAR *qos_p = ((UCHAR *)pNullFr) + Length;

				pNullFr->FC.SubType = SUBTYPE_QOS_NULL;

				/* copy QOS control bytes */
				qos_p[0] = ((bEOSP) ? (1 << 4) : 0) | OldUP;
				qos_p[1] = 0;
				Length += 2;
			} /* End of if */
#endif // UAPSD_AP_SUPPORT //

			DBGPRINT(RT_DEBUG_INFO, ("send NULL Frame @%d Mbps to AID#%d...\n", RateIdToMbps[TxRate], PID & 0x3f));
            MiniportMMRequest(pAd, MapUserPriorityToAccessCategory[7], (PUCHAR)pNullFr, Length);
		}
//#ifdef  WDS
	MlmeFreeMemory(pAd, pFrame);
	}
}

VOID    ApSendFrame(
	IN  PRTMP_ADAPTER   pAd,
	IN  PVOID           pBuffer,
	IN  ULONG           Length,
	IN  UCHAR           TxRate,
	IN  UCHAR           PID)
{
}

/* 
	==========================================================================
	Description:
		Send out a ACK frame to a specified STA upon receiving PS-POLL
	==========================================================================
 */
VOID ApEnqueueAckFrame(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR        pAddr,
	IN UCHAR         TxRate,
	IN UCHAR		 apidx) 
{
	NDIS_STATUS    NState;
	PHEADER_802_11  pAckFr;
	PUCHAR			pFrame;

	// since TxRate may change, we have to change Duration each time
	NState = MlmeAllocateMemory(pAd, &pFrame);
	pAckFr = (PHEADER_802_11) pFrame;
	if (NState == NDIS_STATUS_SUCCESS) 
	{
		MgtMacHeaderInit(pAd, pAckFr, SUBTYPE_ACK, 0, pAddr, pAd->ApCfg.MBSSID[apidx].Bssid);
		pAckFr->FC.Type = BTYPE_CNTL;
		MiniportMMRequest(pAd, 0, (PUCHAR)pAckFr, 10);
		MlmeFreeMemory(pAd, pFrame);
	}
}

VOID APSwitchChannel(
	IN PRTMP_ADAPTER pAd,
	IN INT Channel)
{
	INT CentralChannel = Channel;
#ifdef DOT11_N_SUPPORT
	UCHAR byteValue = 0;
	UINT32 Value;
	
	if ((pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40) && (pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset == EXTCHA_ABOVE))
	{
#ifdef COC_SUPPORT
		if (pAd->CoC_sleep == 1)
			CentralChannel = Channel;
		else
#endif // COC_SUPPORT
			CentralChannel = Channel + 2;

		//  TX : control channel at lower 
		RTMP_IO_READ32(pAd, TX_BAND_CFG, &Value);
		Value &= (~0x1);
		RTMP_IO_WRITE32(pAd, TX_BAND_CFG, Value);

		//  RX : control channel at lower 
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, 3, &byteValue);
		byteValue &= (~0x20);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, 3, byteValue);

	}
	else if ((Channel > 2) && (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40) && (pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset == EXTCHA_BELOW))
	{
#ifdef COC_SUPPORT
		if (pAd->CoC_sleep == 1)
			CentralChannel = Channel;
		else
#endif // COC_SUPPORT
			CentralChannel = Channel - 2;

		//  TX : control channel at upper 
		RTMP_IO_READ32(pAd, TX_BAND_CFG, &Value);
		Value |= (0x1);		
		RTMP_IO_WRITE32(pAd, TX_BAND_CFG, Value);

		//  RX : control channel at upper 
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, 3, &byteValue);
		byteValue |= (0x20);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, 3, byteValue);
	}
#endif // DOT11_N_SUPPORT //

	AsicSwitchChannel(pAd, CentralChannel, TRUE);

}

#ifdef DOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
/*
	Depends on the 802.11n Draft 4.0, Before the HT AP start a BSS, it should scan some specific channels to
collect information of existing BSSs, then depens on the collected channel information, adjust the primary channel 
and secondary channel setting.

	For 5GHz,
		Rule 1: If the AP chooses to start a 20/40 MHz BSS in 5GHz and that occupies the same two channels
				as any existing 20/40 MHz BSSs, then the AP shall ensure that the primary channel of the 
				new BSS is identical to the primary channel of the existing 20/40 MHz BSSs and that the 
				secondary channel of the new 20/40 MHz BSS is identical to the secondary channel of the 
				existing 20/40 MHz BSSs, unless the AP discoverr that on those two channels are existing
				20/40 MHz BSSs with different primary and secondary channels.
		Rule 2: If the AP chooses to start a 20/40MHz BSS in 5GHz, the selected secondary channel should
				correspond to a channel on which no beacons are detected during the overlapping BSS
				scan time performed by the AP, unless there are beacons detected on both the selected
				primary and secondary channels.
		Rule 3: An HT AP should not start a 20 MHz BSS in 5GHz on a channel that is the secondary channel 
				of a 20/40 MHz BSS.
	For 2.4GHz,
		Rule 1: The AP shall not start a 20/40 MHz BSS in 2.4GHz if the value of the local variable "20/40
				Operation Permitted" is FALSE.

		20/40OperationPermitted =  (P == OPi for all values of i) AND
								(P == OTi for all values of i) AND
								(S == OSi for all values if i)
		where
			P 	is the operating or intended primary channel of the 20/40 MHz BSS
			S	is the operating or intended secondary channel of the 20/40 MHz BSS
			OPi  is member i of the set of channels that are members of the channel set C and that are the
				primary operating channel of at least one 20/40 MHz BSS that is detected within the AP's 
				BSA during the previous X seconds
			OSi  is member i of the set of channels that are members of the channel set C and that are the
				secondary operating channel of at least one 20/40 MHz BSS that is detected within AP's
				BSA during the previous X seconds
			OTi  is member i of the set of channels that comparises all channels that are members of the 
				channel set C that were listed once in the Channel List fields of 20/40 BSS Intolerant Channel
				Report elements receved during the previous X seconds and all channels that are members
				of the channel set C and that are the primary operating channel of at least one 20/40 MHz
				BSS that were detected within the AP's BSA during the previous X seconds.
			C	is the set of all channels that are allowed operating channels within the current operational
				regulatory domain and whose center frequency falls within the 40 MHz affected channel 
				range given by following equation:
					                                                 Fp + Fs                  Fp + Fs
					40MHz affected channel range = [ ------  - 25MHz,  ------- + 25MHz ]
					                                                      2                          2
					Where 
						Fp = the center frequency of channel P
						Fs = the center frequency of channel S

			"==" means that the values on either side of the "==" are to be tested for equaliy with a resulting 
				 Boolean value.
			        =>When the value of OPi is the empty set, then the expression (P == OPi for all values of i) 
			        	is defined to be TRUE
			        =>When the value of OTi is the empty set, then the expression (P == OTi for all values of i) 
			        	is defined to be TRUE
			        =>When the value of OSi is the empty set, then the expression (S == OSi for all values of i) 
			        	is defined to be TRUE
*/
VOID APOverlappingBSSScan(
	IN RTMP_ADAPTER *pAd)
{
	BOOLEAN		needFallBack = FALSE;
	UCHAR 		Channel = pAd->CommonCfg.Channel;
	UINT8		BBPValue = 0;
	INT		chStartIdx, chEndIdx, index,curPriChIdx, curSecChIdx;


	// We just care BSS who operating in 40MHz N Mode.
	if ((pAd->CommonCfg.PhyMode < PHY_11ABGN_MIXED) || 
		(pAd->CommonCfg.RegTransmitSetting.field.BW  == BW_20))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("The pAd->PhyMode=%d, BW=%d, didn't need channel adjustment!\n", 
				pAd->CommonCfg.PhyMode, pAd->CommonCfg.RegTransmitSetting.field.BW));
		return;
	}


	// Build the effected channel list, if something wrong, return directly.	
	if (pAd->CommonCfg.Channel <= 14)
	{	// For 2.4GHz band 
		for (index = 0; index < pAd->ChannelListNum; index++)
		{
			if(pAd->ChannelList[index].Channel == pAd->CommonCfg.Channel)
				break;
		}

		if (index < pAd->ChannelListNum)
		{

			if(pAd->CommonCfg.RegTransmitSetting.field.EXTCHA == EXTCHA_ABOVE)
			{
				curPriChIdx = index;
				curSecChIdx = ((index + 4) < pAd->ChannelListNum) ? (index + 4) : (pAd->ChannelListNum - 1);
				
				chStartIdx = (curPriChIdx >= 3) ? (curPriChIdx - 3) : 0;
				chEndIdx = ((curSecChIdx + 3) < pAd->ChannelListNum) ? (curSecChIdx + 3) : (pAd->ChannelListNum - 1);
			}
			else
			{
				curPriChIdx = index;
				curSecChIdx = ((index - 4) >=0 ) ? (index - 4) : 0;
				chStartIdx =(curSecChIdx >= 3) ? (curSecChIdx - 3) : 0;
				chEndIdx =  ((curPriChIdx + 3) < pAd->ChannelListNum) ? (curPriChIdx + 3) : (pAd->ChannelListNum - 1);;
			}
		}
		else
		{
			// It should not happened!
			DBGPRINT(RT_DEBUG_ERROR, ("2.4GHz: Cannot found the Control Channel(%d) in ChannelList, something wrong?\n", 
						pAd->CommonCfg.Channel));
			return;
		}
	}
	else
	{	// For 5GHz band
		for (index = 0; index < pAd->ChannelListNum; index++)
		{
			if(pAd->ChannelList[index].Channel == pAd->CommonCfg.Channel)
				break;
		}

		if (index < pAd->ChannelListNum)
		{
			curPriChIdx = index;
			if(pAd->CommonCfg.RegTransmitSetting.field.EXTCHA == EXTCHA_ABOVE)
			{
				chStartIdx = index;
				chEndIdx = chStartIdx + 1;
				curSecChIdx = chEndIdx;
			}
			else
			{
				chStartIdx = index - 1;
				chEndIdx = index;
				curSecChIdx = chStartIdx;
			}
		}
		else
		{
			// It should not happened!
			DBGPRINT(RT_DEBUG_ERROR, ("5GHz: Cannot found the ControlChannel(%d) in ChannelList, something wrong?\n", 
						pAd->CommonCfg.Channel));
			return;
		}
	}

	// Before we do the scanning, clear the bEffectedChannel as zero for latter use.
	for (index = 0; index < pAd->ChannelListNum; index++)
		pAd->ChannelList[index].bEffectedChannel = 0;
	
		
	// If we are not ready for Tx/Rx Pakcet, enable it now for receiving Beacons.
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP) == 0)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Card still not enable Tx/Rx, enable it now!\n"));
#ifdef RTMP_MAC_PCI
		// Enable Interrupt
		pAd->int_enable_reg = ((DELAYINTMASK)  | (RxINT|TxDataInt|TxMgmtInt)) & ~(0x03);
		pAd->int_disable_mask = 0;
		pAd->int_pending = 0;
	
		RTMP_IO_WRITE32(pAd, INT_SOURCE_CSR, 0xffffffff);  // clear garbage interrupts
		RTMP_ASIC_INTERRUPT_ENABLE(pAd);
#endif // RTMP_MAC_PCI //


		// Now Enable RxTx
		RTMPEnableRxTx(pAd);

		// APRxDoneInterruptHandle API will check this flag to decide accept incoming packet or not.
		// Set the flag be ready to receive Beacon frame for autochannel select.
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_START_UP);
	}


	DBGPRINT(RT_DEBUG_TRACE, ("Ready to do passive scanning for Channel[%d] to Channel[%d]!\n", 
			pAd->ChannelList[chStartIdx].Channel, pAd->ChannelList[chEndIdx].Channel));
	
	// Now start to do the passive scanning.
	pAd->CommonCfg.bOverlapScanning = TRUE;
	for (index = chStartIdx; index<=chEndIdx; index++)
	{
		Channel = pAd->ChannelList[index].Channel;

		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BBPValue);
		BBPValue &= (~0x20);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BBPValue);
		
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BBPValue);
		BBPValue &= (~0x18);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BBPValue);
		
		// Let BBP register at 20MHz to do scan	
		AsicSwitchChannel(pAd, Channel, TRUE);
		AsicLockChannel(pAd, Channel);

		DBGPRINT(RT_DEBUG_ERROR, ("SYNC - BBP R4 to 20MHz.l\n"));
		//DBGPRINT(RT_DEBUG_TRACE, ("Passive scanning for Channel %d.....\n", Channel));
		OS_WAIT(300); // wait for 200 ms at each channel.
	}
	pAd->CommonCfg.bOverlapScanning = FALSE;	
	

	// After scan all relate channels, now check the scan result to find out if we need fallback to 20MHz.
	for (index = chStartIdx; index <= chEndIdx; index++)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Channel[Idx=%d, Ch=%d].bEffectedChannel=0x%x!\n", 
					index, pAd->ChannelList[index].Channel, pAd->ChannelList[index].bEffectedChannel));
		if ((pAd->ChannelList[index].bEffectedChannel & 0x6)  && (index != curPriChIdx))
		{
			needFallBack = TRUE;
			DBGPRINT(RT_DEBUG_TRACE, ("needFallBack=TRUE due to OP/OT!\n"));
		}
		if ((pAd->ChannelList[index].bEffectedChannel & 0x1)  && (index != curSecChIdx))
		{
			needFallBack = TRUE;
			DBGPRINT(RT_DEBUG_TRACE, ("needFallBack=TRUE due to OS!\n"));
		}
	}
	
	// If need fallback, now do it.
	if (needFallBack)
	{
		pAd->CommonCfg.AddHTInfo.AddHtInfo.RecomWidth = 0;
		pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset = 0;
	}

	return;	
}
#endif // DOT11N_DRAFT3 //


VOID EnableAPMIMOPS(
	IN PRTMP_ADAPTER pAd)
{
	UCHAR	BBPR3=0,BBPR1=0,RFValue=0;
	ULONG	TxPinCfg = 0x00050F0A;//Gary 2007/08/09 0x050A0A
	//UINT32 	macdata=0;

#ifdef RT305x
		
		pAd->CommonCfg.bBlockAntDivforGreenAP=TRUE;
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BBPR3);
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BBPR1);
		RT30xxReadRFRegister(pAd, RF_R01, &RFValue);
		//RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R138, &BBPR138);
		
		BBPR3 &= (~0x18);
		BBPR1 &= (~0x18);
		RFValue &= 0x03;	//clear bit[7~2]
		RFValue |= 0xF0;
		// Turn off unused PA or LNA when only 1T or 1R
		TxPinCfg &= 0xFFFFFFF3;
		TxPinCfg &= 0xFFFFF3FF;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BBPR3);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BBPR1);
		RT30xxWriteRFRegister(pAd, RF_R01, RFValue);
		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);
		DBGPRINT(RT_DEBUG_TRACE, ("EnableAPMIMOPS, 305x changes the # of antenna to 1\n"));

#endif // RT305x //
}

VOID DisableAPMIMOPS(
	IN PRTMP_ADAPTER pAd)
{
		UCHAR	BBPR3=0,BBPR1=0,RFValue=0;
		ULONG	TxPinCfg = 0x00050F0A;//Gary 2007/08/09 0x050A0A
		//UINT32 	macdata=0;

#ifdef RT305x
		pAd->CommonCfg.bBlockAntDivforGreenAP=FALSE;
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BBPR3);
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BBPR1);
		RT30xxReadRFRegister(pAd, RF_R01, &RFValue);
		//RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R138, &BBPR138);
		BBPR3 &= (~0x18);
		if(pAd->Antenna.field.RxPath == 3)
		{
			BBPR3 |= (0x10);
		}
		else if(pAd->Antenna.field.RxPath == 2)
		{
			BBPR3 |= (0x8);
		}
		else if(pAd->Antenna.field.RxPath == 1)
		{
			BBPR3 |= (0x0);
		}

#ifdef DOT11_N_SUPPORT
		if ((pAd->CommonCfg.PhyMode >= PHY_11ABGN_MIXED) && (pAd->Antenna.field.TxPath == 2))
		{
			BBPR1 &= (~0x18);
			BBPR1 |= 0x10;
		}
		else
#endif // DOT11_N_SUPPORT //
		{
			BBPR1 &= (~0x18);
		}

		
		RFValue &= 0x03;	//clear bit[7~2]
		if (pAd->Antenna.field.TxPath == 1)
			RFValue |= 0xA0;
		else if (pAd->Antenna.field.TxPath == 2)
			RFValue |= 0x80;
		if (pAd->Antenna.field.RxPath == 1)
			RFValue |= 0x50;
		else if (pAd->Antenna.field.RxPath == 2)
			RFValue |= 0x40;
		// Turn off unused PA or LNA when only 1T or 1R
		if (pAd->Antenna.field.TxPath == 1)
		{
			TxPinCfg &= 0xFFFFFFF3;
		}
		if (pAd->Antenna.field.RxPath == 1)
		{
			TxPinCfg &= 0xFFFFF3FF;
		}
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BBPR3);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BBPR1);
		RT30xxWriteRFRegister(pAd, RF_R01, RFValue);
		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);
		DBGPRINT(RT_DEBUG_TRACE, ("EnableAPMIMOPS, 305x enables all antenna\n"));

#endif // RT305x //
}


#endif // DOT11_N_SUPPORT //

