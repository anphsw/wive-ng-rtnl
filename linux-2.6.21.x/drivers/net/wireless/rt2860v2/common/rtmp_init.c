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
	rtmp_init.c

	Abstract:
	Miniport generic portion header file

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/
#include "rt_config.h"
#ifdef LINUX
#include <linux/autoconf.h>
#endif

UCHAR    BIT8[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
char*   CipherName[] = {"none","wep64","wep128","TKIP","AES","CKIP64","CKIP128","CKIP152","SMS4"};

//
// BBP register initialization set
//
REG_PAIR   BBPRegTable[] = {
#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	{BBP_R4,		0x50}, // 2883 need to 
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //
#ifdef RT305x
	{BBP_R31,		0x08},		//gary recommend for ACE
#endif // RT305x //

#if defined (CONFIG_RALINK_RT3883)
	{BBP_R47,		0x07},	// 5b ADC is used to measure chip temp through PTAT voltage
#endif // CONFIG_RALINK_RT3883 //

// turn on find AGC cause QA have Rx problem 2009-10-26 in 3883
#if defined(CONFIG_RALINK_RT2883)
	{BBP_R65,		0x6C},		// fix rssi issue and add Fine AGC
#else
	{BBP_R65,		0x2C},		// fix rssi issue
#endif // CONFIG_RALINK_RT2883 //
	{BBP_R66,		0x38},	// Also set this default value to pAd->BbpTuning.R66CurrentValue at initial
	{BBP_R69,		0x12},
	{BBP_R70,		0xa},	// BBP_R70 will change to 0x8 in ApStartUp and LinkUp for rt2860C, otherwise value is 0xa
	{BBP_R73,		0x10},
#ifdef RT305x
	{BBP_R78,		0x0E},
	{BBP_R80,		0x08},	// requested by Gary for high power
#endif // RT305x //
	{BBP_R81,		0x37},
	{BBP_R82,		0x62},
	{BBP_R83,		0x6A},
	{BBP_R84,		0x99},	// 0x19 is for rt2860E and after. This is for extension channel overlapping IOT. 0x99 is for rt2860D and before
#ifdef CONFIG_RALINK_RT3883
	{BBP_R86,		0x46},	// for peak throughput, Henry 2009-12-23
	{BBP_R88,		0x90},	// for rt3883 middle range, Henry 2009-12-31
#else
	{BBP_R86,		0x00},	// middle range issue, Rory @2008-01-28 	
#endif
	{BBP_R91,		0x04},	// middle range issue, Rory @2008-01-28
#ifdef CONFIG_RALINK_RT3883
	{BBP_R92,		0x02},	//improve 10~20Mbps peak throughput, requested by Henry 2009-11-26
#else
	{BBP_R92,		0x00},	// middle range issue, Rory @2008-01-28
#endif // CONFIG_RALINK_RT3883 //

#ifdef RT305x
	{BBP_R103,		0xC0}, 	
#elif defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	{BBP_R103,		0xC0}, 	
#else
	{BBP_R103,		0x00}, 	// near range high-power issue, requested from Gary @2008-0528
#endif // RT305x //
#ifdef RTMP_RBUS_SUPPORT
#if defined (CONFIG_RALINK_RT2883)
	//{BBP_R105,		0x04},	// RT2883 default is 0x4
	{BBP_R105,		0xbc},	// Initialized R105 to enable saving of Explicit and Implicit profiles
	{BBP_R137,		0x0a},	// RT2883 default is 0x4

	{BBP_R179,		0x02},	// Set ITxBF timeout to 0x9C40=1000msec
	{BBP_R180,		0x00},
	{BBP_R182,		0x40},
	{BBP_R180,		0x01},
	{BBP_R182,		0x9C},

#elif defined (CONFIG_RALINK_RT3883)
	{BBP_R104,		0x92},	//improve 10~20Mbps peak throughput, requested by Henry 2009-11-26
	{BBP_R105,		0x04},	// RT2883 default is 0x4
	{BBP_R120,		0x50},	// for long range -2db, Gary 2010-01-22
	{BBP_R137,		0x0F},  // julian suggest make the RF output more stable
	{BBP_R163,		0xBC},	// Enable saving of Explicit and Implicit profiles

	{BBP_R179,		0x02},	// Set ITxBF timeout to 0x9C40=1000msec
	{BBP_R180,		0x00},
	{BBP_R182,		0x40},
	{BBP_R180,		0x01},
	{BBP_R182,		0x9C},
	{BBP_R179,		0x00},

	{BBP_R142,		0x04},	// Reprogram the inband interface to put right values in RXWI
	{BBP_R143,		0x3b},
	{BBP_R142,		0x06},
	{BBP_R143,		0xA0},
	{BBP_R142,		0x07},
	{BBP_R143,		0xA1},
	{BBP_R142,		0x08},
	{BBP_R143,		0xA2},

#elif defined (CONFIG_RALINK_RT3350)
	{BBP_R105,		0x04},
#else
	{BBP_R105,		0x01},/*kurtis:0x01 ori*/// 0x05 is for rt2860E to turn on FEQ control. It is safe for rt2860D and before, because Bit 7:2 are reserved in rt2860D and before.
#endif
#else
	{BBP_R105,		0x05},	// 0x05 is for rt2860E to turn on FEQ control. It is safe for rt2860D and before, because Bit 7:2 are reserved in rt2860D and before.
#endif

#ifdef CONFIG_RALINK_RT3883
	{BBP_R106,  	0x05},
#else
#ifdef DOT11_N_SUPPORT
	{BBP_R106,  	0x35},  // improve throughput unstable issue in short GI	
#endif
#endif // CONFIG_RALINK_RT3883 //
};
#define	NUM_BBP_REG_PARMS	(sizeof(BBPRegTable) / sizeof(REG_PAIR))


//
// ASIC register initialization sets
//

RTMP_REG_PAIR	MACRegTable[] =	{
#if defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT3883) && defined (CONFIG_16MBSSID_MODE)
	{BCN_OFFSET0,			0x18100800}, /* if (BCNx_OFFSET > 0x7F) SHR_MSEL = 0 else SHR_MSEL = 1 */
	{BCN_OFFSET1,			0x38302820}, /* if (BCNx_OFFSET > 0x7F) SHR_MSEL = 0 else SHR_MSEL = 1 */
	{BCN_OFFSET2,			0x58504840}, /* if (BCNx_OFFSET > 0x7F) SHR_MSEL = 0 else SHR_MSEL = 1 */
	{BCN_OFFSET3,			0x78706860}, /* if (BCNx_OFFSET > 0x7F) SHR_MSEL = 0 else SHR_MSEL = 1 */
#elif defined(HW_BEACON_OFFSET) && (HW_BEACON_OFFSET == 0x200)
	{BCN_OFFSET0,			0xf8f0e8e0}, /* 0x3800(e0), 0x3A00(e8), 0x3C00(f0), 0x3E00(f8), 512B for each beacon */
	{BCN_OFFSET1,			0x6f77d0c8}, /* 0x3200(c8), 0x3400(d0), 0x1DC0(77), 0x1BC0(6f), 512B for each beacon */
#elif defined(HW_BEACON_OFFSET) && (HW_BEACON_OFFSET == 0x100)
	{BCN_OFFSET0,			0xece8e4e0}, /* 0x3800, 0x3A00, 0x3C00, 0x3E00, 512B for each beacon */
	{BCN_OFFSET1,			0xfcf8f4f0}, /* 0x3800, 0x3A00, 0x3C00, 0x3E00, 512B for each beacon */
#else
    #error You must re-calculate new value for BCN_OFFSET0 & BCN_OFFSET1 in MACRegTable[]!!!
#endif // HW_BEACON_OFFSET //

	{LEGACY_BASIC_RATE,		0x0000013f}, //  Basic rate set bitmap
	{HT_BASIC_RATE,		0x00008003}, // Basic HT rate set , 20M, MCS=3, MM. Format is the same as in TXWI.
	{MAC_SYS_CTRL,		0x00}, // 0x1004, , default Disable RX
	{RX_FILTR_CFG,		0x17f97}, //0x1400  , RX filter control,  
	{BKOFF_SLOT_CFG,	0x209}, // default set short slot time, CC_DELAY_TIME should be 2	 
	//{TX_SW_CFG0,		0x40a06}, // Gary,2006-08-23 
#ifdef RT305x
	//{TX_SW_CFG0,		0x40a06}, // Gary,2006-08-23 
	{TX_SW_CFG0,		0x400},   // Gary,2008-05-21 0x0 for CWC test , 2008-06-19 0x400 for rf reason
	{TX_SW_CFG1,		0x0}, 	  // Gary,2008-06-18 
	{TX_SW_CFG2,		0x30}, 	  // Bruce, CwC IOT issue
#elif defined (CONFIG_RALINK_RT2883)
	{TX_SW_CFG0,		0x0},     // Gary,2008-12-15 for Intel 5300 (FIXME)
	{TX_SW_CFG1,		0x0},     // Gary,2008-12-15 for Intel 5300 (FIXME)
	{TX_SW_CFG2,		0x40000}, // Gary,2008-12-15 for Intel 5300 (FIXME)
	{TX_TXBF_CFG_0,		0x8000FC21},	// Force MCS0 for sounding response
	{TX_TXBF_CFG_3,		0x00009c40},	// ETxBF Timeout = 1 sec = 0x9c40*(25 usec)
#elif defined (CONFIG_RALINK_RT3883)
	{TX_SW_CFG0,		0x402},   // Gary,2009-9-21
	{TX_SW_CFG1,		0x0},     // FIXME
	{TX_SW_CFG2,		0x40000}, // FIXME
	{TX_TXBF_CFG_0,		0x8000FC21},	// Force MCS0 for sounding response
	{TX_TXBF_CFG_3,		0x00009c40},	// ETxBF Timeout = 1 sec = 0x9c40*(25 usec)
#else //original source code
	{TX_SW_CFG0,		0x0}, 		// Gary,2008-05-21 for CWC test 
	{TX_SW_CFG1,		0x80606}, // Gary,2006-08-23 
#endif // RT305x //
	{TX_LINK_CFG,		0x1020},		// Gary,2006-08-23 
	//{TX_TIMEOUT_CFG,	0x00182090},	// CCK has some problem. So increase timieout value. 2006-10-09// MArvek RT
	{TX_TIMEOUT_CFG,	0x000a2090},	// CCK has some problem. So increase timieout value. 2006-10-09// MArvek RT , Modify for 2860E ,2007-08-01
	{MAX_LEN_CFG,		MAX_AGGREGATION_SIZE | 0x00001000},	// 0x3018, MAX frame length. Max PSDU = 16kbytes.
	{LED_CFG,		0x7f031e46}, // Gary, 2006-08-23

#ifdef INF_AMAZON_SE
	{PBF_MAX_PCNT,			0x1F3F6F6F}, 	//iverson modify for usb issue, 2008/09/19
											// 6F + 6F < total page count FE
											// so that RX doesn't occupy TX's buffer space when WMM congestion.
#else
	{PBF_MAX_PCNT,			0x1F3FBF9F}, 	//0x1F3f7f9f},		//Jan, 2006/04/20
#endif // INF_AMAZON_SE //
	//{TX_RTY_CFG,			0x6bb80408},	// Jan, 2006/11/16
// WMM_ACM_SUPPORT
//	{TX_RTY_CFG,			0x6bb80101},	// sample
	{TX_RTY_CFG,			0x47d01f0f},	// Jan, 2006/11/16, Set TxWI->ACK =0 in Probe Rsp Modify for 2860E ,2007-08-03
	
	{AUTO_RSP_CFG,			0x00000053},	// Initial Auto_Responder, because QA will turn off Auto-Responder
	{CCK_PROT_CFG,			0x05740003 /*0x01740003*/},	// Initial Auto_Responder, because QA will turn off Auto-Responder. And RTS threshold is enabled. 
	{OFDM_PROT_CFG,			0x05740003 /*0x01740003*/},	// Initial Auto_Responder, because QA will turn off Auto-Responder. And RTS threshold is enabled. 
	{GF20_PROT_CFG,			0x01744004},    // set 19:18 --> Short NAV for MIMO PS
	{GF40_PROT_CFG,			0x03F44084},    
	{MM20_PROT_CFG,			0x01744004},    
#ifdef RTMP_MAC_PCI
	{MM40_PROT_CFG,			0x03F54084},	
#endif // RTMP_MAC_PCI //
	{TXOP_CTRL_CFG,			0x0000583f, /*0x0000243f*/ /*0x000024bf*/},	//Extension channel backoff.
	{TX_RTS_CFG,			0x00092b20},	
//#ifdef WIFI_TEST
	{EXP_ACK_TIME,			0x002400ca},	// default value
//#else
//	{EXP_ACK_TIME,			0x005400ca},	// suggested by Gray @ 20070323 for 11n intel-sta throughput
//#endif // end - WIFI_TEST //
//#ifdef CONFIG_AP_SUPPORT
//	{TBTT_SYNC_CFG,			0x00422000},	// TBTT_ADJUST(7:0) == 0	
//	{TBTT_SYNC_CFG,			0x00012000},	// TBTT_ADJUST(7:0) == 0	
//#endif // CONFIG_AP_SUPPORT //
#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	{TX_FBK_CFG_3S_0,		0x12111008},	// default value
	{TX_FBK_CFG_3S_1,		0x16151413},	// default value
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //
	{TXOP_HLDR_ET, 			0x00000002},

	/* Jerry comments 2008/01/16: we use SIFS = 10us in CCK defaultly, but it seems that 10us
		is too small for INTEL 2200bg card, so in MBSS mode, the delta time between beacon0
		and beacon1 is SIFS (10us), so if INTEL 2200bg card connects to BSS0, the ping
		will always lost. So we change the SIFS of CCK from 10us to 16us. */
	{XIFS_TIME_CFG,			0x33a41010},
	{PWR_PIN_CFG,			0x00000003},	// patch for 2880-E
};

#ifdef CONFIG_AP_SUPPORT
RTMP_REG_PAIR	APMACRegTable[] =	{
	{WMM_AIFSN_CFG,		0x00001173},
	{WMM_CWMIN_CFG,	0x00002344},
	{WMM_CWMAX_CFG,	0x000034a6},
	{WMM_TXOP0_CFG,		0x00100020},
	{WMM_TXOP1_CFG,		0x002F0038},
	{TBTT_SYNC_CFG,		0x00012000},
#if defined(CONFIG_RALINK_RT2883) || defined(CONFIG_RALINK_RT3883)
	{TX_CHAIN_ADDR0_L,	0xFFFFFFFF},	// Broadcast frames are in stream mode
	{TX_CHAIN_ADDR0_H,	0xFFFFF},
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //
};
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
RTMP_REG_PAIR	STAMACRegTable[] =	{
	{WMM_AIFSN_CFG,		0x00002273},
	{WMM_CWMIN_CFG,	0x00002344},
	{WMM_CWMAX_CFG,	0x000034aa},
};
#endif // CONFIG_STA_SUPPORT //

#define	NUM_MAC_REG_PARMS		(sizeof(MACRegTable) / sizeof(RTMP_REG_PAIR))
#ifdef CONFIG_AP_SUPPORT
#define	NUM_AP_MAC_REG_PARMS		(sizeof(APMACRegTable) / sizeof(RTMP_REG_PAIR))
#endif // CONFIG_AP_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
#define	NUM_STA_MAC_REG_PARMS	(sizeof(STAMACRegTable) / sizeof(RTMP_REG_PAIR))
#endif // CONFIG_STA_SUPPORT //


/*
	========================================================================
	
	Routine Description:
		Allocate RTMP_ADAPTER data block and do some initialization

	Arguments:
		Adapter		Pointer to our adapter

	Return Value:
		NDIS_STATUS_SUCCESS
		NDIS_STATUS_FAILURE

	IRQL = PASSIVE_LEVEL

	Note:
	
	========================================================================
*/
NDIS_STATUS	RTMPAllocAdapterBlock(
	IN  PVOID	handle,
	OUT	PRTMP_ADAPTER	*ppAdapter)
{
	PRTMP_ADAPTER	pAd = NULL;
	NDIS_STATUS		Status;
	INT 			index;
	UCHAR			*pBeaconBuf = NULL;

	DBGPRINT(RT_DEBUG_TRACE, ("--> RTMPAllocAdapterBlock\n"));

	*ppAdapter = NULL;

	do
	{
		// Allocate RTMP_ADAPTER memory block
		pBeaconBuf = kmalloc(MAX_BEACON_SIZE, MEM_ALLOC_FLAG);
		if (pBeaconBuf == NULL)
		{
			Status = NDIS_STATUS_FAILURE;
			DBGPRINT_ERR(("Failed to allocate memory - BeaconBuf!\n"));
			break;
		}
		NdisZeroMemory(pBeaconBuf, MAX_BEACON_SIZE);

		Status = AdapterBlockAllocateMemory(handle, (PVOID *)&pAd);
		if (Status != NDIS_STATUS_SUCCESS)
		{
			DBGPRINT_ERR(("Failed to allocate memory - ADAPTER\n"));
			break;
		}
		pAd->BeaconBuf = pBeaconBuf;
		DBGPRINT(RT_DEBUG_OFF, ("\n\n=== pAd = %p, size = %d ===\n\n", pAd, (UINT32)sizeof(RTMP_ADAPTER)));


		// Init spin locks
		NdisAllocateSpinLock(&pAd->MgmtRingLock);
#ifdef RTMP_MAC_PCI
		NdisAllocateSpinLock(&pAd->RxRingLock);
#endif // RTMP_MAC_PCI //

		for (index =0 ; index < NUM_OF_TX_RING; index++)
		{
			NdisAllocateSpinLock(&pAd->TxSwQueueLock[index]);
			NdisAllocateSpinLock(&pAd->DeQueueLock[index]);
		}

		NdisAllocateSpinLock(&pAd->irq_lock);

#ifdef WMM_ACM_SUPPORT
		NdisAllocateSpinLock(&pAd->AcmTspecSemLock);
		NdisAllocateSpinLock(&pAd->AcmTspecIrqLock);
#endif // WMM_ACM_SUPPORT //

#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_AP_SUPPORT
        UAPSD_Init(pAd);
#endif // UAPSD_AP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //
	} while (FALSE);

	if ((Status != NDIS_STATUS_SUCCESS) && (pBeaconBuf))
		kfree(pBeaconBuf);
	
	*ppAdapter = pAd;

	return Status;
}

#if !defined (CONFIG_RALINK_RT2883) && !defined (CONFIG_RALINK_RT3883)
/*
	========================================================================
	
	Routine Description:
		Read initial Tx power per MCS and BW from EEPROM
		
	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
VOID	RTMPReadTxPwrPerRate(
	IN	PRTMP_ADAPTER	pAd)
{
	ULONG		data, Adata, Gdata;
	USHORT		i, value, value2;
	INT			Apwrdelta, Gpwrdelta;
	UCHAR		t1,t2,t3,t4;
	BOOLEAN		bApwrdeltaMinus = TRUE, bGpwrdeltaMinus = TRUE;
	
	//
	// Get power delta for 20MHz and 40MHz.
	//
	DBGPRINT(RT_DEBUG_TRACE, ("Txpower per Rate\n"));
	RT28xx_EEPROM_READ16(pAd, EEPROM_TXPOWER_DELTA, value2);
	Apwrdelta = 0;
	Gpwrdelta = 0;

	if ((value2 & 0xff) != 0xff)
	{
		if ((value2 & 0x80))
			Gpwrdelta = (value2&0xf);
		
		if ((value2 & 0x40))
			bGpwrdeltaMinus = FALSE;
		else
			bGpwrdeltaMinus = TRUE;
	}
	if ((value2 & 0xff00) != 0xff00)
	{
		if ((value2 & 0x8000))
			Apwrdelta = ((value2&0xf00)>>8);

		if ((value2 & 0x4000))
			bApwrdeltaMinus = FALSE;
		else
			bApwrdeltaMinus = TRUE;
	}	
	DBGPRINT(RT_DEBUG_TRACE, ("Gpwrdelta = %x, Apwrdelta = %x .\n", Gpwrdelta, Apwrdelta));

	//
	// Get Txpower per MCS for 20MHz in 2.4G.
	//
	for (i=0; i<5; i++)
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_TXPOWER_BYRATE_20MHZ_2_4G + i*4, value);
		data = value;
		if (bApwrdeltaMinus == FALSE)
		{
			t1 = (value&0xf)+(Apwrdelta);
			if (t1 > 0xf)
				t1 = 0xf;
			t2 = ((value&0xf0)>>4)+(Apwrdelta);
			if (t2 > 0xf)
				t2 = 0xf;
			t3 = ((value&0xf00)>>8)+(Apwrdelta);
			if (t3 > 0xf)
				t3 = 0xf;
			t4 = ((value&0xf000)>>12)+(Apwrdelta);
			if (t4 > 0xf)
				t4 = 0xf;
		}
		else
		{
			if ((value&0xf) > Apwrdelta)
				t1 = (value&0xf)-(Apwrdelta);
			else
				t1 = 0;
			if (((value&0xf0)>>4) > Apwrdelta)
				t2 = ((value&0xf0)>>4)-(Apwrdelta);
			else
				t2 = 0;
			if (((value&0xf00)>>8) > Apwrdelta)
				t3 = ((value&0xf00)>>8)-(Apwrdelta);
			else
				t3 = 0;
			if (((value&0xf000)>>12) > Apwrdelta)
				t4 = ((value&0xf000)>>12)-(Apwrdelta);
			else
				t4 = 0;
		}				
		Adata = t1 + (t2<<4) + (t3<<8) + (t4<<12);
		if (bGpwrdeltaMinus == FALSE)
		{
			t1 = (value&0xf)+(Gpwrdelta);
			if (t1 > 0xf)
				t1 = 0xf;
			t2 = ((value&0xf0)>>4)+(Gpwrdelta);
			if (t2 > 0xf)
				t2 = 0xf;
			t3 = ((value&0xf00)>>8)+(Gpwrdelta);
			if (t3 > 0xf)
				t3 = 0xf;
			t4 = ((value&0xf000)>>12)+(Gpwrdelta);
			if (t4 > 0xf)
				t4 = 0xf;
		}
		else
		{
			if ((value&0xf) > Gpwrdelta)
				t1 = (value&0xf)-(Gpwrdelta);
			else
				t1 = 0;
			if (((value&0xf0)>>4) > Gpwrdelta)
				t2 = ((value&0xf0)>>4)-(Gpwrdelta);
			else
				t2 = 0;
			if (((value&0xf00)>>8) > Gpwrdelta)
				t3 = ((value&0xf00)>>8)-(Gpwrdelta);
			else
				t3 = 0;
			if (((value&0xf000)>>12) > Gpwrdelta)
				t4 = ((value&0xf000)>>12)-(Gpwrdelta);
			else
				t4 = 0;
		}				
		Gdata = t1 + (t2<<4) + (t3<<8) + (t4<<12);
		
		RT28xx_EEPROM_READ16(pAd, EEPROM_TXPOWER_BYRATE_20MHZ_2_4G + i*4 + 2, value);
		if (bApwrdeltaMinus == FALSE)
		{
			t1 = (value&0xf)+(Apwrdelta);
			if (t1 > 0xf)
				t1 = 0xf;
			t2 = ((value&0xf0)>>4)+(Apwrdelta);
			if (t2 > 0xf)
				t2 = 0xf;
			t3 = ((value&0xf00)>>8)+(Apwrdelta);
			if (t3 > 0xf)
				t3 = 0xf;
			t4 = ((value&0xf000)>>12)+(Apwrdelta);
			if (t4 > 0xf)
				t4 = 0xf;
		}
		else
		{
			if ((value&0xf) > Apwrdelta)
				t1 = (value&0xf)-(Apwrdelta);
			else
				t1 = 0;
			if (((value&0xf0)>>4) > Apwrdelta)
				t2 = ((value&0xf0)>>4)-(Apwrdelta);
			else
				t2 = 0;
			if (((value&0xf00)>>8) > Apwrdelta)
				t3 = ((value&0xf00)>>8)-(Apwrdelta);
			else
				t3 = 0;
			if (((value&0xf000)>>12) > Apwrdelta)
				t4 = ((value&0xf000)>>12)-(Apwrdelta);
			else
				t4 = 0;
		}				
		Adata |= ((t1<<16) + (t2<<20) + (t3<<24) + (t4<<28));
		if (bGpwrdeltaMinus == FALSE)
		{
			t1 = (value&0xf)+(Gpwrdelta);
			if (t1 > 0xf)
				t1 = 0xf;
			t2 = ((value&0xf0)>>4)+(Gpwrdelta);
			if (t2 > 0xf)
				t2 = 0xf;
			t3 = ((value&0xf00)>>8)+(Gpwrdelta);
			if (t3 > 0xf)
				t3 = 0xf;
			t4 = ((value&0xf000)>>12)+(Gpwrdelta);
			if (t4 > 0xf)
				t4 = 0xf;
		}
		else
		{
			if ((value&0xf) > Gpwrdelta)
				t1 = (value&0xf)-(Gpwrdelta);
			else
				t1 = 0;
			if (((value&0xf0)>>4) > Gpwrdelta)
				t2 = ((value&0xf0)>>4)-(Gpwrdelta);
			else
				t2 = 0;
			if (((value&0xf00)>>8) > Gpwrdelta)
				t3 = ((value&0xf00)>>8)-(Gpwrdelta);
			else
				t3 = 0;
			if (((value&0xf000)>>12) > Gpwrdelta)
				t4 = ((value&0xf000)>>12)-(Gpwrdelta);
			else
				t4 = 0;
		}				
		Gdata |= ((t1<<16) + (t2<<20) + (t3<<24) + (t4<<28));
		data |= (value<<16);
		
		/* For 20M/40M Power Delta issue */
		pAd->Tx20MPwrCfgABand[i] = data;
		pAd->Tx20MPwrCfgGBand[i] = data;
		pAd->Tx40MPwrCfgABand[i] = Adata;
		pAd->Tx40MPwrCfgGBand[i] = Gdata;
		
		if (data != 0xffffffff)
			RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + i*4, data);
		DBGPRINT_RAW(RT_DEBUG_TRACE, ("20MHz BW, 2.4G band-%lx,  Adata = %lx,  Gdata = %lx \n", data, Adata, Gdata));
	}
}


