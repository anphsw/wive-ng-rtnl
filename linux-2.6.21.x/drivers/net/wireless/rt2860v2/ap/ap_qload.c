/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************/
 
/****************************************************************************
 
    Abstract:

    Provide information on the current STA population and traffic levels
	in the QBSS.

	This attribute is available only at a QAP. This attribute, when TRUE,
	indicates that the QAP implementation is capable of generating and
	transmitting the QBSS load element in the Beacon and Probe Response frames.
 
***************************************************************************/

#include "rt_config.h"


typedef struct PACKED {

	UINT8 ElementId;
	UINT8 Length;

	/* the total number of STAs currently associated with this QBSS */
	UINT16 StationCount;

	/*	defined as the percentage of time, nomalized to 255, the QAP sensed the
		medium busy, as indicated by either the physical or virtual carrier
		sense mechanism.
		This percentage is computed using the formula:
			((channel busy time / (dot11ChannelUtilizationBeaconIntervals *
			dot11BeaconPeriod * 1024)) * 255) */
	UINT8 ChanUtil;

	/*	specifies the remaining amount of medium time available via explicit
		admission control, in units of 32 microsecond periods per 1 second.
		The field is helpful for roaming non-AP QSTAs to select a QAP that is
		likely to accept future admission control requests, but it does not
		represent a guarantee that the HC will admit these requests. */
	UINT16 AvalAdmCap;

} ELM_QBSS_LOAD;

#define ELM_QBSS_LOAD_ID					11
#define ELM_QBSS_LOAD_LEN					5




/* --------------------------------- Public -------------------------------- */

/*
========================================================================
Routine Description:
	Initialize ASIC Channel Busy Calculation mechanism.

Arguments:
	ad_p				- WLAN control block pointer

Return Value:
	None

Note:
	Init Condition: WMM must be enabled.
========================================================================
*/
VOID QBSS_LoadInit(
 	IN		RTMP_ADAPTER	*pAd)
{
	UINT32 IdBss;


	/* check whether any BSS enables WMM feature */
	for(IdBss=0; IdBss<pAd->ApCfg.BssidNum; IdBss++)
	{
		if ((pAd->ApCfg.MBSSID[IdBss].bWmmCapable)
			)
		{
			pAd->FlgQloadEnable = TRUE;
			break;
		} /* End of if */
	} /* End of for */

	if (pAd->FlgQloadEnable == TRUE)
	{
		/* Count EIFS, NAV, RX busy, TX busy as channel busy and
			enable Channel statistic timer (bit 0) */
		/* if bit 0 == 0, the function will be disabled */
		RTMP_IO_WRITE32(pAd, CH_TIME_CFG, 0x0000001F);

		/* default value is 50, please reference to IEEE802.11e 2005 Annex D */
		pAd->QloadChanUtilBeaconInt = 50;
	}
	else
	{
		/* no any WMM is enabled */
		RTMP_IO_WRITE32(pAd, CH_TIME_CFG, 0x00000000);
	} /* End of if */
} /* End of QBSS_LoadInit */


/*
========================================================================
Routine Description:
	Append the QBSS Load element to the beacon frame.

Arguments:
	ad_p				- WLAN control block pointer
	*pBeaconBuf			- the beacon or probe response frame

Return Value:
	the element total Length

Note:
	Append Condition: You must check whether WMM is enabled before the
	function is using.
========================================================================
*/
UINT32 QBSS_LoadElementAppend(
 	IN		RTMP_ADAPTER	*pAd,
	OUT		UINT8			*pBeaconBuf)
{
	ELM_QBSS_LOAD load, *pLoad = &load;
	ULONG ElmLen;


	/* check whether channel busy time calculation is enabled */
	if (pAd->FlgQloadEnable == 0)
		return 0;
	/* End of if */

	/* init */
	pLoad->ElementId = ELM_QBSS_LOAD_ID;
	pLoad->Length = ELM_QBSS_LOAD_LEN;

	pLoad->StationCount = MacTableAssocStaNumGet(pAd);
	pLoad->ChanUtil = pAd->QloadChanUtil;

	/* because no ACM is supported, the available bandwidth is 1 sec */
	pLoad->AvalAdmCap = 0x7a12; /* 0x7a12 * 32us = 1 second */
	

	/* copy the element to the frame */
    MakeOutgoingFrame(pBeaconBuf, &ElmLen,
						sizeof(ELM_QBSS_LOAD),	pLoad,
						END_OF_ARGS);

	return ElmLen;
} /* End of QBSS_LoadElementAppend */


/*
========================================================================
Routine Description:
	Update Channel Utilization.

Arguments:
	ad_p				- WLAN control block pointer

Return Value:
	None

Note:
	Only be used in TBTT handler.
========================================================================
*/
VOID QBSS_LoadUpdate(
 	IN		RTMP_ADAPTER	*pAd)
{
	UINT32 ChanUtilNu, ChanUtilDe;
	UINT32 CsrTime = 0;


	/* check whether channel busy time calculation is enabled */
	if (pAd->FlgQloadEnable == 0)
		return;
	/* End of if */

	/* accumulate channel busy time */
	RTMP_IO_READ32(pAd, CH_IDLE_STA, &CsrTime);
	pAd->QloadChanUtilTotal += (pAd->CommonCfg.BeaconPeriod*1000) - CsrTime;

	/* update new channel utilization */
	if (++pAd->QloadChanUtilBeaconCnt >= pAd->QloadChanUtilBeaconInt)
	{
		ChanUtilNu = (pAd->QloadChanUtilTotal*255);
		ChanUtilDe = (pAd->QloadChanUtilBeaconInt*\
						pAd->CommonCfg.BeaconPeriod)<<10;
		pAd->QloadChanUtil = (UINT8)(ChanUtilNu/ChanUtilDe);

		/* re-accumulate channel busy time */
		pAd->QloadChanUtilBeaconCnt = 0;
		pAd->QloadChanUtilTotal = 0;
	} /* End of if */
} /* End of QBSS_LoadUpdate */

/* End of ap_qload.c */