/*
	========================================================================
	
	Routine Description:
		Read initial channel power parameters from EEPROM
		
	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
VOID	RTMPReadChannelPwr(
	IN	PRTMP_ADAPTER	pAd)
{
	UCHAR				i, choffset;
	EEPROM_TX_PWR_STRUC	    Power;
	EEPROM_TX_PWR_STRUC	    Power2;
	
	// Read Tx power value for all channels
	// Value from 1 - 0x7f. Default value is 24.
	// Power value : 2.4G 0x00 (0) ~ 0x1F (31)
	//             : 5.5G 0xF9 (-7) ~ 0x0F (15)
	
	// 0. 11b/g, ch1 - ch 14
	for (i = 0; i < 7; i++)
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TX_PWR_OFFSET + i * 2, Power.word);
		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TX2_PWR_OFFSET + i * 2, Power2.word);
		pAd->TxPower[i * 2].Channel = i * 2 + 1;
		pAd->TxPower[i * 2 + 1].Channel = i * 2 + 2;

		if ((Power.field.Byte0 > 31) || (Power.field.Byte0 < 0))
			pAd->TxPower[i * 2].Power = DEFAULT_RF_TX_POWER;
		else
			pAd->TxPower[i * 2].Power = Power.field.Byte0;

		if ((Power.field.Byte1 > 31) || (Power.field.Byte1 < 0))
			pAd->TxPower[i * 2 + 1].Power = DEFAULT_RF_TX_POWER;
		else
			pAd->TxPower[i * 2 + 1].Power = Power.field.Byte1;

		if ((Power2.field.Byte0 > 31) || (Power2.field.Byte0 < 0))
			pAd->TxPower[i * 2].Power2 = DEFAULT_RF_TX_POWER;
		else
			pAd->TxPower[i * 2].Power2 = Power2.field.Byte0;

		if ((Power2.field.Byte1 > 31) || (Power2.field.Byte1 < 0))
			pAd->TxPower[i * 2 + 1].Power2 = DEFAULT_RF_TX_POWER;
		else
			pAd->TxPower[i * 2 + 1].Power2 = Power2.field.Byte1;
	}
	
	// 1. U-NII lower/middle band: 36, 38, 40; 44, 46, 48; 52, 54, 56; 60, 62, 64 (including central frequency in BW 40MHz)
	// 1.1 Fill up channel
	choffset = 14;
	for (i = 0; i < 4; i++)
	{
		pAd->TxPower[3 * i + choffset + 0].Channel	= 36 + i * 8 + 0;
		pAd->TxPower[3 * i + choffset + 0].Power	= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * i + choffset + 0].Power2	= DEFAULT_RF_TX_POWER;

		pAd->TxPower[3 * i + choffset + 1].Channel	= 36 + i * 8 + 2;
		pAd->TxPower[3 * i + choffset + 1].Power	= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * i + choffset + 1].Power2	= DEFAULT_RF_TX_POWER;

		pAd->TxPower[3 * i + choffset + 2].Channel	= 36 + i * 8 + 4;
		pAd->TxPower[3 * i + choffset + 2].Power	= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * i + choffset + 2].Power2	= DEFAULT_RF_TX_POWER;
	}

	// 1.2 Fill up power
	for (i = 0; i < 6; i++)
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + i * 2, Power.word);
		RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX2_PWR_OFFSET + i * 2, Power2.word);

		if ((Power.field.Byte0 < 16) && (Power.field.Byte0 >= -7))
			pAd->TxPower[i * 2 + choffset + 0].Power = Power.field.Byte0;

		if ((Power.field.Byte1 < 16) && (Power.field.Byte1 >= -7))
			pAd->TxPower[i * 2 + choffset + 1].Power = Power.field.Byte1;			

		if ((Power2.field.Byte0 < 16) && (Power2.field.Byte0 >= -7))
			pAd->TxPower[i * 2 + choffset + 0].Power2 = Power2.field.Byte0;

		if ((Power2.field.Byte1 < 16) && (Power2.field.Byte1 >= -7))
			pAd->TxPower[i * 2 + choffset + 1].Power2 = Power2.field.Byte1;			
	}
	
	// 2. HipperLAN 2 100, 102 ,104; 108, 110, 112; 116, 118, 120; 124, 126, 128; 132, 134, 136; 140 (including central frequency in BW 40MHz)
	// 2.1 Fill up channel
	choffset = 14 + 12;
	for (i = 0; i < 5; i++)
	{
		pAd->TxPower[3 * i + choffset + 0].Channel	= 100 + i * 8 + 0;
		pAd->TxPower[3 * i + choffset + 0].Power	= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * i + choffset + 0].Power2	= DEFAULT_RF_TX_POWER;

		pAd->TxPower[3 * i + choffset + 1].Channel	= 100 + i * 8 + 2;
		pAd->TxPower[3 * i + choffset + 1].Power	= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * i + choffset + 1].Power2	= DEFAULT_RF_TX_POWER;

		pAd->TxPower[3 * i + choffset + 2].Channel	= 100 + i * 8 + 4;
		pAd->TxPower[3 * i + choffset + 2].Power	= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * i + choffset + 2].Power2	= DEFAULT_RF_TX_POWER;
	}
	pAd->TxPower[3 * 5 + choffset + 0].Channel		= 140;
	pAd->TxPower[3 * 5 + choffset + 0].Power		= DEFAULT_RF_TX_POWER;
	pAd->TxPower[3 * 5 + choffset + 0].Power2		= DEFAULT_RF_TX_POWER;

	// 2.2 Fill up power
	for (i = 0; i < 8; i++)
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + (choffset - 14) + i * 2, Power.word);
		RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX2_PWR_OFFSET + (choffset - 14) + i * 2, Power2.word);

		if ((Power.field.Byte0 < 16) && (Power.field.Byte0 >= -7))
			pAd->TxPower[i * 2 + choffset + 0].Power = Power.field.Byte0;

		if ((Power.field.Byte1 < 16) && (Power.field.Byte1 >= -7))
			pAd->TxPower[i * 2 + choffset + 1].Power = Power.field.Byte1;			

		if ((Power2.field.Byte0 < 16) && (Power2.field.Byte0 >= -7))
			pAd->TxPower[i * 2 + choffset + 0].Power2 = Power2.field.Byte0;

		if ((Power2.field.Byte1 < 16) && (Power2.field.Byte1 >= -7))
			pAd->TxPower[i * 2 + choffset + 1].Power2 = Power2.field.Byte1;			
	}

	// 3. U-NII upper band: 149, 151, 153; 157, 159, 161; 165, 167, 169; 171, 173 (including central frequency in BW 40MHz)
	// 3.1 Fill up channel
	choffset = 14 + 12 + 16;
	/*for (i = 0; i < 2; i++)*/
	for (i = 0; i < 3; i++)
	{
		pAd->TxPower[3 * i + choffset + 0].Channel	= 149 + i * 8 + 0;
		pAd->TxPower[3 * i + choffset + 0].Power	= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * i + choffset + 0].Power2	= DEFAULT_RF_TX_POWER;

		pAd->TxPower[3 * i + choffset + 1].Channel	= 149 + i * 8 + 2;
		pAd->TxPower[3 * i + choffset + 1].Power	= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * i + choffset + 1].Power2	= DEFAULT_RF_TX_POWER;

		pAd->TxPower[3 * i + choffset + 2].Channel	= 149 + i * 8 + 4;
		pAd->TxPower[3 * i + choffset + 2].Power	= DEFAULT_RF_TX_POWER;
		pAd->TxPower[3 * i + choffset + 2].Power2	= DEFAULT_RF_TX_POWER;
	}
	pAd->TxPower[3 * 3 + choffset + 0].Channel		= 171;
	pAd->TxPower[3 * 3 + choffset + 0].Power		= DEFAULT_RF_TX_POWER;
	pAd->TxPower[3 * 3 + choffset + 0].Power2		= DEFAULT_RF_TX_POWER;

	pAd->TxPower[3 * 3 + choffset + 1].Channel		= 173;
	pAd->TxPower[3 * 3 + choffset + 1].Power		= DEFAULT_RF_TX_POWER;
	pAd->TxPower[3 * 3 + choffset + 1].Power2		= DEFAULT_RF_TX_POWER;

	// 3.2 Fill up power
	/*for (i = 0; i < 4; i++)*/
	for (i = 0; i < 6; i++)
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + (choffset - 14) + i * 2, Power.word);
		RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX2_PWR_OFFSET + (choffset - 14) + i * 2, Power2.word);

		if ((Power.field.Byte0 < 16) && (Power.field.Byte0 >= -7))
			pAd->TxPower[i * 2 + choffset + 0].Power = Power.field.Byte0;

		if ((Power.field.Byte1 < 16) && (Power.field.Byte1 >= -7))
			pAd->TxPower[i * 2 + choffset + 1].Power = Power.field.Byte1;			

		if ((Power2.field.Byte0 < 16) && (Power2.field.Byte0 >= -7))
			pAd->TxPower[i * 2 + choffset + 0].Power2 = Power2.field.Byte0;

		if ((Power2.field.Byte1 < 16) && (Power2.field.Byte1 >= -7))
			pAd->TxPower[i * 2 + choffset + 1].Power2 = Power2.field.Byte1;			
	}

	// 4. Print and Debug
	/*choffset = 14 + 12 + 16 + 7;*/
	choffset = 14 + 12 + 16 + 11;
	

}
#endif

/*
	========================================================================
	
	Routine Description:
		Read the following from the registry
		1. All the parameters
		2. NetworkAddres

	Arguments:
		Adapter						Pointer to our adapter
		WrapperConfigurationContext	For use by NdisOpenConfiguration

	Return Value:
		NDIS_STATUS_SUCCESS
		NDIS_STATUS_FAILURE
		NDIS_STATUS_RESOURCES

	IRQL = PASSIVE_LEVEL

	Note:
	
	========================================================================
*/
NDIS_STATUS	NICReadRegParameters(
	IN	PRTMP_ADAPTER		pAd,
	IN	NDIS_HANDLE			WrapperConfigurationContext
	)
{
	NDIS_STATUS						Status = NDIS_STATUS_SUCCESS;
	DBGPRINT_S(Status, ("<-- NICReadRegParameters, Status=%x\n", Status));
	return Status;
}

/*
	========================================================================
	
	Routine Description:
		Read initial parameters from EEPROM
		
	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
VOID	NICReadEEPROMParameters(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR		mac_addr)
{
	UINT32			data = 0;
	USHORT			i = 0 , value, value2;
	UCHAR			TmpPhy;
	EEPROM_TX_PWR_STRUC	    Power;
	EEPROM_VERSION_STRUC    Version;
	EEPROM_ANTENNA_STRUC	Antenna;
	EEPROM_NIC_CONFIG2_STRUC    NicConfig2;
#ifdef CONFIG_RT2860V2_SET_MAC_FROM_EEPROM
	USHORT  Addr01,Addr23,Addr45 ;
#endif
	MAC_DW0_STRUC csr2;
	MAC_DW1_STRUC csr3;

	DBGPRINT(RT_DEBUG_TRACE, ("--> NICReadEEPROMParameters\n"));	

	if (pAd->chipOps.eeinit)
		pAd->chipOps.eeinit(pAd);

	// Init EEPROM Address Number, before access EEPROM; if 93c46, EEPROMAddressNum=6, else if 93c66, EEPROMAddressNum=8
	RTMP_IO_READ32(pAd, E2PROM_CSR, &data);
	DBGPRINT(RT_DEBUG_TRACE, ("--> E2PROM_CSR = 0x%x\n", data));

	if((data & 0x30) == 0)
		pAd->EEPROMAddressNum = 6;		// 93C46
	else if((data & 0x30) == 0x10)
		pAd->EEPROMAddressNum = 8;     // 93C66
	else
		pAd->EEPROMAddressNum = 8;     // 93C86
	DBGPRINT(RT_DEBUG_TRACE, ("--> EEPROMAddressNum = %d\n", pAd->EEPROMAddressNum ));

#ifdef CONFIG_RT2860V2_SET_MAC_FROM_EEPROM
	/* Read MAC setting from EEPROM and record as permanent MAC address */
	DBGPRINT(RT_DEBUG_TRACE, ("Initialize MAC Address from E2PROM \n"));

	RT28xx_EEPROM_READ16(pAd, 0x04, Addr01);
	RT28xx_EEPROM_READ16(pAd, 0x06, Addr23);
	RT28xx_EEPROM_READ16(pAd, 0x08, Addr45);

	pAd->PermanentAddress[0] = (UCHAR)(Addr01 & 0xff);
	pAd->PermanentAddress[1] = (UCHAR)(Addr01 >> 8);
	pAd->PermanentAddress[2] = (UCHAR)(Addr23 & 0xff);
	pAd->PermanentAddress[3] = (UCHAR)(Addr23 >> 8);
	pAd->PermanentAddress[4] = (UCHAR)(Addr45 & 0xff);
	pAd->PermanentAddress[5] = (UCHAR)(Addr45 >> 8);

	//more conveninet to test mbssid, so ap's bssid &0xf1
	if (pAd->PermanentAddress[0] == 0xff)
		pAd->PermanentAddress[0] = RandomByte(pAd)&0xf8;
			
	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM MAC: =%02x:%02x:%02x:%02x:%02x:%02x\n", PRINT_MAC(pAd->PermanentAddress)));

	/* Assign the actually working MAC Address */
	if (pAd->bLocalAdminMAC)
		DBGPRINT(RT_DEBUG_TRACE, ("Use the MAC address what is assigned from Configuration file(.dat). \n"));
	else 
#endif
	if (mac_addr &&  strlen((PSTRING)mac_addr) == 17 && (strcmp(mac_addr, "00:00:00:00:00:00") != 0))
	{
		INT		j;
		PSTRING	macptr;

		macptr = (PSTRING) mac_addr;

		for (j=0; j<MAC_ADDR_LEN; j++)
		{
			AtoH(macptr, &pAd->CurrentAddress[j], 1);
			macptr=macptr+3;
		}	
		
		DBGPRINT(RT_DEBUG_TRACE, ("Use the MAC address what is assigned from Moudle Parameter. \n"));
	}
#ifdef CONFIG_RT2860V2_SET_MAC_FROM_EEPROM
	else {
		COPY_MAC_ADDR(pAd->CurrentAddress, pAd->PermanentAddress);
		DBGPRINT(RT_DEBUG_TRACE, ("Use the MAC address what is assigned from EEPROM. \n"));
	}
#endif

	/* Set the current MAC to ASIC */	
	csr2.field.Byte0 = pAd->CurrentAddress[0];
	csr2.field.Byte1 = pAd->CurrentAddress[1];
	csr2.field.Byte2 = pAd->CurrentAddress[2];
	csr2.field.Byte3 = pAd->CurrentAddress[3];
	RTMP_IO_WRITE32(pAd, MAC_ADDR_DW0, csr2.word);
	csr3.word = 0;
	csr3.field.Byte4 = pAd->CurrentAddress[4];
	csr3.field.Byte5 = pAd->CurrentAddress[5];
	csr3.field.U2MeMask = 0xff;
	RTMP_IO_WRITE32(pAd, MAC_ADDR_DW1, csr3.word);
	DBGPRINT_RAW(RT_DEBUG_TRACE,("Current MAC: =%02x:%02x:%02x:%02x:%02x:%02x\n", PRINT_MAC(pAd->CurrentAddress)));

	// Set up the Mac address
	RtmpOSNetDevAddrSet(pAd->net_dev, &pAd->CurrentAddress[0]);

#ifdef LINUX
	//Workaround
	dev_set_mac_address(pAd->net_dev, (void*)pAd->CurrentAddress);
#endif

	// if not return early. cause fail at emulation.
	// Init the channel number for TX channel power	
#if defined (CONFIG_RALINK_RT3883)
	if (IS_RT3883(pAd))
		RTMPRT3883ReadChannelPwr(pAd);
#elif defined (CONFIG_RALINK_RT2883)
	if (IS_RT2883(pAd))
		RTMPRT2883ReadChannelPwr(pAd);
#else
        RTMPReadChannelPwr(pAd);
#endif

	// if E2PROM version mismatch with driver's expectation, then skip
	// all subsequent E2RPOM retieval and set a system error bit to notify GUI
	RT28xx_EEPROM_READ16(pAd, EEPROM_VERSION_OFFSET, Version.word);
	pAd->EepromVersion = Version.field.Version + Version.field.FaeReleaseNumber * 256;
	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: Version = %d, FAE release #%d\n", Version.field.Version, Version.field.FaeReleaseNumber));

	if (Version.field.Version > VALID_EEPROM_VERSION)
	{
		DBGPRINT_ERR(("E2PROM: WRONG VERSION 0x%x, should be %d\n",Version.field.Version, VALID_EEPROM_VERSION));
	}

	// Read BBP default value from EEPROM and store to array(EEPROMDefaultValue) in pAd
	RT28xx_EEPROM_READ16(pAd, EEPROM_NIC1_OFFSET, value);
	pAd->EEPROMDefaultValue[0] = value;

	RT28xx_EEPROM_READ16(pAd, EEPROM_NIC2_OFFSET, value);
	pAd->EEPROMDefaultValue[1] = value;

	//RT28xx_EEPROM_READ16(pAd, 0x38, value);	// Country Region
	RT28xx_EEPROM_READ16(pAd, EEPROM_COUNTRY_REGION, value);	// Country Region

	pAd->EEPROMDefaultValue[2] = value;

	for(i = 0; i < 8; i++)
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_BBP_BASE_OFFSET + i*2, value);
		pAd->EEPROMDefaultValue[i+3] = value;
	}

	// We have to parse NIC configuration 0 at here.
	// If TSSI did not have preloaded value, it should reset the TxAutoAgc to false
	// Therefore, we have to read TxAutoAgc control beforehand.
	// Read Tx AGC control bit
	Antenna.word = pAd->EEPROMDefaultValue[0];
	if (Antenna.word == 0xFFFF)
	{
		{
#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
		Antenna.word = 0;
#if defined (CONFIG_RALINK_RT2883)
		Antenna.field.RfIcType = RFIC_2853;
#elif defined (CONFIG_RALINK_RT3883)
		Antenna.field.RfIcType = RFIC_3853;
#endif
		Antenna.field.TxPath = 3;
		Antenna.field.RxPath = 3;
		printk("E2PROM error, hard code as 0x%04x\n", Antenna.word);
#else
		Antenna.word = 0;
#if defined(CONFIG_RALINK_RT3050_1T1R)
		Antenna.field.RfIcType = RFIC_3020;
		Antenna.field.TxPath = 1;
		Antenna.field.RxPath = 1;
		printk("E2PROM error, hard code as 0x%04x\n", Antenna.word);
#elif defined(CONFIG_RALINK_RT3051_1T2R)
		Antenna.field.RfIcType = RFIC_3021;
		Antenna.field.TxPath = 1;
		Antenna.field.RxPath = 2;
		printk("E2PROM error, hard code as 0x%04x\n", Antenna.word);
#elif defined(CONFIG_RALINK_RT3052_2T2R)
		Antenna.field.RfIcType = RFIC_3022;
		Antenna.field.TxPath = 2;
		Antenna.field.RxPath = 2;
		printk("E2PROM error, hard code as 0x%04x\n", Antenna.word);
#else
		Antenna.field.RfIcType = RFIC_3020;
		Antenna.field.TxPath = 1;
		Antenna.field.RxPath = 1;
		printk("E2PROM error, hard code as 0x%04x\n", Antenna.word);
#endif
#endif /* CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 */
		}
	}

	// Choose the desired Tx&Rx stream.
	if ((pAd->CommonCfg.TxStream == 0) || (pAd->CommonCfg.TxStream > Antenna.field.TxPath))
		pAd->CommonCfg.TxStream = Antenna.field.TxPath;

	if ((pAd->CommonCfg.RxStream == 0) || (pAd->CommonCfg.RxStream > Antenna.field.RxPath))
	{
		pAd->CommonCfg.RxStream = Antenna.field.RxPath;
	
		if ((pAd->MACVersion != RALINK_3883_VERSION) &&
			(pAd->MACVersion != RALINK_2883_VERSION) &&
			(pAd->CommonCfg.RxStream > 2))
		{
			// only 2 Rx streams for RT2860 series
			pAd->CommonCfg.RxStream = 2;
		}
	}

	NicConfig2.word = pAd->EEPROMDefaultValue[1];

#ifdef WSC_INCLUDED
	/* WSC hardware push button function 0811 */
// TODO: Shiang, Need to check about the gpio value, it seems has issue in RT2880/RT3052 board.
//	if ((pAd->MACVersion == 0x28600100) || (pAd->MACVersion == 0x28700100))
//	WSC_HDR_BTN_MR_HDR_SUPPORT_SET(pAd, NicConfig2.field.EnableWPSPBC);
        WSC_HDR_BTN_MR_HDR_SUPPORT_SET(pAd, 0);
#endif // WSC_INCLUDED //

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (NicConfig2.word == 0xffff)
		{
			NicConfig2.word = 0;
		}
	}
#endif // CONFIG_AP_SUPPORT //	

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if ((NicConfig2.word & 0x00ff) == 0xff)
		{
			NicConfig2.word &= 0xff00;
		}

		if ((NicConfig2.word >> 8) == 0xff)
		{
			NicConfig2.word &= 0x00ff;
		}
	}
#endif // CONFIG_STA_SUPPORT //

	if (NicConfig2.field.DynamicTxAgcControl == 1)
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = TRUE;
	else
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = FALSE;
	
	/* Save value for future using */
	pAd->NicConfig2.word = NicConfig2.word;
	
	// Save the antenna for future use
	pAd->Antenna.word = Antenna.word;

	// Set the RfICType here, then we can initialize RFIC related operation callbacks
	pAd->Mlme.RealRxPath = (UCHAR) Antenna.field.RxPath;

	//Chipid
	pAd->RfIcType = (UCHAR) Antenna.field.RfIcType;

	printk("NICReadEEPROMParameters: RxPath = %d, TxPath = %d, RFICType = %d\n",
		Antenna.field.RxPath, Antenna.field.TxPath, Antenna.field.RfIcType);

#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_MAC_PCI
	sprintf((PSTRING) pAd->nickname, STA_NIC_DEVICE_NAME);
#endif // RTMP_MAC_PCI //
#endif // CONFIG_STA_SUPPORT //

#ifdef RTMP_RF_RW_SUPPORT
	RtmpChipOpsRFHook(pAd);                                                                                                             
#endif

	//
	// Reset PhyMode if we don't support 802.11a
	// Only RFIC_2850 & RFIC_2750 support 802.11a
	//
	if ((Antenna.field.RfIcType != RFIC_2850)
		&& (Antenna.field.RfIcType != RFIC_2750)
		&& (Antenna.field.RfIcType != RFIC_3052)
		&& (Antenna.field.RfIcType != RFIC_2853)
		&& (Antenna.field.RfIcType != RFIC_3853))
	{
		if ((pAd->CommonCfg.PhyMode == PHY_11ABG_MIXED) || 
			(pAd->CommonCfg.PhyMode == PHY_11A))
			pAd->CommonCfg.PhyMode = PHY_11BG_MIXED;
#ifdef DOT11_N_SUPPORT
		else if ((pAd->CommonCfg.PhyMode == PHY_11ABGN_MIXED)	|| 
				 (pAd->CommonCfg.PhyMode == PHY_11AN_MIXED) 	|| 
				 (pAd->CommonCfg.PhyMode == PHY_11AGN_MIXED) 	||
				 (pAd->CommonCfg.PhyMode == PHY_11N_5G))
			pAd->CommonCfg.PhyMode = PHY_11BGN_MIXED;
#endif // DOT11_N_SUPPORT //
	}
	
	// Read TSSI reference and TSSI boundary for temperature compensation. This is ugly
	// 0. 11b/g
	{
		/* these are tempature reference value (0x00 ~ 0xFE)
		   ex: 0x00 0x15 0x25 0x45 0x88 0xA0 0xB5 0xD0 0xF0
		   TssiPlusBoundaryG [4] [3] [2] [1] [0] (smaller) +
		   TssiMinusBoundaryG[0] [1] [2] [3] [4] (larger) */
		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TSSI_BOUND1, Power.word);
		pAd->TssiMinusBoundaryG[4] = Power.field.Byte0;
		pAd->TssiMinusBoundaryG[3] = Power.field.Byte1;
		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TSSI_BOUND2, Power.word);
		pAd->TssiMinusBoundaryG[2] = Power.field.Byte0;
		pAd->TssiMinusBoundaryG[1] = Power.field.Byte1;
		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TSSI_BOUND3, Power.word);
		pAd->TssiRefG   = Power.field.Byte0; /* reference value [0] */
		pAd->TssiPlusBoundaryG[1] = Power.field.Byte1;
		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TSSI_BOUND4, Power.word);
		pAd->TssiPlusBoundaryG[2] = Power.field.Byte0;
		pAd->TssiPlusBoundaryG[3] = Power.field.Byte1;
		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TSSI_BOUND5, Power.word);
		pAd->TssiPlusBoundaryG[4] = Power.field.Byte0;
		pAd->TxAgcStepG = Power.field.Byte1;    
		pAd->TxAgcCompensateG = 0;
		pAd->TssiMinusBoundaryG[0] = pAd->TssiRefG;
		pAd->TssiPlusBoundaryG[0]  = pAd->TssiRefG;

		// Disable TxAgc if the based value is not right
		if (pAd->TssiRefG == 0xff)
			pAd->bAutoTxAgcG = FALSE;

		DBGPRINT(RT_DEBUG_TRACE,("E2PROM: G Tssi[-4 .. +4] = %d %d %d %d - %d -%d %d %d %d, step=%d, tuning=%d\n",
			pAd->TssiMinusBoundaryG[4], pAd->TssiMinusBoundaryG[3], pAd->TssiMinusBoundaryG[2], pAd->TssiMinusBoundaryG[1],
			pAd->TssiRefG,
			pAd->TssiPlusBoundaryG[1], pAd->TssiPlusBoundaryG[2], pAd->TssiPlusBoundaryG[3], pAd->TssiPlusBoundaryG[4],
			pAd->TxAgcStepG, pAd->bAutoTxAgcG));
	}	
	// 1. 11a
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_A_TSSI_BOUND1, Power.word);
		pAd->TssiMinusBoundaryA[4] = Power.field.Byte0;
		pAd->TssiMinusBoundaryA[3] = Power.field.Byte1;
		RT28xx_EEPROM_READ16(pAd, EEPROM_A_TSSI_BOUND2, Power.word);
		pAd->TssiMinusBoundaryA[2] = Power.field.Byte0;
		pAd->TssiMinusBoundaryA[1] = Power.field.Byte1;
		RT28xx_EEPROM_READ16(pAd, EEPROM_A_TSSI_BOUND3, Power.word);
		pAd->TssiRefA   = Power.field.Byte0;
		pAd->TssiPlusBoundaryA[1] = Power.field.Byte1;
		RT28xx_EEPROM_READ16(pAd, EEPROM_A_TSSI_BOUND4, Power.word);
		pAd->TssiPlusBoundaryA[2] = Power.field.Byte0;
		pAd->TssiPlusBoundaryA[3] = Power.field.Byte1;
		RT28xx_EEPROM_READ16(pAd, EEPROM_A_TSSI_BOUND5, Power.word);
		pAd->TssiPlusBoundaryA[4] = Power.field.Byte0;
		pAd->TxAgcStepA = Power.field.Byte1;    
		pAd->TxAgcCompensateA = 0;
		pAd->TssiMinusBoundaryA[0] = pAd->TssiRefA;
		pAd->TssiPlusBoundaryA[0]  = pAd->TssiRefA;

		// Disable TxAgc if the based value is not right
		if (pAd->TssiRefA == 0xff)
			pAd->bAutoTxAgcA = FALSE;

		DBGPRINT(RT_DEBUG_TRACE,("E2PROM: A Tssi[-4 .. +4] = %d %d %d %d - %d -%d %d %d %d, step=%d, tuning=%d\n",
			pAd->TssiMinusBoundaryA[4], pAd->TssiMinusBoundaryA[3], pAd->TssiMinusBoundaryA[2], pAd->TssiMinusBoundaryA[1],
			pAd->TssiRefA,
			pAd->TssiPlusBoundaryA[1], pAd->TssiPlusBoundaryA[2], pAd->TssiPlusBoundaryA[3], pAd->TssiPlusBoundaryA[4],
			pAd->TxAgcStepA, pAd->bAutoTxAgcA));
	}	
	pAd->BbpRssiToDbmDelta = 0x0;
	
	// Read frequency offset setting for RF
	RT28xx_EEPROM_READ16(pAd, EEPROM_FREQ_OFFSET, value);
	if ((value & 0x00FF) != 0x00FF)
		pAd->RfFreqOffset = (ULONG) (value & 0x00FF);
	else
		pAd->RfFreqOffset = 0;

	if (pAd->RfFreqDelta & 0x10)
	{
		pAd->RfFreqOffset = (pAd->RfFreqOffset >= pAd->RfFreqDelta)? (pAd->RfFreqOffset - (pAd->RfFreqDelta & 0xf)) : 0;
	}
	else
	{
		pAd->RfFreqOffset = ((pAd->RfFreqOffset + pAd->RfFreqDelta) < 0x40)? (pAd->RfFreqOffset + (pAd->RfFreqDelta & 0xf)) : 0x3f;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: RF FreqOffset=0x%lx \n", pAd->RfFreqOffset));

	//CountryRegion byte offset (38h)
	value = pAd->EEPROMDefaultValue[2] >> 8;		// 2.4G band
	value2 = pAd->EEPROMDefaultValue[2] & 0x00FF;	// 5G band
	
	if ((value <= REGION_MAXIMUM_BG_BAND) && (value2 <= REGION_MAXIMUM_A_BAND))
	{
		pAd->CommonCfg.CountryRegion = ((UCHAR) value) | 0x80;
		pAd->CommonCfg.CountryRegionForABand = ((UCHAR) value2) | 0x80;
		TmpPhy = pAd->CommonCfg.PhyMode;
		pAd->CommonCfg.PhyMode = 0xff;
		RTMPSetPhyMode(pAd, TmpPhy);
#ifdef DOT11_N_SUPPORT
		SetCommonHT(pAd);
#endif // DOT11_N_SUPPORT //
	}

	//
	// Get RSSI Offset on EEPROM 0x9Ah & 0x9Ch.
	// The valid value are (-10 ~ 10) 
	// 
	RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_BG_OFFSET, value);
	pAd->BGRssiOffset0 = value & 0x00ff;
	pAd->BGRssiOffset1 = (value >> 8);
	RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_BG_OFFSET+2, value);
	pAd->BGRssiOffset2 = value & 0x00ff;
	pAd->ALNAGain1 = (value >> 8);
	RT28xx_EEPROM_READ16(pAd, EEPROM_LNA_OFFSET, value);
	pAd->BLNAGain = value & 0x00ff;
	pAd->ALNAGain0 = (value >> 8);
	
	// Validate 11b/g RSSI_0 offset.
	if ((pAd->BGRssiOffset0 < -10) || (pAd->BGRssiOffset0 > 10))
		pAd->BGRssiOffset0 = 0;

	// Validate 11b/g RSSI_1 offset.
	if ((pAd->BGRssiOffset1 < -10) || (pAd->BGRssiOffset1 > 10))
		pAd->BGRssiOffset1 = 0;

	// Validate 11b/g RSSI_2 offset.
	if ((pAd->BGRssiOffset2 < -10) || (pAd->BGRssiOffset2 > 10))
		pAd->BGRssiOffset2 = 0;
		
	RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_A_OFFSET, value);
	pAd->ARssiOffset0 = value & 0x00ff;
	pAd->ARssiOffset1 = (value >> 8);
	RT28xx_EEPROM_READ16(pAd, (EEPROM_RSSI_A_OFFSET+2), value);
	pAd->ARssiOffset2 = value & 0x00ff;
	pAd->ALNAGain2 = (value >> 8);

#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	RT28xx_EEPROM_READ16(pAd, EEPROM_LNA_OFFSET2, value);
	pAd->ALNAGain1 = value & 0x00ff;
	pAd->ALNAGain2 = (value >> 8);
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //

	if (((UCHAR)pAd->ALNAGain1 == 0xFF) || (pAd->ALNAGain1 == 0x00))
		pAd->ALNAGain1 = pAd->ALNAGain0;
	if (((UCHAR)pAd->ALNAGain2 == 0xFF) || (pAd->ALNAGain2 == 0x00))
		pAd->ALNAGain2 = pAd->ALNAGain0;

	// Validate 11a RSSI_0 offset.
	if ((pAd->ARssiOffset0 < -10) || (pAd->ARssiOffset0 > 10))
		pAd->ARssiOffset0 = 0;

	// Validate 11a RSSI_1 offset.
	if ((pAd->ARssiOffset1 < -10) || (pAd->ARssiOffset1 > 10))
		pAd->ARssiOffset1 = 0;

	//Validate 11a RSSI_2 offset.
	if ((pAd->ARssiOffset2 < -10) || (pAd->ARssiOffset2 > 10))
		pAd->ARssiOffset2 = 0;

	
	//
	// Get LED Setting.
	//
	RT28xx_EEPROM_READ16(pAd, EEPROM_FREQ_OFFSET, value);
	pAd->LedCntl.word = (value>>8);
	RT28xx_EEPROM_READ16(pAd, EEPROM_LED1_OFFSET, value);
	pAd->Led1 = value;
	RT28xx_EEPROM_READ16(pAd, EEPROM_LED2_OFFSET, value);
	pAd->Led2 = value;
	RT28xx_EEPROM_READ16(pAd, EEPROM_LED3_OFFSET, value);
	pAd->Led3 = value;
		
#if defined (CONFIG_RALINK_RT3883)
	if (IS_RT3883(pAd))
		RTMPRT3883ReadTxPwrPerRate(pAd);
#elif defined (CONFIG_RALINK_RT2883)
	if (IS_RT2883(pAd))
		RTMPRT2883ReadTxPwrPerRate(pAd);
#else
	RTMPReadTxPwrPerRate(pAd);
#endif

#ifdef SINGLE_SKU
	RT28xx_EEPROM_READ16(pAd, EEPROM_DEFINE_MAX_TXPWR, pAd->CommonCfg.DefineMaxTxPwr);
	if (pAd->CommonCfg.DefineMaxTxPwr <= 0x50 && pAd->CommonCfg.AntGain > 0 && pAd->CommonCfg.BandedgeDelta >= 0)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Single SKU Mode is enabled\n"));
		pAd->CommonCfg.bSKUMode = TRUE;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Single SKU Mode is disabled\n"));
		pAd->CommonCfg.bSKUMode = FALSE;
	}
#endif // SINGLE_SKU //

#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	{
		UCHAR BbpValue;
		
		// select Rx chain 0
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R27, &BbpValue);
		BbpValue &= ~0x60;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, BbpValue);

		RT28xx_EEPROM_READ16(pAd, 0x1a0, value);
		BbpValue = (UCHAR) (value & 0xff);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R174, BbpValue);
		BbpValue = (UCHAR) (value >> 8);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R175, BbpValue);

		RT28xx_EEPROM_READ16(pAd, 0x1a2, value);
		BbpValue = (UCHAR) (value & 0xff);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R176, BbpValue);
		BbpValue = (UCHAR) (value >> 8);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R177, BbpValue);


		// select Rx chain 1
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R27, &BbpValue);
		BbpValue &= ~0x60;
		BbpValue |= 0x20;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, BbpValue);

		RT28xx_EEPROM_READ16(pAd, 0x1a4, value);
		BbpValue = (UCHAR) (value >> 8);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R175, BbpValue);

		RT28xx_EEPROM_READ16(pAd, 0x1a6, value);
		BbpValue = (UCHAR) (value >> 8);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R177, BbpValue);


		// select Rx chain 2
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R27, &BbpValue);
		BbpValue &= ~0x60;
		BbpValue |= 0x40;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, BbpValue);

		RT28xx_EEPROM_READ16(pAd, 0x1a8, value);
		BbpValue = (UCHAR) (value & 0xff);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R174, BbpValue);
		BbpValue = (UCHAR) (value >> 8);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R175, BbpValue);

		RT28xx_EEPROM_READ16(pAd, 0x1aa, value);
		BbpValue = (UCHAR) (value & 0xff);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R176, BbpValue);
		BbpValue = (UCHAR) (value >> 8);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R177, BbpValue);

	}
#ifdef TXBF_SUPPORT
	// Read ITxBF compensation parameters from EEPROM
	for (i=0; i<6; i++) {
		RT28xx_EEPROM_READ16(pAd, EEPROM_ITXBF_CAL_RX0+2*i, value);
		pAd->EEPROMITxBFCalParams[i] = value;
	}
#endif // TXBF_SUPPORT //

#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //


	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICReadEEPROMParameters\n"));
}


/*
	========================================================================
	
	Routine Description:
		Set default value from EEPROM
		
	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	
	Note:
		
	========================================================================
*/
VOID	NICInitAsicFromEEPROM(
	IN	PRTMP_ADAPTER	pAd)
{
#ifdef CONFIG_STA_SUPPORT
	UINT32					data = 0;
	UCHAR	BBPR1 = 0; 
#endif // CONFIG_STA_SUPPORT //
	USHORT					i;
//	EEPROM_ANTENNA_STRUC	Antenna;
	EEPROM_NIC_CONFIG2_STRUC    NicConfig2;
	UCHAR	BBPR3 = 0;
#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	UCHAR	bbpValue;
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //
	
	DBGPRINT(RT_DEBUG_TRACE, ("--> NICInitAsicFromEEPROM\n"));
	for(i = 3; i < NUM_EEPROM_BBP_PARMS; i++)
	{
		UCHAR BbpRegIdx, BbpValue;
	
		if ((pAd->EEPROMDefaultValue[i] != 0xFFFF) && (pAd->EEPROMDefaultValue[i] != 0))
		{
			BbpRegIdx = (UCHAR)(pAd->EEPROMDefaultValue[i] >> 8);
			BbpValue  = (UCHAR)(pAd->EEPROMDefaultValue[i] & 0xff);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BbpRegIdx, BbpValue);
		}
	}


	NicConfig2.word = pAd->EEPROMDefaultValue[1];

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (NicConfig2.word == 0xffff)
		{
			NicConfig2.word = 0;
		}
	}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if ((NicConfig2.word & 0x00ff) == 0xff)
		{
			NicConfig2.word &= 0xff00;
		}

		if ((NicConfig2.word >> 8) == 0xff)
		{
			NicConfig2.word &= 0x00ff;
		}
	}
#endif // CONFIG_STA_SUPPORT //

	// Save the antenna for future use
	pAd->NicConfig2.word = NicConfig2.word;


	//
	// Send LED Setting to MCU.
	//
	if (pAd->LedCntl.word == 0xFF)
	{
		pAd->LedCntl.word = 0x01;
		pAd->Led1 = 0x5555;
		pAd->Led2 = 0x2221;

#ifdef RTMP_MAC_PCI
		pAd->Led3 = 0xA9F8;
#endif // RTMP_MAC_PCI //
	}

	AsicSendCommandToMcu(pAd, 0x52, 0xff, (UCHAR)pAd->Led1, (UCHAR)(pAd->Led1 >> 8));
	AsicSendCommandToMcu(pAd, 0x53, 0xff, (UCHAR)pAd->Led2, (UCHAR)(pAd->Led2 >> 8));
	AsicSendCommandToMcu(pAd, 0x54, 0xff, (UCHAR)pAd->Led3, (UCHAR)(pAd->Led3 >> 8));
	pAd->LedIndicatorStrength = 0xFF;
	RTMPSetSignalLED(pAd, -100);	// Force signal strength Led to be turned off, before link up

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		// Read Hardware controlled Radio state enable bit
		if (NicConfig2.field.HardwareRadioControl == 1)
		{
			pAd->StaCfg.bHardwareRadio = TRUE;

			// Read GPIO pin2 as Hardware controlled radio state
			RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &data);
			if ((data & 0x04) == 0)
			{
				pAd->StaCfg.bHwRadio = FALSE;
				pAd->StaCfg.bRadio = FALSE;
//				RTMP_IO_WRITE32(pAd, PWR_PIN_CFG, 0x00001818);
				RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
			}
		}
		else
			pAd->StaCfg.bHardwareRadio = FALSE;		

		if (pAd->StaCfg.bRadio == FALSE)
		{
			RTMPSetLED(pAd, LED_RADIO_OFF);
		}
		else
		{
			RTMPSetLED(pAd, LED_RADIO_ON);
#ifdef RTMP_MAC_PCI

			AsicSendCommandToMcu(pAd, 0x30, PowerRadioOffCID, 0xff, 0x02);
			AsicCheckCommanOk(pAd, PowerRadioOffCID);

			//AsicSendCommandToMcu(pAd, 0x30, 0xff, 0xff, 0x02);
			AsicSendCommandToMcu(pAd, 0x31, PowerWakeCID, 0x00, 0x00);
			// 2-1. wait command ok.
			AsicCheckCommanOk(pAd, PowerWakeCID);
#endif // RTMP_MAC_PCI //
		}
	}
#endif // CONFIG_STA_SUPPORT //

	// Turn off patching for cardbus controller
	if (NicConfig2.field.CardbusAcceleration == 1)
	{
//		pAd->bTest1 = TRUE;
	}

	if (NicConfig2.field.DynamicTxAgcControl == 1)
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = TRUE;
	else
		pAd->bAutoTxAgcA = pAd->bAutoTxAgcG = FALSE;
	//
	// Since BBP has been progamed, to make sure BBP setting will be 
	// upate inside of AsicAntennaSelect, so reset to UNKNOWN_BAND!!
	//
	pAd->CommonCfg.BandState = UNKNOWN_BAND;
	
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

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		// Handle the difference when 1T
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BBPR1);
		if(pAd->Antenna.field.TxPath == 1)
		{
		BBPR1 &= (~0x18);
		}
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BBPR1);
    
		DBGPRINT(RT_DEBUG_TRACE, ("Use Hw Radio Control Pin=%d; if used Pin=%d;\n", 
					pAd->CommonCfg.bHardwareRadio, pAd->CommonCfg.bHardwareRadio));
	}
#endif // CONFIG_STA_SUPPORT //


#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	// Set ITxBF compensation parameters if they are not all the uninitialized value 0xFFFF

#ifdef TXBF_SUPPORT	
	if ((pAd->EEPROMITxBFCalParams[0] & pAd->EEPROMITxBFCalParams[1] & pAd->EEPROMITxBFCalParams[2] &
		pAd->EEPROMITxBFCalParams[3] & pAd->EEPROMITxBFCalParams[4] & pAd->EEPROMITxBFCalParams[5]) != 0xFFFF) {
		
		// Select Ant0 
		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R27, &bbpValue);
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, (bbpValue & ~0x60));
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R174, (pAd->EEPROMITxBFCalParams[0] & 0xFF));
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R175, (pAd->EEPROMITxBFCalParams[0] >> 8));
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R176, (pAd->EEPROMITxBFCalParams[1] & 0xFF));
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R177, (pAd->EEPROMITxBFCalParams[1] >> 8));
		
		// Select Ant1 
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, (bbpValue & ~0x60) | 0x20);
		//BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R174, (pAd->EEPROMITxBFCalParams[2] & 0xFF));
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R175, (pAd->EEPROMITxBFCalParams[2] >> 8));
		//BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R176, (pAd->EEPROMITxBFCalParams[3] & 0xFF));
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R177, (pAd->EEPROMITxBFCalParams[3] >> 8));
		
		// Select Ant2
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, (bbpValue & ~0x60) | 0x40);
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R174, (pAd->EEPROMITxBFCalParams[4] & 0xFF));
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R175, (pAd->EEPROMITxBFCalParams[4] >> 8));
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R176, (pAd->EEPROMITxBFCalParams[5] & 0xFF));
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R177, (pAd->EEPROMITxBFCalParams[5] >> 8));
		
		// Enable Phase Compensation
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R173, 0x3C);

		// Restore R27
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, bbpValue);
	}
	else
		printk("EEPROM TxBF is not set\n");
#endif // TXBF_SUPPORT //
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //

	DBGPRINT(RT_DEBUG_TRACE, ("TxPath = %d, RxPath = %d, RFIC=%d, Polar+LED mode=%x\n", 
				pAd->Antenna.field.TxPath, pAd->Antenna.field.RxPath, 
				pAd->RfIcType, pAd->LedCntl.word));
	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICInitAsicFromEEPROM\n"));
}

/*
	========================================================================
	
	Routine Description:
		Initialize NIC hardware

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
NDIS_STATUS	NICInitializeAdapter(
	IN	PRTMP_ADAPTER	pAd,
	IN   BOOLEAN    bHardReset)
{
	NDIS_STATUS     Status = NDIS_STATUS_SUCCESS;
	WPDMA_GLO_CFG_STRUC	GloCfg;
#ifdef RTMP_MAC_PCI
	UINT32			Value;
	DELAY_INT_CFG_STRUC	IntCfg;
#endif // RTMP_MAC_PCI //
//	INT_MASK_CSR_STRUC		IntMask;
	ULONG	i =0;
#ifndef RTMP_RBUS_SUPPORT
	ULONG	j=0;
#endif
	AC_TXOP_CSR0_STRUC	csr0;

	DBGPRINT(RT_DEBUG_TRACE, ("--> NICInitializeAdapter\n"));
	
	// 3. Set DMA global configuration except TX_DMA_EN and RX_DMA_EN bits:
#ifndef RTMP_RBUS_SUPPORT
retry:
#endif
	i = 0;
	do
	{
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);
		if ((GloCfg.field.TxDMABusy == 0)  && (GloCfg.field.RxDMABusy == 0))
			break;
		
		RTMPusecDelay(1000);
		i++;
	}while ( i<100);
	DBGPRINT(RT_DEBUG_TRACE, ("<== DMA offset 0x208 = 0x%x\n", GloCfg.word));	
	GloCfg.word &= 0xff0;
	GloCfg.field.EnTXWriteBackDDONE =1;
	RTMP_IO_WRITE32(pAd, WPDMA_GLO_CFG, GloCfg.word);
	
	// Record HW Beacon offset
	pAd->BeaconOffset[0] = HW_BEACON_BASE0;
	pAd->BeaconOffset[1] = HW_BEACON_BASE1;
	pAd->BeaconOffset[2] = HW_BEACON_BASE2;
	pAd->BeaconOffset[3] = HW_BEACON_BASE3;
	pAd->BeaconOffset[4] = HW_BEACON_BASE4;
	pAd->BeaconOffset[5] = HW_BEACON_BASE5;
	pAd->BeaconOffset[6] = HW_BEACON_BASE6;
	pAd->BeaconOffset[7] = HW_BEACON_BASE7;
#if defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT3883) && defined (CONFIG_16MBSSID_MODE)
	pAd->BeaconOffset[8] = HW_BEACON_BASE8;
	pAd->BeaconOffset[9] = HW_BEACON_BASE9;
	pAd->BeaconOffset[10] = HW_BEACON_BASE10;
	pAd->BeaconOffset[11] = HW_BEACON_BASE11;
	pAd->BeaconOffset[12] = HW_BEACON_BASE12;
	pAd->BeaconOffset[13] = HW_BEACON_BASE13;
	pAd->BeaconOffset[14] = HW_BEACON_BASE14;
	pAd->BeaconOffset[15] = HW_BEACON_BASE15;
#endif
	
	//
	// write all shared Ring's base address into ASIC
	//

	// asic simulation sequence put this ahead before loading firmware.
	// pbf hardware reset
#ifdef RTMP_MAC_PCI
	RTMP_IO_WRITE32(pAd, WPDMA_RST_IDX, 0x1003f);	// 0x10000 for reset rx, 0x3f resets all 6 tx rings.
	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0xe1f);
	RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, 0xe00);
#endif // RTMP_MAC_PCI //

	// Initialze ASIC for TX & Rx operation
	if (NICInitializeAsic(pAd , bHardReset) != NDIS_STATUS_SUCCESS)
	{
#ifndef RTMP_RBUS_SUPPORT
		if (j++ == 0)
		{
			NICLoadFirmware(pAd);
			goto retry;
		}
#endif
		return NDIS_STATUS_FAILURE;
	}


#ifdef RTMP_MAC_PCI
	// Write AC_BK base address register
	Value = RTMP_GetPhysicalAddressLow(pAd->TxRing[QID_AC_BK].Cell[0].AllocPa);
	RTMP_IO_WRITE32(pAd, TX_BASE_PTR1, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR1 : 0x%x\n", Value));

	// Write AC_BE base address register
	Value = RTMP_GetPhysicalAddressLow(pAd->TxRing[QID_AC_BE].Cell[0].AllocPa);
	RTMP_IO_WRITE32(pAd, TX_BASE_PTR0, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR0 : 0x%x\n", Value));

	// Write AC_VI base address register
	Value = RTMP_GetPhysicalAddressLow(pAd->TxRing[QID_AC_VI].Cell[0].AllocPa);
	RTMP_IO_WRITE32(pAd, TX_BASE_PTR2, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR2 : 0x%x\n", Value));

	// Write AC_VO base address register
	Value = RTMP_GetPhysicalAddressLow(pAd->TxRing[QID_AC_VO].Cell[0].AllocPa);	
	RTMP_IO_WRITE32(pAd, TX_BASE_PTR3, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR3 : 0x%x\n", Value));

	// Write HCCA base address register
	  Value = RTMP_GetPhysicalAddressLow(pAd->TxRing[QID_HCCA].Cell[0].AllocPa);
	  RTMP_IO_WRITE32(pAd, TX_BASE_PTR4, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR4 : 0x%x\n", Value));

	// Write MGMT_BASE_CSR register
	Value = RTMP_GetPhysicalAddressLow(pAd->MgmtRing.Cell[0].AllocPa);
	RTMP_IO_WRITE32(pAd, TX_BASE_PTR5, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> TX_BASE_PTR5 : 0x%x\n", Value));

	// Write RX_BASE_CSR register
	Value = RTMP_GetPhysicalAddressLow(pAd->RxRing.Cell[0].AllocPa);
	RTMP_IO_WRITE32(pAd, RX_BASE_PTR, Value);
	DBGPRINT(RT_DEBUG_TRACE, ("--> RX_BASE_PTR : 0x%x\n", Value));

	// Init RX Ring index pointer
	pAd->RxRing.RxSwReadIdx = 0;
	pAd->RxRing.RxCpuIdx = RX_RING_SIZE-1;
	RTMP_IO_WRITE32(pAd, RX_CRX_IDX, pAd->RxRing.RxCpuIdx);
	
	// Init TX rings index pointer
	{
		for (i=0; i<NUM_OF_TX_RING; i++)
		{
			pAd->TxRing[i].TxSwFreeIdx = 0;
			pAd->TxRing[i].TxCpuIdx = 0;
			RTMP_IO_WRITE32(pAd, (TX_CTX_IDX0 + i * 0x10) ,  pAd->TxRing[i].TxCpuIdx);
		}
	}

	// init MGMT ring index pointer
	pAd->MgmtRing.TxSwFreeIdx = 0;
	pAd->MgmtRing.TxCpuIdx = 0;
	RTMP_IO_WRITE32(pAd, TX_MGMTCTX_IDX,  pAd->MgmtRing.TxCpuIdx);

	//
	// set each Ring's SIZE  into ASIC. Descriptor Size is fixed by design.
	//

	// Write TX_RING_CSR0 register
	Value = TX_RING_SIZE;
	RTMP_IO_WRITE32(pAd, TX_MAX_CNT0, Value);
	RTMP_IO_WRITE32(pAd, TX_MAX_CNT1, Value);
	RTMP_IO_WRITE32(pAd, TX_MAX_CNT2, Value);
	RTMP_IO_WRITE32(pAd, TX_MAX_CNT3, Value);
	RTMP_IO_WRITE32(pAd, TX_MAX_CNT4, Value);
	Value = MGMT_RING_SIZE;
	RTMP_IO_WRITE32(pAd, TX_MGMTMAX_CNT, Value);

	// Write RX_RING_CSR register
	Value = RX_RING_SIZE;
	RTMP_IO_WRITE32(pAd, RX_MAX_CNT, Value);
#endif // RTMP_MAC_PCI //


	// WMM parameter
	csr0.word = 0;
	RTMP_IO_WRITE32(pAd, WMM_TXOP0_CFG, csr0.word);
	if (pAd->CommonCfg.PhyMode == PHY_11B)
	{
		csr0.field.Ac0Txop = 192;	// AC_VI: 192*32us ~= 6ms
		csr0.field.Ac1Txop = 96;	// AC_VO: 96*32us  ~= 3ms
	}
	else
	{
		csr0.field.Ac0Txop = 96;	// AC_VI: 96*32us ~= 3ms
		csr0.field.Ac1Txop = 48;	// AC_VO: 48*32us ~= 1.5ms
	}
	RTMP_IO_WRITE32(pAd, WMM_TXOP1_CFG, csr0.word);


#ifdef RTMP_MAC_PCI
	// 3. Set DMA global configuration except TX_DMA_EN and RX_DMA_EN bits:
	i = 0;
	do
	{
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);
		if ((GloCfg.field.TxDMABusy == 0)  && (GloCfg.field.RxDMABusy == 0))
			break;
		
		RTMPusecDelay(1000);
		i++;
	}while ( i < 100);

	GloCfg.word &= 0xff0;
	GloCfg.field.EnTXWriteBackDDONE =1;
	RTMP_IO_WRITE32(pAd, WPDMA_GLO_CFG, GloCfg.word);
	
	IntCfg.word = 0;
	RTMP_IO_WRITE32(pAd, DELAY_INT_CFG, IntCfg.word);
#endif // RTMP_MAC_PCI //


	// reset action
	// Load firmware
	//  Status = NICLoadFirmware(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICInitializeAdapter\n"));
	return Status;
}

/*
	========================================================================
	
	Routine Description:
		Initialize ASIC

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
NDIS_STATUS	NICInitializeAsic(
	IN	PRTMP_ADAPTER	pAd,
	IN  BOOLEAN		bHardReset)
{
	ULONG			Index = 0;
	UCHAR			R0 = 0xff;
	UINT32			MacCsr12 = 0, Counter = 0;
	USHORT			KeyIdx;
	INT				i,apidx;

	DBGPRINT(RT_DEBUG_TRACE, ("--> NICInitializeAsic\n"));

#ifdef RTMP_MAC_PCI
	RTMP_IO_WRITE32(pAd, PWR_PIN_CFG, 0x3);	// To fix driver disable/enable hang issue when radio off
	if (bHardReset == TRUE)
	{
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x3);
	}
	else
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x1);

	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x0);
	// Initialize MAC register to default value
	for (Index = 0; Index < NUM_MAC_REG_PARMS; Index++)
	{
		RTMP_IO_WRITE32(pAd, MACRegTable[Index].Register, MACRegTable[Index].Value);
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		for (Index = 0; Index < NUM_AP_MAC_REG_PARMS; Index++)
		{
			RTMP_IO_WRITE32(pAd, APMACRegTable[Index].Register, APMACRegTable[Index].Value);
		}
	}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		for (Index = 0; Index < NUM_STA_MAC_REG_PARMS; Index++)
		{
			RTMP_IO_WRITE32(pAd, STAMACRegTable[Index].Register, STAMACRegTable[Index].Value);
		}
	}
#endif // CONFIG_STA_SUPPORT //



#endif // RTMP_MAC_PCI //

	//
	// Before program BBP, we need to wait BBP/RF get wake up.
	//
	Index = 0;
	do
	{
		RTMP_IO_READ32(pAd, MAC_STATUS_CFG, &MacCsr12);

		if ((MacCsr12 & 0x03) == 0)	// if BB.RF is stable
			break;
		
		DBGPRINT(RT_DEBUG_TRACE, ("Check MAC_STATUS_CFG  = Busy = %x\n", MacCsr12));
		RTMPusecDelay(1000);
	} while (Index++ < 100);

#ifdef RTMP_MAC_PCI
	// The commands to firmware should be after these commands, these commands will init firmware
	// PCI and USB are not the same because PCI driver needs to wait for PCI bus ready
	RTMP_IO_WRITE32(pAd, H2M_BBP_AGENT, 0);	// initialize BBP R/W access agent
	RTMP_IO_WRITE32(pAd, H2M_MAILBOX_CSR, 0);
	AsicSendCommandToMcu(pAd, 0x72, 0, 0, 0);
#endif // RTMP_MAC_PCI //

	// Wait to be stable.
	RTMPusecDelay(1000);

	// Read BBP register, make sure BBP is up and running before write new data
	Index = 0;
	do 
	{
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R0, &R0);
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))			
			return NDIS_STATUS_FAILURE;
		DBGPRINT(RT_DEBUG_TRACE, ("BBP version = %x\n", R0));
	} while ((++Index < 20) && ((R0 == 0xff) || (R0 == 0x00)));
	//ASSERT(Index < 20); //this will cause BSOD on Check-build driver

	if ((R0 == 0xff) || (R0 == 0x00))
		return NDIS_STATUS_FAILURE;

	// Initialize BBP register to default value
	for (Index = 0; Index < NUM_BBP_REG_PARMS; Index++)
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBPRegTable[Index].Register, BBPRegTable[Index].Value);
	}


#ifdef RT305x
	if ((pAd->CommonCfg.CN >> 16) == 0x3333)
	{
		UINT8 BBPValue = 0;
		
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R104, &BBPValue);
		BBPValue |= 0x80;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R104, BBPValue);

#ifndef CONFIG_RALINK_RT3350
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R105, &BBPValue);
		BBPValue &= 0x07;
		BBPValue |= 0x38;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R105, BBPValue);
#else //3350  disable re-mode
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R105, &BBPValue);
		BBPValue &= 0x07;
		BBPValue |= 0x30;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R105, BBPValue);

#endif
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R106, 0x46);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R113, 0x0d);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R114, 0x64);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R115, 0x34);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R116, 0x67);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R117, 0x37);

	}	
#endif

#ifdef RTMP_MAC_PCI
	// TODO: shiang, check MACVersion, currently, rbus-based chip use this.
	if (pAd->MACVersion == 0x28720200)
	{
		//UCHAR value;
		ULONG value2;

		//disable MLD by Bruce 20080704
		//BBP_IO_READ8_BY_REG_ID(pAd, BBP_R105, &value);
		//BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R105, value | 4);
	
		//Maximum PSDU length from 16K to 32K bytes	
		RTMP_IO_READ32(pAd, MAX_LEN_CFG, &value2);
		value2 &= ~(0x3<<12);
		value2 |= (0x2<<12);
		RTMP_IO_WRITE32(pAd, MAX_LEN_CFG, value2);
	}
#endif // RTMP_MAC_PCI //

#if !defined(CONFIG_RT2883_FPGA) && !defined(CONFIG_RT3883_FPGA)
	// for rt2860E and after, init BBP_R84 with 0x19. This is for extension channel overlapping IOT.
	// RT3090 should not program BBP R84 to 0x19, otherwise TX will block.
	if (((pAd->MACVersion & 0xffff) != 0x0101) && (!IS_RT30xx(pAd)))
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R84, 0x19);


	if (pAd->MACVersion == 0x28600100)
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x16);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x12);
	}
#endif
	
	if ((pAd->MACVersion == RALINK_3883_VERSION) || ((pAd->MACVersion >= RALINK_2880MP2_VERSION) && (pAd->MACVersion < RALINK_3070_VERSION))) // 3*3
	{
		// enlarge MAX_LEN_CFG
		UINT32 csr;
		RTMP_IO_READ32(pAd, MAX_LEN_CFG, &csr);
#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
		csr |= 0x3fff; // fix me
#else
		csr &= 0xFFF;
		csr |= 0x2000;
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //
		RTMP_IO_WRITE32(pAd, MAX_LEN_CFG, csr);
	}


#ifdef CONFIG_STA_SUPPORT
	// Add radio off control
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->StaCfg.bRadio == FALSE)
		{
//			RTMP_IO_WRITE32(pAd, PWR_PIN_CFG, 0x00001818);
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
			DBGPRINT(RT_DEBUG_TRACE, ("Set Radio Off\n"));
		}
	}
#endif // CONFIG_STA_SUPPORT //	

	// Clear raw counters
	RTMP_IO_READ32(pAd, RX_STA_CNT0, &Counter);
	RTMP_IO_READ32(pAd, RX_STA_CNT1, &Counter);
	RTMP_IO_READ32(pAd, RX_STA_CNT2, &Counter);
	RTMP_IO_READ32(pAd, TX_STA_CNT0, &Counter);
	RTMP_IO_READ32(pAd, TX_STA_CNT1, &Counter);
	RTMP_IO_READ32(pAd, TX_STA_CNT2, &Counter);
	
	// ASIC will keep garbage value after boot
	// Clear all shared key table when initial
	// This routine can be ignored in radio-ON/OFF operation. 
	if (bHardReset)
	{
		for (KeyIdx = 0; KeyIdx < 4; KeyIdx++)
		{
			RTMP_IO_WRITE32(pAd, SHARED_KEY_MODE_BASE + 4*KeyIdx, 0);
		}

		// Clear all pairwise key table when initial
		for (KeyIdx = 0; KeyIdx < 256; KeyIdx++)
		{
			RTMP_IO_WRITE32(pAd, MAC_WCID_ATTRIBUTE_BASE + (KeyIdx * HW_WCID_ATTRI_SIZE), 1);
		}
	}
	
	// assert HOST ready bit
//  RTMP_IO_WRITE32(pAd, MAC_CSR1, 0x0); // 2004-09-14 asked by Mark
//  RTMP_IO_WRITE32(pAd, MAC_CSR1, 0x4);

	// It isn't necessary to clear this space when not hard reset. 	
	if (bHardReset == TRUE)
	{
		// clear all on-chip BEACON frame space			
		RTMP_MAC_SHR_MSEL_LOCK(pAd, HIGHER_SHRMEM);
		for (apidx = 0; apidx < HW_BEACON_MAX_COUNT; apidx++)
		{
			for (i = 0; i < HW_BEACON_OFFSET>>2; i+=4)
				RTMP_IO_WRITE32(pAd, pAd->BeaconOffset[apidx] + i, 0x00); 
		}
		RTMP_MAC_SHR_MSEL_UNLOCK(pAd, LOWER_SHRMEM);
	}
	

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		// for rt2860E and after, init TXOP_CTRL_CFG with 0x583f. This is for extension channel overlapping IOT.
		if ((pAd->MACVersion&0xffff) != 0x0101)
			RTMP_IO_WRITE32(pAd, TXOP_CTRL_CFG, 0x583f);
	}
#endif // CONFIG_STA_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICInitializeAsic\n"));
	return NDIS_STATUS_SUCCESS;
}

/*
	========================================================================
	
	Routine Description:
		Reset NIC Asics

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		Reset NIC to initial state AS IS system boot up time.
		
	========================================================================
*/
VOID	NICIssueReset(
	IN	PRTMP_ADAPTER	pAd)
{
	UINT32	Value = 0;
	DBGPRINT(RT_DEBUG_TRACE, ("--> NICIssueReset\n"));

	// Abort Tx, prevent ASIC from writing to Host memory
	//RTMP_IO_WRITE32(pAd, TX_CNTL_CSR, 0x001f0000);
	
	// Disable Rx, register value supposed will remain after reset
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &Value);
	Value &= (0xfffffff3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, Value);

	// Issue reset and clear from reset state
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x03); // 2004-09-17 change from 0x01
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x00);

	DBGPRINT(RT_DEBUG_TRACE, ("<-- NICIssueReset\n"));
}

/*
	========================================================================
	
	Routine Description:
		Check ASIC registers and find any reason the system might hang

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = DISPATCH_LEVEL
	
	========================================================================
*/
BOOLEAN	NICCheckForHang(
	IN	PRTMP_ADAPTER	pAd)
{
	return (FALSE);
}

#ifdef CONFIG_AP_SUPPORT
#ifdef RTMP_MAC_PCI
/*
	========================================================================
	
	Routine Description:
		In the case, Client may be silent left without sending DeAuth or DeAssoc.
		AP'll continue retry packets for the client since AP doesn't know the STA
		is gone. To Minimum affection of exist traffic is disable retransmition for
		all those packet relative to the STA.
		So decide to change ack required setting of all packet in TX ring
		to "no ACK" requirement for specific Client.

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = DISPATCH_LEVEL
	
	========================================================================
*/
static VOID ClearTxRingClientAck(
	IN PRTMP_ADAPTER pAd,
	IN MAC_TABLE_ENTRY *pEntry)

{
	INT index;
	USHORT TxIdx;
	PRTMP_TX_RING pTxRing;
	PTXD_STRUC pTxD;
	PTXWI_STRUC pTxWI;

	if (!pAd || !pEntry)
		return;

	for (index = 3; index >= 0; index--)
	{
		pTxRing = &pAd->TxRing[index];
		for (TxIdx = 0; TxIdx < TX_RING_SIZE; TxIdx++)
		{
			pTxD = (PTXD_STRUC) pTxRing->Cell[TxIdx].AllocVa;
			if (!pTxD->DMADONE)
			{
				 pTxWI = (PTXWI_STRUC) pTxRing->Cell[TxIdx].DmaBuf.AllocVa;
				 if (pTxWI->WirelessCliID == pEntry->Aid)
					pTxWI->ACK = FALSE;
			}
		}
	}
}
#endif // RTMP_MAC_PCI //
#endif // CONFIG_AP_SUPPORT //

VOID NICUpdateFifoStaCounters(
	IN PRTMP_ADAPTER pAd)
{
	TX_STA_FIFO_STRUC	StaFifo;
	MAC_TABLE_ENTRY		*pEntry;
	UINT32			i = 0;
	UCHAR			pid = 0, wcid = 0;
	INT32			reTry;
	UCHAR			succMCS;

#ifdef RALINK_ATE		
	/* Nothing to do in ATE mode */
	if (ATE_ON(pAd))
		return;
#endif // RALINK_ATE //

		do
		{
			RTMP_IO_READ32(pAd, TX_STA_FIFO, &StaFifo.word);

			if (StaFifo.field.bValid == 0)
				break;
		
			wcid = (UCHAR)StaFifo.field.wcid;

#ifdef CONFIG_AP_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
			{
#ifdef DFS_SUPPORT
				/* the CTS frame for DFS and Carrier-Sense. */
				if ((pAd->CommonCfg.Channel > 14)
					&& (pAd->CommonCfg.bIEEE80211H == TRUE)
					&& (pAd->CommonCfg.RadarDetect.RDMode == RD_NORMAL_MODE)
					&& (wcid == DFS_CTS_WCID))
				{
					i++;
					DFSStartTrigger(pAd);
					continue;
				}
#endif // DFS_SUPPORT //

#ifdef CARRIER_DETECTION_SUPPORT
				if ((pAd->CommonCfg.CarrierDetect.Enable == TRUE)
					&& (wcid == CS_CTS_WCID))
				{
					i++;
					CarrierDetectStartTrigger(pAd);
					continue;
				}
#endif // CARRIER_DETECTION_SUPPORT //
			}
#endif // CONFIG_AP_SUPPORT //

		/* ignore NoACK and MGMT frame use 0xFF as WCID */
			if ((StaFifo.field.TxAckRequired == 0) || (wcid >= MAX_LEN_OF_MAC_TABLE))
			{
				i++;
				continue;
			}

			/* PID store Tx MCS Rate */
			pid = (UCHAR)StaFifo.field.PidType;

			pEntry = &pAd->MacTab.Content[wcid];

			pEntry->DebugFIFOCount++;

#ifdef TXBF_SUPPORT
			if ((StaFifo.field.eTxBF) || (StaFifo.field.iTxBF))
				pEntry->TxBFCount++;

			// Update statistics
			{
			int succMCS = (StaFifo.field.SuccessRate & 0x7F);
			int origMCS = pid;

			if (succMCS==32)
				origMCS = 32;
#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
			if (succMCS>origMCS && pEntry->HTCapability.MCSSet[2]==0xff)
				origMCS += 16;
#endif
#ifdef INCLUDE_DEBUG_QUEUE
			// Log unusual cases
			if ((pAd->CommonCfg.DebugFlags & DBF_UNUSED0400) &&
				(succMCS>origMCS || (origMCS-succMCS)>1) ) {
				dbQueueEnqueue(0x73, (UCHAR *)(&StaFifo.word));
			}
#endif
			if (succMCS>origMCS)
				origMCS = succMCS+1;

			// MCS16 falls back to MCS8
			if (origMCS>=16 && succMCS<=8)
				succMCS += 8;

			if (StaFifo.field.eTxBF) {
				if (StaFifo.field.TxSuccess)
					pEntry->TxBFCounters.ETxSuccessCount++;
				else
					pEntry->TxBFCounters.ETxFailCount++;
				pEntry->TxBFCounters.ETxRetryCount += origMCS-succMCS;
			}
			else if (StaFifo.field.iTxBF) {
				if (StaFifo.field.TxSuccess)
					pEntry->TxBFCounters.ITxSuccessCount++;
				else
					pEntry->TxBFCounters.ITxFailCount++;
				pEntry->TxBFCounters.ITxRetryCount += origMCS-succMCS;
			}
			else {
				if (StaFifo.field.TxSuccess)
					pEntry->TxBFCounters.TxSuccessCount++;
				else
					pEntry->TxBFCounters.TxFailCount++;
				pEntry->TxBFCounters.TxRetryCount += origMCS-succMCS;
			}
			}
#endif // TXBF_SUPPORT
						
#ifdef INCLUDE_DEBUG_QUEUE
		if (pAd->CommonCfg.DebugFlags & DBF_DBQ_TXFIFO) {
			dbQueueEnqueue(0x73, (UCHAR *)(&StaFifo.word));
		}
#endif

#ifdef UAPSD_AP_SUPPORT
			UAPSD_SP_AUE_Handle(pAd, pEntry, StaFifo.field.TxSuccess);
#endif // UAPSD_AP_SUPPORT //
 
#ifdef CONFIG_STA_SUPPORT
			if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
				continue;
#endif // CONFIG_STA_SUPPORT //

			if (!StaFifo.field.TxSuccess)
			{
				pEntry->FIFOCount++;
				pEntry->OneSecTxFailCount++;
									
				if (pEntry->FIFOCount >= 1)
				{			
					DBGPRINT(RT_DEBUG_TRACE, ("#"));
#ifdef DOT11_N_SUPPORT
					pEntry->NoBADataCountDown = 64;
#endif // DOT11_N_SUPPORT //

					// Update the continuous transmission counter.
					pEntry->ContinueTxFailCnt++;

					if(pEntry->PsMode == PWR_ACTIVE)
					{
#ifdef DOT11_N_SUPPORT					
						int tid;
						for (tid=0; tid<NUM_OF_TID; tid++)
						{
							BAOriSessionTearDown(pAd, pEntry->Aid,  tid, FALSE, FALSE);
						}
#endif // DOT11_N_SUPPORT //




#ifdef WDS_SUPPORT
						// fix WDS Jam issue
						if(IS_ENTRY_WDS(pEntry)
							&& (pEntry->LockEntryTx == FALSE)
							&& (pEntry->ContinueTxFailCnt >= pAd->ApCfg.EntryLifeCheck))
						{ 
							DBGPRINT(RT_DEBUG_TRACE, ("Entry %02x:%02x:%02x:%02x:%02x:%02x Blocked!! (Fail Cnt = %d)\n",
								pEntry->Addr[0],pEntry->Addr[1],pEntry->Addr[2],pEntry->Addr[3],
								pEntry->Addr[4],pEntry->Addr[5],pEntry->ContinueTxFailCnt ));

							pEntry->LockEntryTx = TRUE;
						}
#endif // WDS_SUPPORT //
					}

					//pEntry->FIFOCount = 0;
				}
				//pEntry->bSendBAR = TRUE;
#ifdef CONFIG_AP_SUPPORT
#ifdef RTMP_MAC_PCI
			// if Tx fail >= 20, then clear TXWI ack in Tx Ring
			if (pEntry->ContinueTxFailCnt >= pAd->ApCfg.EntryLifeCheck)
				ClearTxRingClientAck(pAd, pEntry);	
#endif // RTMP_MAC_PCI //				
#endif // CONFIG_AP_SUPPORT //
			}
			else
			{
#ifdef DOT11_N_SUPPORT
				if ((pEntry->PsMode != PWR_SAVE) && (pEntry->NoBADataCountDown > 0))
				{
					pEntry->NoBADataCountDown--;
					if (pEntry->NoBADataCountDown==0)
					{
						DBGPRINT(RT_DEBUG_TRACE, ("@\n"));
					}
				}
#endif // DOT11_N_SUPPORT //
				pEntry->FIFOCount = 0;
				pEntry->OneSecTxNoRetryOkCount++;
				// update NoDataIdleCount when sucessful send packet to STA.
				pEntry->NoDataIdleCount = 0;
				pEntry->ContinueTxFailCnt = 0;

#ifdef TXBF_SUPPORT
				if (StaFifo.field.eTxBF)
					pEntry->DebugTxBFCount++;
				else
					pEntry->DebugTxNormalCount++;
#endif // TXBF_SUPPORT //

#ifdef WDS_SUPPORT
				pEntry->LockEntryTx = FALSE;
#endif // WDS_SUPPORT //
			}

			succMCS = StaFifo.field.SuccessRate & 0x7F;

#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
			if (pEntry->HTCapability.MCSSet[2] == 0xff)
			{
				if (succMCS > pid)
					pid = pid + 16;
			}
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //

			reTry = pid - succMCS;

			if (StaFifo.field.TxSuccess)
			{
				pEntry->TXMCSExpected[pid]++;
				if (pid == succMCS)
				{
					pEntry->TXMCSSuccessful[pid]++;
				}
				else 
				{
					pEntry->TXMCSAutoFallBack[pid][succMCS]++;
				}
			}
			else
			{
				pEntry->TXMCSFailed[pid]++;
			}


#if defined(NEW_RATE_ADAPT_SUPPORT)
			reTry = (pid%8)-(succMCS%8)+(pid>>3)-(succMCS>>3);
#endif
#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
			if (pEntry->HTCapability.MCSSet[2] == 0xff)
				reTry = (pid % 3) - (succMCS % 3) + (pid >> 3) - (succMCS >> 3);
                        else
                        {
#endif
			if (reTry > 0)
			{
#if !defined(NEW_RATE_ADAPT_SUPPORT)
				if ((pid >= 12) && succMCS <=7)
				{
					reTry -= 4;
				} 
#endif
				pEntry->OneSecTxRetryOkCount += reTry;
			}
#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
			}
#endif

			i++;
			// ASIC store 16 stack
		} while ( i < (TX_RING_SIZE<<1) );

}

/*
	========================================================================
	
	Routine Description:
		Read statistical counters from hardware registers and record them
		in software variables for later on query

	Arguments:
		pAd					Pointer to our adapter

	Return Value:
		None

	IRQL = DISPATCH_LEVEL
	
	========================================================================
*/
VOID NICUpdateRawCounters(
	IN PRTMP_ADAPTER pAd)
{
	UINT32	OldValue;//, Value2;
	//ULONG	PageSum, OneSecTransmitCount;
	//ULONG	TxErrorRatio, Retry, Fail;
	RX_STA_CNT0_STRUC	 RxStaCnt0;
	RX_STA_CNT1_STRUC   RxStaCnt1;
	RX_STA_CNT2_STRUC   RxStaCnt2;
	TX_STA_CNT0_STRUC 	 TxStaCnt0;
	TX_STA_CNT1_STRUC	 StaTx1;
	TX_STA_CNT2_STRUC	 StaTx2;
#ifdef STATS_COUNT_SUPPORT
	TX_AGG_CNT_STRUC	TxAggCnt;
	TX_AGG_CNT0_STRUC	TxAggCnt0;
	TX_AGG_CNT1_STRUC	TxAggCnt1;
	TX_AGG_CNT2_STRUC	TxAggCnt2;
	TX_AGG_CNT3_STRUC	TxAggCnt3;
	TX_AGG_CNT4_STRUC	TxAggCnt4;
	TX_AGG_CNT5_STRUC	TxAggCnt5;
	TX_AGG_CNT6_STRUC	TxAggCnt6;
	TX_AGG_CNT7_STRUC	TxAggCnt7;
#endif // STATS_COUNT_SUPPORT //
	COUNTER_RALINK		*pRalinkCounters;


	pRalinkCounters = &pAd->RalinkCounters;

#ifdef RALINK_ATE
	// In ATE mode the QA Tool will read RX statistics counters 
	if (!ATE_ON(pAd))
	{
#endif

	RTMP_IO_READ32(pAd, RX_STA_CNT0, &RxStaCnt0.word);
	RTMP_IO_READ32(pAd, RX_STA_CNT2, &RxStaCnt2.word);

#ifdef CONFIG_AP_SUPPORT
#ifdef CARRIER_DETECTION_SUPPORT
	if ((pAd->CommonCfg.CarrierDetect.Enable == FALSE) || (pAd->OpMode == OPMODE_STA))
#endif // CARRIER_DETECTION_SUPPORT //
#endif // CONFIG_AP_SUPPORT //
	{
		RTMP_IO_READ32(pAd, RX_STA_CNT1, &RxStaCnt1.word);
	    // Update RX PLCP error counter
	    pAd->PrivateInfo.PhyRxErrCnt += RxStaCnt1.field.PlcpErr;
		// Update False CCA counter
		pAd->RalinkCounters.OneSecFalseCCACnt += RxStaCnt1.field.FalseCca;
	}

#ifdef STATS_COUNT_SUPPORT
	// Update FCS counters
	OldValue= pAd->WlanCounters.FCSErrorCount.u.LowPart;
	pAd->WlanCounters.FCSErrorCount.u.LowPart += (RxStaCnt0.field.CrcErr); // >> 7);
	if (pAd->WlanCounters.FCSErrorCount.u.LowPart < OldValue)
		pAd->WlanCounters.FCSErrorCount.u.HighPart++;
#endif // STATS_COUNT_SUPPORT //

	// Add FCS error count to private counters
	pRalinkCounters->OneSecRxFcsErrCnt += RxStaCnt0.field.CrcErr;
	OldValue = pRalinkCounters->RealFcsErrCount.u.LowPart;
	pRalinkCounters->RealFcsErrCount.u.LowPart += RxStaCnt0.field.CrcErr;
	if (pRalinkCounters->RealFcsErrCount.u.LowPart < OldValue)
		pRalinkCounters->RealFcsErrCount.u.HighPart++;

	// Update Duplicate Rcv check
	pRalinkCounters->DuplicateRcv += RxStaCnt2.field.RxDupliCount;
#ifdef STATS_COUNT_SUPPORT
	pAd->WlanCounters.FrameDuplicateCount.u.LowPart += RxStaCnt2.field.RxDupliCount;
#endif // STATS_COUNT_SUPPORT //
	// Update RX Overflow counter
	pAd->Counters8023.RxNoBuffer += (RxStaCnt2.field.RxFifoOverflowCount);
	
	//pAd->RalinkCounters.RxCount = 0;

#ifdef RALINK_ATE
	}
#endif
	
	//if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_TX_RATE_SWITCH_ENABLED) || 
	//	(OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_TX_RATE_SWITCH_ENABLED) && (pAd->MacTab.Size != 1)))
	if (!pAd->bUpdateBcnCntDone)
	{
	// Update BEACON sent count
	RTMP_IO_READ32(pAd, TX_STA_CNT0, &TxStaCnt0.word);
	RTMP_IO_READ32(pAd, TX_STA_CNT1, &StaTx1.word);
	RTMP_IO_READ32(pAd, TX_STA_CNT2, &StaTx2.word);
	pRalinkCounters->OneSecBeaconSentCnt += TxStaCnt0.field.TxBeaconCount;
	pRalinkCounters->OneSecTxRetryOkCount += StaTx1.field.TxRetransmit;
	pRalinkCounters->OneSecTxNoRetryOkCount += StaTx1.field.TxSuccess;
	pRalinkCounters->OneSecTxFailCount += TxStaCnt0.field.TxFailCount;

#ifdef STATS_COUNT_SUPPORT
	pAd->WlanCounters.TransmittedFragmentCount.u.LowPart += StaTx1.field.TxSuccess;
	pAd->WlanCounters.RetryCount.u.LowPart += StaTx1.field.TxRetransmit;
	pAd->WlanCounters.FailedCount.u.LowPart += TxStaCnt0.field.TxFailCount;
#endif // STATS_COUNT_SUPPORT //
	}


	//if (pAd->bStaFifoTest == TRUE)
#ifdef STATS_COUNT_SUPPORT
	{
		RTMP_IO_READ32(pAd, TX_AGG_CNT, &TxAggCnt.word);
	RTMP_IO_READ32(pAd, TX_AGG_CNT0, &TxAggCnt0.word);
	RTMP_IO_READ32(pAd, TX_AGG_CNT1, &TxAggCnt1.word);
	RTMP_IO_READ32(pAd, TX_AGG_CNT2, &TxAggCnt2.word);
	RTMP_IO_READ32(pAd, TX_AGG_CNT3, &TxAggCnt3.word);
		RTMP_IO_READ32(pAd, TX_AGG_CNT4, &TxAggCnt4.word);
		RTMP_IO_READ32(pAd, TX_AGG_CNT5, &TxAggCnt5.word);
		RTMP_IO_READ32(pAd, TX_AGG_CNT6, &TxAggCnt6.word);
		RTMP_IO_READ32(pAd, TX_AGG_CNT7, &TxAggCnt7.word);
		pRalinkCounters->TxAggCount += TxAggCnt.field.AggTxCount;
		pRalinkCounters->TxNonAggCount += TxAggCnt.field.NonAggTxCount;
		pRalinkCounters->TxAgg1MPDUCount += TxAggCnt0.field.AggSize1Count;
		pRalinkCounters->TxAgg2MPDUCount += TxAggCnt0.field.AggSize2Count;
		
		pRalinkCounters->TxAgg3MPDUCount += TxAggCnt1.field.AggSize3Count;
		pRalinkCounters->TxAgg4MPDUCount += TxAggCnt1.field.AggSize4Count;
		pRalinkCounters->TxAgg5MPDUCount += TxAggCnt2.field.AggSize5Count;
		pRalinkCounters->TxAgg6MPDUCount += TxAggCnt2.field.AggSize6Count;
	
		pRalinkCounters->TxAgg7MPDUCount += TxAggCnt3.field.AggSize7Count;
		pRalinkCounters->TxAgg8MPDUCount += TxAggCnt3.field.AggSize8Count;
		pRalinkCounters->TxAgg9MPDUCount += TxAggCnt4.field.AggSize9Count;
		pRalinkCounters->TxAgg10MPDUCount += TxAggCnt4.field.AggSize10Count;

		pRalinkCounters->TxAgg11MPDUCount += TxAggCnt5.field.AggSize11Count;
		pRalinkCounters->TxAgg12MPDUCount += TxAggCnt5.field.AggSize12Count;
		pRalinkCounters->TxAgg13MPDUCount += TxAggCnt6.field.AggSize13Count;
		pRalinkCounters->TxAgg14MPDUCount += TxAggCnt6.field.AggSize14Count;

		pRalinkCounters->TxAgg15MPDUCount += TxAggCnt7.field.AggSize15Count;
		pRalinkCounters->TxAgg16MPDUCount += TxAggCnt7.field.AggSize16Count;

		// Calculate the transmitted A-MPDU count
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += TxAggCnt0.field.AggSize1Count;
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt0.field.AggSize2Count >> 1);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt1.field.AggSize3Count / 3);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt1.field.AggSize4Count >> 2);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt2.field.AggSize5Count / 5);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt2.field.AggSize6Count / 6);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt3.field.AggSize7Count / 7);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt3.field.AggSize8Count >> 3);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt4.field.AggSize9Count / 9);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt4.field.AggSize10Count / 10);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt5.field.AggSize11Count / 11);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt5.field.AggSize12Count / 12);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt6.field.AggSize13Count / 13);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt6.field.AggSize14Count / 14);

		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt7.field.AggSize15Count / 15);
		pRalinkCounters->TransmittedAMPDUCount.u.LowPart += (TxAggCnt7.field.AggSize16Count >> 4);	
	}
#endif // STATS_COUNT_SUPPORT //			

#ifdef DBG_DIAGNOSE
	{
		RtmpDiagStruct	*pDiag;
		UCHAR			ArrayCurIdx, i;
		
		pDiag = &pAd->DiagStruct;
		ArrayCurIdx = pDiag->ArrayCurIdx;
		
		if (pDiag->inited == 0)
		{
			NdisZeroMemory(pDiag, sizeof(struct _RtmpDiagStrcut_));
			pDiag->ArrayStartIdx = pDiag->ArrayCurIdx = 0;
			pDiag->inited = 1;
		}
		else
		{
			// Tx
			pDiag->TxFailCnt[ArrayCurIdx] = TxStaCnt0.field.TxFailCount;
			pDiag->TxAggCnt[ArrayCurIdx] = TxAggCnt.field.AggTxCount;
			pDiag->TxNonAggCnt[ArrayCurIdx] = TxAggCnt.field.NonAggTxCount;
			pDiag->TxAMPDUCnt[ArrayCurIdx][0] = TxAggCnt0.field.AggSize1Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][1] = TxAggCnt0.field.AggSize2Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][2] = TxAggCnt1.field.AggSize3Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][3] = TxAggCnt1.field.AggSize4Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][4] = TxAggCnt2.field.AggSize5Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][5] = TxAggCnt2.field.AggSize6Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][6] = TxAggCnt3.field.AggSize7Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][7] = TxAggCnt3.field.AggSize8Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][8] = TxAggCnt4.field.AggSize9Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][9] = TxAggCnt4.field.AggSize10Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][10] = TxAggCnt5.field.AggSize11Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][11] = TxAggCnt5.field.AggSize12Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][12] = TxAggCnt6.field.AggSize13Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][13] = TxAggCnt6.field.AggSize14Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][14] = TxAggCnt7.field.AggSize15Count;
			pDiag->TxAMPDUCnt[ArrayCurIdx][15] = TxAggCnt7.field.AggSize16Count;

			pDiag->RxCrcErrCnt[ArrayCurIdx] = RxStaCnt0.field.CrcErr;
			
			INC_RING_INDEX(pDiag->ArrayCurIdx,  DIAGNOSE_TIME);
			ArrayCurIdx = pDiag->ArrayCurIdx;
			for (i =0; i < 9; i++)
			{
				pDiag->TxDescCnt[ArrayCurIdx][i]= 0;
				pDiag->TxSWQueCnt[ArrayCurIdx][i] =0;
				pDiag->TxMcsCnt[ArrayCurIdx][i] = 0;
				pDiag->RxMcsCnt[ArrayCurIdx][i] = 0;
			}
			pDiag->TxDataCnt[ArrayCurIdx] = 0;
			pDiag->TxFailCnt[ArrayCurIdx] = 0;
			pDiag->RxDataCnt[ArrayCurIdx] = 0;
			pDiag->RxCrcErrCnt[ArrayCurIdx]  = 0;
//			for (i = 9; i < 16; i++)
			for (i = 9; i < 24; i++) // 3*3
			{
				pDiag->TxDescCnt[ArrayCurIdx][i] = 0;
				pDiag->TxMcsCnt[ArrayCurIdx][i] = 0;
				pDiag->RxMcsCnt[ArrayCurIdx][i] = 0;
}

			if (pDiag->ArrayCurIdx == pDiag->ArrayStartIdx)
				INC_RING_INDEX(pDiag->ArrayStartIdx,  DIAGNOSE_TIME);
		}
		
	}
#endif // DBG_DIAGNOSE //


}


/*
	========================================================================
	
	Routine Description:
		Reset NIC from error

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		Reset NIC from error state
		
	========================================================================
*/
VOID	NICResetFromError(
	IN	PRTMP_ADAPTER	pAd)
{
	// Reset BBP (according to alex, reset ASIC will force reset BBP
	// Therefore, skip the reset BBP
	// RTMP_IO_WRITE32(pAd, MAC_CSR1, 0x2);
		
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x1);
	// Remove ASIC from reset state
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x0);

	NICInitializeAdapter(pAd, FALSE);	
	NICInitAsicFromEEPROM(pAd);

	AsicBBPAdjust(pAd);

	// Switch to current channel, since during reset process, the connection should remains on.	
	AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
	AsicLockChannel(pAd, pAd->CommonCfg.CentralChannel);
}


NDIS_STATUS NICLoadFirmware(
	IN PRTMP_ADAPTER pAd)
{
	NDIS_STATUS	 status = NDIS_STATUS_SUCCESS;
	if (pAd->chipOps.loadFirmware)
		status = pAd->chipOps.loadFirmware(pAd);

	return status;
}


/*
	========================================================================
	
	Routine Description:
		erase 8051 firmware image in MAC ASIC

	Arguments:
		Adapter						Pointer to our adapter

	IRQL = PASSIVE_LEVEL
		
	========================================================================
*/
VOID NICEraseFirmware(
	IN PRTMP_ADAPTER pAd)
{
	if (pAd->chipOps.eraseFirmware)
		pAd->chipOps.eraseFirmware(pAd);
	
}/* End of NICEraseFirmware */


/*
	========================================================================
	
	Routine Description:
		Load Tx rate switching parameters

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		NDIS_STATUS_SUCCESS         firmware image load ok
		NDIS_STATUS_FAILURE         image not found

	IRQL = PASSIVE_LEVEL

	Rate Table Format:
		1. (B0: Valid Item number) (B1:Initial item from zero)
		2. Item Number(Dec)      Mode(Hex)     Current MCS(Dec)    TrainUp(Dec)    TrainDown(Dec)
		
	========================================================================
*/
NDIS_STATUS NICLoadRateSwitchingParams(
	IN PRTMP_ADAPTER pAd)
{
	return NDIS_STATUS_SUCCESS;
}


/*
	========================================================================
	
	Routine Description:
		Compare two memory block

	Arguments:
		pSrc1		Pointer to first memory address
		pSrc2		Pointer to second memory address
		
	Return Value:
		0:			memory is equal
		1:			pSrc1 memory is larger
		2:			pSrc2 memory is larger

	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
ULONG	RTMPCompareMemory(
	IN	PVOID	pSrc1,
	IN	PVOID	pSrc2,
	IN	ULONG	Length)
{
	PUCHAR	pMem1;
	PUCHAR	pMem2;
	ULONG	Index = 0;

	pMem1 = (PUCHAR) pSrc1;
	pMem2 = (PUCHAR) pSrc2;

	for (Index = 0; Index < Length; Index++)
	{
		if (pMem1[Index] > pMem2[Index])
			return (1);
		else if (pMem1[Index] < pMem2[Index])
			return (2);
	}

	// Equal
	return (0);
}


/*
	========================================================================
	
	Routine Description:
		Zero out memory block

	Arguments:
		pSrc1		Pointer to memory address
		Length		Size

	Return Value:
		None
		
	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
VOID	RTMPZeroMemory(
	IN	PVOID	pSrc,
	IN	ULONG	Length)
{
	PUCHAR	pMem;
	ULONG	Index = 0;

	pMem = (PUCHAR) pSrc;

	for (Index = 0; Index < Length; Index++)
	{
		pMem[Index] = 0x00;
	}
}


/*
	========================================================================
	
	Routine Description:
		Copy data from memory block 1 to memory block 2

	Arguments:
		pDest		Pointer to destination memory address
		pSrc		Pointer to source memory address
		Length		Copy size
		
	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
VOID RTMPMoveMemory(
	OUT	PVOID	pDest,
	IN	PVOID	pSrc,
	IN	ULONG	Length)
{
	PUCHAR	pMem1;
	PUCHAR	pMem2;
	UINT	Index;

	ASSERT((Length==0) || (pDest && pSrc));

	pMem1 = (PUCHAR) pDest;
	pMem2 = (PUCHAR) pSrc;

	for (Index = 0; Index < Length; Index++)
	{
		pMem1[Index] = pMem2[Index];
	}
}


/*
	========================================================================
	
	Routine Description:
		Initialize port configuration structure

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		
	========================================================================
*/
VOID	UserCfgInit(
	IN	PRTMP_ADAPTER pAd)
{
#ifdef CONFIG_AP_SUPPORT
	UINT i, j;
#endif // CONFIG_AP_SUPPORT //
//	EDCA_PARM DefaultEdcaParm;
	UINT key_index, bss_index;

	DBGPRINT(RT_DEBUG_TRACE, ("--> UserCfgInit\n"));    
	 

	//
	//  part I. intialize common configuration
	//

	for(key_index=0; key_index<SHARE_KEY_NUM; key_index++)
	{
		for(bss_index = 0; bss_index < MAX_MBSSID_NUM; bss_index++)
		{
			pAd->SharedKey[bss_index][key_index].KeyLen = 0;
			pAd->SharedKey[bss_index][key_index].CipherAlg = CIPHER_NONE;
		}
	}

	pAd->bLocalAdminMAC = FALSE;
	pAd->EepromAccess = FALSE;
	
	pAd->Antenna.word = 0; 
	pAd->CommonCfg.BBPCurrentBW = BW_40;

	pAd->LedCntl.word = 0;
#ifdef RTMP_MAC_PCI
	pAd->LedIndicatorStrength = 0;
	pAd->RLnkCtrlOffset = 0;
	pAd->HostLnkCtrlOffset = 0;
#endif // RTMP_MAC_PCI //

	pAd->bAutoTxAgcA = FALSE;			// Default is OFF
	pAd->bAutoTxAgcG = FALSE;			// Default is OFF
	pAd->RfIcType = RFIC_2820;

	// Init timer for reset complete event
	pAd->CommonCfg.CentralChannel = 1;
	pAd->bForcePrintTX = FALSE;
	pAd->bForcePrintRX = FALSE;
	pAd->bStaFifoTest = FALSE;
	pAd->bProtectionTest = FALSE;
	pAd->bHCCATest = FALSE;
	pAd->bGenOneHCCA = FALSE;
	pAd->CommonCfg.Dsifs = 10;      // in units of usec 
	pAd->CommonCfg.TxPower = 100; //mW
	pAd->CommonCfg.TxPowerPercentage = 0xffffffff; // AUTO
	pAd->CommonCfg.TxPowerDefault = 0xffffffff; // AUTO
	pAd->CommonCfg.TxPreamble = Rt802_11PreambleAuto; // use Long preamble on TX by defaut
	pAd->CommonCfg.bUseZeroToDisableFragment = FALSE;
	pAd->CommonCfg.RtsThreshold = 2347;
	pAd->CommonCfg.FragmentThreshold = 2346;
	pAd->CommonCfg.UseBGProtection = 0;    // 0: AUTO
	pAd->CommonCfg.bEnableTxBurst = TRUE; //0;    	
	pAd->CommonCfg.PhyMode = 0xff;     // unknown
	pAd->CommonCfg.BandState = UNKNOWN_BAND;
	pAd->CommonCfg.RadarDetect.CSPeriod = 10;
	pAd->CommonCfg.RadarDetect.CSCount = 0;
	pAd->CommonCfg.RadarDetect.RDMode = RD_NORMAL_MODE;
	pAd->CommonCfg.bDFSIndoor = 1;
	
#ifdef CONFIG_RALINK_RT3052
	RTMP_SYS_IO_READ32(0xb000000c, &pAd->CommonCfg.CID);
	RTMP_SYS_IO_READ32(0xb0000000, &pAd->CommonCfg.CN);
#endif

#ifdef CONFIG_AP_SUPPORT
#ifdef DFS_SUPPORT
#ifdef RTMP_RBUS_SUPPORT
	// TODO: shiang, check the R65 is specific for RT2880 only or also need to set this register in RT3052
	pAd->CommonCfg.R65 = 0x1d;
	pAd->CommonCfg.DFS_R66 = 0x60;

#endif // RTMP_RBUS_SUPPORT //
#ifdef NEW_DFS
#ifdef RT2880
	RTMP_SYS_IO_READ32(0xa030000c, &pAd->CommonCfg.CID);
#endif
	pAd->CommonCfg.fcc_5_threshold = 1000;
	pAd->CommonCfg.fcc_5_idx = 0;
	pAd->CommonCfg.fcc_5_last_idx = 0;
#endif // NEW_DFS //

	pAd->CommonCfg.McuRadarDebug = 0;

	if ((pAd->MACVersion == 0x28720200) && (pAd->CommonCfg.CID == 0x200))
		pAd->CommonCfg.dfs_func = 1;
	else if (pAd->MACVersion <= 0x28720200)
		pAd->CommonCfg.dfs_func = 0;
	else
		pAd->CommonCfg.dfs_func = 2;

	pAd->CommonCfg.DeltaDelay = 0x3;
	pAd->CommonCfg.ChEnable = 0xf;
	pAd->CommonCfg.RadarReEnable = 1;
	pAd->CommonCfg.PollTime = 1;
	pAd->CommonCfg.DfsRssiHigh = -30;
	pAd->CommonCfg.DfsRssiLow = -90;


#ifdef DFS_2_SUPPORT
	pAd->CommonCfg.RadarEventExpire[3] = 0x99999999;
	pAd->CommonCfg.Symmetric_Round = 1;
	pAd->CommonCfg.VGA_Mask = 45;
	pAd->CommonCfg.Packet_End_Mask = 45;
	pAd->CommonCfg.Rx_PE_Mask = 45;
	pAd->CommonCfg.PollTime = 3;
#endif // DFS_2_SUPPORT //
#endif // DFS_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

#ifdef CARRIER_DETECTION_SUPPORT
	pAd->CommonCfg.carrier_func = 0;
	
#ifdef TONE_RADAR_DETECT_SUPPORT
#ifdef CONFIG_RALINK_RT3052
	pAd->CommonCfg.carrier_func = 1;
#endif

#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	pAd->CommonCfg.carrier_func = 2;
#endif

	pAd->CommonCfg.CarrierDetect.delta = CARRIER_DETECT_DELTA;
	pAd->CommonCfg.CarrierDetect.div_flag = CARRIER_DETECT_DIV_FLAG;
	pAd->CommonCfg.CarrierDetect.criteria = CARRIER_DETECT_CRITIRIA;
	pAd->CommonCfg.CarrierDetect.threshold = CARRIER_DETECT_THRESHOLD;
#endif // TONE_RADAR_DETECT_SUPPORT //
#endif // CARRIER_DETECTION_SUPPORT //

	pAd->CommonCfg.RadarDetect.ChMovingTime = 65;
#ifdef MERGE_ARCH_TEAM
	pAd->CommonCfg.RadarDetect.LongPulseRadarTh = 2;
	pAd->CommonCfg.RadarDetect.AvgRssiReq = -75;
#else // original rt28xx source code
	pAd->CommonCfg.RadarDetect.LongPulseRadarTh = 3;
#endif // MERGE_ARCH_TEAM //
	pAd->CommonCfg.bAPSDCapable = FALSE;
	pAd->CommonCfg.bNeedSendTriggerFrame = FALSE;
	pAd->CommonCfg.TriggerTimerCount = 0;
	pAd->CommonCfg.bAPSDForcePowerSave = FALSE;
	pAd->CommonCfg.bCountryFlag = FALSE;
	pAd->CommonCfg.TxStream = 0;
	pAd->CommonCfg.RxStream = 0;

	NdisZeroMemory(&pAd->BeaconTxWI, sizeof(pAd->BeaconTxWI));

#ifdef DOT11_N_SUPPORT
	NdisZeroMemory(&pAd->CommonCfg.HtCapability, sizeof(pAd->CommonCfg.HtCapability));
	pAd->HTCEnable = FALSE;
	pAd->bBroadComHT = FALSE;
	pAd->CommonCfg.bRdg = FALSE;
	
#ifdef DOT11N_DRAFT3
	pAd->CommonCfg.Dot11OBssScanPassiveDwell = dot11OBSSScanPassiveDwell;	// Unit : TU. 5~1000
	pAd->CommonCfg.Dot11OBssScanActiveDwell = dot11OBSSScanActiveDwell;	// Unit : TU. 10~1000
	pAd->CommonCfg.Dot11BssWidthTriggerScanInt = dot11BSSWidthTriggerScanInterval;	// Unit : Second	
	pAd->CommonCfg.Dot11OBssScanPassiveTotalPerChannel = dot11OBSSScanPassiveTotalPerChannel;	// Unit : TU. 200~10000
	pAd->CommonCfg.Dot11OBssScanActiveTotalPerChannel = dot11OBSSScanActiveTotalPerChannel;	// Unit : TU. 20~10000
	pAd->CommonCfg.Dot11BssWidthChanTranDelayFactor = dot11BSSWidthChannelTransactionDelayFactor;
	pAd->CommonCfg.Dot11OBssScanActivityThre = dot11BSSScanActivityThreshold;	// Unit : percentage
	pAd->CommonCfg.Dot11BssWidthChanTranDelay = (pAd->CommonCfg.Dot11BssWidthTriggerScanInt * pAd->CommonCfg.Dot11BssWidthChanTranDelayFactor);
#endif  // DOT11N_DRAFT3 //

	NdisZeroMemory(&pAd->CommonCfg.AddHTInfo, sizeof(pAd->CommonCfg.AddHTInfo));
	pAd->CommonCfg.BACapability.field.MMPSmode = MMPS_ENABLE;
	pAd->CommonCfg.BACapability.field.MpduDensity = 0;
	pAd->CommonCfg.BACapability.field.Policy = IMMED_BA;
	pAd->CommonCfg.BACapability.field.RxBAWinLimit = 64; //32;
	pAd->CommonCfg.BACapability.field.TxBAWinLimit = 64; //32;
	DBGPRINT(RT_DEBUG_TRACE, ("--> UserCfgInit. BACapability = 0x%x\n", pAd->CommonCfg.BACapability.word));    

	pAd->CommonCfg.BACapability.field.AutoBA = FALSE;	
	BATableInit(pAd, &pAd->BATable);

	pAd->CommonCfg.bExtChannelSwitchAnnouncement = 1;
	pAd->CommonCfg.bHTProtect = 1;
	pAd->CommonCfg.bMIMOPSEnable = FALSE;
#ifdef GREENAP_SUPPORT
	pAd->ApCfg.bGreenAPEnable=FALSE;
	pAd->ApCfg.bBlockAntDivforGreenAP = FALSE;
	pAd->ApCfg.bGreenAPIsOn= FALSE;
#endif // GREENAP_SUPPORT //
	pAd->CommonCfg.bBADecline = FALSE;
	pAd->CommonCfg.bDisableReordering = FALSE;

	if (pAd->MACVersion == 0x28720200)
	{
		pAd->CommonCfg.TxBASize = 13; //by Jerry recommend
	}else{
		pAd->CommonCfg.TxBASize = 7;
	}

	pAd->CommonCfg.REGBACapability.word = pAd->CommonCfg.BACapability.word;
#endif // DOT11_N_SUPPORT //

	pAd->CommonCfg.TxRate = RATE_6;
	pAd->CommonCfg.MlmeTransmit.field.MCS = MCS_RATE_6;
	pAd->CommonCfg.MlmeTransmit.field.BW = BW_40;
	pAd->CommonCfg.MlmeTransmit.field.MODE = MODE_OFDM;

	pAd->CommonCfg.BeaconPeriod = 100;     // in mSec

#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	pAd->CommonCfg.PreAntSwitch = 0;
	pAd->CommonCfg.StreamMode = 1;
	pAd->CommonCfg.PhyRateLimit = 0;
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //

	pAd->CommonCfg.DebugFlags = 0;

#ifdef CONFIG_RALINK_RT3883
	pAd->CommonCfg.VCORecalibrationThreshold = DEFAULT_VCO_RECALIBRATION_THRESHOLD;
	pAd->RefreshTssi = 1;
#endif // CONFIG_RALINK_RT3883 //

#ifdef MCAST_RATE_SPECIFIC
	pAd->CommonCfg.MCastPhyMode.word = pAd->MacTab.Content[MCAST_WCID].HTPhyMode.word;
#endif // MCAST_RATE_SPECIFIC //

	//
	// part II. intialize STA specific configuration
	//
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_DIRECT);
		RX_FILTER_CLEAR_FLAG(pAd, fRX_FILTER_ACCEPT_MULTICAST);
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_BROADCAST);
		RX_FILTER_SET_FLAG(pAd, fRX_FILTER_ACCEPT_ALL_MULTICAST);

		pAd->StaCfg.Psm = PWR_ACTIVE;

		pAd->StaCfg.OrigWepStatus = Ndis802_11EncryptionDisabled;
		pAd->StaCfg.PairCipher = Ndis802_11EncryptionDisabled;
		pAd->StaCfg.GroupCipher = Ndis802_11EncryptionDisabled;
		pAd->StaCfg.bMixCipher = FALSE;	
		pAd->StaCfg.DefaultKeyId = 0;

		// 802.1x port control
		pAd->StaCfg.PrivacyFilter = Ndis802_11PrivFilter8021xWEP;
		pAd->StaCfg.PortSecured = WPA_802_1X_PORT_NOT_SECURED;
		pAd->StaCfg.LastMicErrorTime = 0;
		pAd->StaCfg.MicErrCnt        = 0;
		pAd->StaCfg.bBlockAssoc      = FALSE;
		pAd->StaCfg.WpaState         = SS_NOTUSE;

		pAd->CommonCfg.NdisRadioStateOff = FALSE;		// New to support microsoft disable radio with OID command

		pAd->StaCfg.RssiTrigger = 0;
		NdisZeroMemory(&pAd->StaCfg.RssiSample, sizeof(RSSI_SAMPLE));
		pAd->StaCfg.RssiTriggerMode = RSSI_TRIGGERED_UPON_BELOW_THRESHOLD;
		pAd->StaCfg.AtimWin = 0;
		pAd->StaCfg.DefaultListenCount = 3;//default listen count;
		pAd->StaCfg.BssType = BSS_INFRA;  // BSS_INFRA or BSS_ADHOC or BSS_MONITOR
		pAd->StaCfg.bScanReqIsFromWebUI = FALSE;
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_WAKEUP_NOW);

		pAd->StaCfg.bAutoTxRateSwitch = TRUE;
		pAd->StaCfg.DesiredTransmitSetting.field.MCS = MCS_AUTO;
	}

#ifdef EXT_BUILD_CHANNEL_LIST
	pAd->StaCfg.IEEE80211dClientMode = Rt802_11_D_None;
#endif // EXT_BUILD_CHANNEL_LIST //

#ifdef RTMP_MAC_PCI

pAd->brt30xxBanMcuCmd = FALSE;
pAd->StaCfg.PSControl.field.EnableNewPS=FALSE;

#ifdef PCIE_PS_SUPPORT
pAd->StaCfg.PSControl.field.EnableNewPS=TRUE;
pAd->b3090ESpecialChip = FALSE;
//The value of PowerMode could be 1 or 3. Level 3 could save more power than Level 1. 
pAd->StaCfg.PSControl.field.rt30xxPowerMode=1;
pAd->StaCfg.PSControl.field.rt30xxForceASPMTest=0;
pAd->StaCfg.PSControl.field.rt30xxFollowHostASPM=1;
#endif // PCIE_PS_SUPPORT //
#endif // RTMP_MAC_PCI //
#endif // CONFIG_STA_SUPPORT //

	// global variables mXXXX used in MAC protocol state machines
	OPSTATUS_SET_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM);
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_ADHOC_ON);
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_INFRA_ON);

	// PHY specification
	pAd->CommonCfg.PhyMode = PHY_11BG_MIXED;		// default PHY mode
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED);  // CCK use LONG preamble

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		// user desired power mode
		pAd->StaCfg.WindowsPowerMode = Ndis802_11PowerModeCAM;
		pAd->StaCfg.WindowsBatteryPowerMode = Ndis802_11PowerModeCAM;
		pAd->StaCfg.bWindowsACCAMEnable = FALSE;

		RTMPInitTimer(pAd, &pAd->StaCfg.StaQuickResponeForRateUpTimer, GET_TIMER_FUNCTION(StaQuickResponeForRateUpExec), pAd, FALSE);
		pAd->StaCfg.StaQuickResponeForRateUpTimerRunning = FALSE;

		// Patch for Ndtest
		pAd->StaCfg.ScanCnt = 0;

		pAd->StaCfg.bHwRadio  = TRUE; // Default Hardware Radio status is On
		pAd->StaCfg.bSwRadio  = TRUE; // Default Software Radio status is On
		pAd->StaCfg.bRadio    = TRUE; // bHwRadio && bSwRadio
		pAd->StaCfg.bHardwareRadio = FALSE;		// Default is OFF
		pAd->StaCfg.bShowHiddenSSID = FALSE;		// Default no show

		// Nitro mode control
		pAd->StaCfg.bAutoReconnect = TRUE;

		// Save the init time as last scan time, the system should do scan after 2 seconds.
		// This patch is for driver wake up from standby mode, system will do scan right away.
		NdisGetSystemUpTime(&pAd->StaCfg.LastScanTime);
		if (pAd->StaCfg.LastScanTime > 10 * OS_HZ)
			pAd->StaCfg.LastScanTime -= (10 * OS_HZ);
		
		NdisZeroMemory(pAd->nickname, IW_ESSID_MAX_SIZE+1);
		RTMPInitTimer(pAd, &pAd->StaCfg.WpaDisassocAndBlockAssocTimer, GET_TIMER_FUNCTION(WpaDisassocApAndBlockAssoc), pAd, FALSE);

#ifdef WSC_STA_SUPPORT
		{
		PWSC_CTRL pWscControl;
		/*
			WscControl cannot be zero here, because WscControl timers are initial in MLME Initialize 
			and MLME Initialize is called before UserCfgInit.

		*/
		pWscControl = &pAd->StaCfg.WscControl;
		pWscControl->WscConfMode = WSC_DISABLE;
		pWscControl->WscMode = WSC_PIN_MODE;
		pWscControl->WscConfStatus = WSC_SCSTATE_UNCONFIGURED;
		pWscControl->WscConfigMethods= 0x008C;
		pWscControl->WscState = WSC_STATE_OFF;
		pWscControl->WscPinCode = 0;
		pWscControl->WscLastPinFromEnrollee = 0;
		pWscControl->WscEnrolleePinCode = 0;
		pWscControl->WscSelReg = 0;
		NdisZeroMemory(&pAd->StaCfg.WscControl.RegData, sizeof(WSC_REG_DATA));
		pWscControl->WscUseUPnP = 0;
		pWscControl->WscEnAssociateIE = TRUE;
		pWscControl->WscEnProbeReqIE = TRUE;
		pWscControl->RegData.ReComputePke = 1;
		pWscControl->lastId = 1;
		pWscControl->EntryIfIdx = BSS0;
		pWscControl->pAd = pAd;
		pWscControl->WscDriverAutoConnect = TRUE;
		pAd->WriteWscCfgToDatFile = 0xFF;
		pWscControl->WscRejectSamePinFromEnrollee = FALSE;
		pWscControl->WpsApBand = PREFERRED_WPS_AP_PHY_TYPE_AUTO_SELECTION;
		}
#endif // WSC_STA_SUPPORT //

		NdisZeroMemory(pAd->StaCfg.ReplayCounter, 8);
		pAd->StaCfg.bAutoConnectByBssid = FALSE;
		pAd->StaCfg.BeaconLostTime = BEACON_LOST_TIME;
		NdisZeroMemory(pAd->StaCfg.WpaPassPhrase, 64);    
		pAd->StaCfg.WpaPassPhraseLen = 0;
		pAd->StaCfg.bAutoRoaming = FALSE;
		pAd->StaCfg.bForceTxBurst = FALSE;
	}
#endif // CONFIG_STA_SUPPORT //

	// Default for extra information is not valid
	pAd->ExtraInfo = EXTRA_INFO_CLEAR;
	
	// Default Config change flag
	pAd->bConfigChanged = FALSE;

	// 
	// part III. AP configurations
	//
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		// Set MBSS Default Configurations
		pAd->ApCfg.BssidNum = MAX_MBSSID_NUM;
		for(j = BSS0; j < pAd->ApCfg.BssidNum; j++)
		{
			pAd->ApCfg.MBSSID[j].AuthMode = Ndis802_11AuthModeOpen;
			pAd->ApCfg.MBSSID[j].WepStatus = Ndis802_11EncryptionDisabled;
			pAd->ApCfg.MBSSID[j].GroupKeyWepStatus = Ndis802_11EncryptionDisabled;
			pAd->ApCfg.MBSSID[j].DefaultKeyId = 0;
			pAd->ApCfg.MBSSID[j].WpaMixPairCipher = MIX_CIPHER_NOTUSE;

			pAd->ApCfg.MBSSID[j].IEEE8021X = FALSE;
			pAd->ApCfg.MBSSID[j].PreAuth = FALSE;

			// PMK cache setting
			pAd->ApCfg.MBSSID[j].PMKCachePeriod = (10 * 60 * OS_HZ); // unit : tick(default: 10 minute)
			NdisZeroMemory(&pAd->ApCfg.MBSSID[j].PMKIDCache, sizeof(NDIS_AP_802_11_PMKID));

			/* dot1x related per BSS */
			pAd->ApCfg.MBSSID[j].radius_srv_num = 0;
			pAd->ApCfg.MBSSID[j].NasIdLen = 0;

			/* VLAN related */
        		pAd->ApCfg.MBSSID[j].VLAN_VID = 0;

			// Default MCS as AUTO
			pAd->ApCfg.MBSSID[j].bAutoTxRateSwitch = TRUE;
			pAd->ApCfg.MBSSID[j].DesiredTransmitSetting.field.MCS = MCS_AUTO;

			// Default is zero. It means no limit.
			pAd->ApCfg.MBSSID[j].MaxStaNum = 0;
			pAd->ApCfg.MBSSID[j].StaCount = 0;
			
#ifdef WSC_AP_SUPPORT
			{
				PWSC_CTRL pWscControl;
				INT idx;
				/*
					WscControl cannot be zero here, because WscControl timers are initial in MLME Initialize 
					and MLME Initialize is called before UserCfgInit.

				*/
				pWscControl = &pAd->ApCfg.MBSSID[j].WscControl;
				NdisZeroMemory(&pWscControl->RegData, sizeof(WSC_REG_DATA));
				NdisZeroMemory(&pWscControl->WscStaPbcProbeInfo, sizeof(WSC_STA_PBC_PROBE_INFO));
				pWscControl->WscMode = 1;
				pWscControl->WscConfStatus = 1;
				pWscControl->WscConfigMethods= 0x0084;
				pWscControl->RegData.ReComputePke = 1;
				pWscControl->lastId = 1;
				pWscControl->EntryIfIdx = (MIN_NET_DEVICE_FOR_MBSSID | j);
				pWscControl->pAd = pAd;
				pWscControl->WscRejectSamePinFromEnrollee = FALSE;
				pWscControl->WscPBCOverlap= FALSE;
				pWscControl->WscConfMode = 0;
				pWscControl->WscStatus = 0;
				pWscControl->WscState = 0;
				pWscControl->WscPinCode = 0;
				pWscControl->WscLastPinFromEnrollee = 0;
				pWscControl->WscEnrolleePinCode = 0;
				pWscControl->WscSelReg = 0;
				pWscControl->WscUseUPnP = 0;
				pWscControl->bWCNTest = FALSE;
				pWscControl->WscKeyASCII = 0; /* default, 0 (64 Hex) */
				
				/*
					Enrollee 192 random bytes for DH key generation
				*/
				for (idx = 0; idx < 192; idx++)
					pWscControl->RegData.EnrolleeRandom[idx] = RandomByte(pAd);

				NdisZeroMemory(&pWscControl->WscDefaultSsid, sizeof(NDIS_802_11_SSID));

			}
#endif // WSC_AP_SUPPORT //        

			for(i = 0; i < WLAN_MAX_NUM_OF_TIM; i++)
	        		pAd->ApCfg.MBSSID[j].TimBitmaps[i] = 0;
		}
		pAd->ApCfg.DtimCount  = 0;
		pAd->ApCfg.DtimPeriod = DEFAULT_DTIM_PERIOD;

		pAd->ApCfg.ErpIeContent = 0;

		pAd->ApCfg.StaIdleTimeout = MAC_TABLE_AGEOUT_TIME;

#ifdef IDS_SUPPORT
		// Default disable IDS threshold and reset all IDS counters
		pAd->ApCfg.IdsEnable = FALSE;
		pAd->ApCfg.AuthFloodThreshold = 0;
		pAd->ApCfg.AssocReqFloodThreshold = 0;
		pAd->ApCfg.ReassocReqFloodThreshold = 0;
		pAd->ApCfg.ProbeReqFloodThreshold = 0;
		pAd->ApCfg.DisassocFloodThreshold = 0;
		pAd->ApCfg.DeauthFloodThreshold = 0;
		pAd->ApCfg.EapReqFooldThreshold = 0;
		RTMPClearAllIdsCounter(pAd);
#endif // IDS_SUPPORT //

#ifdef WDS_SUPPORT
		APWdsInitialize(pAd);
#endif // WDS_SUPPORT

#ifdef WSC_AP_SUPPORT
		pAd->WriteWscCfgToDatFile = 0xFF;
		pAd->WriteWscCfgToAr9DatFile = FALSE;
#endif // WSC_AP_SUPPORT //

#ifdef APCLI_SUPPORT
		for(j = 0; j < MAX_APCLI_NUM; j++) 
		{
			pAd->ApCfg.ApCliTab[j].bAutoTxRateSwitch = TRUE;
			pAd->ApCfg.ApCliTab[j].DesiredTransmitSetting.field.MCS = MCS_AUTO;
		}
#endif // APCLI_SUPPORT //
	}
#endif // CONFIG_AP_SUPPORT //


	//
	// part IV. others
	//
	// dynamic BBP R66:sensibity tuning to overcome background noise
	pAd->BbpTuning.bEnable                = TRUE;  
	pAd->BbpTuning.FalseCcaLowerThreshold = 100;
	pAd->BbpTuning.FalseCcaUpperThreshold = 512;
	pAd->BbpTuning.R66Delta               = 4;
	pAd->Mlme.bEnableAutoAntennaCheck = TRUE;
	
	//
	// Also initial R66CurrentValue, RTUSBResumeMsduTransmission might use this value.
	// if not initial this value, the default value will be 0.
	//
	pAd->BbpTuning.R66CurrentValue = 0x38;

	pAd->Bbp94 = BBPR94_DEFAULT;
	pAd->BbpForCCK = FALSE;
	
	// Default is FALSE for test bit 1
	//pAd->bTest1 = FALSE;
	
	// initialize MAC table and allocate spin lock
	NdisZeroMemory(&pAd->MacTab, sizeof(MAC_TABLE));
	InitializeQueueHeader(&pAd->MacTab.McastPsQueue);
	NdisAllocateSpinLock(&pAd->MacTabLock);

	//RTMPInitTimer(pAd, &pAd->RECBATimer, RECBATimerTimeout, pAd, TRUE);
	//RTMPSetTimer(&pAd->RECBATimer, REORDER_EXEC_INTV);

#ifdef RALINK_ATE
	NdisZeroMemory(&pAd->ate, sizeof(ATE_INFO));
	pAd->ate.Mode = ATE_STOP;
#ifdef CONFIG_RALINK_RT3350
	pAd->ate.PABias = 0;
#endif // CONFIG_RALINK_RT3350  //
	pAd->ate.TxCount = 200;/* to exceed TX_RING_SIZE ... */
	pAd->ate.TxDoneCount = 0;
	pAd->ate.RFFreqOffset = 0;
	pAd->ate.Payload = 0xA5;//peter	
	pAd->ate.IPG = 0;//peter	
	pAd->ate.TxLength = 1024;
	pAd->ate.TxWI.ShortGI = 0;// LONG GI : 800 ns
	pAd->ate.TxWI.PHYMODE = MODE_CCK;
	pAd->ate.TxWI.MCS = 3;
	pAd->ate.TxWI.BW = BW_40;
	pAd->ate.Channel = 1;
	pAd->ate.QID = QID_AC_BE;

#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
	/* For stream mode in 3T/3R ++ */
	/* use broadcast address as default value */
	pAd->ate.Addr1[0] = 0xFF;
	pAd->ate.Addr1[1] = 0xFF;
	pAd->ate.Addr1[2] = 0xFF;
	pAd->ate.Addr1[3] = 0xFF;
	pAd->ate.Addr1[4] = 0xFF;
	pAd->ate.Addr1[5] = 0xFF;


	pAd->ate.Addr2[0] = 0x00;
	pAd->ate.Addr2[1] = 0x11;
	pAd->ate.Addr2[2] = 0x22;
	pAd->ate.Addr2[3] = 0xAA;
	pAd->ate.Addr2[4] = 0xBB;
	pAd->ate.Addr2[5] = 0xCC;

	NdisMoveMemory(pAd->ate.Addr3, pAd->ate.Addr2, ETH_LENGTH_OF_ADDRESS);

	{		
		UINT32 data;

		data = 0xFFFFFFFF;
		RTMP_IO_WRITE32(pAd, 0x1044, data); 
		RTMP_IO_READ32(pAd, 0x1048, &data); 

		data = data | 0x0000FFFF;
		RTMP_IO_WRITE32(pAd, 0x1048, data); 
	}
	/* For stream mode in 3T/3R -- */
#else
	pAd->ate.Addr1[0] = 0x00;
	pAd->ate.Addr1[1] = 0x11;
	pAd->ate.Addr1[2] = 0x22;
	pAd->ate.Addr1[3] = 0xAA;
	pAd->ate.Addr1[4] = 0xBB;
	pAd->ate.Addr1[5] = 0xCC;

	NdisMoveMemory(pAd->ate.Addr2, pAd->ate.Addr1, ETH_LENGTH_OF_ADDRESS);
	NdisMoveMemory(pAd->ate.Addr3, pAd->ate.Addr1, ETH_LENGTH_OF_ADDRESS);
#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //
	pAd->ate.bRxFER = 0;
	pAd->ate.bQATxStart = FALSE;
	pAd->ate.bQARxStart = FALSE;

#ifdef RTMP_MAC_PCI 
	pAd->ate.bFWLoading = FALSE;
#endif // RTMP_MAC_PCI //


#ifdef RALINK_28xx_QA
	pAd->ate.TxStatus = 0;
	pAd->ate.AtePid = THREAD_PID_INIT_VALUE;
#endif // RALINK_28xx_QA //
#endif // RALINK_ATE //


	pAd->CommonCfg.bWiFiTest = FALSE;
#ifdef RTMP_MAC_PCI
	pAd->bPCIclkOff = FALSE;
#endif // RTMP_MAC_PCI //

#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.EntryLifeCheck = MAC_ENTRY_LIFE_CHECK_CNT;

#endif // CONFIG_AP_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
#ifdef PCIE_PS_SUPPORT
RTMP_SET_PSFLAG(pAd, fRTMP_PS_CAN_GO_SLEEP);
#endif // PCIE_PS_SUPPORT //
#endif // CONFIG_STA_SUPPORT //
#ifdef ANT_DIVERSITY_SUPPORT
		if ( pAd->CommonCfg.bRxAntDiversity == ANT_FIX_ANT2)
		{
			pAd->RxAnt.Pair1PrimaryRxAnt = 1;
			pAd->RxAnt.Pair1SecondaryRxAnt = 0;
		}
		else // Default
		{
			pAd->RxAnt.Pair1PrimaryRxAnt = 0;
			pAd->RxAnt.Pair1SecondaryRxAnt = 1;
		}
		pAd->RxAnt.EvaluatePeriod = 0;
		pAd->RxAnt.RcvPktNumWhenEvaluate = 0;
#ifdef CONFIG_STA_SUPPORT
		pAd->RxAnt.Pair1AvgRssi[0] = pAd->RxAnt.Pair1AvgRssi[1] = 0;
#endif // CONFIG_STA_SUPPORT //
#ifdef CONFIG_AP_SUPPORT
		pAd->RxAnt.Pair1AvgRssiGroup1[0] = pAd->RxAnt.Pair1AvgRssiGroup1[1] = 0;
		pAd->RxAnt.Pair1AvgRssiGroup2[0] = pAd->RxAnt.Pair1AvgRssiGroup2[1] = 0;
#endif // CONFIG_AP_SUPPORT //
#endif // AP_ANTENNA_DIVERSITY_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
RTMP_SET_PSFLAG(pAd, fRTMP_PS_CAN_GO_SLEEP);
#endif // CONFIG_STA_SUPPORT //
	DBGPRINT(RT_DEBUG_TRACE, ("<-- UserCfgInit\n"));
}

// IRQL = PASSIVE_LEVEL
UCHAR BtoH(STRING ch)
{
	if (ch >= '0' && ch <= '9') return (ch - '0');        // Handle numerals
	if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  // Handle capitol hex digits
	if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  // Handle small hex digits
	return(255);
}

//
//  FUNCTION: AtoH(char *, UCHAR *, int)
//
//  PURPOSE:  Converts ascii string to network order hex
//
//  PARAMETERS:
//    src    - pointer to input ascii string
//    dest   - pointer to output hex
//    destlen - size of dest
//
//  COMMENTS:
//
//    2 ascii bytes make a hex byte so must put 1st ascii byte of pair
//    into upper nibble and 2nd ascii byte of pair into lower nibble.
//
// IRQL = PASSIVE_LEVEL

void AtoH(PSTRING src, PUCHAR dest, int destlen)
{
	PSTRING srcptr;
	PUCHAR destTemp;

	srcptr = src;	
	destTemp = (PUCHAR) dest; 

	while(destlen--)
	{
		*destTemp = BtoH(*srcptr++) << 4;    // Put 1st ascii byte in upper nibble.
		*destTemp += BtoH(*srcptr++);      // Add 2nd ascii byte to above.
		destTemp++;
	}
}


//+++Mark by shiang, not use now, need to remove after confirm
//---Mark by shiang, not use now, need to remove after confirm


/*
	========================================================================
	
	Routine Description:
		Init timer objects

	Arguments:
		pAd			Pointer to our adapter
		pTimer				Timer structure
		pTimerFunc			Function to execute when timer expired
		Repeat				Ture for period timer

	Return Value:
		None

	Note:
		
	========================================================================
*/
VOID	RTMPInitTimer(
	IN	PRTMP_ADAPTER			pAd,
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	PVOID					pTimerFunc,
	IN	PVOID					pData,
	IN	BOOLEAN					Repeat)
{
	//
	// Set Valid to TRUE for later used.
	// It will crash if we cancel a timer or set a timer 
	// that we haven't initialize before.
	// 
	pTimer->Valid      = TRUE;
	
	pTimer->PeriodicType = Repeat;
	pTimer->State      = FALSE;
	pTimer->cookie = (ULONG) pData;

	pTimer->pAd = pAd;

	RTMP_OS_Init_Timer(pAd, &pTimer->TimerObj,	pTimerFunc, (PVOID) pTimer);	
}


/*
	========================================================================
	
	Routine Description:
		Init timer objects

	Arguments:
		pTimer				Timer structure
		Value				Timer value in milliseconds

	Return Value:
		None

	Note:
		To use this routine, must call RTMPInitTimer before.
		
	========================================================================
*/
VOID	RTMPSetTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	ULONG					Value)
{
	if (pTimer->Valid)
	{
		RTMP_ADAPTER *pAd;
		
		pAd = (RTMP_ADAPTER *)pTimer->pAd;
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))
		{
			DBGPRINT_ERR(("RTMPSetTimer failed, Halt in Progress!\n"));
			return;
		}
		
		pTimer->TimerValue = Value;
		pTimer->State      = FALSE;
		if (pTimer->PeriodicType == TRUE)
		{
			pTimer->Repeat = TRUE;
			RTMP_SetPeriodicTimer(&pTimer->TimerObj, Value);
		}
		else
		{
			pTimer->Repeat = FALSE;
			RTMP_OS_Add_Timer(&pTimer->TimerObj, Value);
		}
	}
	else
	{
		DBGPRINT_ERR(("RTMPSetTimer failed, Timer hasn't been initialize!\n"));
	}
}


/*
	========================================================================
	
	Routine Description:
		Init timer objects

	Arguments:
		pTimer				Timer structure
		Value				Timer value in milliseconds

	Return Value:
		None

	Note:
		To use this routine, must call RTMPInitTimer before.
		
	========================================================================
*/
VOID	RTMPModTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	ULONG					Value)
{
	BOOLEAN	Cancel;

	if (pTimer->Valid)
	{
		pTimer->TimerValue = Value;
		pTimer->State      = FALSE;
		if (pTimer->PeriodicType == TRUE)
		{
			RTMPCancelTimer(pTimer, &Cancel);
			RTMPSetTimer(pTimer, Value);
		}
		else
		{
			RTMP_OS_Mod_Timer(&pTimer->TimerObj, Value);
		}
	}
	else
	{
		DBGPRINT_ERR(("RTMPModTimer failed, Timer hasn't been initialize!\n"));
	}
}


/*
	========================================================================
	
	Routine Description:
		Cancel timer objects

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	Note:
		1.) To use this routine, must call RTMPInitTimer before.
		2.) Reset NIC to initial state AS IS system boot up time.
		
	========================================================================
*/
VOID	RTMPCancelTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	OUT	BOOLEAN					*pCancelled)
{
	if (pTimer->Valid)
	{
		if (pTimer->State == FALSE)
			pTimer->Repeat = FALSE;
		
		RTMP_OS_Del_Timer(&pTimer->TimerObj, pCancelled);
		
		if (*pCancelled == TRUE)
			pTimer->State = TRUE;

#ifdef RTMP_TIMER_TASK_SUPPORT
		// We need to go-through the TimerQ to findout this timer handler and remove it if 
		//		it's still waiting for execution.
		RtmpTimerQRemove(pTimer->pAd, pTimer);
#endif // RTMP_TIMER_TASK_SUPPORT //
	}
	else
	{
		DBGPRINT_ERR(("RTMPCancelTimer failed, Timer hasn't been initialize!\n"));
	}
}


/*
	========================================================================
	
	Routine Description:
		Set LED Status

	Arguments:
		pAd						Pointer to our adapter
		Status					LED Status

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
VOID RTMPSetLED(
	IN PRTMP_ADAPTER 	pAd, 
	IN UCHAR			Status)
{
	//ULONG			data;
	UCHAR			HighByte = 0;
	UCHAR			LowByte;
#ifdef WSC_INCLUDED
#ifdef WSC_LED_SUPPORT
	PWSC_CTRL		pWscControl = NULL;

#ifdef CONFIG_AP_SUPPORT
	pWscControl = &pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl;
#endif // CONFIG_AP_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
	pWscControl = &pAd->StaCfg.WscControl;
#endif // CONFIG_STA_SUPPORT //
#endif // WSC_LED_SUPPORT //
#endif // WSC_INCLUDED //
	BOOLEAN 		bIgnored = FALSE;

#ifdef RALINK_ATE
	/*
		In ATE mode of RT2860 AP/STA, we have erased 8051 firmware.
		So LED mode is not supported when ATE is running.
	*/
	if (!IS_RT3572(pAd))
	{
		if (ATE_ON(pAd))
			return;
	}
#endif // RALINK_ATE //

	LowByte = pAd->LedCntl.field.LedMode&0x7f;
	switch (Status)
	{
		case LED_LINK_DOWN:
			HighByte = 0x20;
			AsicSendCommandToMcu(pAd, 0x50, 0xff, LowByte, HighByte);
			pAd->LedIndicatorStrength = 0; 
			break;
		case LED_LINK_UP:
			if (pAd->CommonCfg.Channel > 14)
				HighByte = 0xa0;
			else
				HighByte = 0x60;
			AsicSendCommandToMcu(pAd, 0x50, 0xff, LowByte, HighByte);
			break;
		case LED_RADIO_ON:
			HighByte = 0x20;
			AsicSendCommandToMcu(pAd, 0x50, 0xff, LowByte, HighByte);
			break;
		case LED_HALT: 
			LowByte = 0; // Driver sets MAC register and MAC controls LED
		case LED_RADIO_OFF:
			HighByte = 0;
			AsicSendCommandToMcu(pAd, 0x50, 0xff, LowByte, HighByte);
			break;
		case LED_WPS:
			HighByte = 0x10;
			AsicSendCommandToMcu(pAd, 0x50, 0xff, LowByte, HighByte);
			break;
		case LED_ON_SITE_SURVEY:
			HighByte = 0x08;
			AsicSendCommandToMcu(pAd, 0x50, 0xff, LowByte, HighByte);
			break;
		case LED_POWER_UP:
			HighByte = 0x04;
			AsicSendCommandToMcu(pAd, 0x50, 0xff, LowByte, HighByte);
			break;
#ifdef WSC_INCLUDED
#ifdef WSC_LED_SUPPORT
		case LED_WPS_IN_PROCESS:
			if (WscSupportWPSLEDMode(pAd))
			{
				HighByte = LINK_STATUS_WPS_IN_PROCESS;
				AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
				pWscControl->WscLEDMode = LED_WPS_IN_PROCESS;
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_IN_PROCESS\n", __FUNCTION__));				
			}
			else
				bIgnored = TRUE;
			break;
		case LED_WPS_ERROR:
			if (WscSupportWPSLEDMode(pAd))
			{
				// In the case of LED mode 9, the error LED should be turned on only after WPS walk time expiration.
				if ((pWscControl->bWPSWalkTimeExpiration == FALSE) && 
					 ((pAd->LedCntl.field.LedMode & LED_MODE) == WPS_LED_MODE_9))
				{
					// do nothing.
				}
				else
				{
					HighByte = LINK_STATUS_WPS_ERROR;
					AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
				}
		
				pWscControl->WscLEDMode = LED_WPS_ERROR;
				pWscControl->WscLastWarningLEDMode = LED_WPS_ERROR;
				
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_ERROR\n", __FUNCTION__));
			}
			else
				bIgnored = TRUE;
			break;
		case LED_WPS_SESSION_OVERLAP_DETECTED:
			if (WscSupportWPSLEDMode(pAd))
			{
				HighByte = LINK_STATUS_WPS_SESSION_OVERLAP_DETECTED;
				AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
				pWscControl->WscLEDMode = LED_WPS_SESSION_OVERLAP_DETECTED;
				pWscControl->WscLastWarningLEDMode = LED_WPS_SESSION_OVERLAP_DETECTED;
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_SESSION_OVERLAP_DETECTED\n", __FUNCTION__));
			}
			else
				bIgnored = TRUE;
			break;
		case LED_WPS_SUCCESS:
			if (WscSupportWPSLEDMode(pAd))
			{
				if (((pAd->LedCntl.field.LedMode & LED_MODE) == WPS_LED_MODE_7) || 
					((pAd->LedCntl.field.LedMode & LED_MODE) == WPS_LED_MODE_11) || 
					((pAd->LedCntl.field.LedMode & LED_MODE) == WPS_LED_MODE_12))
				{
					// In the WPS LED mode 7, 11 and 12, the blue LED would last 300 seconds regardless of the AP's security settings.
					HighByte = LINK_STATUS_WPS_SUCCESS_WITH_SECURITY;
					AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
					pWscControl->WscLEDMode = LED_WPS_SUCCESS;
		
					// Turn off the WPS successful LED pattern after 300 seconds.
					RTMPSetTimer(&pWscControl->WscLEDTimer, WSC_SUCCESSFUL_LED_PATTERN_TIMEOUT);
					
					DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_SUCCESS (LINK_STATUS_WPS_SUCCESS_WITH_SECURITY)\n", __FUNCTION__));
				}
				else if ((pAd->LedCntl.field.LedMode & LED_MODE) == WPS_LED_MODE_8) // The WPS LED mode 8
				{
					if (WscAPHasSecuritySetting(pAd, pWscControl)) // The WPS AP has the security setting.
					{
						HighByte = LINK_STATUS_WPS_SUCCESS_WITH_SECURITY;
						AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
						pWscControl->WscLEDMode = LED_WPS_SUCCESS;
		
						// Turn off the WPS successful LED pattern after 300 seconds.
						RTMPSetTimer(&pWscControl->WscLEDTimer, WSC_SUCCESSFUL_LED_PATTERN_TIMEOUT);
						
						DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_SUCCESS (LINK_STATUS_WPS_SUCCESS_WITH_SECURITY)\n", __FUNCTION__));
					}
					else // The WPS AP does not have the secuirty setting.
					{
						HighByte = LINK_STATUS_WPS_SUCCESS_WITHOUT_SECURITY;
						AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
						pWscControl->WscLEDMode = LED_WPS_SUCCESS;
		
						// Turn off the WPS successful LED pattern after 300 seconds.
						RTMPSetTimer(&pWscControl->WscLEDTimer, WSC_SUCCESSFUL_LED_PATTERN_TIMEOUT);
						
						DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_SUCCESS (LINK_STATUS_WPS_SUCCESS_WITHOUT_SECURITY)\n", __FUNCTION__));
					}
				}
				else if ((pAd->LedCntl.field.LedMode & LED_MODE) == WPS_LED_MODE_9) // The WPS LED mode 9.
				{
					// Always turn on the WPS blue LED for 300 seconds.
					HighByte = LINK_STATUS_WPS_BLUE_LED;
					AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
					pWscControl->WscLEDMode = LED_WPS_SUCCESS;
		
					// Turn off the WPS successful LED pattern after 300 seconds.
					RTMPSetTimer(&pWscControl->WscLEDTimer, WSC_SUCCESSFUL_LED_PATTERN_TIMEOUT);
					
					DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_SUCCESS (LINK_STATUS_WPS_BLUE_LED)\n", __FUNCTION__));
				}
				else
				{
					DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_SUCCESS (Incorrect LED mode = %d)\n", 
						__FUNCTION__, (pAd->LedCntl.field.LedMode & LED_MODE)));
					ASSERT(FALSE);
				}
			}
			else
				bIgnored = TRUE;
			break;
		case LED_WPS_TURN_LED_OFF:
			if (WscSupportWPSLEDMode(pAd))
			{
				HighByte = LINK_STATUS_WPS_TURN_LED_OFF;
				AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
				pWscControl->WscLEDMode = LED_WPS_TURN_LED_OFF;
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_TURN_LED_OFF\n", __FUNCTION__));
			}
			else
				bIgnored = TRUE;
			break;
		case LED_WPS_TURN_ON_BLUE_LED:
			if (WscSupportWPSLEDMode(pAd))
			{
				HighByte = LINK_STATUS_WPS_BLUE_LED;
				AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
				pWscControl->WscLEDMode = LED_WPS_SUCCESS;
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_TURN_ON_BLUE_LED\n", __FUNCTION__));
			}
			else
				bIgnored = TRUE;
			break;
		case LED_NORMAL_CONNECTION_WITHOUT_SECURITY:
			if (WscSupportWPSLEDMode(pAd))
			{
				HighByte = LINK_STATUS_NORMAL_CONNECTION_WITHOUT_SECURITY;
				AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
				pWscControl->WscLEDMode = LED_WPS_SUCCESS;
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LINK_STATUS_NORMAL_CONNECTION_WITHOUT_SECURITY\n", __FUNCTION__));
			}
			else
				bIgnored = TRUE;
			break;
		case LED_NORMAL_CONNECTION_WITH_SECURITY:
			if (WscSupportWPSLEDMode(pAd))
			{
				HighByte = LINK_STATUS_NORMAL_CONNECTION_WITH_SECURITY;
				AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
				pWscControl->WscLEDMode = LED_WPS_SUCCESS;
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LINK_STATUS_NORMAL_CONNECTION_WITH_SECURITY\n", __FUNCTION__));
			}
			else
				bIgnored = TRUE;
			break;
		//WPS LED Mode 10
		case LED_WPS_MODE10_TURN_ON:
			if(WscSupportWPSLEDMode10(pAd))
			{
				HighByte = LINK_STATUS_WPS_MODE10_TURN_ON;
				AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_MODE10_TURN_ON\n", __FUNCTION__));
			}
			else
				bIgnored = TRUE;
			break;
		case LED_WPS_MODE10_FLASH:
			if(WscSupportWPSLEDMode10(pAd))
			{	
				HighByte = LINK_STATUS_WPS_MODE10_FLASH;
				AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_MODE10_FLASH\n", __FUNCTION__));
			}
			else
				bIgnored = TRUE;
			break;
		case LED_WPS_MODE10_TURN_OFF:
			if(WscSupportWPSLEDMode10(pAd))
			{
				HighByte = LINK_STATUS_WPS_MODE10_TURN_OFF;
				AsicSendCommandToMcu(pAd, MCU_SET_WPS_LED_MODE, 0xff, LowByte, HighByte);
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_MODE10_TURN_OFF\n", __FUNCTION__));
			}
			else
				bIgnored = TRUE;
			break;
#endif // WSC_LED_SUPPORT //
#endif // WSC_INCLUDED //
		default:
			DBGPRINT(RT_DEBUG_WARN, ("RTMPSetLED::Unknown Status %d\n", Status));
			break;
	}

    //
	// Keep LED status for LED SiteSurvey mode.
	// After SiteSurvey, we will set the LED mode to previous status.
	//
	if ((Status != LED_ON_SITE_SURVEY) && (Status != LED_POWER_UP) && (bIgnored == FALSE))
		pAd->LedStatus = Status;
    
	DBGPRINT(RT_DEBUG_TRACE, ("RTMPSetLED::Mode=%d,HighByte=0x%02x,LowByte=0x%02x\n", pAd->LedCntl.field.LedMode, HighByte, LowByte));
}

/*
	========================================================================
	
	Routine Description:
		Set LED Signal Stregth 

	Arguments:
		pAd						Pointer to our adapter
		Dbm						Signal Stregth

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	
	Note:
		Can be run on any IRQL level. 

		According to Microsoft Zero Config Wireless Signal Stregth definition as belows.
		<= -90  No Signal
		<= -81  Very Low
		<= -71  Low
		<= -67  Good
		<= -57  Very Good
		 > -57  Excellent		
	========================================================================
*/
VOID RTMPSetSignalLED(
	IN PRTMP_ADAPTER 	pAd, 
	IN NDIS_802_11_RSSI Dbm)
{
	UCHAR		nLed = 0;

	if (pAd->LedCntl.field.LedMode == LED_MODE_SIGNAL_STREGTH)
	{
	if (Dbm <= -90)
		nLed = 0;
	else if (Dbm <= -81)
		nLed = 1;
	else if (Dbm <= -71)
		nLed = 3;
	else if (Dbm <= -67)
		nLed = 7;
	else if (Dbm <= -57)
		nLed = 15;
	else 
		nLed = 31;

	//
	// Update Signal Stregth to firmware if changed.
	//
	if (pAd->LedIndicatorStrength != nLed)
	{
		AsicSendCommandToMcu(pAd, 0x51, 0xff, nLed, pAd->LedCntl.field.Polarity);
		pAd->LedIndicatorStrength = nLed;
	}
}
}

#ifdef WSC_STA_SUPPORT
#ifdef WSC_LED_SUPPORT
//
// LED indication for normal connection start.
//
VOID
LEDConnectionStart(
	IN PRTMP_ADAPTER pAd)
{
	// LED indication.
	//if (pAd->StaCfg.WscControl.bWPSSession == FALSE)
	//if (pAd->StaCfg.WscControl.WscConfMode != WSC_DISABLE && pAd->StaCfg.WscControl.bWscTrigger)
	if (pAd->StaCfg.WscControl.WscConfMode == WSC_DISABLE)
	{
		if ((pAd->LedCntl.field.LedMode & LED_MODE) == WPS_LED_MODE_9) // LED mode 9.
		{
			UCHAR WPSLEDStatus = 0;
			
			// The AP uses OPEN-NONE.
			if ((pAd->StaCfg.AuthMode == Ndis802_11AuthModeOpen) && (pAd->StaCfg.WepStatus == Ndis802_11WEPDisabled))
			{
				WPSLEDStatus = LED_WPS_TURN_LED_OFF;
#ifdef RTMP_MAC_PCI
				RTMPSetLED(pAd, WPSLEDStatus);
#endif // RTMP_MAC_PCI //
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_TURN_LED_OFF\n", __FUNCTION__));
			}
			else // The AP uses an encryption algorithm.
			{
				WPSLEDStatus = LED_WPS_IN_PROCESS;
#ifdef RTMP_MAC_PCI
				RTMPSetLED(pAd, WPSLEDStatus);
#endif // RTMP_MAC_PCI //
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_IN_PROCESS\n", __FUNCTION__));
			}
		}
	}
}


//
// LED indication for normal connection completion.
//
VOID
LEDConnectionCompletion(
	IN PRTMP_ADAPTER pAd, 
	IN BOOLEAN bSuccess)
{
	// LED indication.
	//if (pAd->StaCfg.WscControl.bWPSSession == FALSE)
	if (pAd->StaCfg.WscControl.WscConfMode == WSC_DISABLE)
	{
		if ((pAd->LedCntl.field.LedMode & LED_MODE) == WPS_LED_MODE_9) // LED mode 9.
		{
			UCHAR WPSLEDStatus = 0;
			
			if (bSuccess == TRUE) // Successful connenction.
			{				
				// The AP uses OPEN-NONE.
				if ((pAd->StaCfg.AuthMode == Ndis802_11AuthModeOpen) && (pAd->StaCfg.WepStatus == Ndis802_11WEPDisabled))
				{
					WPSLEDStatus = LED_NORMAL_CONNECTION_WITHOUT_SECURITY;
#ifdef RTMP_MAC_PCI
					RTMPSetLED(pAd, WPSLEDStatus);
#endif // RTMP_MAC_PCI //
					DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_NORMAL_CONNECTION_WITHOUT_SECURITY\n", __FUNCTION__));
				}
				else // The AP uses an encryption algorithm.
				{
					WPSLEDStatus = LED_NORMAL_CONNECTION_WITH_SECURITY;
#ifdef RTMP_MAC_PCI
					RTMPSetLED(pAd, WPSLEDStatus);
#endif // RTMP_MAC_PCI //
					DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_NORMAL_CONNECTION_WITH_SECURITY\n", __FUNCTION__));
				}
			}
			else // Connection failure.
			{
				WPSLEDStatus = LED_WPS_TURN_LED_OFF;
#ifdef RTMP_MAC_PCI
				RTMPSetLED(pAd, WPSLEDStatus);
#endif // RTMP_MAC_PCI //
				DBGPRINT(RT_DEBUG_TRACE, ("%s: LED_WPS_TURN_LED_OFF\n", __FUNCTION__));
			}
		}
	}
}
#endif // WSC_LED_SUPPORT //
#endif // WSC_STA_SUPPORT //

/*
	========================================================================
	
	Routine Description:
		Enable RX 

	Arguments:
		pAd						Pointer to our adapter

	Return Value:
		None

	IRQL <= DISPATCH_LEVEL
	
	Note:
		Before Enable RX, make sure you have enabled Interrupt.
	========================================================================
*/
VOID RTMPEnableRxTx(
	IN PRTMP_ADAPTER	pAd)
{
//	WPDMA_GLO_CFG_STRUC	GloCfg;
//	ULONG	i = 0;
	UINT32 rx_filter_flag;

	DBGPRINT(RT_DEBUG_TRACE, ("==> RTMPEnableRxTx\n"));

	// Enable Rx DMA.
	RT28XXDMAEnable(pAd);

	// enable RX of MAC block
	if (pAd->OpMode == OPMODE_AP)
	{
		rx_filter_flag = APNORMAL;

#ifdef CONFIG_AP_SUPPORT
#ifdef IDS_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			if (pAd->ApCfg.IdsEnable)
				rx_filter_flag &= (~0x4);	// Don't drop those not-U2M frames
		}
#endif // IDS_SUPPORT //			
#endif // CONFIG_AP_SUPPORT //

		RTMP_IO_WRITE32(pAd, RX_FILTR_CFG, rx_filter_flag);     // enable RX of DMA block
	}
	else
	{
		if (pAd->CommonCfg.PSPXlink)
			rx_filter_flag = PSPXLINK;
		else
			rx_filter_flag = STANORMAL;     // Staion not drop control frame will fail WiFi Certification.
		RTMP_IO_WRITE32(pAd, RX_FILTR_CFG, rx_filter_flag);
	}
	
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0xc);
	DBGPRINT(RT_DEBUG_TRACE, ("<== RTMPEnableRxTx\n"));	
}


//+++Add by shiang, move from os/linux/rt_main_dev.c
void CfgInitHook(PRTMP_ADAPTER pAd)
{
	pAd->bBroadComHT = TRUE;
}


int rt28xx_init(
	IN PRTMP_ADAPTER pAd, 
	IN PSTRING pDefaultMac, 
	IN PSTRING pHostName)
{
	UINT					index;
	UCHAR					TmpPhy;
	NDIS_STATUS				Status;
	UINT32 					MacCsr0 = 0;


#ifdef CONFIG_STA_SUPPORT
#ifdef PCIE_PS_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
    	// If dirver doesn't wake up firmware here,
    	// NICLoadFirmware will hang forever when interface is up again.
    	// RT2860 PCI
    	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE) &&
        	OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE))
    	{
        	AUTO_WAKEUP_STRUC AutoWakeupCfg;
			AsicForceWakeup(pAd, TRUE);
        	AutoWakeupCfg.word = 0;
	    	RTMP_IO_WRITE32(pAd, AUTO_WAKEUP_CFG, AutoWakeupCfg.word);
        	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
    	}
	}
#endif // PCIE_PS_SUPPORT //
#endif // CONFIG_STA_SUPPORT //


	// reset Adapter flags
	RTMP_CLEAR_FLAGS(pAd);

	// Init BssTab & ChannelInfo tabbles for auto channel select.
#ifdef CONFIG_AP_SUPPORT	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		AutoChBssTableInit(pAd);
		ChannelInfoInit(pAd);
	}
#endif // CONFIG_AP_SUPPORT //

#ifdef DOT11_N_SUPPORT
	// Allocate BA Reordering memory
	ba_reordering_resource_init(pAd, MAX_REORDERING_MPDU_NUM);
#endif // DOT11_N_SUPPORT //

	// Make sure MAC gets ready.
	index = 0;
	do
	{
		RTMP_IO_READ32(pAd, MAC_CSR0, &MacCsr0);
		pAd->MACVersion = MacCsr0;

		if ((pAd->MACVersion != 0x00) && (pAd->MACVersion != 0xFFFFFFFF))
			break;

		RTMPusecDelay(10);
	} while (index++ < 100);
	DBGPRINT(RT_DEBUG_TRACE, ("MAC_CSR0  [ Ver:Rev=0x%08x]\n", pAd->MACVersion));

#ifdef RTMP_MAC_PCI
#if defined(RT3090) || defined(RT3592) || defined(RT3390)
	/*Iverson patch PCIE L1 issue to make sure that driver can be read,write ,BBP and RF register  at pcie L.1 level */
	if ((IS_RT3090(pAd) || IS_RT3572(pAd) || IS_RT3390(pAd))&&pAd->infType==RTMP_DEV_INF_PCIE)
	{
		RTMP_IO_READ32(pAd, AUX_CTRL, &MacCsr0);
		MacCsr0 |= 0x402;
		RTMP_IO_WRITE32(pAd, AUX_CTRL, MacCsr0);
		DBGPRINT(RT_DEBUG_TRACE, ("AUX_CTRL = 0x%x\n", MacCsr0));
	}
#endif // RT3090 //

	// To fix driver disable/enable hang issue when radio off
	RTMP_IO_WRITE32(pAd, PWR_PIN_CFG, 0x2);
#endif // RTMP_MAC_PCI //

	// Disable DMA
	RT28XXDMADisable(pAd);


	// Load 8051 firmware
	Status = NICLoadFirmware(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("NICLoadFirmware failed, Status[=0x%08x]\n", Status));
		goto err1;
	}

	NICLoadRateSwitchingParams(pAd);

	// Disable interrupts here which is as soon as possible
	// This statement should never be true. We might consider to remove it later
#ifdef RTMP_MAC_PCI
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE))
	{
		RTMP_ASIC_INTERRUPT_DISABLE(pAd);
	}
#endif // RTMP_MAC_PCI //

	Status = RTMPAllocTxRxRingMemory(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("RTMPAllocDMAMemory failed, Status[=0x%08x]\n", Status));
		goto err1;
	}

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE);

	// initialize MLME
	//

	Status = RtmpMgmtTaskInit(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
		goto err2;

	Status = MlmeInit(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("MlmeInit failed, Status[=0x%08x]\n", Status));
		goto err2;
	}


	// Initialize pAd->StaCfg, pAd->ApCfg, pAd->CommonCfg to manufacture default
	//
	UserCfgInit(pAd);
	Status = RtmpNetTaskInit(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
		goto err3;

//	COPY_MAC_ADDR(pAd->ApCfg.MBSSID[apidx].Bssid, netif->hwaddr);
//	pAd->bForcePrintTX = TRUE;

	CfgInitHook(pAd);

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		APInitialize(pAd);
#endif // CONFIG_AP_SUPPORT //	

#ifdef BLOCK_NET_IF
	initblockQueueTab(pAd);
#endif // BLOCK_NET_IF //


	MeasureReqTabInit(pAd);
	TpcReqTabInit(pAd);

	//
	// Init the hardware, we need to init asic before read registry, otherwise mac register will be reset
	//
	Status = NICInitializeAdapter(pAd, TRUE);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("NICInitializeAdapter failed, Status[=0x%08x]\n", Status));
		if (Status != NDIS_STATUS_SUCCESS)
		goto err3;
	}	

	// Read parameters from Config File 
	Status = RTMPReadParametersHook(pAd);

	DBGPRINT(RT_DEBUG_OFF, ("1. Phy Mode = %d\n", pAd->CommonCfg.PhyMode));
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("RTMPReadParametersHook failed, Status[=0x%08x]\n",Status));
		goto err4;
	}

#ifdef DOT11_N_SUPPORT
   	//Init Ba Capability parameters.
//	RT28XX_BA_INIT(pAd);
	pAd->CommonCfg.DesiredHtPhy.MpduDensity = (UCHAR)pAd->CommonCfg.BACapability.field.MpduDensity;
	pAd->CommonCfg.DesiredHtPhy.AmsduEnable = (USHORT)pAd->CommonCfg.BACapability.field.AmsduEnable;
	pAd->CommonCfg.DesiredHtPhy.AmsduSize = (USHORT)pAd->CommonCfg.BACapability.field.AmsduSize;
	pAd->CommonCfg.DesiredHtPhy.MimoPs = (USHORT)pAd->CommonCfg.BACapability.field.MMPSmode;
	// UPdata to HT IE
	pAd->CommonCfg.HtCapability.HtCapInfo.MimoPs = (USHORT)pAd->CommonCfg.BACapability.field.MMPSmode;
	pAd->CommonCfg.HtCapability.HtCapInfo.AMsduSize = (USHORT)pAd->CommonCfg.BACapability.field.AmsduSize;
	pAd->CommonCfg.HtCapability.HtCapParm.MpduDensity = (UCHAR)pAd->CommonCfg.BACapability.field.MpduDensity;
#endif // DOT11_N_SUPPORT //

	// after reading Registry, we now know if in AP mode or STA mode

	// Load 8051 firmware; crash when FW image not existent
	// Status = NICLoadFirmware(pAd);
	// if (Status != NDIS_STATUS_SUCCESS)
	//    break;

	DBGPRINT(RT_DEBUG_OFF, ("2. Phy Mode = %d\n", pAd->CommonCfg.PhyMode));

	// We should read EEPROM for all cases.  rt2860b
	NICReadEEPROMParameters(pAd, (PUCHAR)pDefaultMac);
	DBGPRINT(RT_DEBUG_OFF, ("3. Phy Mode = %d\n", pAd->CommonCfg.PhyMode));
	NICInitAsicFromEEPROM(pAd); //rt2860b
	
	// Set PHY to appropriate mode
#ifdef CONFIG_STA_SUPPORT
#ifdef DOT11_N_SUPPORT
	if ((pAd->OpMode == OPMODE_STA) &&
		(pAd->CommonCfg.HT_DisallowTKIP == TRUE) &&
		(pAd->StaCfg.BssType == BSS_ADHOC) &&
		(pAd->CommonCfg.PhyMode >= PHY_11ABGN_MIXED) &&
		((pAd->StaCfg.WepStatus == Ndis802_11WEPEnabled) ||
		 (pAd->StaCfg.WepStatus == Ndis802_11Encryption2Enabled)))
	{
		ULONG tmpPhymode = PHY_11ABGN_MIXED;
		
		switch(pAd->CommonCfg.PhyMode)
		{
			case PHY_11ABGN_MIXED:
			case PHY_11AGN_MIXED:
				tmpPhymode = PHY_11ABG_MIXED;
				break;					
			case PHY_11N_2_4G:
			case PHY_11GN_MIXED:
				tmpPhymode = PHY_11G;
				break;
			case PHY_11AN_MIXED:
			case PHY_11N_5G:
				tmpPhymode = PHY_11A;
				break;
			case PHY_11BGN_MIXED:
				tmpPhymode = PHY_11BG_MIXED;
				break;
		}
		RTMPSetPhyMode(pAd, tmpPhymode);
	}
	else
#endif // DOT11_N_SUPPORT //
#endif // CONFIG_STA_SUPPORT //
	{
	TmpPhy = pAd->CommonCfg.PhyMode;
	pAd->CommonCfg.PhyMode = 0xff;
	RTMPSetPhyMode(pAd, TmpPhy);
#ifdef DOT11_N_SUPPORT
	SetCommonHT(pAd);
#endif // DOT11_N_SUPPORT //
	}

	// No valid channels.
	if (pAd->ChannelListNum == 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Wrong configuration. No valid channel found. Check \"ContryCode\" and \"ChannelGeography\" setting.\n"));
		goto err4;
	}

#ifdef DOT11_N_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("MCS Set = %02x %02x %02x %02x %02x\n", pAd->CommonCfg.HtCapability.MCSSet[0],
           pAd->CommonCfg.HtCapability.MCSSet[1], pAd->CommonCfg.HtCapability.MCSSet[2],
           pAd->CommonCfg.HtCapability.MCSSet[3], pAd->CommonCfg.HtCapability.MCSSet[4]));
#endif // DOT11_N_SUPPORT //


#ifdef RTMP_RF_RW_SUPPORT
	//Init RT30xx RFRegisters after read RFIC type from EEPROM
	NICInitRFRegisters(pAd);
#endif // RTMP_RF_RW_SUPPORT //


#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		/* init QBSS Element */
		QBSS_LoadInit(pAd);

	}
#endif // CONFIG_AP_SUPPORT //

//		APInitialize(pAd);

#ifdef IKANOS_VX_1X0
	VR_IKANOS_FP_Init(pAd->ApCfg.BssidNum, pAd->PermanentAddress);
#endif // IKANOS_VX_1X0 //

		//
	// Initialize RF register to default value
	//
	AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
	AsicLockChannel(pAd, pAd->CommonCfg.Channel);		

	// 8051 firmware require the signal during booting time.
	//2008/11/28:KH marked the following codes to patch Frequency offset bug
	//AsicSendCommandToMcu(pAd, 0x72, 0xFF, 0x00, 0x00);

	if (pAd && (Status != NDIS_STATUS_SUCCESS))
	{
		//
		// Undo everything if it failed
		//
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
		{
			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE);
		}
	}
	else if (pAd)
	{
		// Microsoft HCT require driver send a disconnect event after driver initialization.
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED);
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_MEDIA_STATE_CHANGE);

		DBGPRINT(RT_DEBUG_TRACE, ("NDIS_STATUS_MEDIA_DISCONNECT Event B!\n"));

#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			if (pAd->ApCfg.bAutoChannelAtBootup || (pAd->CommonCfg.Channel == 0))
			{
				UINT8 BBPValue = 0;
				
				// Enable Interrupt first due to we need to scan channel to receive beacons.
				RTMP_IRQ_ENABLE(pAd);
				// Now Enable RxTx
				RTMPEnableRxTx(pAd);
				RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_START_UP);

				// Let BBP register at 20MHz to do scan		
				RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BBPValue);
				BBPValue &= (~0x18);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BBPValue);
				DBGPRINT(RT_DEBUG_ERROR, ("SYNC - BBP R4 to 20MHz.l\n"));

				// Now we can receive the beacon and do the listen beacon
				pAd->CommonCfg.Channel = RandomChannel(pAd);
			}

#ifdef DOT11_N_SUPPORT
			// If phymode > PHY_11ABGN_MIXED and BW=40 check extension channel, after select channel  
			N_ChannelCheck(pAd);

#ifdef DOT11N_DRAFT3
        	/* 
         	* We only do this Overlapping BSS Scan when system up, for the 
         	* other situation of channel changing, we depends on station's report to adjust ourself.
         	*/
			APOverlappingBSSScan(pAd);
			RTMPInitTimer(pAd, &pAd->CommonCfg.Bss2040CoexistTimer, GET_TIMER_FUNCTION(Bss2040CoexistTimeOut), pAd, FALSE);
#endif // DOT11N_DRAFT3 //
#endif // DOT11_N_SUPPORT //

			APStartUp(pAd);
			DBGPRINT(RT_DEBUG_OFF, ("Main bssid = %02x:%02x:%02x:%02x:%02x:%02x\n", 
									PRINT_MAC(pAd->ApCfg.MBSSID[BSS0].Bssid)));
		}
#endif // CONFIG_AP_SUPPORT //

	}// end of else

#ifdef WSC_INCLUDED
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		INT apidx;
#ifdef HOSTAPD_SUPPORT
		if (pAd->ApCfg.Hostapd == TRUE)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("WPS is control by hostapd now.\n"));
		}
		else
#endif //HOSTAPD_SUPPORT//
		for (apidx = 0; apidx < pAd->ApCfg.BssidNum; apidx++)
		{
			PWSC_CTRL pWscControl;
			pWscControl = &pAd->ApCfg.MBSSID[apidx].WscControl;
			DBGPRINT(RT_DEBUG_TRACE, ("Generate UUID for apidx(%d)\n", apidx));
			WscGenerateUUID(pAd, &pWscControl->Wsc_Uuid_E[0], &pWscControl->Wsc_Uuid_Str[0], apidx, FALSE);
			WscInit(pAd, FALSE, apidx);
			if (pWscControl->WscEnrolleePinCode == 0)
				pWscControl->WscEnrolleePinCode = WscGeneratePinCode(pAd, FALSE, apidx);
		}
	}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		PWSC_CTRL pWscControl = &pAd->StaCfg.WscControl;
		WscGenerateUUID(pAd, &pWscControl->Wsc_Uuid_E[0], &pWscControl->Wsc_Uuid_Str[0], 0, FALSE);
		WscInit(pAd, BSS0);
		if (pWscControl->WscEnrolleePinCode == 0)
			pWscControl->WscEnrolleePinCode = WscGeneratePinCode(pAd, BSS0);
	}
#endif // CONFIG_STA_SUPPORT //

	/* WSC hardware push button function 0811 */
	WSC_HDR_BTN_Init(pAd);
#endif // WSC_INCLUDED //
	// Various AP function init
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef MBSS_SUPPORT
		/* the function can not be moved to RT2860_probe() even register_netdev()
		   is changed as register_netdevice().
		   Or in some PC, kernel will panic (Fedora 4) */
		RT28xx_MBSS_Init(pAd, pAd->net_dev);
#endif // MBSS_SUPPORT //

#ifdef WDS_SUPPORT
		RT28xx_WDS_Init(pAd, pAd->net_dev);
#endif // WDS_SUPPORT //

#ifdef APCLI_SUPPORT
		RT28xx_ApCli_Init(pAd, pAd->net_dev);
#endif // APCLI_SUPPORT //
	}
#endif // CONFIG_AP_SUPPORT //


#ifdef RT3XXX_ANTENNA_DIVERSITY_SUPPORT
	RT3XXX_AntDiversity_Init(pAd);
#endif // RT3XXX_ANTENNA_DIVERSITY_SUPPORT //


#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef MAT_SUPPORT
		MATEngineInit(pAd);
#endif // MAT_SUPPORT //

#ifdef CLIENT_WDS
	CliWds_ProxyTabInit(pAd);
#endif // CLIENT_WDS //
	}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{

	}
#endif // CONFIG_STA_SUPPORT //

#ifdef WMM_ACM_SUPPORT
#ifdef CONFIG_AP_SUPPORT
	ACMP_Init(pAd,
			pAd->CommonCfg.APEdcaParm.bACM[0],
			pAd->CommonCfg.APEdcaParm.bACM[1],
			pAd->CommonCfg.APEdcaParm.bACM[2],
			pAd->CommonCfg.APEdcaParm.bACM[3], 0);
#endif // CONFIG_AP_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
	ACMP_Init(pAd);
#endif // CONFIG_STA_SUPPORT //
#endif // WMM_ACM_SUPPORT //


#if defined (CONFIG_RALINK_RT2883) || defined (CONFIG_RALINK_RT3883)
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (IS_RT2883(pAd) || IS_RT3883(pAd))
		{
			RTMP_IO_WRITE32(pAd, TX_FBK_CFG_3S_0, 0x12111008);
			RTMP_IO_WRITE32(pAd, TX_FBK_CFG_3S_1, 0x16151413);
		}
	}

	// Init static Stream Mode
	if (pAd->CommonCfg.StreamMode == 2) // Static
	{
		ULONG streamWord;
		// Enable Stream mode for first three entries in MAC table
		if (pAd->Antenna.field.TxPath==3)
			streamWord = 0xF0000;
		else if (pAd->Antenna.field.TxPath==2)
			streamWord = 0x30000;
		else
			streamWord = 0x00000;

		RTMP_IO_WRITE32(pAd, TX_CHAIN_ADDR0_L, (ULONG)(pAd->CommonCfg.StreamModeMac[0][0]) | (ULONG)(pAd->CommonCfg.StreamModeMac[0][1] << 8)  | 
						(ULONG)(pAd->CommonCfg.StreamModeMac[0][2] << 16) | (ULONG)(pAd->CommonCfg.StreamModeMac[0][3] << 24));
		RTMP_IO_WRITE32(pAd, TX_CHAIN_ADDR0_H, streamWord | (ULONG)(pAd->CommonCfg.StreamModeMac[0][4]) | (ULONG)(pAd->CommonCfg.StreamModeMac[0][5] << 8));
		
		RTMP_IO_WRITE32(pAd, TX_CHAIN_ADDR1_L, (ULONG)(pAd->CommonCfg.StreamModeMac[1][0]) | (ULONG)(pAd->CommonCfg.StreamModeMac[1][1] << 8)  | 
					(ULONG)(pAd->CommonCfg.StreamModeMac[1][2] << 16) | (ULONG)(pAd->CommonCfg.StreamModeMac[1][3] << 24));
		RTMP_IO_WRITE32(pAd, TX_CHAIN_ADDR1_H, streamWord | (ULONG)(pAd->CommonCfg.StreamModeMac[1][4]) | (ULONG)(pAd->CommonCfg.StreamModeMac[1][5] << 8));

		RTMP_IO_WRITE32(pAd, TX_CHAIN_ADDR2_L, (ULONG)(pAd->CommonCfg.StreamModeMac[2][0]) | (ULONG)(pAd->CommonCfg.StreamModeMac[2][1] << 8)  | 
					(ULONG)(pAd->CommonCfg.StreamModeMac[2][2] << 16) | (ULONG)(pAd->CommonCfg.StreamModeMac[2][3] << 24));
		RTMP_IO_WRITE32(pAd, TX_CHAIN_ADDR2_H, streamWord | (ULONG)(pAd->CommonCfg.StreamModeMac[2][4]) | (ULONG)(pAd->CommonCfg.StreamModeMac[2][5] << 8));

		RTMP_IO_WRITE32(pAd, TX_CHAIN_ADDR3_L, (ULONG)(pAd->CommonCfg.StreamModeMac[3][0]) | (ULONG)(pAd->CommonCfg.StreamModeMac[3][1] << 8)  | 
					(ULONG)(pAd->CommonCfg.StreamModeMac[3][2] << 16) | (ULONG)(pAd->CommonCfg.StreamModeMac[3][3] << 24));
		RTMP_IO_WRITE32(pAd, TX_CHAIN_ADDR3_H, streamWord | (ULONG)(pAd->CommonCfg.StreamModeMac[3][4]) | (ULONG)(pAd->CommonCfg.StreamModeMac[3][5] << 8));

	}

#endif // CONFIG_AP_SUPPORT //

	if (pAd->CommonCfg.FineAGC)
	{
		UINT8 BBPValue = 0;
		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BBPValue);
		BBPValue |= 0x40; // turn on fine AGC
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, BBPValue);
	}
	else
	{
		UINT8 BBPValue = 0;
		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BBPValue);
		BBPValue &= ~0x40; // turn off fine AGC
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, BBPValue);
	}
	

#endif // CONFIG_RALINK_RT2883 || CONFIG_RALINK_RT3883 //
	
#ifdef TXBF_SUPPORT
	if (pAd->CommonCfg.ITxBfTimeout)
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R179, 0x02);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R180, 0);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R182, pAd->CommonCfg.ITxBfTimeout & 0xFF);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R180, 1);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R182, (pAd->CommonCfg.ITxBfTimeout>>8) & 0xFF);
	}

	if (pAd->CommonCfg.ETxBfTimeout)
	{
		RTMP_IO_WRITE32(pAd, TX_TXBF_CFG_3, pAd->CommonCfg.ETxBfTimeout);
	}
#endif // TXBF_SUPPORT //


#ifdef CONFIG_RALINK_RT3350
	if(1)
	{
		USHORT i;
	        USHORT value;
		UCHAR  rf_offset;
		UCHAR  rf_value;

		RT28xx_EEPROM_READ16(pAd, 0x120, value);
		rf_value = value & 0x00FF;
                rf_offset = (value & 0xFF00) >> 8;
		
		if(rf_offset == 0xff)
		        rf_offset = RF_R09;
		if(rf_value == 0xff)
			rf_value = 0x01;

		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, rf_offset, (UCHAR)rf_value);
		
		RT28xx_EEPROM_READ16(pAd, 0x122, value);
		rf_value = value & 0x00FF;
                rf_offset = (value & 0xFF00) >> 8;

		if(rf_offset == 0xff)
		        rf_offset = RF_R19;
		if(rf_value == 0xff)
			rf_value = 0xcc;

		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, rf_offset, (UCHAR)rf_value);
	}

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
	DBGPRINT_S(Status, ("<==== rt28xx_init, Status=%x\n", Status));

	return TRUE;


err4:
err3:
	MlmeHalt(pAd);
err2:
	RTMPFreeTxRxRingMemory(pAd);
err1:

#ifdef DOT11_N_SUPPORT
	os_free_mem(pAd, pAd->mpdu_blk_pool.mem); // free BA pool
#endif // DOT11_N_SUPPORT //

	// shall not set priv to NULL here because the priv didn't been free yet.
	//net_dev->ml_priv = 0;
#ifdef INF_AMAZON_SE
err0:
#endif // INF_AMAZON_SE //
#ifdef ST
err0:
#endif // ST //

	DBGPRINT(RT_DEBUG_ERROR, ("!!! rt28xx Initialized fail !!!\n"));
	return FALSE;
}
//---Add by shiang, move from os/linux/rt_main_dev.c


static INT RtmpChipOpsRegister(
	IN RTMP_ADAPTER *pAd,
	IN INT			infType)
{
	RTMP_CHIP_OP	*pChipOps = &pAd->chipOps;
	int status;
	
	memset(pChipOps, 0, sizeof(RTMP_CHIP_OP));

	/* set eeprom related hook functions */
	status = RtmpChipOpsEepromHook(pAd, infType);

	/* set mcu related hook functions */
	switch(infType)
	{
#ifdef RTMP_PCI_SUPPORT
		case RTMP_DEV_INF_PCI:
		case RTMP_DEV_INF_PCIE:
			pChipOps->loadFirmware = RtmpAsicLoadFirmware;
			pChipOps->eraseFirmware = RtmpAsicEraseFirmware;
			pChipOps->sendCommandToMcu = RtmpAsicSendCommandToMcu;
			break;
#endif // RTMP_PCI_SUPPORT //


#ifdef RTMP_RBUS_SUPPORT
		case RTMP_DEV_INF_RBUS:
			pChipOps->sendCommandToMcu = (void *)RtmpAsicSendCommandToSwMcu;
			break;
#endif //RTMP_RBUS_SUPPORT //
		default:
			break;
	}

	return status;
}


INT RtmpRaDevCtrlInit(
	IN RTMP_ADAPTER *pAd, 
	IN RTMP_INF_TYPE infType)
{
	//VOID	*handle;

	// Assign the interface type. We need use it when do register/EEPROM access.
	pAd->infType = infType;

	
#ifdef CONFIG_STA_SUPPORT
	pAd->OpMode = OPMODE_STA;
	printk("STA Driver version-%s\n", STA_DRIVER_VERSION);
#endif // CONFIG_STA_SUPPORT //

#ifdef CONFIG_AP_SUPPORT
	pAd->OpMode = OPMODE_AP;
	printk("AP Driver version-%s\n", AP_DRIVER_VERSION);
#endif // CONFIG_AP_SUPPORT //


	RtmpChipOpsRegister(pAd, infType);

#ifdef MULTIPLE_CARD_SUPPORT
{
	extern BOOLEAN RTMP_CardInfoRead(PRTMP_ADAPTER pAd);

	// find its profile path
	pAd->MC_RowID = -1; // use default profile path
	RTMP_CardInfoRead(pAd);

	if (pAd->MC_RowID == -1)
#ifdef CONFIG_AP_SUPPORT
		strcpy(pAd->MC_FileName, AP_PROFILE_PATH);
#endif // CONFIG_AP_SUPPORT //
#ifdef CONFIG_STA_SUPPORT
		strcpy(pAd->MC_FileName, STA_PROFILE_PATH);
#endif // CONFIG_STA_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("MC> ROW = %d, PATH = %s\n", pAd->MC_RowID, pAd->MC_FileName));
}
#endif // MULTIPLE_CARD_SUPPORT //

	return 0;
}


BOOLEAN RtmpRaDevCtrlExit(IN RTMP_ADAPTER *pAd)
{
#ifdef MULTIPLE_CARD_SUPPORT
extern UINT8  MC_CardUsed[MAX_NUM_OF_MULTIPLE_CARD];

	if ((pAd->MC_RowID >= 0) && (pAd->MC_RowID <= MAX_NUM_OF_MULTIPLE_CARD))
		MC_CardUsed[pAd->MC_RowID] = 0; // not clear MAC address
#endif // MULTIPLE_CARD_SUPPORT //

	RTMPFreeAdapter(pAd);

	return TRUE;
}

// not yet support MBSS
PNET_DEV get_netdev_from_bssid(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			FromWhichBSSID)
{
	PNET_DEV dev_p = NULL;

	
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		UCHAR infRealIdx;

		infRealIdx = FromWhichBSSID & (NET_DEVICE_REAL_IDX_MASK);
#ifdef APCLI_SUPPORT
		if(FromWhichBSSID >= MIN_NET_DEVICE_FOR_APCLI)
		{
			dev_p = (infRealIdx > MAX_APCLI_NUM ? NULL : pAd->ApCfg.ApCliTab[infRealIdx].dev);
		} 
		else
#endif // APCLI_SUPPORT //
#ifdef WDS_SUPPORT
		if(FromWhichBSSID >= MIN_NET_DEVICE_FOR_WDS)
		{
			dev_p = ((infRealIdx > MAX_WDS_ENTRY) ? NULL : pAd->WdsTab.WdsEntry[infRealIdx].dev);
		}
		else
#endif // WDS_SUPPORT //
		{
			if (FromWhichBSSID >= pAd->ApCfg.BssidNum)
	    		{
				DBGPRINT(RT_DEBUG_ERROR, ("%s: fatal error ssid > ssid num!\n", __FUNCTION__));
				dev_p = pAd->net_dev;
	    		}

	    		if (FromWhichBSSID == BSS0)
				dev_p = pAd->net_dev;
	    		else
	    		{
	    	    		dev_p = pAd->ApCfg.MBSSID[FromWhichBSSID].MSSIDDev;
	    		}
		}
	}
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		dev_p = pAd->net_dev;
	}
#endif // CONFIG_STA_SUPPORT //

	ASSERT(dev_p);
	return dev_p; /* return one of MBSS */
}
