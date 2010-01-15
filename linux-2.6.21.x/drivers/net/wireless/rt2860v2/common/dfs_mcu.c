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
	dfs_mcu.c
 
    Abstract:
 
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */
 
#include <rt_config.h>

#ifdef CONFIG_AP_SUPPORT	
// Radar detection and carrier detection for 2880-SW-MCU

#if defined(DFS_SUPPORT) || defined(CARRIER_DETECTION_SUPPORT)

static void TimerCB_Radar(PRTMP_ADAPTER pAd);
static void TimerCB_Carrier(PRTMP_ADAPTER pAd);

#ifdef RT305x
#ifdef CARRIER_DETECTION_SUPPORT
static void ToneRadarProgram(PRTMP_ADAPTER pAd);
static void ToneRadarEnable(PRTMP_ADAPTER pAd);
#endif // CARRIER_DETECTION_SUPPORT //
#endif // RT305x //

#ifdef NEW_DFS
static VOID NewTimerCB_Radar(PRTMP_ADAPTER pAd);
#endif // NEW_DFS //

void TimerCB(unsigned long data)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)data;
	if (pAd->CommonCfg.McuRadarProtection == 1)
	{
		return;
	}

#ifdef DFS_SUPPORT

#ifdef NEW_DFS
	if ((pAd->MACVersion == 0x28720200) && (pAd->CommonCfg.CID == 0x200))
	{
		NewTimerCB_Radar(pAd);
	}
	else
#endif // NEW_DFS //


	if (pAd->CommonCfg.McuRadarCmd & RADAR_DETECTION)
		TimerCB_Radar(pAd);
#endif // DFS_SUPPORT //

#ifdef CARRIER_DETECTION_SUPPORT
	if (pAd->CommonCfg.McuRadarCmd & CARRIER_DETECTION)
		TimerCB_Carrier(pAd);
#endif // CARRIER_DETECTION_SUPPORT //

}

#ifdef DFS_SUPPORT

#ifdef DFS_DEBUG
static unsigned int dbg_idx=0;

#define DFS_DEBUG_BEGIN				0x1
#define DFS_DEBUG_CTS_KICK			0x2
#define DFS_DEBUG_CTS_SENT			0x3
#define DFS_DEBUG_WAIT_CTS			0x4
#define DFS_DEBUG_DETECT_NO_CTS		0x5
#define DFS_DEBUG_DETECT_AFTER_CTS	0x6
#define DFS_DEBUG_WAIT_CTS_TIMEOUT	0x7
#define DFS_DEBUG_IN_DETECTION		0x8
#define DFS_DEBUG_FREE_TX			0x9
#define DFS_DEBUG_LONG_REPORT		0x10
#define DFS_DEBUG_SHORT_REPORT		0x11
#define DFS_DEBUG_SWITCH_PRI		0x12
#define DFS_DEBUG_SWITCH_EXT		0x13
#define DFS_DEBUG_0001				0x14


void DFS_Debug(UCHAR i, PRTMP_ADAPTER pAd)
{
	if (dbg_idx < 100)
	{
		pAd->CommonCfg.Detect1msIndex[dbg_idx] = i;
		pAd->CommonCfg.Detect1msTick[dbg_idx] = pAd->CommonCfg.McuRadarTick;
		pAd->CommonCfg.Detect1msDetectCount[dbg_idx] = pAd->CommonCfg.McuRadarDetectCount;
		pAd->CommonCfg.Detect1msState[dbg_idx] = pAd->CommonCfg.McuRadarState;
		pAd->CommonCfg.Detect1msEvent[dbg_idx] = pAd->CommonCfg.McuRadarEvent;

		RTMP_IO_READ32(pAd, PBF_LIFE_TIMER, &pAd->CommonCfg.Detect1msTime[dbg_idx]);

		dbg_idx++;
	}
	if (dbg_idx == 100)
	{
		DBGPRINT(RT_DEBUG_OFF, ("data collect done\n"));
		dbg_idx++;
	}
}

void DFS_Dump(PRTMP_ADAPTER pAd)
{
	int i;
	for (i = 0; i < 100; i++)
	{
		DBGPRINT(RT_DEBUG_OFF,("%02x : %02d %d ", pAd->CommonCfg.Detect1msIndex[i], pAd->CommonCfg.Detect1msTick[i], pAd->CommonCfg.Detect1msDetectCount[i]));
		DBGPRINT(RT_DEBUG_OFF,("%d %x (%lx)\n", 
				pAd->CommonCfg.Detect1msState[i], pAd->CommonCfg.Detect1msEvent[i], 
				(pAd->CommonCfg.Detect1msTime[i] - ((i == 0)? 0 : pAd->CommonCfg.Detect1msTime[i-1]))));
	}
	
	dbg_idx=0;
}
#endif // DFS_DEBUG //

#define CTS_WAIT_LOOP		500

static void TimerCB_Radar(PRTMP_ADAPTER pAd)
{
	ULONG Value;

	if (pAd->CommonCfg.McuRadarEvent & RADAR_EVENT_CARRIER_DETECTING)
	{
		pAd->CommonCfg.McuRadarTick++;
		return;
	}
	else
		pAd->CommonCfg.McuRadarTick++;
	

	if ((pAd->CommonCfg.McuRadarTick >= pAd->CommonCfg.McuRadarPeriod) && (pAd->CommonCfg.McuRadarState != DO_DETECTION))
	{

#ifdef DFS_DEBUG
			if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
			{
				DFS_Debug(DFS_DEBUG_BEGIN, pAd);
			}
#endif // DFS_DEBUG //

		/* Roger add to fix false detection(long pulse only) in the first 60 seconds */
		if (pAd->CommonCfg.W56_debug)
		{
			//ULONG time;
			RTMP_IO_READ32(pAd, PBF_LIFE_TIMER, &pAd->CommonCfg.W56_hw_2);
			RTMP_IO_READ32(pAd, CH_IDLE_STA, &Value);
			
			pAd->CommonCfg.W56_hw_sum += pAd->CommonCfg.W56_hw_2 - pAd->CommonCfg.W56_hw_1;
			pAd->CommonCfg.W56_hw_1 = pAd->CommonCfg.W56_hw_2;
			pAd->CommonCfg.W56_idx++;
			pAd->CommonCfg.W56_1s += Value;
			if ((pAd->CommonCfg.W56_idx % 100) == 0)
			{
				int i;
				//printk("~~~ %d   %d   ", pAd->CommonCfg.W56_idx, pAd->CommonCfg.W56_hw_sum - pAd->CommonCfg.W56_1s);
				if (pAd->CommonCfg.W56_hw_sum > pAd->CommonCfg.W56_1s)
					pAd->CommonCfg.W56_4s[(pAd->CommonCfg.W56_idx / 100) % 4] = pAd->CommonCfg.W56_hw_sum - pAd->CommonCfg.W56_1s;
				else
					pAd->CommonCfg.W56_4s[(pAd->CommonCfg.W56_idx / 100) % 4] = 0;
				Value = 0;
				for (i = 0; i < 4; i++)
					Value += pAd->CommonCfg.W56_4s[i];
				//printk("%d\n", Value);
				pAd->CommonCfg.W56_total = Value;
				pAd->CommonCfg.W56_hw_sum = 0;
				pAd->CommonCfg.W56_1s = 0;
			}
		}

		// Start of period
		pAd->CommonCfg.McuRadarTick = 0;
		pAd->CommonCfg.McuRadarDetectCount = 0;
		pAd->CommonCfg.McuRadarState = WAIT_CTS_BEING_SENT;
		pAd->CommonCfg.McuRadarEvent |= RADAR_EVENT_RADAR_DETECTING;


		if ((pAd->CommonCfg.McuRadarCtsProtect == 1)
#ifdef CARRIER_DETECTION_SUPPORT
			|| (isCarrierDetectExist(pAd))
#endif
			)
		{
			// no CTS protect
			pAd->CommonCfg.McuRadarState = DO_DETECTION;
			MCURadarDetect(pAd);

#ifdef DFS_DEBUG
			if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
			{
				DFS_Debug(DFS_DEBUG_DETECT_NO_CTS, pAd);
			}
#endif // DFS_DEBUG //
			
		}
		else if (pAd->CommonCfg.McuRadarCtsProtect == 2 || pAd->CommonCfg.McuRadarCtsProtect == 3)
		{
			int i;
						
			if (pAd->CommonCfg.McuRadarCtsProtect == 3)
			{
				
				RTMP_IO_READ32(pAd, 0x7784, &Value);
				Value &= 0xffff00ff;
				RTMP_IO_WRITE32(pAd, 0x7784, Value);
				
				// pAd->CommonCfg.McuRadarCtsProtect == 3, need to kick CTS two times
				RTMP_IO_WRITE32(pAd, PBF_CTRL, 0x40);
				for (i = 0; i < CTS_WAIT_LOOP; i++)
				{
					RTMP_IO_READ32(pAd, PBF_CTRL, &Value);
					if (!Value)
						break;
				}
				
				if (i >= CTS_WAIT_LOOP)
				{
					pAd->CommonCfg.McuRadarEvent |= RADAR_EVENT_CTS_KICKED;
					return;
				}
			}


			RTMP_IO_READ32(pAd, 0x7784, &Value);
			Value &= 0xffff00ff;
			Value |= (DFS_CTS_WCID << 8);
			RTMP_IO_WRITE32(pAd, 0x7784, Value);

#ifdef DFS_DEBUG
			if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
			{
				DFS_Debug(DFS_DEBUG_CTS_KICK, pAd);
			}
#endif // DFS_DEBUG //
			// kick CTS
			RTMP_IO_WRITE32(pAd, PBF_CTRL, 0x40);

		}
		return;
	}

	if (pAd->CommonCfg.McuRadarState == WAIT_CTS_BEING_SENT)
	{

#ifdef DFS_DEBUG
			if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
			{
				DFS_Debug(DFS_DEBUG_WAIT_CTS, pAd);
			}
#endif // DFS_DEBUG //

		// check event or timeout
		if (pAd->CommonCfg.McuRadarEvent & RADAR_EVENT_CTS_SENT)
		{
			pAd->CommonCfg.McuRadarEvent &= ~RADAR_EVENT_CTS_SENT;
			pAd->CommonCfg.McuRadarDetectCount = 1;
			pAd->CommonCfg.McuRadarState = DO_DETECTION;

			{
				UCHAR BBPR115;
				BBP_IO_READ8_BY_REG_ID(pAd, BBP_R115, &BBPR115);
			}

#ifdef DFS_DEBUG			
			if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
			{
				DFS_Debug(DFS_DEBUG_DETECT_AFTER_CTS, pAd);
			}
#endif // DFS_DEBUG //
			return;
		}
		
		if (pAd->CommonCfg.McuRadarDetectCount >= (4-1)) // timeout 4ms, start from 0
		{
			pAd->CommonCfg.McuRadarState = FREE_FOR_TX; // timeout 5ms, give up
			pAd->CommonCfg.McuRadarEvent &= ~(RADAR_EVENT_RADAR_DETECTING | RADAR_EVENT_CTS_KICKED);
			pAd->CommonCfg.McuRadarTick = pAd->CommonCfg.McuRadarPeriod;

#ifdef DFS_DEBUG
			if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
			{
				DFS_Debug(DFS_DEBUG_WAIT_CTS_TIMEOUT, pAd);
			}
#endif // DFS_DEBUG //

			RTMP_IO_WRITE32(pAd, TX_RTS_CFG, pAd->CommonCfg.OldRtsRetryLimit);
			// Restore BBP
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, pAd->CommonCfg._R65);
				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, pAd->CommonCfg._R66);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R113, 0x01);

			// EnableNormalTx
			RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &Value);
			Value |= 0x04;
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, Value);
			
		}

		if (pAd->CommonCfg.McuRadarEvent & RADAR_EVENT_CTS_KICKED)
		{
			int i;

			for (i = 0; i < CTS_WAIT_LOOP; i++)
			{
				RTMP_IO_READ32(pAd, PBF_CTRL, &Value);
				if (!Value)
					break;
			}
				
			if (i >= CTS_WAIT_LOOP)
			{
				pAd->CommonCfg.McuRadarDetectCount++;
				return;
			}
			
			RTMP_IO_READ32(pAd, 0x7784, &Value);
			Value &= 0xffff00ff;
			Value |= (DFS_CTS_WCID << 8) ;
			RTMP_IO_WRITE32(pAd, 0x7784, Value);

#ifdef DFS_DEBUG
			if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
			{
				DFS_Debug(DFS_DEBUG_CTS_KICK, pAd);
			}
#endif // DFS_DEBUG //

			// kick CTS
			RTMP_IO_WRITE32(pAd, PBF_CTRL, 0x40);
			
			pAd->CommonCfg.McuRadarEvent &= ~RADAR_EVENT_CTS_KICKED;
		}

		
		pAd->CommonCfg.McuRadarDetectCount++;
	}
	else if (pAd->CommonCfg.McuRadarState == DO_DETECTION)
	{
		UCHAR BBPR115 = 0;
		UCHAR bbp = 0;

#ifdef DFS_DEBUG
		if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
		{
			DFS_Debug(DFS_DEBUG_IN_DETECTION, pAd);
		}
#endif // DFS_DEBUG //

		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R115, &BBPR115);

		if (pAd->CommonCfg.RadarDetect.RDDurRegion == FCC) 
		{
			if  ((pAd->CommonCfg.McuRadarDetectCount % 2) == 0)
			{
#ifdef DFS_DEBUG
				if (pAd->CommonCfg.McuRadarDebug & RADAR_SIMULATE2) 
				{
					if (pAd->CommonCfg.McuRadarCmd & RADAR_DETECTION)
					{
						RTMP_IO_WRITE32(pAd, 0x7100, 0x400);
						pAd->CommonCfg.McuRadarEvent |= RADAR_EVENT_WIDTH_RADAR;
					}
					pAd->CommonCfg.McuRadarDebug &= ~RADAR_SIMULATE2;

				}
#endif


				if (BBPR115 & 0x2)
				{
					BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R115, 0);
#ifdef DFS_DEBUG
					if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
					{
						DFS_Debug(DFS_DEBUG_LONG_REPORT, pAd);
					}
#endif // DFS_DEBUG //

					Value = BBPR115 << 24;
					BBP_IO_READ8_BY_REG_ID(pAd, BBP_R116, &bbp);
					Value |= (bbp << 16);
					BBP_IO_READ8_BY_REG_ID(pAd, BBP_R117, &bbp);
					Value |= (bbp << 8);
					BBP_IO_READ8_BY_REG_ID(pAd, BBP_R118, &bbp);
					Value |= bbp;
					RTMP_IO_WRITE32(pAd, 0x7100 + (4 * (pAd->CommonCfg.McuRadarDetectCount / 2 - 1)) , Value);
					BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R113, 0x01);
					BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R113, 0x21);
				
					pAd->CommonCfg.McuRadarEvent |= RADAR_EVENT_WIDTH_RADAR;
				}
				else
				{
					RTMP_IO_WRITE32(pAd, 0x7100 + (4 * (pAd->CommonCfg.McuRadarDetectCount / 2 - 1)) , 0);
				}
			}
		}
		else if ((pAd->CommonCfg.RadarDetect.RDDurRegion == JAP_W56) && ((pAd->CommonCfg.McuRadarDetectCount % 2) == 0))
		{
			if (BBPR115 & 0x2)
			{

					BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R115, 0);

#ifdef DFS_DEBUG
					if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
			{
						DFS_Debug(DFS_DEBUG_LONG_REPORT, pAd);
					}
#endif // DFS_DEBUG //

				Value = BBPR115 << 24;
				BBP_IO_READ8_BY_REG_ID(pAd, BBP_R116, &bbp);
				Value |= (bbp << 16);
				BBP_IO_READ8_BY_REG_ID(pAd, BBP_R117, &bbp);
				Value |= (bbp << 8);
				BBP_IO_READ8_BY_REG_ID(pAd, BBP_R118, &bbp);
				Value |= bbp;
				RTMP_IO_WRITE32(pAd, 0x7100 + (4 * (pAd->CommonCfg.McuRadarDetectCount / 2 - 1)) , Value);
			
				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R113, 0x01);
				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R113, 0x21);
			
				pAd->CommonCfg.McuRadarEvent |= RADAR_EVENT_WIDTH_RADAR;
				
			}
			else
			{
				RTMP_IO_WRITE32(pAd, 0x7100 + (4 * (pAd->CommonCfg.McuRadarDetectCount / 2 - 1)) , 0);
			}
						
		}
		
		if (pAd->CommonCfg.McuRadarDetectCount >= (pAd->CommonCfg.McuRadarDetectPeriod - 1))
		{
			if (pAd->CommonCfg.McuRadarEvent & RADAR_EVENT_WIDTH_RADAR)
			{
				pAd->CommonCfg.McuRadarEvent &= ~(RADAR_EVENT_WIDTH_RADAR);
				RadarSMDetect(pAd, RADAR_WIDTH);
			}
			
#ifdef DFS_DEBUG
			if (pAd->CommonCfg.McuRadarDebug & RADAR_SIMULATE) 
			{
				if (pAd->CommonCfg.McuRadarCmd & RADAR_DETECTION)
				{
					RadarSMDetect(pAd, RADAR_PULSE);
				}
			}
#endif // DFS_DEBUG //
			

			if (BBPR115 & 0x1)
			{
				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R115, 0);
#ifdef DFS_DEBUG
				if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
				{
					DFS_Debug(DFS_DEBUG_SHORT_REPORT, pAd);
				}
#endif // DFS_DEBUG //
				// pulse radar detected
				if (pAd->CommonCfg.McuRadarCmd & RADAR_DETECTION)
				{
					RadarSMDetect(pAd, RADAR_PULSE);
				}
			}


			pAd->CommonCfg.McuRadarState = FREE_FOR_TX;
			pAd->CommonCfg.McuRadarEvent &= ~(RADAR_EVENT_RADAR_DETECTING);

			RTMP_IO_WRITE32(pAd, TX_RTS_CFG, pAd->CommonCfg.OldRtsRetryLimit);
			// Restore BBP
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, pAd->CommonCfg._R65);
				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, pAd->CommonCfg._R66);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R113, 0x01);

#ifdef DFS_DEBUG
			if ((pAd->CommonCfg.McuRadarDebug & RADAR_GPIO_DEBUG))
				RTMP_DFS_IO_WRITE32(0xa0300630, 0x2000);
#endif // DFS_DEBUG //
			// EnableNormalTx
			RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &Value);
			Value |= 0x04;
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, Value);

		}
		pAd->CommonCfg.McuRadarDetectCount++;

	}
	else // FREE_FOR_TX
	{

#ifdef DFS_DEBUG
			if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
			{
				DFS_Debug(DFS_DEBUG_FREE_TX, pAd);
			}
#endif // DFS_DEBUG //

		if (pAd->CommonCfg.McuRadarEvent & RADAR_EVENT_CTS_SENT)
		{

#ifdef DFS_DEBUG
			if (pAd->CommonCfg.McuRadarDebug & RADAR_LOG)
			{
				DFS_Debug(DFS_DEBUG_0001, pAd);
			}
#endif // DFS_DEBUG //

			pAd->CommonCfg.McuRadarEvent &= ~RADAR_EVENT_CTS_SENT;
			pAd->CommonCfg.McuRadarDetectCount = 1;

			RTMP_IO_WRITE32(pAd, TX_RTS_CFG, pAd->CommonCfg.OldRtsRetryLimit);
			// Restore BBP
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, pAd->CommonCfg._R65);
				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, pAd->CommonCfg._R66);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R113, 0x01);


#ifdef DFS_DEBUG
			if ((pAd->CommonCfg.McuRadarDebug & RADAR_GPIO_DEBUG))
				RTMP_DFS_IO_WRITE32(0xa0300630, 0x2000);
#endif // DFS_DEBUG //
			// EnableNormalTx
			RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &Value);
			Value |= 0x04;
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, Value);
		}
	}	
}
#endif // DFS_SUPPORT //

#ifdef CARRIER_DETECTION_SUPPORT

static void TimerCB_Carrier(PRTMP_ADAPTER pAd)
{
	ULONG Value;

	if (pAd->CommonCfg.McuRadarEvent & RADAR_EVENT_RADAR_DETECTING)
	{
		pAd->CommonCfg.McuCarrierTick++;
		return;
	}
	else
		pAd->CommonCfg.McuCarrierTick++;


	if (pAd->CommonCfg.McuCarrierTick >= pAd->CommonCfg.McuCarrierPeriod)
	{
		// Start of period
		pAd->CommonCfg.McuCarrierTick = 0;
		pAd->CommonCfg.McuCarrierDetectCount = 0;
		pAd->CommonCfg.McuCarrierState = WAIT_CTS_BEING_SENT;
		pAd->CommonCfg.McuRadarEvent |= RADAR_EVENT_CARRIER_DETECTING;

		if ((pAd->CommonCfg.McuCarrierCtsProtect == 1)
			|| (isCarrierDetectExist(pAd))
			)
		{
			// no CTS protect
			pAd->CommonCfg.McuCarrierState = DO_DETECTION;
			MCURadarDetect(pAd);
		}
		else if (pAd->CommonCfg.McuCarrierCtsProtect == 2)
		{
			// kick CTS
			RTMP_IO_WRITE32(pAd, PBF_CTRL, 0x80);
		}
		return;
	}

	if (pAd->CommonCfg.McuCarrierState == WAIT_CTS_BEING_SENT)
	{
		// check event or timeout
		if (pAd->CommonCfg.McuRadarEvent & RADAR_EVENT_CTS_CARRIER_SENT)
		{
			pAd->CommonCfg.McuRadarEvent &= ~RADAR_EVENT_CTS_CARRIER_SENT;
			pAd->CommonCfg.McuCarrierDetectCount = 1;
			pAd->CommonCfg.McuCarrierState = DO_DETECTION;

			return;
		}
		
		if (pAd->CommonCfg.McuCarrierDetectCount >= (5-1)) // timeout 5ms, start from 0
		{
			pAd->CommonCfg.McuCarrierState = FREE_FOR_TX; // timeout 5ms, give up
			pAd->CommonCfg.McuRadarEvent &= ~(RADAR_EVENT_CARRIER_DETECTING);

		}
		
		pAd->CommonCfg.McuCarrierDetectCount++;
	}
	else if (pAd->CommonCfg.McuCarrierState == DO_DETECTION)
	{
		if (pAd->CommonCfg.McuCarrierDetectCount >= (pAd->CommonCfg.McuCarrierDetectPeriod - 1))
		{
			UCHAR BBPR115;

			BBP_IO_READ8_BY_REG_ID(pAd, BBP_R115, &BBPR115);

			CarrierDetectionCheck(pAd);

			pAd->CommonCfg.McuCarrierState = FREE_FOR_TX;
			pAd->CommonCfg.McuRadarEvent &= ~(RADAR_EVENT_CARRIER_DETECTING);

			RTMP_IO_WRITE32(pAd, TX_RTS_CFG, pAd->CommonCfg.OldRtsRetryLimit);
			// Restore BBP

				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, pAd->CommonCfg._R66);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, pAd->CommonCfg._R69);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, pAd->CommonCfg._R70);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, pAd->CommonCfg._R73);

#ifdef DFS_DEBUG
			if ((pAd->CommonCfg.McuRadarDebug & RADAR_GPIO_DEBUG))
				RTMP_DFS_IO_WRITE32(0xa0300630, 0x2000);
#endif // DFS_DEBUG //
			// EnableNormalTx
			RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &Value);
			Value |= 0x04;
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, Value);

			

		}
		pAd->CommonCfg.McuCarrierDetectCount++;
	}
	else // FREE_FOR_TX
	{
		if (pAd->CommonCfg.McuRadarEvent & RADAR_EVENT_CTS_CARRIER_SENT)
		{
			pAd->CommonCfg.McuRadarEvent &= ~RADAR_EVENT_CTS_CARRIER_SENT;
			pAd->CommonCfg.McuCarrierDetectCount = 1;

			RTMP_IO_WRITE32(pAd, TX_RTS_CFG, pAd->CommonCfg.OldRtsRetryLimit);
			// Restore BBP
				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, pAd->CommonCfg._R66);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, pAd->CommonCfg._R69);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, pAd->CommonCfg._R70);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, pAd->CommonCfg._R73);


#ifdef DFS_DEBUG
			if ((pAd->CommonCfg.McuRadarDebug & RADAR_GPIO_DEBUG))
				RTMP_DFS_IO_WRITE32(0xa0300630, 0x2000);
#endif // DFS_DEBUG //
			// EnableNormalTx
			RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &Value);
			Value |= 0x04;
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, Value);
		}
	}	
}
#endif // CARRIER_DETECTION_SUPPORT //

void MCURadarDetect(PRTMP_ADAPTER pAd)
{
	ULONG Value;
	UCHAR bbp = 0;
	
	BBP_IO_READ8_BY_REG_ID(pAd, BBP_R113, &bbp);
	if (bbp & 0x20)
		return;

	//DisableNormalTx
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &Value);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0xc);
	Value &= (ULONG)~(0x04);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, Value);
				
#ifdef DFS_DEBUG
	if ((pAd->CommonCfg.McuRadarDebug & RADAR_GPIO_DEBUG))
		if (pAd->CommonCfg.McuRadarCtsProtect != 1)
			RTMP_DFS_IO_WRITE32(0xa030062c, 0x2000);
#endif // DFS_DEBUG //


	RTMP_IO_READ32(pAd, TX_RTS_CFG, &pAd->CommonCfg.OldRtsRetryLimit);
	RTMP_IO_WRITE32(pAd, TX_RTS_CFG, 0);
	
	//change BBP now
	if (pAd->CommonCfg.McuRadarEvent & RADAR_EVENT_CARRIER_DETECTING)
	{
		UCHAR BBPR4 = 0;
		
		// Clear False CCA
		RTMP_IO_READ32(pAd, RX_STA_CNT1, &Value);
		
		// save old value
		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R66, &pAd->CommonCfg._R66);
		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R69, &pAd->CommonCfg._R69);
		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R70, &pAd->CommonCfg._R70);
		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R73, &pAd->CommonCfg._R73);

		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BBPR4);
		if (BBPR4 & 0x18) // BW 40
		{
			if (pAd->CommonCfg.Channel > 14)
			{
				// BW 40, A band
				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x50);
			}
			else
			{
				// BW 40, G band
				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x40);
			}
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x30);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, 0x30);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x08);
		}
		else // BW 20
		{
			if (pAd->CommonCfg.Channel > 14)
			{
				// BW 20, A band
				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x60);
			}
			else
			{
				// BW 20, G band
				BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x30);
			}
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x28);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, 0x28);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x08);
		}
		
	}
	else // RADAR_DETECTION
	{
		// save old value				
		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &pAd->CommonCfg._R65);
		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R66, &pAd->CommonCfg._R66);

		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, pAd->CommonCfg.R65);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, pAd->CommonCfg.R66);
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R113, 0x21);
	}		
	return;
}

#ifdef RT305x
#ifdef CARRIER_DETECTION_SUPPORT

// for debug and test
static ULONG time[20];
static ULONG idle[20];
static ULONG busy[20];
static ULONG cd_idx=0;

void RTMPHandleRadarInterrupt(PRTMP_ADAPTER  pAd)
{
	UINT32 value, delta;
	
	RTMP_IO_READ32(pAd, PBF_LIFE_TIMER, &value);
	RTMP_IO_READ32(pAd, CH_IDLE_STA, &pAd->CommonCfg.CarrierDetect.idle_time);
	RTMP_IO_READ32(pAd, CH_BUSY_STA, &pAd->CommonCfg.CarrierDetect.busy_time);
	delta = (value >> 4) - pAd->CommonCfg.CarrierDetect.TimeStamp;
	pAd->CommonCfg.CarrierDetect.TimeStamp = value >> 4;
	
	if (pAd->CommonCfg.CarrierDetect.Debug)
	{
		if (cd_idx < 20)
		{
			time[cd_idx] = delta;
			idle[cd_idx] = pAd->CommonCfg.CarrierDetect.idle_time;
			busy[cd_idx] = pAd->CommonCfg.CarrierDetect.busy_time;
			cd_idx++;
		}
		else
		{
			int i;
			pAd->CommonCfg.CarrierDetect.Debug = 0;
			for (i = 0; i < 20; i++)
			{
				printk("%3d %4ld %ld %ld\n", i, time[i], idle[i], busy[i]);
			}
			cd_idx = 0;
			
		}
	}
	

	if (pAd->CommonCfg.CarrierDetect.CD_State == CD_NORMAL)
	{
		if ((delta < pAd->CommonCfg.CarrierDetect.criteria) && (pAd->CommonCfg.CarrierDetect.recheck))
			pAd->CommonCfg.CarrierDetect.recheck --;

		if (pAd->CommonCfg.CarrierDetect.recheck == 0)
		{
			// declare carrier sense
			pAd->CommonCfg.CarrierDetect.CD_State = CD_SILENCE;
			pAd->CommonCfg.CarrierDetect.recheck = pAd->CommonCfg.CarrierDetect.recheck2;
			if (pAd->CommonCfg.CarrierDetect.CarrierDebug == 0)
			{
	
				DBGPRINT(RT_DEBUG_TRACE, ("Carrier Detected\n"));

				// disconnect all STAs behind AP.
				//MacTableReset(pAd);
				
				// stop all TX actions including Beacon sending.
				AsicDisableSync(pAd);
			}
			else
			{
				printk("Carrier Detected\n");
			}
			

		}
	}
	else if (pAd->CommonCfg.CarrierDetect.CD_State == CD_SILENCE)
	{
		// check if carrier gone??
		if ((delta > (pAd->CommonCfg.CarrierDetect.criteria * CARRIER_DETECT_STOP_RATIO)) && (pAd->CommonCfg.CarrierDetect.recheck))
			pAd->CommonCfg.CarrierDetect.recheck --;
		if (pAd->CommonCfg.CarrierDetect.recheck == 0)
		{
			pAd->CommonCfg.CarrierDetect.CD_State = CD_NORMAL;
			pAd->CommonCfg.CarrierDetect.recheck = pAd->CommonCfg.CarrierDetect.recheck1;
			if (pAd->CommonCfg.CarrierDetect.CarrierDebug == 0)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Carrier gone\n"));
				// start all TX actions.
				APMakeAllBssBeacon(pAd);
				APUpdateAllBeaconFrame(pAd);
				AsicEnableBssSync(pAd);
			}
			else
			{
				printk("Carrier gone\n");
			}
		}
		
	}
	
	
	if (pAd->CommonCfg.CarrierDetect.Enable)
	{
		ToneRadarProgram(pAd);
		ToneRadarEnable(pAd);
	}
}



static void ToneRadarProgram(PRTMP_ADAPTER pAd)
{
	UCHAR bbp;
	// programe delta delay & division bit
	BBP_IO_WRITE8_BY_REG_ID(pAd, 184, 0xf0);
	bbp = pAd->CommonCfg.CarrierDetect.delta << 4;
	bbp |= (pAd->CommonCfg.CarrierDetect.div_flag & 0x1) << 3;
	BBP_IO_WRITE8_BY_REG_ID(pAd, 185, bbp);
	
	// program threshold
	BBP_IO_WRITE8_BY_REG_ID(pAd, 184, 0x34);
	BBP_IO_WRITE8_BY_REG_ID(pAd, 185, (pAd->CommonCfg.CarrierDetect.threshold & 0xff000000) >> 24);
	
	BBP_IO_WRITE8_BY_REG_ID(pAd, 184, 0x24);
	BBP_IO_WRITE8_BY_REG_ID(pAd, 185, (pAd->CommonCfg.CarrierDetect.threshold & 0xff0000) >> 16);
	
	BBP_IO_WRITE8_BY_REG_ID(pAd, 184, 0x14);
	BBP_IO_WRITE8_BY_REG_ID(pAd, 185, (pAd->CommonCfg.CarrierDetect.threshold & 0xff00) >> 8);

	BBP_IO_WRITE8_BY_REG_ID(pAd, 184, 0x04);
	BBP_IO_WRITE8_BY_REG_ID(pAd, 185, pAd->CommonCfg.CarrierDetect.threshold & 0xff);
}

static void ToneRadarEnable(PRTMP_ADAPTER pAd)
{
	BBP_IO_WRITE8_BY_REG_ID(pAd, 184, 0x05);
}


void NewCarrierDetectionStart(PRTMP_ADAPTER pAd)
{	
	ULONG Value;
	// Enable Bandwidth usage monitor
		
	RTMP_IO_READ32(pAd, CH_TIME_CFG, &Value);
	RTMP_IO_WRITE32(pAd, CH_TIME_CFG, Value | 0x1f);
	pAd->CommonCfg.CarrierDetect.recheck1 = CARRIER_DETECT_RECHECK_TIME;
	pAd->CommonCfg.CarrierDetect.recheck2 = CARRIER_DETECT_STOP_RECHECK_TIME;
	

	// Init Carrier Detect
	if (pAd->CommonCfg.CarrierDetect.Enable)
	{
		pAd->CommonCfg.CarrierDetect.TimeStamp = 0;
		pAd->CommonCfg.CarrierDetect.recheck = pAd->CommonCfg.CarrierDetect.recheck1;
		ToneRadarProgram(pAd);
		ToneRadarEnable(pAd);
	}
	
}
#endif // CARRIER_DETECTION_SUPPORT //

#endif // RT305x //

#ifdef DFS_SUPPORT
#ifdef NEW_DFS
typedef struct _NewDFSProgParam
{
	UCHAR channel;
	UCHAR mode;
	USHORT avgLen; // M = Average length
	USHORT ELow;
	USHORT EHigh;
	USHORT WLow;
	USHORT WHigh;
	USHORT EpsilonW;
	ULONG TLow;
	ULONG THigh;
	USHORT EpsilonT;
}NewDFSProgParam, *pNewDFSProgParam;

#define NEW_DFS_FCC		0x1 // include Japan
#define NEW_DFS_EU		0x2
#define NEW_DFS_JAP		0x4
#define NEW_DFS_END		0xff
#define MAX_VALID_RADAR_W	5
#define MAX_VALID_RADAR_T	5

typedef struct _NewDFSTable
{
	USHORT type;
	NewDFSProgParam entry[NEW_DFS_MAX_CHANNEL];
}NewDFSTable, *pNewDFSTable;



static NewDFSTable NewDFSTable1[] = 
{
	{
		NEW_DFS_FCC,
		{
		{0, 0,  20,  18,  26,   10, 2000, 1,  3000, 30000, 5},
		{1, 0, 100, 100, 140,   12, 2000, 3,  2500, 30000, 5},
		{2, 0, 100,  60, 120,   12, 2000, 3,  3000, 30000, 5},
		{3, 2, 200,  20, 150, 300, 2000,  50, 15000, 45000, 200},
		}
	},
	{
		NEW_DFS_EU,
		{
		{0, 0, 12,	16,	20,	10,	1000,	1, 4900, 101000, 5},
		{1, 0, 70,	80,110, 20, 2000,	3, 4900, 101000, 5},
		{2, 0, 80,	60, 120, 20, 3000,	3, 4900, 101000, 5},
		{3, 3, 250,	20, 200, 300,4000,	50, 4900, 10100, 200},
		}
	},
	{
		NEW_DFS_JAP,
		{
		{0, 0,  10,  10,  15,   3,  2000,  1,  3000, 85000, 10},
		{1, 0, 70, 100, 140,   12, 2000, 3,  2500, 85000, 10},
		{2, 0, 80,  60, 120,   12, 2000, 3,  3000, 85000, 10},
		{3, 2, 200,  20, 150, 300, 2000,  50, 15000, 45000, 200},
		}
	}
	
};

typedef struct _NewDFSValidRadar
{
	USHORT type;
	USHORT channel; // bit map
	USHORT WLow;
	USHORT WHigh;
	USHORT W[MAX_VALID_RADAR_W];
	USHORT WMargin;
	ULONG TLow;
	ULONG THigh;
	ULONG T[MAX_VALID_RADAR_T];
	USHORT TMargin;
}NewDFSValidRadar, *pNewDFSValidRadar;

#ifdef DFS_DEBUG

typedef struct _NewDFSDebugResult
{
	char delta_delay_shift;
	char EL_shift;
	char EH_shift;
	char WL_shift;
	char WH_shift;
	ULONG hit_time;
	ULONG false_time;
}NewDFSDebugResult, *pNewDFSDebugResult;

NewDFSDebugResult TestResult[1000];

#endif


NewDFSValidRadar NewDFSValidTable[] = 
{
	// FCC-1  && (Japan W53 Radar 1 / W56 Radar 2)
	{
	(NEW_DFS_FCC | NEW_DFS_JAP),
	//3,
	7,
	//0, 0, 
	//{20, 0, 0, 0, 0},
	10, 1000,
	{0, 0, 0, 0, 0},
	4,
	0, 0, 
	{28560 - 70, 0, 0, 0, 0},
	150
	},
	// FCC-2
	{
	(NEW_DFS_FCC | NEW_DFS_JAP),
	//3,
	7,
	//20, 100, 
	13, 1000,
	{0, 0, 0, 0, 0},
	1,
	3000, 4600 - 20,
	{0, 0, 0, 0, 0},
	25
	},
	// FCC-3 & FCC-4
	{
	(NEW_DFS_FCC | NEW_DFS_JAP),
	//6,
	7,
	//120, 200, FCC-3 
	//220, 400, FCC-4
	100, 1500, 
	{0, 0, 0, 0, 0},
	1,
	4000, 10000 - 40, 
	{0, 0, 0, 0, 0},
	60
	},
	// FCC-6
	{
	(NEW_DFS_FCC | NEW_DFS_JAP),
	//3,
	7,
	//0, 0, 
	//{20, 0, 0, 0, 0},
	12, 1000,
	{0, 0, 0, 0, 0},
	1,
	0, 0, 
	{6660-10, 0, 0, 0, 0},
	35
	},
	// Japan W53 Radar 2
	{
	NEW_DFS_JAP,
	7,
	//0, 0, 
	//{50, 0, 0, 0, 0},
	40, 1000, 
	{0, 0, 0, 0, 0},
	1,
	0, 0, 
	{76923 - 30, 0, 0, 0, 0},
	180
	},
	// Japan W56 Radar 1
	{
	NEW_DFS_JAP,
	7,
	//0, 0, 
	//{10, 0, 0, 0, 0},
	5, 500, 
	{0, 0, 0, 0, 0},
	2,
	0, 0, 
	{27777 - 30, 0, 0, 0, 0},
	70
	},
	// Japan W56 Radar 3
	{
	NEW_DFS_JAP,
	7,
	//0, 0, 
	//{40, 0, 0, 0, 0},
	30, 1000, 
	{0, 0, 0, 0, 0},
	1,
	0, 0, 
	{80000 - 50, 0, 0, 0, 0},
	200
	},

// CE Staggered radar

	{
	//	EN-1
	//	width	0.8 - 5 us
	//	PRF		200 - 1000 Hz
	//	PRI		5000 - 1000 us	(T: 20000 - 100000)
	//	
	NEW_DFS_EU,
	0xf,
	10, 1000, 
	{0, 0, 0, 0, 0},
	1,
	20000-15, 100000-70, 
	{0, 0, 0, 0, 0},
	120
	},
	//	EN-2
	//	width	0.8 - 15 us
	//	PRF		200 - 1600 Hz
	//	PRI		5000 - 625 us	(T: 12500 - 100000)
	{
	NEW_DFS_EU,
	0xf,
	10, 2000, 
	{0, 0, 0, 0, 0},
	1,
	12500 - 10, 100000 - 70, 
	{0, 0, 0, 0, 0},
	120
	},
	
	//	EN-3
	//	width	0.8 - 15 us
	//	PRF		2300 - 4000 Hz
	//	PRI		434 - 250 us	(T: 5000 - 8695)
	//
	{
	NEW_DFS_EU,
	0xf,
	21, 2000, 
	{0, 0, 0, 0, 0},
	1,
	5000 - 4, 8695 - 7, 
	{0, 0, 0, 0, 0},
	50
	},
	//	EN-4
	//	width	20 - 30 us
	//	PRF		2000 - 4000 Hz
	//	PRI		500 - 250 us	(T: 5000 - 10000)
	//	Note : with Chirp Modulation +- 2,5Mhz
	{
	NEW_DFS_EU,
	0xf,
	380, 3000, 
	{0, 0, 0, 0, 0},
	4,
	5000 - 4, 10000 - 8, 
	{0, 0, 0, 0, 0},
	60
	},
	//	EN-5
	//	width	0.8 - 2 us
	//	PRF		300 - 400 Hz
	//	PRI		3333 - 2500 us	(T: 50000 - 66666)
	//	Staggered PRF, 20 - 50 pps
	{
	NEW_DFS_EU,
	0xf,
	10, 800, 
	{0, 0, 0, 0, 0},
	1,
	50000 - 35, 66666 - 50, 
	{0, 0, 0, 0, 0},
	90
	},
	//	EN-6
	//	width	0.8 - 2 us
	//	PRF		400 - 1200 Hz
	//	PRI		2500 - 833 us	(T: 16666 - 50000)
	//	Staggered PRF, 80 - 400 pps
	{
	NEW_DFS_EU,
	0xf,
	10, 800, 
	{0, 0, 0, 0, 0},
	1,
	16666 - 13, 50000 - 35, 
	{0, 0, 0, 0, 0},
	80
	},
	
	{
	NEW_DFS_END,
	0,
	0, 0, 
	{0, 0, 0, 0, 0},
	0,
	0, 0, 
	{0, 0, 0, 0, 0},
	0,
	},
};



void 	modify_table1(PRTMP_ADAPTER pAd, ULONG idx, ULONG value)
{
	pNewDFSTable pDFS2Table;
	ULONG x, y;	
	
	if (idx <= 40)
	{
		if (pAd->CommonCfg.MCURadarRegion == NEW_DFS_FCC)
			pDFS2Table = &NewDFSTable1[0];
		else if (pAd->CommonCfg.MCURadarRegion == NEW_DFS_EU)
		{
			pDFS2Table = &NewDFSTable1[1];
		}
		else // Japan
			pDFS2Table = &NewDFSTable1[2];
	}
	else
		return;
		
		

	idx--;
	
	x = idx / 10;
	y = idx % 10;

	switch (y)
	{
	case 0:
		pDFS2Table->entry[x].mode = (USHORT)value;
		break;
	case 1:
		pDFS2Table->entry[x].avgLen = (USHORT)value;
		break;
	case 2:
		pDFS2Table->entry[x].ELow = (USHORT)value;
		break;

	case 3:
		pDFS2Table->entry[x].EHigh = (USHORT)value;
		break;

	case 4:
		pDFS2Table->entry[x].WLow = (USHORT)value;
		break;

	case 5:
		pDFS2Table->entry[x].WHigh = (USHORT)value;
		break;

	case 6:
		pDFS2Table->entry[x].EpsilonW = (USHORT)value;
		break;

	case 7:
		pDFS2Table->entry[x].TLow = (ULONG)value;
		break;

	case 8:
		pDFS2Table->entry[x].THigh = (ULONG)value;
		break;

	case 9:
		pDFS2Table->entry[x].EpsilonT = (USHORT)value;
		break;
	default:
		break;
	}

	for (x = 0; x < 4; x++)
		printk("mode %d, (M)%03d  (E)%02d %03d (W) %03d %04d <EW> %02d (T) %05d %05d <ET> %03d\n", (unsigned int)pDFS2Table->entry[x].mode, (unsigned int)pDFS2Table->entry[x].avgLen,
		(unsigned int)pDFS2Table->entry[x].ELow, (unsigned int)pDFS2Table->entry[x].EHigh, (unsigned int)pDFS2Table->entry[x].WLow, (unsigned int)pDFS2Table->entry[x].WHigh,
		(unsigned int)pDFS2Table->entry[x].EpsilonW, (unsigned int)pDFS2Table->entry[x].TLow, (unsigned int)pDFS2Table->entry[x].THigh, (unsigned int)pDFS2Table->entry[x].EpsilonT);
	
}



void 	modify_table2(PRTMP_ADAPTER pAd, ULONG idx, ULONG value)
{
	pNewDFSValidRadar pDFSValidRadar;
	ULONG x, y;
	
	idx--;

	x = idx / 17;
	y = idx % 17;
	
	pDFSValidRadar = &NewDFSValidTable[0];
	
	while (pDFSValidRadar->type != NEW_DFS_END)
	{
		if (pDFSValidRadar->type & pAd->CommonCfg.MCURadarRegion)
		{
			if (x == 0)
				break;
			else
			{
				x--;
				pDFSValidRadar++;
			}
		}
		else
			pDFSValidRadar++;
	}
	
	if (pDFSValidRadar->type == NEW_DFS_END)
	{
		printk("idx=%d exceed max number\n", (unsigned int)idx);
		return;
	}
	switch(y)
	{
	case 0:
		pDFSValidRadar->channel = value;
		break;
	case 1:
		pDFSValidRadar->WLow = value;
		break;
	case 2:
		pDFSValidRadar->WHigh = value;
		break;
	case 3:
		pDFSValidRadar->W[0] = value;
		break;
	case 4:
		pDFSValidRadar->W[1] = value;
		break;
	case 5:
		pDFSValidRadar->W[2] = value;
		break;
	case 6:
		pDFSValidRadar->W[3] = value;
		break;
	case 7:
		pDFSValidRadar->W[5] = value;
		break;
	case 8:
		pDFSValidRadar->WMargin = value;
		break;
	case 9:
		pDFSValidRadar->TLow = value;
		break;
	case 10:
		pDFSValidRadar->THigh = value;
		break;
	case 11:
		pDFSValidRadar->T[0] = value;
		break;
	case 12:
		pDFSValidRadar->T[1] = value;
		break;
	case 13:
		pDFSValidRadar->T[2] = value;
		break;
	case 14:
		pDFSValidRadar->T[3] = value;
		break;
	case 15:
		pDFSValidRadar->T[4] = value;
		break;
	case 16:
		pDFSValidRadar->TMargin = value;
		break;
	}
	
	pDFSValidRadar = &NewDFSValidTable[0];
	while (pDFSValidRadar->type != NEW_DFS_END)
	{
		if (pDFSValidRadar->type & pAd->CommonCfg.MCURadarRegion)
		{
			printk("ch = %x  --- ", pDFSValidRadar->channel);
			printk("wl:wh = %d:%d  ", pDFSValidRadar->WLow, pDFSValidRadar->WHigh);
			printk("w[] = %d %d %d %d %d --- ", pDFSValidRadar->W[0], pDFSValidRadar->W[1], pDFSValidRadar->W[2], pDFSValidRadar->W[3], pDFSValidRadar->W[4]);
			printk("W Margin = %d\n", pDFSValidRadar->WMargin);
			printk("        --- Tl:Th = %d:%d  ", (unsigned int)pDFSValidRadar->TLow, (unsigned int)pDFSValidRadar->THigh);
			printk("T[] = %d %d %d %d %d --- ", (unsigned int)pDFSValidRadar->T[0], (unsigned int)pDFSValidRadar->T[1], (unsigned int)pDFSValidRadar->T[2], (unsigned int)pDFSValidRadar->T[3], (unsigned int)pDFSValidRadar->T[4]);
			printk("T Margin = %d\n", pDFSValidRadar->TMargin);
		}
		pDFSValidRadar++;
	}

}

#define NEW_DFS_BANDWITH_MONITOR_TIME (NEW_DFS_CHECK_TIME / NEW_DFS_CHECK_TIME_TASKLET)
#define NEW_DFS_CHECK_TIME			300
#define NEW_DFS_CHECK_TIME_TASKLET		2
#define NEW_DFS_WATCH_DOG_TIME		1 // note that carrier detection also need timer interrupt hook

#define DFS_SW_RADAR_DECLARE_THRES	3
#define DFS_SW_RADAR_CHECK_LOOP		50
#define DFS_SW_RADAR_CH1_SHIFT		4
#define DFS_SW_RADAR_CH2_SHIFT		6

#define DFS_SW_RADAR_CH0_ERR		8
#define DFS_SW_RADAR_PERIOD_ERR		4

#define CE_STAGGERED_RADAR_CH0_H_ERR		(DFS_SW_RADAR_CH0_ERR + 16) // the step is 16 for every 0.1 us different in width
#define CE_STAGGERED_RADAR_PERIOD_MAX		(133333 + 125000 + 117647 + 1000)
#define FCC_RADAR_PERIOD_MAX				((28560 << 1) + 1000)
#define JAP_RADAR_PERIOD_MAX				((80000 << 1) + 1000)

#define CE_STAGGERED_RADAR_DECLARE_THRES	2

void schedule_dfs_task(PRTMP_ADAPTER pAd);

static VOID NewTimerCB_Radar(PRTMP_ADAPTER pAd)
{
	UCHAR BBPR141;
	pNewDFSTable pDFS2Table;
	pNewDFSValidRadar pDFSValidRadar;
	UCHAR channel;
	UCHAR radarDeclared;
	INT i, j;
	UCHAR BBPR140;
	UCHAR BBPR156, BBPR157, BBPR158, BBPR159;
	UCHAR BBPR160, BBPR161, BBPR162, BBPR163;
	
	if (pAd->CommonCfg.McuRadarTick++ >= pAd->CommonCfg.PollTime)
	{
		radarDeclared = 0;
		pAd->CommonCfg.McuRadarTick = 0;

		{
			if (pAd->CommonCfg.use_tasklet)
			{
				pAd->CommonCfg.BUM_time++;
				if (pAd->CommonCfg.BUM_time > NEW_DFS_BANDWITH_MONITOR_TIME)
				{
					pAd->CommonCfg.BUM_time = 0;
				}
			}
			// Bandwidth usage monitor, Execute every NEW_DFS_CHECK_TIME (ms)
			if (pAd->CommonCfg.BUM_time == 0)
			{
				RTMP_IO_READ32(pAd, CH_IDLE_STA, &pAd->CommonCfg.idle_time);
				RTMP_IO_READ32(pAd, CH_BUSY_STA, &pAd->CommonCfg.busy_time);

				pAd->CommonCfg.rssi = RTMPMaxRssi(pAd, (CHAR)pAd->ApCfg.RssiSample.AvgRssi0,
							(CHAR)pAd->ApCfg.RssiSample.AvgRssi1,
							(CHAR)pAd->ApCfg.RssiSample.AvgRssi2);


				pAd->CommonCfg.ch_busy = 0;
				if (pAd->CommonCfg.rssi != 0)
				{
					// The False Detection Filter is :
					// if rssi is large than a certain value and channel bandwidth usage is more than a certain percentage
					// then any radar detected count as false detection.
					for (i = 0; i < pAd->CommonCfg.fdf_num; i++)
					{
						if (((pAd->CommonCfg.ch_busy_threshold[i] * pAd->CommonCfg.idle_time) < (pAd->CommonCfg.busy_time * (100 - pAd->CommonCfg.ch_busy_threshold[i]))) 
						 && (pAd->CommonCfg.rssi > pAd->CommonCfg.rssi_threshold[i]))
							pAd->CommonCfg.ch_busy = 1;
					}
				}
#if 0 // turn on to allow max TP
                                if ((pAd->CommonCfg.idle_time << 4) < (pAd->CommonCfg.busy_time << pAd->CommonCfg.ch_busy_idle_ratio))
                                        pAd->CommonCfg.ch_busy = 2;
                        
                                if (pAd->CommonCfg.ch_busy == 2)
                                {
                                        pAd->CommonCfg.Ch_Busy2_Num++;
                                        /*if (pAd->CommonCfg.Ch_Busy2_Num == pAd->CommonCfg.ch_busy_idle_threshold)*/
                                                /*DBGPRINT(RT_DEBUG_TRACE, ("~\n"));*/
                                }

                                else if(pAd->CommonCfg.ch_last_busy == 2 && pAd->CommonCfg.ch_busy <= 1)
                                {
                                        /*if (pAd->CommonCfg.Ch_Busy2_Num >= pAd->CommonCfg.ch_busy_idle_threshold)*/
                                                /*DBGPRINT(RT_DEBUG_TRACE, ("@\n"));*/
                                        pAd->CommonCfg.Ch_Busy2_Num = 0;
                                }         
                        
                                pAd->CommonCfg.ch_last_busy = pAd->CommonCfg.ch_busy;
#endif

			
#ifdef DFS_DEBUG
				DBGPRINT(RT_DEBUG_TRACE, ("%d:%d,%d (%d)\n", (unsigned int)pAd->CommonCfg.idle_time, (unsigned int)pAd->CommonCfg.busy_time, (unsigned int)pAd->CommonCfg.rssi, (unsigned int)pAd->CommonCfg.ch_busy));
#endif
			}

#if 0 // turn on to allow max TP
			if (pAd->CommonCfg.Ch_Busy2_Num <= pAd->CommonCfg.ch_busy_idle_threshold)
#endif
			{
				//int k, count, limit = ((pAd->CommonCfg.RadarDetect.RDDurRegion != CE)? 384: 288);
				int k, count, limit = 384;
				UCHAR BBPR127, BBPR126, LastBBPR127 = 0;
				ULONG time = 0;
				USHORT width = 0;
				UCHAR id = 0;
				UCHAR alignment = FALSE;
								
				pAd->CommonCfg.dfs_w_counter++;

				// disable debug mode to read debug port of channel 3
				BBP_IO_READ8_BY_REG_ID(pAd, 126, &BBPR126);
				BBP_IO_WRITE8_BY_REG_ID(pAd, 126, (BBPR126 & 0xfe));
				count = 0;
				for (k = 0; k < limit; k++)
				{
					BBP_IO_READ8_BY_REG_ID(pAd, 127, &BBPR127);
															
                                        if (count != 0 && alignment == FALSE)
                                        {
                                                LastBBPR127 = BBPR127;
                                                count++;
                                                if (count >= 6)
                                                        count = 0;
                                                continue;
                                        }
					switch(count)
					{
					case 0:
                                                width = 0;
                                                time = 0;
                                                id = BBPR127;
                                                if ((id > 3) && (id != 0xff))
                                                {
                                                        alignment = FALSE;
                                                        LastBBPR127 = BBPR127;
                                                }
                                                else
                                                {
                                                        if ((LastBBPR127 & 0x1f) != 0)
                                                        {
                                                                LastBBPR127 = BBPR127;
                                                                alignment = FALSE;
                                                        }
                                                        else
                                                                alignment = TRUE;
                                                }
                                                break;
					case 1:
						time |= BBPR127;
						break;
					case 2:
						time |= BBPR127 << 8;
						break;
					case 3:
						time |= (BBPR127 & 0x3f) << 16;
						width |= (BBPR127 & 0xc0) >> 6;
						break;
					case 4:
						width |= BBPR127 << 2;
						break;
					case 5:

                                                if ((alignment == TRUE) && ((BBPR127 & 0x1f) != 0))
                                                {
                                                        alignment = FALSE;
                                                        LastBBPR127 = BBPR127;
                                                        break;
                                                }

						width |= (BBPR127 & 0x7) << 10;
						

						if ((id == 0x3) && (pAd->CommonCfg.RadarDetect.RDDurRegion != CE) && (width > 500)) // FCC && Japan only
						{
							if (pAd->CommonCfg.NewDfsDebug & RADAR_DEBUG_EVENT)
							{
								printk("counter = %d  ", (unsigned int)pAd->CommonCfg.dfs_w_counter);
								printk("time = %d  ", (unsigned int)time);
								printk("width = %d\n", (unsigned int)width);
							}
                                                        if ((pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_last_idx].timestamp != time) ||
                                                                (pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_last_idx].width != width) ||
                                                                (pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_last_idx].counter != pAd->CommonCfg.dfs_w_counter))
                                                        {	// if fcc_5
                                                        pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_idx].counter = pAd->CommonCfg.dfs_w_counter;
							pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_idx].timestamp = time;
							pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_idx].width = width;
							
							if (pAd->CommonCfg.fcc_5_last_idx != pAd->CommonCfg.fcc_5_idx)
							{
								int delta;
								
								if (pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_idx].counter == pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_last_idx].counter)
								{
									delta = pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_idx].width - pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_last_idx].width;

									DBGPRINT(RT_DEBUG_WARN,("w0 = %d, timestamp = %d\n", (unsigned int)pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_last_idx].width, (unsigned int)pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_last_idx].timestamp));
									DBGPRINT(RT_DEBUG_WARN,("w1 = %d, timestamp = %d\n", (unsigned int)pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_idx].width, (unsigned int)pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_idx].timestamp));
									// debug
									//if (delta < pAd->CommonCfg.fcc_5_threshold && delta > (-1 * pAd->CommonCfg.fcc_5_threshold))
									{
										// looks like there are no long pulse false detection
										if (pAd->CommonCfg.ch_busy == 0)
										{
											printk("radar detected!!! id == %d\n", id);
											radarDeclared = 1;
										}
#ifdef DFS_DEBUG
										//else
										//	printk("FDF: long pulse\n");
										
										DBGPRINT(RT_DEBUG_INFO, ("%d:%d,%d (%d)\n", (unsigned int)pAd->CommonCfg.idle_time, (unsigned int)pAd->CommonCfg.busy_time, (unsigned int)pAd->CommonCfg.rssi, (unsigned int)pAd->CommonCfg.ch_busy));
#endif
									}
									
								}
							}

							pAd->CommonCfg.fcc_5_last_idx = pAd->CommonCfg.fcc_5_idx;
							pAd->CommonCfg.fcc_5_idx++;
							if (pAd->CommonCfg.fcc_5_idx >= NEW_DFS_FCC_5_ENT_NUM)
								pAd->CommonCfg.fcc_5_idx = 0;
							}	// end if fcc_5
						}

						if (pAd->CommonCfg.use_tasklet)
						{
							//if (id <= 0x2) // && (id >= 0)
							if (id < 0x3) // && (id >= 0)
							{
								if (time != pAd->CommonCfg.DFS_W[id][((pAd->CommonCfg.dfs_w_idx[id] == 0)? (NEW_DFS_DBG_PORT_ENT_NUM-1):(pAd->CommonCfg.dfs_w_idx[id] - 1))].timestamp)
								{
									pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.dfs_w_idx[id]].counter = pAd->CommonCfg.dfs_w_counter;
									pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.dfs_w_idx[id]].timestamp = time;
									pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.dfs_w_idx[id]].width = width;
                        	
									pAd->CommonCfg.dfs_w_last_idx[id] = pAd->CommonCfg.dfs_w_idx[id];
									pAd->CommonCfg.dfs_w_idx[id]++;
									if (pAd->CommonCfg.dfs_w_idx[id] >= NEW_DFS_DBG_PORT_ENT_NUM)
										pAd->CommonCfg.dfs_w_idx[id] = 0;
								}
							}
						}


#ifdef DFS_DEBUG
						pAd->CommonCfg.TotalEntries[id]++;						
#endif 
						break;

					}
                                        if ((id == 0xff) && (alignment == TRUE))
                                        {
                                                break;
                                        }
						
#ifdef DFS_DEBUG
					if (pAd->CommonCfg.DebugPortPrint == 1 && (k == 0))
					{
						pAd->CommonCfg.DebugPortPrint = 2;
					}

					if ((pAd->CommonCfg.DebugPortPrint == 2))
					{
						pAd->CommonCfg.DebugPort[k] = BBPR127;
					}
#endif 

					if (alignment != FALSE)
					{
                                                count++;
                                                if (count >= 6)
                                                        count = 0;
					}
				
				
				
				} // for (k = 0; k < limit; k++)
			
#ifdef DFS_DEBUG			
				if (pAd->CommonCfg.DebugPortPrint == 2)
				{
					pAd->CommonCfg.DebugPortPrint = 3;
				}
#endif


#ifdef DFS_DEBUG
				if (pAd->CommonCfg.BBP127Repeat)
				{
					for (k = 0; k < pAd->CommonCfg.BBP127Repeat; k++)
						BBP_IO_READ8_BY_REG_ID(pAd, 127, &BBPR127);
					
					pAd->CommonCfg.BBP127Repeat = 0;
				}
				else
#endif
				// read to 5 times for alignment
				{
					BBP_IO_READ8_BY_REG_ID(pAd, 127, &BBPR127);
					BBP_IO_READ8_BY_REG_ID(pAd, 127, &BBPR127);
					BBP_IO_READ8_BY_REG_ID(pAd, 127, &BBPR127);
					BBP_IO_READ8_BY_REG_ID(pAd, 127, &BBPR127);
					BBP_IO_READ8_BY_REG_ID(pAd, 127, &BBPR127);
				}
			
				
				// enable debug mode
				if (pAd->CommonCfg.dfs_w_counter & 1)
				{
					BBP_IO_WRITE8_BY_REG_ID(pAd, 126, 3);
				}
				else
				{
					BBP_IO_WRITE8_BY_REG_ID(pAd, 126, 7);
				}

				if (pAd->CommonCfg.use_tasklet)
				{
					// set hw_idx
					pAd->CommonCfg.hw_idx[0] = pAd->CommonCfg.dfs_w_idx[0];
					pAd->CommonCfg.hw_idx[1] = pAd->CommonCfg.dfs_w_idx[1];
					pAd->CommonCfg.hw_idx[2] = pAd->CommonCfg.dfs_w_idx[2];
					pAd->CommonCfg.hw_idx[3] = pAd->CommonCfg.dfs_w_idx[3];
					schedule_dfs_task(pAd);
				}

			}

		}
		
		
		// Poll Status register
		BBP_IO_READ8_BY_REG_ID(pAd, 141, &BBPR141);
		channel = BBPR141 & 0xf;

#if 0
		if (pAd->CommonCfg.DfsDebug.start == 2)
		{
			pAd->CommonCfg.DfsDebug.count++;
			if (pAd->CommonCfg.DfsDebug.count >= pAd->CommonCfg.DfsDebug.wait_time)
			{
				pAd->CommonCfg.DfsDebug.count = 0;
				RadarDebugNext(pAd);
			}
		}
#endif		
		// Hardware Detection
		if (channel)
		{
			UCHAR WMatched, TMatched;
			USHORT W, W1;
			ULONG T, T1;

			DBGPRINT(RT_DEBUG_TRACE, ("NewTimerCB_Radar, BBPR141 = 0x%x, channel bit= 0x%x\n", BBPR141, channel));
			
			if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40)
				BBPR140 = 0x80 | pAd->CommonCfg.ChEnable;
			else
				BBPR140 = 0 | pAd->CommonCfg.ChEnable;

			{
			
				for (j = 0;j < NEW_DFS_MAX_CHANNEL; j++)
				{
					if (channel & (0x1 << j))
					{

						// select Channel first
						BBP_IO_WRITE8_BY_REG_ID(pAd, 140, BBPR140 | (j << 4));
						DBGPRINT(RT_DEBUG_TRACE, ("Select Channel = 0x%x\n", BBPR140 | (j << 4)));
						BBP_IO_READ8_BY_REG_ID(pAd, 160, &BBPR160);
						BBP_IO_READ8_BY_REG_ID(pAd, 161, &BBPR161);
						BBP_IO_READ8_BY_REG_ID(pAd, 162, &BBPR162);
						BBP_IO_READ8_BY_REG_ID(pAd, 163, &BBPR163);
						//printk("... 160 ~ 163  %x %x %x %x\n", BBPR160, BBPR161, BBPR162, BBPR163);
						BBP_IO_READ8_BY_REG_ID(pAd, 156, &BBPR156);
						BBP_IO_READ8_BY_REG_ID(pAd, 157, &BBPR157);
						BBP_IO_READ8_BY_REG_ID(pAd, 158, &BBPR158);
						BBP_IO_READ8_BY_REG_ID(pAd, 159, &BBPR159);
						//printk("... 156 ~ 159  %x %x %x %x\n", BBPR156, BBPR157, BBPR158, BBPR159);


						pDFSValidRadar = &NewDFSValidTable[0];

						while (pDFSValidRadar->type != NEW_DFS_END)
						{
							if ((pDFSValidRadar->type & pAd->CommonCfg.MCURadarRegion) == 0)
							{
								pDFSValidRadar++;
								continue;
							}

							if (! (pDFSValidRadar->channel & channel))
							{
								pDFSValidRadar++;
								continue;
							}

							if (pAd->CommonCfg.MCURadarRegion == NEW_DFS_FCC)
								pDFS2Table = &NewDFSTable1[0];
							else if (pAd->CommonCfg.MCURadarRegion == NEW_DFS_EU)
							{
								pDFS2Table = &NewDFSTable1[1];
							}
							else // Japan
								pDFS2Table = &NewDFSTable1[2];

							
								
							//printk("Table type = %d, Channel bit map = 0x%x\n", pDFSValidRadar->type, pDFSValidRadar->channel);

							W = 0;
							T = 0;
							T |= BBPR156 << 8;
							T |= BBPR157;
							T |= (BBPR158 & 0x80) << 9;
			
							W |= (BBPR158 & 0xf) << 8;
							W |= BBPR159;

							if ( (pDFS2Table->entry[j].mode == 1) || (pDFS2Table->entry[j].mode == 2) || (pDFS2Table->entry[j].mode == 6) )
							{
								W1 = 0;
								T1 = 0;
								T1 |= BBPR160 << 8;
								T1 |= BBPR161;
								T1 |= (BBPR162 & 0x80) << 9;
			
								W1 |= (BBPR162 & 0xf) << 8;
								W1 |= BBPR163;
							}
							

							WMatched = 0;
							// check valid Radar
							if (pDFSValidRadar->WLow)
							{
								if ( (W > (pDFSValidRadar->WLow - pDFSValidRadar->WMargin)) && 
								     (W < (pDFSValidRadar->WHigh + pDFSValidRadar->WMargin)) )
								{
									WMatched = 1;
								}
								
							}
							else
							{
								i = 0;
								while( (i < MAX_VALID_RADAR_W) && (pDFSValidRadar->W[i] != 0) )
								{
									if ( (W > (pDFSValidRadar->W[i] - pDFSValidRadar->WMargin)) &&
									     (W < (pDFSValidRadar->W[i] + pDFSValidRadar->WMargin)) )
									{
										WMatched = 1;
										break;
									}
									
									i++;
								}
							}
					
							//printk("WMatched == %d   ", WMatched);
							TMatched = 0;
							if (pDFSValidRadar->TLow)
							{
								if ( (T > (pDFSValidRadar->TLow - pDFSValidRadar->TMargin)) && 
								     (T < (pDFSValidRadar->THigh + pDFSValidRadar->TMargin)) )
								{
									TMatched = 1;
								}
							}
							else
							{
								i = 0;
								while( (i < MAX_VALID_RADAR_T) && (pDFSValidRadar->T[i] != 0) )
								{
									if ( (T > (pDFSValidRadar->T[i] - pDFSValidRadar->TMargin)) &&
									     (T < (pDFSValidRadar->T[i] + pDFSValidRadar->TMargin)) )
									{
										TMatched = 1;
										break;
									}
									
									i++;
								}
							}
							//printk("TMatched == %d\n", TMatched);

#ifdef DFS_DEBUG
							if (pAd->CommonCfg.DfsDebug.start == 2)
							{
								if ((T > (pAd->CommonCfg.DfsDebug.T_expected - pAd->CommonCfg.DfsDebug.T_margin)) && (T < (pAd->CommonCfg.DfsDebug.T_expected + pAd->CommonCfg.DfsDebug.T_margin)))
								{
									if (pAd->CommonCfg.DfsDebug.idx < 1000)
									{
										TestResult[pAd->CommonCfg.DfsDebug.idx].hit_time ++;
									}
								}
								else
								{
									if (pAd->CommonCfg.DfsDebug.idx < 1000)
									{
										TestResult[pAd->CommonCfg.DfsDebug.idx].false_time ++;
									}
								}
							}

#endif

						if ((TMatched == 1) && (WMatched == 1) && (!pAd->CommonCfg.hw_dfs_disabled))
						{
							// found that seem all false detection are between 3000 ~ 10000
							if (pAd->CommonCfg.ch_busy == 0)
							{
								if (pAd->CommonCfg.RadarDetect.RDMode == RD_SILENCE_MODE)
								{
									printk ("%d W = %d, T= %d (%d)\n", (unsigned int)jiffies, (unsigned int)W, (unsigned int)T, (unsigned int)j);
									printk ("radar detected!!\n");
									radarDeclared = 1;
								}
								else
								{
									if (pAd->CommonCfg.RadarDetect.RDDurRegion != CE)
									{
										printk ("%d W = %d, T= %d (%d)\n", (unsigned int)jiffies, (unsigned int)W, (unsigned int)T, (unsigned int)j);
										radarDeclared = 1;
									}
									// false detection observation :
									// almost all false detection is ch0 when rssi is not too high,
									// almost all false detection of ch0 have width < 30
									else if ((j == 0) && (W < 30))
									{
#if 1
										;
#else
										// if the radar have the same width with most false detection, then re-check
										if ((jiffies == pAd->CommonCfg.re_check_jiffies[j]) && 
											(pAd->CommonCfg.re_check_Width[j] == W) && (pAd->CommonCfg.re_check_Period[j] == T))
										{
											printk ("%d W = %d, T= %d (%d)(re-check)\n", (unsigned int)jiffies, (unsigned int)W, (unsigned int)T, (unsigned int)j);
											radarDeclared = 1;
										}
										else
										{
											printk("@%d@%d@\n", (unsigned int)j, (unsigned int)W);
										}
#endif
									}
									else 
									{
										printk ("%d W = %d, T= %d (%d)\n", (unsigned int)jiffies, (unsigned int)W, (unsigned int)T, (unsigned int)j);
										radarDeclared = 1;
									}
									
								}
								
							}
#ifdef DFS_DEBUG
							else
							{
								DBGPRINT(RT_DEBUG_TRACE, ("FDF: ch %d, w %d, t %d jf %d\n", (unsigned int)j, (unsigned int)W, (unsigned int)T, (unsigned int)jiffies));
								DBGPRINT(RT_DEBUG_TRACE, ("%d:%d,%d (%d)\n", (unsigned int)pAd->CommonCfg.idle_time, (unsigned int)pAd->CommonCfg.busy_time, (unsigned int)pAd->CommonCfg.rssi, (unsigned int)pAd->CommonCfg.ch_busy));
							}
#endif


							if (j < 4)
							{
								pAd->CommonCfg.re_check_jiffies[j] = jiffies;
								pAd->CommonCfg.re_check_Width[j] = W;
								pAd->CommonCfg.re_check_Period[j] = T;
							}

							if (pAd->CommonCfg.RadarReEnable == 0)
							{
								BBP_IO_WRITE8_BY_REG_ID(pAd, 140, BBPR140);
								printk("Disable detecting : write 140 = 0x%x\n", BBPR140);
								NewRadarDetectionStop(pAd);
								
							}
						}
						else if ((TMatched == 1) && (!pAd->CommonCfg.hw_dfs_disabled))
						{
							if (pAd->CommonCfg.ch_busy == 0)
							{
								if (pAd->CommonCfg.RadarDetect.RDMode == RD_SILENCE_MODE)
								{
									printk ("%d W = %d, T= %d (%d)\n", (unsigned int)jiffies, (unsigned int)W, (unsigned int)T, (unsigned int)j);
									printk ("radar detected!!\n");
									radarDeclared = 1;
								}
								else
								{
									if (pAd->CommonCfg.RadarDetect.RDDurRegion != CE)
									{
										printk ("%d W = %d, T= %d (%d)\n", (unsigned int)jiffies, (unsigned int)W, (unsigned int)T, (unsigned int)j);
										radarDeclared = 1;
									}
									// false detection observation :
									// almost all false detection is ch0 when rssi is not too high,
									// almost all false detection of ch0 have width < 30
									else if ((j == 0) && (W < 30))
									{
#if 1
										;
#else
										// if the radar have the same width with most false detection, then re-check
										if ((jiffies == pAd->CommonCfg.re_check_jiffies[j]) && 
											(pAd->CommonCfg.re_check_Width[j] == W) && (pAd->CommonCfg.re_check_Period[j] == T))
										{
											printk ("%d W = %d, T= %d (%d)(re-check)\n", (unsigned int)jiffies, (unsigned int)W, (unsigned int)T, (unsigned int)j);
											radarDeclared = 1;
										}
										else
										{
											printk("@%d@%d@\n", j, W);
										}
#endif
									}
									else 
									{
										printk ("%d W = %d, T= %d (%d)\n", (unsigned int)jiffies, (unsigned int)W, (unsigned int)T, (unsigned int)j);
										radarDeclared = 1;
									}
									
								}
								

								
							}
#ifdef DFS_DEBUG
							else
							{
								DBGPRINT(RT_DEBUG_TRACE, ("FDF: ch %d, w %d, t %d jf %d\n", (unsigned int)j, (unsigned int)W, (unsigned int)T, (unsigned int)jiffies));
								DBGPRINT(RT_DEBUG_TRACE, ("%d:%d,%d (%d)\n", (unsigned int)pAd->CommonCfg.idle_time, (unsigned int)pAd->CommonCfg.busy_time, (unsigned int)pAd->CommonCfg.rssi, (unsigned int)pAd->CommonCfg.ch_busy));
							}
#endif
							if (j < 4)
							{
								pAd->CommonCfg.re_check_jiffies[j] = jiffies;
								pAd->CommonCfg.re_check_Width[j] = W;
								pAd->CommonCfg.re_check_Period[j] = T;
							}

						}				
								pDFSValidRadar++;						
							
						}
							
					} // if (channel & (0x1 << j))
						
				} // for (j = 0;j < NEW_DFS_MAX_CHANNEL; j++)
			}

		} // if (channel)

		if (pAd->CommonCfg.RadarReEnable == 1)
		{
			BBP_IO_WRITE8_BY_REG_ID(pAd, 141, BBPR141);
		}

#ifdef DFS_DEBUG
		if (pAd->CommonCfg.McuRadarDebug & RADAR_SIMULATE)
		{
			radarDeclared = 1;
			pAd->CommonCfg.McuRadarDebug &= ~RADAR_SIMULATE;
		}
#endif
		

		if (radarDeclared || pAd->CommonCfg.radarDeclared)
		{
			// Radar found!!!
			if (!(pAd->CommonCfg.NewDfsDebug & RADAR_DONT_SWITCH))
			{
			
				for (i=0; i<pAd->ChannelListNum; i++)
				{
					if (pAd->CommonCfg.Channel == pAd->ChannelList[i].Channel)
					{
						pAd->ChannelList[i].RemainingTimeForUse = 1800;//30 min = 1800 sec
						break;
					}
				}

				if ((pAd->CommonCfg.RadarDetect.RDDurRegion == CE) && (pAd->CommonCfg.Channel >= 116) && (pAd->CommonCfg.Channel <= 128))
					pAd->CommonCfg.RadarDetect.ChMovingTime = 605;
				else
					pAd->CommonCfg.RadarDetect.ChMovingTime = 65;
			
				if (pAd->CommonCfg.RadarDetect.RDDurRegion == JAP_W56)
				{
					for (i = 0; i < pAd->ChannelListNum ; i++)
					{
						pAd->CommonCfg.Channel = APAutoSelectChannel(pAd, FALSE);
						if ((pAd->CommonCfg.Channel >= 100) && (pAd->CommonCfg.Channel <= 140))
							break;
					}
				}
				else if (pAd->CommonCfg.RadarDetect.RDDurRegion == JAP_W53)
				{
					for (i = 0; i < pAd->ChannelListNum ; i++)
					{
						pAd->CommonCfg.Channel = APAutoSelectChannel(pAd, FALSE);
						if ((pAd->CommonCfg.Channel >= 36) && (pAd->CommonCfg.Channel <= 60))
							break;
					}
				}
				else
					pAd->CommonCfg.Channel = APAutoSelectChannel(pAd, FALSE);
				
#ifdef DOT11_N_SUPPORT
				N_ChannelCheck(pAd);
#endif // DOT11_N_SUPPORT //
				//ApSelectChannelCheck(pAd);
				if (pAd->CommonCfg.RadarDetect.RDMode != RD_SILENCE_MODE)
				{
					pAd->CommonCfg.RadarDetect.RDMode = RD_SWITCHING_MODE;
					pAd->CommonCfg.RadarDetect.CSCount = 0;
				}
				else
				{
					pAd->CommonCfg.DFSAPRestart=1;
					schedule_dfs_task(pAd);
					//APStop(pAd);
					//APStartUp(pAd);
				}
					pAd->CommonCfg.radarDeclared = 0;
				
				
			}
			else
				pAd->CommonCfg.radarDeclared = 0;
			
			// clear long pulse table
			pAd->CommonCfg.FCC_5[pAd->CommonCfg.fcc_5_idx].counter = 0;
			pAd->CommonCfg.fcc_5_idx = 0;
			pAd->CommonCfg.fcc_5_last_idx = 0;
			

		
		}
	}
	
    
}


void TimerCB(unsigned long data);

VOID NewRadarDetectionStart(
	IN PRTMP_ADAPTER pAd)
{
	pNewDFSTable pDFS2Table;
	int i;
	UCHAR BBP_R140;
	UCHAR BBP_R142, BBP_R143, BBP_R144, BBP_R145, BBP_R146;
	UCHAR BBP_R147, BBP_R148, BBP_R149, BBP_R150, BBP_R151;
	UCHAR BBP_R152, BBP_R153, BBP_R154, BBP_R155;
		
	UCHAR BBPR4;
	
	BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BBPR4);

	if ((pAd->CommonCfg.RadarDetect.RDDurRegion == CE) && (pAd->CommonCfg.Channel >= 116) && (pAd->CommonCfg.Channel <= 128))
		pAd->CommonCfg.RadarDetect.ChMovingTime = 605;
	else
		pAd->CommonCfg.RadarDetect.ChMovingTime = 65;


	if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40)
	{
		BBP_IO_WRITE8_BY_REG_ID(pAd, 92, 3);
	}
	else
	{
		BBP_IO_WRITE8_BY_REG_ID(pAd, 91, 9);
	}
	
		
	DBGPRINT(RT_DEBUG_TRACE, ("NewRadarDetectionStart\n"));
	// choose Table
	if (pAd->CommonCfg.RadarDetect.RDDurRegion == FCC)
	{
		// Set up False detection filter
		// The test power is around -4x ~ -6x dBm, 
		// the test should not test with power more than -30dBm
		
		// FCC False detection Fliter		
		// FCC test traffic load requirement is transmit a media,
		// even with minimum MCS (MCS=0), the bandwith usage is never > 50%
		// average bandwidth usage is 30%, ref power level is -36
		// with MCS == auto, the bandwidth usage is never > 10%
		// average bandwidth usage is 5% ~ 6%
		// the following parameters is based on above result
		
		pAd->CommonCfg.fdf_num = 5;
		pAd->CommonCfg.ch_busy_threshold[0] = 2;
		pAd->CommonCfg.rssi_threshold[0] = -31;
		pAd->CommonCfg.ch_busy_threshold[1] = 15;
		pAd->CommonCfg.rssi_threshold[1] = -36;
		pAd->CommonCfg.ch_busy_threshold[2] = 50;
		pAd->CommonCfg.rssi_threshold[2] = -42;
		pAd->CommonCfg.ch_busy_threshold[3] = 60;
		pAd->CommonCfg.rssi_threshold[3] = -70;
		pAd->CommonCfg.ch_busy_threshold[4] = 85;
		pAd->CommonCfg.rssi_threshold[4] = -120;

		pDFS2Table = &NewDFSTable1[0];
		
                pAd->CommonCfg.ch_busy_idle_threshold = 10;
                pAd->CommonCfg.ch_busy_idle_ratio = 4;
	}
	else if (pAd->CommonCfg.RadarDetect.RDDurRegion == CE)
	{
		// Set up False detection filter

		// CE False detection Fliter		
		// CE test traffic load requirement is 30% of max traffic load,
		// CE test tool in Sportan Lab is land-test
		// with MCS == auto, the lan-test's max throughput == 82, 82* 30% = 24.6 
		// Use 25Mbps to test
		// average bandwidth usage is 25%, max is not more than 40%
		// ref power = -39 dBm
		// the following parameters is based on above result

		pAd->CommonCfg.fdf_num = 5;
		pAd->CommonCfg.ch_busy_threshold[0] = 2;
		pAd->CommonCfg.rssi_threshold[0] = -31;
		pAd->CommonCfg.ch_busy_threshold[1] = 36;
		pAd->CommonCfg.rssi_threshold[1] = -36;
		pAd->CommonCfg.ch_busy_threshold[2] = 50;
		pAd->CommonCfg.rssi_threshold[2] = -42;
		pAd->CommonCfg.ch_busy_threshold[3] = 60;
		pAd->CommonCfg.rssi_threshold[3] = -70;
		pAd->CommonCfg.ch_busy_threshold[4] = 85;
		pAd->CommonCfg.rssi_threshold[4] = -120;
		
		pDFS2Table = &NewDFSTable1[1];

                pAd->CommonCfg.ch_busy_idle_threshold = 10;
                pAd->CommonCfg.ch_busy_idle_ratio = 1;
	}
	else // Japan
	{
		// Set up False detection filter
		
		// JAP False detection Fliter		
		// JAP test traffic load has 3 different criteria,
		// Most radar are same as FCC, but it did not require to test with the minimum data rate, 
		// so we can use MCS == auto
		// there are 2 vedio for JAP test
		// FCC vedio (Used in W56 and FCC Radar), W53 vedio
		// W53 vedio has more traffic load than FCC vedio, 
		// If using MCS == Auto, the average bandwidth usage is
		// about 10% ~ 15%, max not more than 25%
		// the following parameters is based on above result
		
		pAd->CommonCfg.fdf_num = 5;
		pAd->CommonCfg.ch_busy_threshold[0] = 2;
		pAd->CommonCfg.rssi_threshold[0] = -31;
		pAd->CommonCfg.ch_busy_threshold[1] = 30;
		pAd->CommonCfg.rssi_threshold[1] = -36;
		pAd->CommonCfg.ch_busy_threshold[2] = 50;
		pAd->CommonCfg.rssi_threshold[2] = -42;
		pAd->CommonCfg.ch_busy_threshold[3] = 60;
		pAd->CommonCfg.rssi_threshold[3] = -70;
		pAd->CommonCfg.ch_busy_threshold[4] = 85;
		pAd->CommonCfg.rssi_threshold[4] = -120;

		pDFS2Table = &NewDFSTable1[2];

                pAd->CommonCfg.ch_busy_idle_threshold = 10;
                pAd->CommonCfg.ch_busy_idle_ratio = 3;
	}
	
	pAd->CommonCfg.MCURadarRegion = pDFS2Table->type;
	DBGPRINT(RT_DEBUG_TRACE, ("........pAd->CommonCfg.MCURadarRegion = %d\n", (unsigned int)pAd->CommonCfg.MCURadarRegion));

	// delta delay
	BBP_IO_WRITE8_BY_REG_ID(pAd, 141, pAd->CommonCfg.DeltaDelay << 4);
	DBGPRINT(RT_DEBUG_TRACE, ("........Write 141 = 0x%x\n", pAd->CommonCfg.DeltaDelay << 4));
	
	if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40)
		BBP_R140 = 0x80;
	else
		BBP_R140 = 0;
	
	
	// program channel i
	for (i = 0; i < NEW_DFS_MAX_CHANNEL; i++)
	{
		if (!((1 << i) & pAd->CommonCfg.ChEnable))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("skip channel %d\n", i));
			continue;
		}
		BBP_R142 = 0;
		BBP_R142 |= (pDFS2Table->entry[i].avgLen & 0x100) >> 8;
		BBP_R142 |= (pDFS2Table->entry[i].TLow & 0x10000) >> 13;
		BBP_R142 |= (pDFS2Table->entry[i].THigh & 0x10000) >> 14;
		BBP_R142 |= (pDFS2Table->entry[i].mode & 0x7) << 4;
		
		BBP_R143 = pDFS2Table->entry[i].avgLen & 0xff;

		BBP_R144 = 0;
		BBP_R144 |= (pDFS2Table->entry[i].ELow & 0xf00) >> 4;
		BBP_R144 |= (pDFS2Table->entry[i].EHigh & 0xf00) >> 8;
		
		BBP_R145 = pDFS2Table->entry[i].ELow & 0xff;
		BBP_R146 = pDFS2Table->entry[i].EHigh & 0xff;

		BBP_R147 = (pDFS2Table->entry[i].TLow & 0xff00) >> 8;
		BBP_R148 = pDFS2Table->entry[i].TLow & 0xff;

		BBP_R149 = (pDFS2Table->entry[i].THigh & 0xff00) >> 8;
		BBP_R150 = pDFS2Table->entry[i].THigh & 0xff;

		BBP_R151 = pDFS2Table->entry[i].EpsilonT & 0xff;
		
		BBP_R152 = 0;
		BBP_R152 |= (pDFS2Table->entry[i].WLow & 0xf00) >> 4;
		BBP_R152 |= (pDFS2Table->entry[i].WHigh & 0xf00) >> 8;
		
		BBP_R153 = pDFS2Table->entry[i].WLow & 0xff;

		BBP_R154 = pDFS2Table->entry[i].WHigh & 0xff;
		
		BBP_R155 = pDFS2Table->entry[i].EpsilonW & 0xff;
		
		// select channel
		BBP_IO_WRITE8_BY_REG_ID(pAd, 140, BBP_R140 | (i << 4));
		DBGPRINT(RT_DEBUG_TRACE, ("........select channel, Write 140 = 0x%x\n", BBP_R140 | (i << 4)));
		
		// start programing
		BBP_IO_WRITE8_BY_REG_ID(pAd, 142, BBP_R142);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 143, BBP_R143);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 144, BBP_R144);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 145, BBP_R145);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 146, BBP_R146);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 147, BBP_R147);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 148, BBP_R148);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 149, BBP_R149);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 150, BBP_R150);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 151, BBP_R151);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 152, BBP_R152);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 153, BBP_R153);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 154, BBP_R154);
		BBP_IO_WRITE8_BY_REG_ID(pAd, 155, BBP_R155);
		DBGPRINT(RT_DEBUG_TRACE, ("........142 = 0x%x ", BBP_R142));
		DBGPRINT(RT_DEBUG_TRACE, ("143 = %x ", BBP_R143));
		DBGPRINT(RT_DEBUG_TRACE, ("144 = %x ", BBP_R144));
		DBGPRINT(RT_DEBUG_TRACE, ("145 = %x ", BBP_R145));
		DBGPRINT(RT_DEBUG_TRACE, ("146 = %x ", BBP_R146));
		DBGPRINT(RT_DEBUG_TRACE, ("147 = %x ", BBP_R147));
		DBGPRINT(RT_DEBUG_TRACE, ("148 = %x ", BBP_R148));
		DBGPRINT(RT_DEBUG_TRACE, ("149 = %x\n........", BBP_R149));
		DBGPRINT(RT_DEBUG_TRACE, ("150 = %x ", BBP_R150));
		DBGPRINT(RT_DEBUG_TRACE, ("151 = %x ", BBP_R151));
		DBGPRINT(RT_DEBUG_TRACE, ("152 = %x ", BBP_R152));
		DBGPRINT(RT_DEBUG_TRACE, ("153 = %x ", BBP_R153));
		DBGPRINT(RT_DEBUG_TRACE, ("154 = %x ", BBP_R154));
		DBGPRINT(RT_DEBUG_TRACE, ("155 = %x\n", BBP_R155));
	}
	
	// enable debug mode
	BBP_IO_WRITE8_BY_REG_ID(pAd, 126, 3);
	
	// Enable Radar detection
	BBP_IO_WRITE8_BY_REG_ID(pAd, 140, BBP_R140 | pAd->CommonCfg.ChEnable);
	DBGPRINT(RT_DEBUG_TRACE, ("........Write BBP 140 = 0x%x\n", BBP_R140 | pAd->CommonCfg.ChEnable));

	// Enable Bandwidth usage monitor
	{
		ULONG Value;
		RTMP_IO_READ32(pAd, CH_TIME_CFG, &Value);
		RTMP_IO_WRITE32(pAd, CH_TIME_CFG, Value | 0x1f);
	}

	// Enable Tasklet anyway
	pAd->CommonCfg.use_tasklet = 1;
	
	// init CE - detector
	if ((pAd->CommonCfg.RadarDetect.RDDurRegion == CE) || (pAd->CommonCfg.use_tasklet == 1))
	{
		int j, k;
		for (k = 0; k < NEW_DFS_MAX_CHANNEL; k++)
		{
			for (j = 0; j < NEW_DFS_DBG_PORT_ENT_NUM; j++)
			{
				pAd->CommonCfg.DFS_W[k][j].start_idx = 0xffff;
			}
		}
		pAd->CommonCfg.use_tasklet = 1;
		pAd->CommonCfg.dfs_check_loop = DFS_SW_RADAR_CHECK_LOOP;
		pAd->CommonCfg.dfs_width_diff_ch1_Shift = DFS_SW_RADAR_CH1_SHIFT;
		pAd->CommonCfg.dfs_width_diff_ch2_Shift = DFS_SW_RADAR_CH2_SHIFT;
		pAd->CommonCfg.dfs_width_ch0_err_L = DFS_SW_RADAR_CH0_ERR;
		if (pAd->CommonCfg.RadarDetect.RDDurRegion == CE)
			pAd->CommonCfg.dfs_period_err = (DFS_SW_RADAR_PERIOD_ERR << 2);
		else
			pAd->CommonCfg.dfs_period_err = DFS_SW_RADAR_PERIOD_ERR;
			

		if (pAd->CommonCfg.RadarDetect.RDDurRegion == CE)
		{
			pAd->CommonCfg.dfs_width_ch0_err_H = CE_STAGGERED_RADAR_CH0_H_ERR;
			pAd->CommonCfg.dfs_declare_thres = CE_STAGGERED_RADAR_DECLARE_THRES;
			pAd->CommonCfg.dfs_max_period = CE_STAGGERED_RADAR_PERIOD_MAX;
		}
		else
		{
			pAd->CommonCfg.dfs_declare_thres = DFS_SW_RADAR_DECLARE_THRES;
			if (pAd->CommonCfg.RadarDetect.RDDurRegion == FCC)
				pAd->CommonCfg.dfs_max_period = FCC_RADAR_PERIOD_MAX;
			else if (pAd->CommonCfg.RadarDetect.RDDurRegion == JAP)
				pAd->CommonCfg.dfs_max_period = JAP_RADAR_PERIOD_MAX;
		}

		for (k = 0; k < NEW_DFS_MAX_CHANNEL; k++)
		{
			pAd->CommonCfg.sw_idx[k] = NEW_DFS_DBG_PORT_ENT_NUM - 1;
			pAd->CommonCfg.hw_idx[k] = 0;
		}
		
	}

	if (pAd->CommonCfg.use_tasklet)
		pAd->CommonCfg.PollTime = NEW_DFS_CHECK_TIME_TASKLET;
	else
		pAd->CommonCfg.PollTime = NEW_DFS_CHECK_TIME;


	request_tmr_service(NEW_DFS_WATCH_DOG_TIME, &TimerCB, (void *)pAd);

}


VOID NewRadarDetectionStop(
	IN PRTMP_ADAPTER pAd)
{
	DBGPRINT(RT_DEBUG_TRACE, ("NewRadarDetectionStop\n"));
	unregister_tmr_service();
	BBP_IO_WRITE8_BY_REG_ID(pAd, 140, 0x0);
	
	if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40)
	{
		BBP_IO_WRITE8_BY_REG_ID(pAd, 92, 0);
	}
	else
	{
		BBP_IO_WRITE8_BY_REG_ID(pAd, 91, 4);
	}
}


#define PERIOD_MATCH(a, b, c)			((a >= b)? ((a-b) <= c):((b-a) <= c))
#define ENTRY_PLUS(a, b, c)				(((a+b) < c)? (a+b) : (a+b-c))
#define ENTRY_MINUS(a, b, c)			((a >= b)? (a - b) : (a+c-b))
#define MAX_PROCESS_ENTRY 				16

#define IS_FCC_RADAR_1(HT_BW, T)			(((HT_BW)? ((T > 57000) && (T < 57240)) : (T > 28500) && (T < 28620)))

// the debug port have timestamp 22 digit, the max number is 0x3fffff, each unit is 25ns for 40Mhz mode and 50ns for 20Mhz mode
// so a round of timestamp is about 25 * 0x3fffff / 1000 = 104857us (about 100ms) or
// 50 * 0x3fffff / 1000 = 209715us (about 200ms) in 20Mhz mode
// 3ms = 3000,000 ns / 25ns = 120000 -- a unit 
// 0x3fffff/120000 = 34.9 ~= 35
// CE Staggered radar check
// At beginning, the goal is to detect staggered radar, now, we also detect regular radar with this function.


int SWRadarCheck(
	IN PRTMP_ADAPTER pAd, USHORT id)
{
	int i, j, k, start_idx, end_idx;
	pNewDFSDebugPort pCurrent, p1, pEnd;
	ULONG period;
	int radar_detected = 0, regular_radar = 0;
	ULONG PRF1 = 0, PRF2 = 0, PRF3 = 0;
	USHORT	minDiff, maxDiff, widthsum;
	UCHAR	Radar2PRF=0, Radar3PRF=0;
	USHORT	Total, SwIdxPlus = ENTRY_PLUS(pAd->CommonCfg.sw_idx[id], 1, NEW_DFS_DBG_PORT_ENT_NUM);
	UCHAR	CounterToCheck;
	
	

	if (SwIdxPlus == pAd->CommonCfg.hw_idx[id])
		return 0; // no entry to process
	
	// process how many entries?? total NEW_DFS_DBG_PORT_ENT_NUM
	if (pAd->CommonCfg.hw_idx[id] > SwIdxPlus)
		Total = pAd->CommonCfg.hw_idx[id] - SwIdxPlus;
	else
		Total = pAd->CommonCfg.hw_idx[id] + NEW_DFS_DBG_PORT_ENT_NUM - SwIdxPlus;
	
	if (Total > NEW_DFS_DBG_PORT_ENT_NUM)
		pAd->CommonCfg.pr_idx[id] = ENTRY_PLUS(pAd->CommonCfg.sw_idx[id], MAX_PROCESS_ENTRY, NEW_DFS_DBG_PORT_ENT_NUM);
	else
		pAd->CommonCfg.pr_idx[id] = ENTRY_PLUS(pAd->CommonCfg.sw_idx[id], Total, NEW_DFS_DBG_PORT_ENT_NUM);
	
	
	start_idx = ENTRY_PLUS(pAd->CommonCfg.pr_idx[id], 1, NEW_DFS_DBG_PORT_ENT_NUM);
	end_idx = pAd->CommonCfg.pr_idx[id];
	
	pEnd = &pAd->CommonCfg.DFS_W[id][end_idx];
	//printk("start_idx = %d, end_idx=%d, counter=%d\n", start_idx, end_idx, pEnd->counter);
	
	//if (pAd->CommonCfg.dfs_w_counter != pEnd->counter)
	//	return 0;
	
	if (start_idx > end_idx)
		end_idx += NEW_DFS_DBG_PORT_ENT_NUM;
	
	
	pAd->CommonCfg.sw_idx[id] = pAd->CommonCfg.pr_idx[id];
	

	

	// FCC && Japan

	if (pAd->CommonCfg.RadarDetect.RDDurRegion != CE)
	{
		ULONG minPeriod = (3000 << 1);
		// Calculate how many counters to check
		// if pAd->CommonCfg.PollTime is 1ms, a round of timestamp is 107 for 20Mhz, 53 for 40Mhz
		// if pAd->CommonCfg.PollTime is 2ms, a round of timestamp is 71 for 20Mhz, 35 for 40Mhz
		// if pAd->CommonCfg.PollTime is 3ms, a round of timestamp is 53 for 20Mhz, 27 for 40Mhz
		// if pAd->CommonCfg.PollTime is 4ms, a round of timestamp is 43 for 20Mhz, 21 for 40Mhz
		// the max period to check for 40Mhz for FCC is 28650 * 2
		// the max period to check for 40Mhz for Japan is 80000 * 2
		// 0x40000 = 4194304 / 57129 = 73.xxx
		// 0x40000 = 4194304 / 160000 = 26.2144
		// 53/73 < 1 (1+1)
		// 53/26.2144 = 2.02... (2+1)
		// 27/26.2144 = 1.02... (1+1)
		// 20M should use the same value as 40Mhz mode


		
		if (pAd->CommonCfg.RadarDetect.RDDurRegion == FCC)
		{
			CounterToCheck = 1+1; 
		}
		else // if (pAd->CommonCfg.RadarDetect.RDDurRegion == JAP)
		{
			if (pAd->CommonCfg.PollTime <= 2)
				CounterToCheck = 2+1;
			else
				CounterToCheck = 1+1;
		}
		

		
		// First Loop for FCC/JAP
		for (i = end_idx; i > start_idx; i--)
		{
			pCurrent = &pAd->CommonCfg.DFS_W[id][i & NEW_DFS_DBG_PORT_MASK];
				
			// we only handle entries has same counter with the last one
			if (pCurrent->counter != pEnd->counter)
				break;
		
			pCurrent->start_idx = 0xffff;

			// calculate if any two pulse become a valid period, add it in period table,
			for (j = i - 1; j > start_idx; j--)
			{
				p1 = &pAd->CommonCfg.DFS_W[id][j & NEW_DFS_DBG_PORT_MASK];
				
				// check period, must within max period
				if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40)
				{
					if (p1->counter + CounterToCheck < pCurrent->counter)
						break;
            	
					widthsum = p1->width + pCurrent->width;
					if (id == 0)
					{
						if (widthsum < 600)
							pAd->CommonCfg.dfs_width_diff = pAd->CommonCfg.dfs_width_ch0_err_L;
						else
							pAd->CommonCfg.dfs_width_diff = widthsum >> pAd->CommonCfg.dfs_width_diff_ch2_Shift;
					}
					else if (id == 1)
						pAd->CommonCfg.dfs_width_diff = widthsum >> pAd->CommonCfg.dfs_width_diff_ch1_Shift;
					else if (id == 2)
						pAd->CommonCfg.dfs_width_diff = widthsum >> pAd->CommonCfg.dfs_width_diff_ch2_Shift;
					
					if ( (pAd->CommonCfg.RadarDetect.RDMode == RD_SILENCE_MODE) ||
						 (PERIOD_MATCH(p1->width, pCurrent->width, pAd->CommonCfg.dfs_width_diff)) )
					{
						if (p1->timestamp >= pCurrent->timestamp)
							period = 0x400000 + pCurrent->timestamp - p1->timestamp;
						else
							period = pCurrent->timestamp - p1->timestamp;
						
						if ((period >= (minPeriod - 2)) && (period <= pAd->CommonCfg.dfs_max_period))
						{
            	
							// add in period table
							pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].idx = (i & NEW_DFS_DBG_PORT_MASK);
							pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].width = pCurrent->width;
							pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].idx2 = (j & NEW_DFS_DBG_PORT_MASK);
							pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].width2 = p1->width;
							pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].period = period;
            	
            	
							if (pCurrent->start_idx == 0xffff)
								pCurrent->start_idx = pAd->CommonCfg.dfs_t_idx[id];
							pCurrent->end_idx = pAd->CommonCfg.dfs_t_idx[id];
							
							pAd->CommonCfg.dfs_t_idx[id]++;
							if (pAd->CommonCfg.dfs_t_idx[id] >= NEW_DFS_MPERIOD_ENT_NUM)
								pAd->CommonCfg.dfs_t_idx[id] = 0;
						}
						else if (period > pAd->CommonCfg.dfs_max_period)
							break;
					}
					
				}
				else
				{
					if (p1->counter + CounterToCheck < pCurrent->counter)
						break;
					
					widthsum = p1->width + pCurrent->width;
					if (id == 0)
					{
						if (widthsum < 600)
							pAd->CommonCfg.dfs_width_diff = pAd->CommonCfg.dfs_width_ch0_err_L;
						else
							pAd->CommonCfg.dfs_width_diff = widthsum >> pAd->CommonCfg.dfs_width_diff_ch2_Shift;
					}
					else if (id == 1)
						pAd->CommonCfg.dfs_width_diff = widthsum >> 4;
					else if (id == 2)
						pAd->CommonCfg.dfs_width_diff = widthsum >> 6;
            	
					if ( (pAd->CommonCfg.RadarDetect.RDMode == RD_SILENCE_MODE) || 
						 (PERIOD_MATCH(p1->width, pCurrent->width, pAd->CommonCfg.dfs_width_diff)) )
            	
					{
						if (p1->timestamp >= pCurrent->timestamp)
							period = 0x400000 + pCurrent->timestamp - p1->timestamp;
						else
							period = pCurrent->timestamp - p1->timestamp;
            	
						if ((period >= ((minPeriod >> 1) - 2)) && (period <= (pAd->CommonCfg.dfs_max_period >> 1)))
						{
							// add in period table
							pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].idx = (i & NEW_DFS_DBG_PORT_MASK);
							pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].width = pCurrent->width;
							pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].idx2 = (j & NEW_DFS_DBG_PORT_MASK);
							pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].width2 = p1->width;
							pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].period = period;
							
							if (pCurrent->start_idx == 0xffff)
								pCurrent->start_idx = pAd->CommonCfg.dfs_t_idx[id];
							pCurrent->end_idx = pAd->CommonCfg.dfs_t_idx[id];
							
							pAd->CommonCfg.dfs_t_idx[id]++;
							if (pAd->CommonCfg.dfs_t_idx[id] >= NEW_DFS_MPERIOD_ENT_NUM)
								pAd->CommonCfg.dfs_t_idx[id] = 0;
						}
						else if (period > (pAd->CommonCfg.dfs_max_period >> 1))
							break;
					}
				}

			} // for (j = i - 1; j > start_idx; j--)

		} // for (i = end_idx; i > start_idx; i--)


		// Second Loop for FCC/JAP
		for (i = end_idx; i > start_idx; i--)
		{
			
			pCurrent = &pAd->CommonCfg.DFS_W[id][i & NEW_DFS_DBG_PORT_MASK];
				
			// we only handle entries has same counter with the last one
			if (pCurrent->counter != pEnd->counter)
				break;
			if (pCurrent->start_idx != 0xffff)
			{
				//pNewDFSDebugPort	p2, p3, p4, p5, p6;
				pNewDFSDebugPort	p2, p3;
				pNewDFSMPeriod pCE_T;
				ULONG idx[10], T[10];

				for (idx[0] = pCurrent->start_idx; idx[0] <= pCurrent->end_idx; idx[0]++)
				{

					pCE_T = &pAd->CommonCfg.DFS_T[id][idx[0]];
				
					p2 = &pAd->CommonCfg.DFS_W[id][pCE_T->idx2];
				
					if (p2->start_idx == 0xffff)
						continue;
				
					T[0] = pCE_T->period;


					for (idx[1] = p2->start_idx; idx[1] <= p2->end_idx; idx[1]++)
					{
						
						pCE_T = &pAd->CommonCfg.DFS_T[id][idx[1]];
					
						p3 = &pAd->CommonCfg.DFS_W[id][pCE_T->idx2];

						if (idx[0] == idx[1])
							continue;
						
						if (p3->start_idx == 0xffff)
							continue;
					


						T[1] = pCE_T->period;
						
						
						if ( PERIOD_MATCH(T[0], T[1], pAd->CommonCfg.dfs_period_err))
						{
							if (id <= 2) // && (id >= 0)
							{

								//if (((pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40) && (T[1] > minPeriod)) ||
								//	((pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_20) && (T[1] > (minPeriod >> 1))) )
								{
									unsigned int loop, PeriodMatched = 0, idx1;
									for (loop = 1; loop < pAd->CommonCfg.dfs_check_loop; loop++)
									{
										idx1 = (idx[1] >= loop)? (idx[1] - loop): (NEW_DFS_MPERIOD_ENT_NUM + idx[1] - loop);
										if (PERIOD_MATCH(pAd->CommonCfg.DFS_T[id][idx1].period, T[1], pAd->CommonCfg.dfs_period_err))
										{
#ifdef DFS_DEBUG
											if (PeriodMatched < 5)
											{
												pAd->CommonCfg.CounterStored[PeriodMatched] = pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx1].idx].counter;
												pAd->CommonCfg.CounterStored2[PeriodMatched] = loop;
												pAd->CommonCfg.CounterStored3 = idx[1];
											}
#endif
											//printk("%d %d\n", loop, pAd->CommonCfg.DFS_T[id][idx[1]-loop].period);
											PeriodMatched++;
										}
										
									}
								
								
									if (PeriodMatched > pAd->CommonCfg.dfs_declare_thres)
									{
#ifdef DFS_DEBUG
										if (PeriodMatched == 3)
										{
											pAd->CommonCfg.T_Matched_3++;
											//printk("counter=%d %d %d\n", pAd->CommonCfg.CounterStored[0], pAd->CommonCfg.CounterStored[1], pAd->CommonCfg.CounterStored[2]);
											//printk("idx[1]=%d, loop =%d %d %d\n", pAd->CommonCfg.CounterStored3, pAd->CommonCfg.CounterStored2[0], pAd->CommonCfg.CounterStored2[1], pAd->CommonCfg.CounterStored2[2]);
										}
										else if (PeriodMatched == 4)
										{
											pAd->CommonCfg.T_Matched_4++;
											//printk("counter=%d %d %d %d\n", pAd->CommonCfg.CounterStored[0], pAd->CommonCfg.CounterStored[1], pAd->CommonCfg.CounterStored[2], pAd->CommonCfg.CounterStored[3]);
											//printk("idx[1]=%d, loop =%d %d %d %d\n", pAd->CommonCfg.CounterStored3, pAd->CommonCfg.CounterStored2[0], pAd->CommonCfg.CounterStored2[1], pAd->CommonCfg.CounterStored2[2], pAd->CommonCfg.CounterStored2[3]);
										}
										else
										{
											pAd->CommonCfg.T_Matched_5++;
											//printk("counter=%d %d %d %d %d\n", pAd->CommonCfg.CounterStored[0], pAd->CommonCfg.CounterStored[1], pAd->CommonCfg.CounterStored[2], pAd->CommonCfg.CounterStored[3], pAd->CommonCfg.CounterStored[4]);
											//printk("idx[1]=%d, loop =%d %d %d %d %d\n", pAd->CommonCfg.CounterStored3, pAd->CommonCfg.CounterStored2[0], pAd->CommonCfg.CounterStored2[1], pAd->CommonCfg.CounterStored2[2], pAd->CommonCfg.CounterStored2[3], pAd->CommonCfg.CounterStored2[4]);
										}
                                    	
										pAd->CommonCfg.DebugPortPrint = 1;
									
#endif

										{
											pNewDFSValidRadar pDFSValidRadar;
											ULONG T1 = (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40)? (T[1]>>1) : T[1];
											
											pDFSValidRadar = &NewDFSValidTable[0];
											
                    					
											while (pDFSValidRadar->type != NEW_DFS_END)
											{
												if ((pDFSValidRadar->type & pAd->CommonCfg.MCURadarRegion) == 0)
												{
													pDFSValidRadar++;
													continue;
												}
												
												if (pDFSValidRadar->TLow)
												{
													if ( (T1 > (pDFSValidRadar->TLow - pDFSValidRadar->TMargin)) && 
													     (T1 < (pDFSValidRadar->THigh + pDFSValidRadar->TMargin)) )
													{
														radar_detected = 1;
													}
												}
												else
												{
													k = 0;
													while( (k < MAX_VALID_RADAR_T) && (pDFSValidRadar->T[k] != 0) )
													{
														if ( (T1 > (pDFSValidRadar->T[k] - pDFSValidRadar->TMargin)) &&
														     (T1 < (pDFSValidRadar->T[k] + pDFSValidRadar->TMargin)) )
														{
															radar_detected = 1;
															break;
														}
														
														k++;
													}
												}												
												
												pDFSValidRadar++;
											}

											if (radar_detected == 1)
											{
												printk("Radar Detected, W=%d, T=%d (%d), period matched=%d\n", (unsigned int)pCE_T->width, (unsigned int)T1, (unsigned int)id, PeriodMatched);
	                                    	
												return radar_detected;
											}
											else
											{
												printk("False report, W=%d, T=%d (%d), period matched=%d\n", (unsigned int)pCE_T->width, (unsigned int)T1, (unsigned int)id, PeriodMatched);
											}

										}

										
									}
#ifdef DFS_DEBUG
									else if (PeriodMatched == 2)
									{
										pAd->CommonCfg.T_Matched_2++;
									}
#endif
								
								
								}
								
							} // if (id <= 2) // && (id >= 0)
							
						}
						// increase FCC-1 detection on the central channel

						if (id == 2)
						{
							if (IS_FCC_RADAR_1((pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40), T[1]))
							{
									int loop, idx1, PeriodMatched_fcc1 = 0;
									for (loop = 1; loop < pAd->CommonCfg.dfs_check_loop; loop++)
									{
										idx1 = (idx[1] >= loop)? (idx[1] - loop): (NEW_DFS_MPERIOD_ENT_NUM + idx[1] - loop);
										if ( IS_FCC_RADAR_1((pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40), pAd->CommonCfg.DFS_T[id][idx1].period) )
											PeriodMatched_fcc1++;
									}
									
									if (PeriodMatched_fcc1 > 3)
									{
										printk("PeriodMatched_fcc1 = %d\n", PeriodMatched_fcc1);
										radar_detected = 1;
										return radar_detected;
									}
									
							}
							
						}
						
#if 0 // FCC and Japan seems don't need so much check
						for (idx[2] = p3->start_idx; idx[2] <= p3->end_idx; idx[2]++)
						{
							
							pCE_T = &pAd->CommonCfg.DFS_T[id][idx[2]];
							
							p4 = &pAd->CommonCfg.DFS_W[id][pCE_T->idx2];
							
							if (idx[0] == idx[2])
								continue;
                        	
							if (p4->start_idx == 0xffff)
								continue;
							
							T[2] = pCE_T->period;
													
							for (idx[3] = p4->start_idx; idx[3] <= p4->end_idx; idx[3]++)
							{
								
								pCE_T = &pAd->CommonCfg.DFS_T[id][idx[3]];
				            	
								p5 = &pAd->CommonCfg.DFS_W[id][pCE_T->idx2];
				            	
								if ((idx[0] == idx[3]) || (idx[1] == idx[3]))
									continue;
                            	
								if (p5->start_idx == 0xffff)
									continue;
				            	
								T[3] = pCE_T->period;
                            	
								
								for (idx[4] = p5->start_idx; idx[4] <= p5->end_idx; idx[4]++)
								{
									
									pCE_T = &pAd->CommonCfg.DFS_T[id][idx[4]];
				                	
									p6 = &pAd->CommonCfg.DFS_W[id][pCE_T->idx2];
				                	
									if ((idx[0] == idx[4]) || (idx[1] == idx[4]))
										continue;
                                	
									if (p6->start_idx == 0xffff)
										continue;
				                	                                	
									T[4] = pCE_T->period;


									// regular radar with a PRF
									if ((PERIOD_MATCH(T[0], T[1], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[2], T[3], pAd->CommonCfg.dfs_period_err)) &&
										(PERIOD_MATCH(T[4], T[3], pAd->CommonCfg.dfs_period_err)))
									{
										if (pAd->CommonCfg.ch_busy == 0)
										{
											radar_detected = 1;
											//printk("*** T=%d, w=%d (%d) <1>\n", pCE_T->period, pCE_T->width, id);
										}
										else
											printk("FDF: regular radar, T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);
										
										regular_radar = 1;
									}
                                	
									// regular Radar miss a pulse
									// (A+A), A, A, A, A / (A+A)
									if ((PERIOD_MATCH(T[0], T[1]+T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[2], T[3], pAd->CommonCfg.dfs_period_err)) &&
										( (PERIOD_MATCH(T[4], T[3], pAd->CommonCfg.dfs_period_err)) || (PERIOD_MATCH(T[0], T[4], pAd->CommonCfg.dfs_period_err)) ))
									{
										if (pAd->CommonCfg.ch_busy == 0)
										{
											radar_detected = 1;
											//printk("*** T=%d, w=%d (%d) <BAAAC>\n", pCE_T->period, pCE_T->width, id);
										}
										else
											printk("FDF: regular radar 1:T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);
                                	
										regular_radar = 1;
									}
									// A, A+A, A, A, A / (A+A)
									if ((PERIOD_MATCH(T[1], T[0]+T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[0], T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[2], T[3], pAd->CommonCfg.dfs_period_err)) &&
										( (PERIOD_MATCH(T[4], T[3], pAd->CommonCfg.dfs_period_err)) || (PERIOD_MATCH(T[1], T[4], pAd->CommonCfg.dfs_period_err))  ) )
									{
										if (pAd->CommonCfg.ch_busy == 0)
										{
											radar_detected = 1;
											//printk("*** T=%d, w=%d (%d) <ABAAC>\n", pCE_T->period, pCE_T->width, id);
										}
										else
											printk("FDF: regular radar 2:T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);
										
										regular_radar = 1;
									}
                                	
									// A, A, A+A, A, A / (A+A)
									if ((PERIOD_MATCH(T[2], T[1]+T[0], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[0], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[3], pAd->CommonCfg.dfs_period_err)) &&
										 ( (PERIOD_MATCH(T[4], T[3], pAd->CommonCfg.dfs_period_err)) || (PERIOD_MATCH(T[2], T[4], pAd->CommonCfg.dfs_period_err)) ))
									{
										if (pAd->CommonCfg.ch_busy == 0)
										{
											radar_detected = 1;
											//printk("*** T=%d, w=%d (%d) <AABAC>\n", pCE_T->period, pCE_T->width, id);
										}
										else
											printk("FDF: regular radar: 3T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);
										
										regular_radar = 1;
									}
								
									if (regular_radar)
									{
										if (id <= 2) // && (id >= 0)
										{
											// make sure regular radar have closed widths in ch0
											if ( (PERIOD_MATCH(pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width, pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx2].width, pAd->CommonCfg.dfs_width_ch0_err_L)) &&
												 (PERIOD_MATCH(pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width, pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[1]].idx2].width, pAd->CommonCfg.dfs_width_ch0_err_L)) &&
												 (PERIOD_MATCH(pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width, pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[2]].idx2].width, pAd->CommonCfg.dfs_width_ch0_err_L)) &&
												 (PERIOD_MATCH(pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width, pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[3]].idx2].width, pAd->CommonCfg.dfs_width_ch0_err_L)) &&
												 (PERIOD_MATCH(pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width, pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[4]].idx2].width, pAd->CommonCfg.dfs_width_ch0_err_L)) )
											{
												printk("*** T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);

#ifdef DFS_DEBUG
												pAd->CommonCfg.DebugPortPrint = 1;
                                    	
										// Roger debug, Log CE_DebugCh0 & CE_TCh0
										{
											for (k = 0; k < NEW_DFS_DBG_PORT_ENT_NUM; k++)
											{
												pAd->CommonCfg.CE_DebugCh0[k].width = pAd->CommonCfg.DFS_W[0][k].width;
												pAd->CommonCfg.CE_DebugCh0[k].counter = pAd->CommonCfg.DFS_W[0][k].counter;
												pAd->CommonCfg.CE_DebugCh0[k].timestamp = pAd->CommonCfg.DFS_W[0][k].timestamp;
												pAd->CommonCfg.CE_DebugCh0[k].start_idx = pAd->CommonCfg.DFS_W[0][k].start_idx;
												pAd->CommonCfg.CE_DebugCh0[k].end_idx = pAd->CommonCfg.DFS_W[0][k].end_idx;
											}
											
											for (k = 0; k < NEW_DFS_MPERIOD_ENT_NUM; k++)
											{
												pAd->CommonCfg.CE_TCh0[k].idx = pAd->CommonCfg.DFS_T[0][k].idx;
												pAd->CommonCfg.CE_TCh0[k].width = pAd->CommonCfg.DFS_T[0][k].width;
												pAd->CommonCfg.CE_TCh0[k].idx2 = pAd->CommonCfg.DFS_T[0][k].idx2;
												pAd->CommonCfg.CE_TCh0[k].width2 = pAd->CommonCfg.DFS_T[0][k].width2;
												pAd->CommonCfg.CE_TCh0[k].period = pAd->CommonCfg.DFS_T[0][k].period;
											}
											
										}

#endif
												return radar_detected;
											}
											else
											{
												radar_detected = 0;
												//printk("width miss \n");
											}
                                	
										}
										else
										{
											printk("*** T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);
										
											return radar_detected;
										}
									}
								}
							}
						}
#endif // #if 0 // FCC and Japan seems don't need so much check


					} // for (idx[1] = p2->start_idx; idx[1] <= p2->end_idx; idx[1]++)
				} // for (idx[0] = pCurrent->start_idx; idx[0] <= pCurrent->end_idx; idx[0]++)
			} // if (pCurrent->start_idx != 0xffff)
		} // for (i = end_idx; i > start_idx; i--)
		
		return radar_detected;
	}

	// CE have staggered radar	
	
	// Calculate how many counters to check
	// if pAd->CommonCfg.PollTime is 1ms, a round of timestamp is 107 for 20Mhz, 53 for 40Mhz
	// if pAd->CommonCfg.PollTime is 2ms, a round of timestamp is 71 for 20Mhz, 35 for 40Mhz
	// if pAd->CommonCfg.PollTime is 3ms, a round of timestamp is 53 for 20Mhz, 27 for 40Mhz
	// if pAd->CommonCfg.PollTime is 4ms, a round of timestamp is 43 for 20Mhz, 21 for 40Mhz
	// if pAd->CommonCfg.PollTime is 8ms, a round of timestamp is ?? for 20Mhz, 12 for 40Mhz
	// the max period to check for 40Mhz is 133333 + 125000 + 117647 = 375980
	// 0x40000 = 4194304 / 375980 = 11.1556
	// 53/11.1556 = 4.75...
	// 35/11.1556 = 3.1374, (4+1) is safe, (3+1) to save CPU power, but may lost some data
	// 27/11.1556 = 2.42, (3+1) is OK
	// 21/11.1556 = 1.88, (2+1) is OK
	// 20M should use the same value as 40Mhz mode
	if (pAd->CommonCfg.PollTime == 1)
		CounterToCheck = 5+1;
	else if (pAd->CommonCfg.PollTime == 2)
		CounterToCheck = 4+1;
	else if (pAd->CommonCfg.PollTime == 3)
		CounterToCheck = 3+1;
	else if (pAd->CommonCfg.PollTime <= 8)
		CounterToCheck = 2+1;
	else
		CounterToCheck = 1+1;

	// First Loop for CE
	for (i = end_idx; i > start_idx; i--)
	{
		pCurrent = &pAd->CommonCfg.DFS_W[id][i & NEW_DFS_DBG_PORT_MASK];
				
		// we only handle entries has same counter with the last one
		if (pCurrent->counter != pEnd->counter)
			break;
		
		pCurrent->start_idx = 0xffff;

		// calculate if any two pulse become a valid period, add it in period table,
		for (j = i - 1; j > start_idx; j--)
		{
			p1 = &pAd->CommonCfg.DFS_W[id][j & NEW_DFS_DBG_PORT_MASK];
			

			// check period, must within 16666 ~ 66666
			if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40)
			{
				if (p1->counter + CounterToCheck < pCurrent->counter)
						break;

				widthsum = p1->width + pCurrent->width;
				if (id == 0)
				{
					if (((p1->width > 310) && (pCurrent->width < 300)) || ((pCurrent->width > 310) && ((p1->width < 300))) )
						continue;
					if (widthsum < 620)
						pAd->CommonCfg.dfs_width_diff = pAd->CommonCfg.dfs_width_ch0_err_H;
					else
						pAd->CommonCfg.dfs_width_diff = pAd->CommonCfg.dfs_width_ch0_err_L;
					
				}
				else if (id == 1)
					pAd->CommonCfg.dfs_width_diff = widthsum >> pAd->CommonCfg.dfs_width_diff_ch1_Shift;
				else if (id == 2)
					pAd->CommonCfg.dfs_width_diff = widthsum >> pAd->CommonCfg.dfs_width_diff_ch2_Shift;
				
				if ( (pAd->CommonCfg.RadarDetect.RDMode == RD_SILENCE_MODE) ||
					 (PERIOD_MATCH(p1->width, pCurrent->width, pAd->CommonCfg.dfs_width_diff)) )
				{
					if (p1->timestamp >= pCurrent->timestamp)
						period = 0x400000 + pCurrent->timestamp - p1->timestamp;
					else
						period = pCurrent->timestamp - p1->timestamp;
					
					//if ((period >= (33333 - 20)) && (period <= (133333 + 20)))
					if ((period >= (10000 - 2)) && (period <= pAd->CommonCfg.dfs_max_period))
					{

						// add in period table
						pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].idx = (i & NEW_DFS_DBG_PORT_MASK);
						pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].width = pCurrent->width;
						pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].idx2 = (j & NEW_DFS_DBG_PORT_MASK);
						pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].width2 = p1->width;
						pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].period = period;
        
        
						if (pCurrent->start_idx == 0xffff)
							pCurrent->start_idx = pAd->CommonCfg.dfs_t_idx[id];
						pCurrent->end_idx = pAd->CommonCfg.dfs_t_idx[id];
						
						pAd->CommonCfg.dfs_t_idx[id]++;
						if (pAd->CommonCfg.dfs_t_idx[id] >= NEW_DFS_MPERIOD_ENT_NUM)
							pAd->CommonCfg.dfs_t_idx[id] = 0;
					}
					else if (period > pAd->CommonCfg.dfs_max_period) // to allow miss a pulse
						break;
				}
				
			}
			else
			{
				if (p1->counter + CounterToCheck < pCurrent->counter)
					break;
				
				widthsum = p1->width + pCurrent->width;
				if (id == 0)
				{
					if (((p1->width > 300) && (pCurrent->width < 300)) || ((pCurrent->width > 300) && ((p1->width < 300))) )
						continue;
					if (widthsum < 620)
						pAd->CommonCfg.dfs_width_diff = pAd->CommonCfg.dfs_width_ch0_err_H;
					else
						pAd->CommonCfg.dfs_width_diff = pAd->CommonCfg.dfs_width_ch0_err_L;
				}
				else if (id == 1)
					pAd->CommonCfg.dfs_width_diff = widthsum >> 4;
				else if (id == 2)
					pAd->CommonCfg.dfs_width_diff = widthsum >> 6;

				if ( (pAd->CommonCfg.RadarDetect.RDMode == RD_SILENCE_MODE) || 
					 (PERIOD_MATCH(p1->width, pCurrent->width, pAd->CommonCfg.dfs_width_diff)) )

				{
					if (p1->timestamp >= pCurrent->timestamp)
						period = 0x400000 + pCurrent->timestamp - p1->timestamp;
					else
						period = pCurrent->timestamp - p1->timestamp;

					if ((period >= (5000 - 2)) && (period <= (pAd->CommonCfg.dfs_max_period >> 1)))
					{
						// add in period table
						pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].idx = (i & NEW_DFS_DBG_PORT_MASK);
						pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].width = pCurrent->width;
						pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].idx2 = (j & NEW_DFS_DBG_PORT_MASK);
						pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].width2 = p1->width;
						pAd->CommonCfg.DFS_T[id][pAd->CommonCfg.dfs_t_idx[id]].period = period;
						
						if (pCurrent->start_idx == 0xffff)
							pCurrent->start_idx = pAd->CommonCfg.dfs_t_idx[id];
						pCurrent->end_idx = pAd->CommonCfg.dfs_t_idx[id];
						
						pAd->CommonCfg.dfs_t_idx[id]++;
						if (pAd->CommonCfg.dfs_t_idx[id] >= NEW_DFS_MPERIOD_ENT_NUM)
							pAd->CommonCfg.dfs_t_idx[id] = 0;
					}
					else if (period > (pAd->CommonCfg.dfs_max_period >> 1))
						break;
				}
			}
			
		} // for (j = i - 1; j > start_idx; j--)
	}

	// Second Loop for CE
	for (i = end_idx; i > start_idx; i--)
	{
		pCurrent = &pAd->CommonCfg.DFS_W[id][i & NEW_DFS_DBG_PORT_MASK];
				
		// we only handle entries has same counter with the last one
		if (pCurrent->counter != pEnd->counter)
			break;
		
		// Check Staggered radar
		if (pCurrent->start_idx != 0xffff)
		{
			pNewDFSDebugPort	p2, p3, p4, p5, p6, p7;
			pNewDFSMPeriod pCE_T;
			ULONG idx[10], T[10];
			
			//printk("pCurrent=%d, idx=%d~%d\n", pCurrent->timestamp, pCurrent->start_idx, pCurrent->end_idx);

			for (idx[0] = pCurrent->start_idx; idx[0] <= pCurrent->end_idx; idx[0]++)
			{
				pCE_T = &pAd->CommonCfg.DFS_T[id][idx[0]];
				
				p2 = &pAd->CommonCfg.DFS_W[id][pCE_T->idx2];
				
				//printk("idx[0]= %d, idx=%d p2=%d, idx=%d~%d\n", idx[0], pCE_T->idx2, p2->timestamp, p2->start_idx, p2->end_idx);
				
				if (p2->start_idx == 0xffff)
					continue;
				
				T[0] = pCE_T->period;


				for (idx[1] = p2->start_idx; idx[1] <= p2->end_idx; idx[1]++)
				{
					
					pCE_T = &pAd->CommonCfg.DFS_T[id][idx[1]];
					
					p3 = &pAd->CommonCfg.DFS_W[id][pCE_T->idx2];
					
					//printk("p3=%d, idx=%d~%d\n", p3->timestamp, p3->start_idx, p3->end_idx);

					if (idx[0] == idx[1])
						continue;
						
					if (p3->start_idx == 0xffff)
						continue;
					


					T[1] = pCE_T->period;

		
					if (PERIOD_MATCH(T[0], T[1], pAd->CommonCfg.dfs_period_err))
					{
						if (id <= 2) // && (id >= 0)
						{

							
							if (((pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_40) && (T[1] > 66666)) ||
								((pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_20) && (T[1] > 33333)) )
							{
								unsigned int loop, PeriodMatched = 0, idx1;
								
								for (loop = 1; loop < pAd->CommonCfg.dfs_check_loop; loop++)
								{
									idx1 = (idx[1] >= loop)? (idx[1] - loop): (NEW_DFS_MPERIOD_ENT_NUM + idx[1] - loop);
									if (PERIOD_MATCH(pAd->CommonCfg.DFS_T[id][idx1].period, T[1], pAd->CommonCfg.dfs_period_err))
									{
#ifdef DFS_DEBUG
										if (PeriodMatched < 5)
										{
											pAd->CommonCfg.CounterStored[PeriodMatched] = pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx1].idx].counter;
											pAd->CommonCfg.CounterStored2[PeriodMatched] = loop;
											pAd->CommonCfg.CounterStored3 = idx[1];
										}
#endif
										//printk("%d %d\n", loop, pAd->CommonCfg.DFS_T[id][idx[1]-loop].period);
										PeriodMatched++;
									}
									
								}
								
								
								if (PeriodMatched > pAd->CommonCfg.dfs_declare_thres)
								{
#ifdef DFS_DEBUG
									if (PeriodMatched == 3)
									{
										pAd->CommonCfg.T_Matched_3++;
										//printk("counter=%d %d %d\n", pAd->CommonCfg.CounterStored[0], pAd->CommonCfg.CounterStored[1], pAd->CommonCfg.CounterStored[2]);
										//printk("idx[1]=%d, loop =%d %d %d\n", pAd->CommonCfg.CounterStored3, pAd->CommonCfg.CounterStored2[0], pAd->CommonCfg.CounterStored2[1], pAd->CommonCfg.CounterStored2[2]);
									}
									else if (PeriodMatched == 4)
									{
										pAd->CommonCfg.T_Matched_4++;
										//printk("counter=%d %d %d %d\n", pAd->CommonCfg.CounterStored[0], pAd->CommonCfg.CounterStored[1], pAd->CommonCfg.CounterStored[2], pAd->CommonCfg.CounterStored[3]);
										//printk("idx[1]=%d, loop =%d %d %d %d\n", pAd->CommonCfg.CounterStored3, pAd->CommonCfg.CounterStored2[0], pAd->CommonCfg.CounterStored2[1], pAd->CommonCfg.CounterStored2[2], pAd->CommonCfg.CounterStored2[3]);
									}
									else
									{
										pAd->CommonCfg.T_Matched_5++;
										//printk("counter=%d %d %d %d %d\n", pAd->CommonCfg.CounterStored[0], pAd->CommonCfg.CounterStored[1], pAd->CommonCfg.CounterStored[2], pAd->CommonCfg.CounterStored[3], pAd->CommonCfg.CounterStored[4]);
										//printk("idx[1]=%d, loop =%d %d %d %d %d\n", pAd->CommonCfg.CounterStored3, pAd->CommonCfg.CounterStored2[0], pAd->CommonCfg.CounterStored2[1], pAd->CommonCfg.CounterStored2[2], pAd->CommonCfg.CounterStored2[3], pAd->CommonCfg.CounterStored2[4]);
									}

									pAd->CommonCfg.DebugPortPrint = 1;
#endif
									printk("Radar Detected(CE), W=%d, T=%d (%d), period matched=%d\n", (unsigned int)pCE_T->width, (unsigned int)T[1], (unsigned int)id, PeriodMatched);
									

									radar_detected = 1;
									return radar_detected;
								}
#ifdef DFS_DEBUG
								else if (PeriodMatched == 2)
								{
									pAd->CommonCfg.T_Matched_2++;
									//printk("counter=%d %d\n", pAd->CommonCfg.CounterStored[0], pAd->CommonCfg.CounterStored[1]);
									//printk("idx[1]=%d, loop =%d %d\n", pAd->CommonCfg.CounterStored3, pAd->CommonCfg.CounterStored2[0], pAd->CommonCfg.CounterStored2[1]);
								}
#endif
								
								
							}
						}
						
					}

					


					if (pAd->CommonCfg.ce_staggered_check)
					{

						for (idx[2] = p3->start_idx; idx[2] <= p3->end_idx; idx[2]++)
						{
							pCE_T = &pAd->CommonCfg.DFS_T[id][idx[2]];
							
							p4 = &pAd->CommonCfg.DFS_W[id][pCE_T->idx2];
							
							//printk("p4=%d, idx=%d~%d\n", p4->timestamp, p4->start_idx, p4->end_idx);
							
							if (idx[0] == idx[2])
								continue;
                    	
							if (p4->start_idx == 0xffff)
								continue;
							
							T[2] = pCE_T->period;
													
							for (idx[3] = p4->start_idx; idx[3] <= p4->end_idx; idx[3]++)
							{
								pCE_T = &pAd->CommonCfg.DFS_T[id][idx[3]];
				    	
								p5 = &pAd->CommonCfg.DFS_W[id][pCE_T->idx2];
				    	
								if ((idx[0] == idx[3]) || (idx[1] == idx[3]))
									continue;
                    	
								if (p5->start_idx == 0xffff)
									continue;
				    	
								T[3] = pCE_T->period;
                    	
								
								for (idx[4] = p5->start_idx; idx[4] <= p5->end_idx; idx[4]++)
								{
									pCE_T = &pAd->CommonCfg.DFS_T[id][idx[4]];
				    	
									p6 = &pAd->CommonCfg.DFS_W[id][pCE_T->idx2];
				    	
									if ((idx[0] == idx[4]) || (idx[1] == idx[4]))
										continue;
                    	
									if (p6->start_idx == 0xffff)
										continue;
				    	
									T[4] = pCE_T->period;
                    	
									// regular radar with a PRF
									if ((PERIOD_MATCH(T[0], T[1], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[2], T[3], pAd->CommonCfg.dfs_period_err)) &&
										(PERIOD_MATCH(T[4], T[3], pAd->CommonCfg.dfs_period_err)))
									{
										if (pAd->CommonCfg.ch_busy == 0)
										{
											radar_detected = 1;
											//printk("*** T=%d, w=%d (%d) <1>\n", pCE_T->period, pCE_T->width, id);
										}
										else
											printk("FDF: regular radar, T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);
										
										regular_radar = 1;
									}
                    	
									// regular Radar miss a pulse
									// (A+A), A, A, A, A / (A+A)
									if ((PERIOD_MATCH(T[0], T[1]+T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[2], T[3], pAd->CommonCfg.dfs_period_err)) &&
										( (PERIOD_MATCH(T[4], T[3], pAd->CommonCfg.dfs_period_err)) || (PERIOD_MATCH(T[0], T[4], pAd->CommonCfg.dfs_period_err)) ))
									{
										if (pAd->CommonCfg.ch_busy == 0)
										{
											radar_detected = 1;
											//printk("*** T=%d, w=%d (%d) <BAAAC>\n", pCE_T->period, pCE_T->width, id);
										}
										else
											printk("FDF: regular radar 1:T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);
                    	
										regular_radar = 1;
									}
									// A, A+A, A, A, A / (A+A)
									if ((PERIOD_MATCH(T[1], T[0]+T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[0], T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[2], T[3], pAd->CommonCfg.dfs_period_err)) &&
										( (PERIOD_MATCH(T[4], T[3], pAd->CommonCfg.dfs_period_err)) || (PERIOD_MATCH(T[1], T[4], pAd->CommonCfg.dfs_period_err))  ) )
									{
										if (pAd->CommonCfg.ch_busy == 0)
										{
											radar_detected = 1;
											//printk("*** T=%d, w=%d (%d) <ABAAC>\n", pCE_T->period, pCE_T->width, id);
										}
										else
											printk("FDF: regular radar 2:T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);
										
										regular_radar = 1;
									}
                    	
									// A, A, A+A, A, A / (A+A)
									if ((PERIOD_MATCH(T[2], T[1]+T[0], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[0], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[3], pAd->CommonCfg.dfs_period_err)) &&
										 ( (PERIOD_MATCH(T[4], T[3], pAd->CommonCfg.dfs_period_err)) || (PERIOD_MATCH(T[2], T[4], pAd->CommonCfg.dfs_period_err)) ))
									{
										if (pAd->CommonCfg.ch_busy == 0)
										{
											radar_detected = 1;
											//printk("*** T=%d, w=%d (%d) <AABAC>\n", pCE_T->period, pCE_T->width, id);
										}
										else
											printk("FDF: regular radar: 3T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);
										
										regular_radar = 1;
									}
									
									if (regular_radar)
									{
                    	
										//printk("%d %d  ", pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width,  pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx2].width);
										//printk("%d %d  ", pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width,  pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[1]].idx2].width);
										//printk("%d %d  ", pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width,  pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[2]].idx2].width);
										//printk("%d %d  ", pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width,  pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[3]].idx2].width);
										//printk("%d %d\n", pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width,  pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[4]].idx2].width);
                    	
										if (id <= 2) // && (id >= 0)
										{
											// make sure regular radar have closed widths in ch0
											if ( (PERIOD_MATCH(pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width, pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx2].width, pAd->CommonCfg.dfs_width_ch0_err_L)) &&
												 (PERIOD_MATCH(pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width, pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[1]].idx2].width, pAd->CommonCfg.dfs_width_ch0_err_L)) &&
												 (PERIOD_MATCH(pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width, pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[2]].idx2].width, pAd->CommonCfg.dfs_width_ch0_err_L)) &&
												 (PERIOD_MATCH(pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width, pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[3]].idx2].width, pAd->CommonCfg.dfs_width_ch0_err_L)) &&
												 (PERIOD_MATCH(pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[0]].idx].width, pAd->CommonCfg.DFS_W[id][pAd->CommonCfg.DFS_T[id][idx[4]].idx2].width, pAd->CommonCfg.dfs_width_ch0_err_L)) )
											{
												printk("*** T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);
                    	
#ifdef DFS_DEBUG
												pAd->CommonCfg.DebugPortPrint = 1;
                        	
										// Roger debug, Log CE_DebugCh0 & CE_TCh0
										{
											for (k = 0; k < NEW_DFS_DBG_PORT_ENT_NUM; k++)
											{
												pAd->CommonCfg.CE_DebugCh0[k].width = pAd->CommonCfg.DFS_W[0][k].width;
												pAd->CommonCfg.CE_DebugCh0[k].counter = pAd->CommonCfg.DFS_W[0][k].counter;
												pAd->CommonCfg.CE_DebugCh0[k].timestamp = pAd->CommonCfg.DFS_W[0][k].timestamp;
												pAd->CommonCfg.CE_DebugCh0[k].start_idx = pAd->CommonCfg.DFS_W[0][k].start_idx;
												pAd->CommonCfg.CE_DebugCh0[k].end_idx = pAd->CommonCfg.DFS_W[0][k].end_idx;
											}
											
											for (k = 0; k < NEW_DFS_MPERIOD_ENT_NUM; k++)
											{
												pAd->CommonCfg.CE_TCh0[k].idx = pAd->CommonCfg.DFS_T[0][k].idx;
												pAd->CommonCfg.CE_TCh0[k].width = pAd->CommonCfg.DFS_T[0][k].width;
												pAd->CommonCfg.CE_TCh0[k].idx2 = pAd->CommonCfg.DFS_T[0][k].idx2;
												pAd->CommonCfg.CE_TCh0[k].width2 = pAd->CommonCfg.DFS_T[0][k].width2;
												pAd->CommonCfg.CE_TCh0[k].period = pAd->CommonCfg.DFS_T[0][k].period;
											}
											
										}
                        	
#endif
												return radar_detected;
											}
											else
											{
												radar_detected = 0;
												//printk("width miss \n");
											}
                        	
										}
										else
										{
											printk("*** T=%d, w=%d (%d)\n", (unsigned int)pCE_T->period, (unsigned int)pCE_T->width, (unsigned int)id);
										
											return radar_detected;
										}
                        	
                        	
									}
									
                        	
                        	
                        	
									for (idx[5] = p6->start_idx; idx[5] <= p6->end_idx; idx[5]++)
									{
										
										pCE_T = &pAd->CommonCfg.DFS_T[id][idx[5]];
				        	
										p7 = &pAd->CommonCfg.DFS_W[id][pCE_T->idx2];
				        	
										if ((idx[0] == idx[5]) || (idx[1] == idx[5]))
											continue;
                        	
										if (p7->start_idx == 0xffff)
											continue;
														
										T[5] = pCE_T->period;
										
										// Check Staggered radar with 3 PRFs
										if ((PERIOD_MATCH(T[0], T[3], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[4], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[2], T[5], pAd->CommonCfg.dfs_period_err)))
										{
                        	
											PRF1 = 40000000 / T[0];
											PRF2 = 40000000 / T[1];
											PRF3 = 40000000 / T[2];
											
                        	
											if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_20)
											{
												PRF1 = PRF1 >> 1;
												PRF2 = PRF2 >> 1;
												PRF3 = PRF3 >> 1;
											}
                        	
											Radar3PRF = 1;
                        	
										}
                        	
										// Check Staggered radar with 2 PRFs
										if ( (PERIOD_MATCH(T[0], T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[3], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[2], T[4], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[3], T[5], pAd->CommonCfg.dfs_period_err)) )
										{
                        	
											PRF1 = 40000000 / T[0];
											PRF2 = 40000000 / T[1];
                        	
											if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_20)
											{
												PRF1 = PRF1 >> 1;
												PRF2 = PRF2 >> 1;
											}
                        	
											Radar2PRF = 1;
											
										}
										
										// Check 2 PRF staggered radar that miss a pulse
										// (A+B), A , B, A, B, A 
										if ( (PERIOD_MATCH((T[2]+T[1]), T[0], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[3], pAd->CommonCfg.dfs_period_err)) && 
											 (PERIOD_MATCH(T[2], T[4], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[5], T[3], pAd->CommonCfg.dfs_period_err)) )
										{
                        	
											PRF1 = 40000000 / T[1];
											PRF2 = 40000000 / T[2];
                        	
											if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_20)
											{
												PRF1 = PRF1 >> 1;
												PRF2 = PRF2 >> 1;
											}
											Radar2PRF = 2;
										}
										
										// A, (B+A), B, A, B, A 
										if ( (PERIOD_MATCH((T[0]+T[2]), T[1], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[0], T[3], pAd->CommonCfg.dfs_period_err)) && 
											 (PERIOD_MATCH(T[2], T[4], pAd->CommonCfg.dfs_period_err)) &&  (PERIOD_MATCH(T[5], T[3], pAd->CommonCfg.dfs_period_err))  )
										{
											PRF1 = 40000000 / T[0];
											PRF2 = 40000000 / T[2];
                        	
											if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_20)
											{
												PRF1 = PRF1 >> 1;
												PRF2 = PRF2 >> 1;
											}
											Radar2PRF = 3;
										}
										
                        	
										// Check 3 PRF staggered radar that miss a pulse
										// (A+B), C, A, B, C, A 
										if ( (PERIOD_MATCH((T[2]+T[3]), T[0], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[4], pAd->CommonCfg.dfs_period_err))
											&& (PERIOD_MATCH(T[2], T[5], pAd->CommonCfg.dfs_period_err)) )
										{
                        	
											PRF1 = 40000000 / T[2];
											PRF2 = 40000000 / T[3];
											PRF3 = 40000000 / T[4];
											
                        	
											if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_20)
											{
												PRF1 = PRF1 >> 1;
												PRF2 = PRF2 >> 1;
												PRF3 = PRF3 >> 1;
											}
											Radar3PRF = 2;
										}
										
										// A, (B+C), A, B, C, A
										if ( (PERIOD_MATCH((T[4]+T[3]), T[1], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[0], T[2], pAd->CommonCfg.dfs_period_err))
											&& (PERIOD_MATCH(T[2], T[5], pAd->CommonCfg.dfs_period_err)) )
										{
											PRF1 = 40000000 / T[2];
											PRF2 = 40000000 / T[3];
											PRF3 = 40000000 / T[4];
											
                        	
											if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_20)
											{
												PRF1 = PRF1 >> 1;
												PRF2 = PRF2 >> 1;
												PRF3 = PRF3 >> 1;
											}
											Radar3PRF = 3;
										}
										
										// A, B, (C+A), B, C, A
										if ( (PERIOD_MATCH((T[0]+T[4]), T[2], pAd->CommonCfg.dfs_period_err)) && (PERIOD_MATCH(T[1], T[3], pAd->CommonCfg.dfs_period_err))
											&& (PERIOD_MATCH(T[0], T[5], pAd->CommonCfg.dfs_period_err)) )
										{
											PRF1 = 40000000 / T[0];
											PRF2 = 40000000 / T[1];
											PRF3 = 40000000 / T[4];
											
                        	
											if (pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth  == BW_20)
											{
												PRF1 = PRF1 >> 1;
												PRF2 = PRF2 >> 1;
												PRF3 = PRF3 >> 1;
											}
											Radar3PRF = 4;
										}
										
                        	
										if (Radar2PRF)
										{
											if ( (PRF1+PRF2) > (400 * 2))
											{
												if ( (PRF1 > 398) && (PRF2 > 398)  && (PRF1 < 1202) && (PRF2 < 1202) )
												{
													minDiff = 80 - 2;
													maxDiff = 400 + 2;
												}
												else
													continue;
											}
											else
											{
												if ( (PRF1 < 402) && (PRF2 < 402) && (PRF1 > 298) && (PRF2 > 298))
												{
													minDiff = 20 - 2;
													maxDiff = 50 + 2;
												}
												else
													continue;
											}
                        	
											if (!PERIOD_MATCH(PRF1, PRF2, minDiff) && (PERIOD_MATCH(PRF1, PRF2, maxDiff)) )
											{
												// Staggered radar with 2 PRF
												if (pAd->CommonCfg.ch_busy == 0)
												{
#ifdef DFS_DEBUG
													pAd->CommonCfg.DebugPortPrint = 1;
                            	
										// Roger debug, Log CE_DebugCh0 & CE_TCh0
										{
											for (k = 0; k < NEW_DFS_DBG_PORT_ENT_NUM; k++)
											{
												pAd->CommonCfg.CE_DebugCh0[k].width = pAd->CommonCfg.DFS_W[0][k].width;
												pAd->CommonCfg.CE_DebugCh0[k].counter = pAd->CommonCfg.DFS_W[0][k].counter;
												pAd->CommonCfg.CE_DebugCh0[k].timestamp = pAd->CommonCfg.DFS_W[0][k].timestamp;
												pAd->CommonCfg.CE_DebugCh0[k].start_idx = pAd->CommonCfg.DFS_W[0][k].start_idx;
												pAd->CommonCfg.CE_DebugCh0[k].end_idx = pAd->CommonCfg.DFS_W[0][k].end_idx;
											}
											
											for (k = 0; k < NEW_DFS_MPERIOD_ENT_NUM; k++)
											{
												pAd->CommonCfg.CE_TCh0[k].idx = pAd->CommonCfg.DFS_T[0][k].idx;
												pAd->CommonCfg.CE_TCh0[k].width = pAd->CommonCfg.DFS_T[0][k].width;
												pAd->CommonCfg.CE_TCh0[k].idx2 = pAd->CommonCfg.DFS_T[0][k].idx2;
												pAd->CommonCfg.CE_TCh0[k].width2 = pAd->CommonCfg.DFS_T[0][k].width2;
												pAd->CommonCfg.CE_TCh0[k].period = pAd->CommonCfg.DFS_T[0][k].period;
											}
											
										}

#endif
													radar_detected = 1;
													printk("%d %d, w=%d %d\n", (unsigned int)T[1], (unsigned int)T[0], (unsigned int)pCE_T->width, (unsigned int)pCE_T->width2);
													printk("*2* (%d),  PRF = %d, %d <%d>\n", (unsigned int)id, (unsigned int)PRF1, (unsigned int)PRF2, (unsigned int)Radar2PRF);
												}
												else
													printk("FDF: *** 2 PRF ***(%d),  PRF = %d, %d <%d>\n", (unsigned int)id, (unsigned int)PRF1, (unsigned int)PRF2, (unsigned int)Radar2PRF);
                            	
												return radar_detected;
											}
										}
										
                            	
										if (Radar3PRF)
										{
											if ( ((PRF1+PRF2+PRF3)) > (400 * 3) )
											{
												if ( (PRF1 > 398) && (PRF2 > 398) && (PRF3 > 398) && (PRF1 < 1202) && (PRF2 < 1202) && (PRF3 < 1202))
												{
													minDiff = 80 - 2;
													maxDiff = 400 + 2;
												}
												else
													continue;
											}
											else
											{
												if ( (PRF1 < 402) && (PRF2 < 402) && (PRF3 < 402) && (PRF1 > 298) && (PRF2 > 298) && (PRF3 > 298))
												{
													minDiff = 20 - 2;
													maxDiff = 50 + 2;
												}
												else
													continue;
											}
											
											
											if ( ( !PERIOD_MATCH(PRF1, PRF2, minDiff) && (PERIOD_MATCH(PRF1, PRF2, maxDiff)) ) &&
												 ( !PERIOD_MATCH(PRF3, PRF2, minDiff) && (PERIOD_MATCH(PRF3, PRF2, maxDiff)) )
												)
											{
												// Staggered radar with 3 PRF
												if (pAd->CommonCfg.ch_busy == 0)
												{
#ifdef DFS_DEBUG
													pAd->CommonCfg.DebugPortPrint = 1;
                            	
										// Roger debug, Log CE_DebugCh0 & CE_TCh0
										{
											for (k = 0; k < NEW_DFS_DBG_PORT_ENT_NUM; k++)
											{
												pAd->CommonCfg.CE_DebugCh0[k].width = pAd->CommonCfg.DFS_W[0][k].width;
												pAd->CommonCfg.CE_DebugCh0[k].counter = pAd->CommonCfg.DFS_W[0][k].counter;
												pAd->CommonCfg.CE_DebugCh0[k].timestamp = pAd->CommonCfg.DFS_W[0][k].timestamp;
												pAd->CommonCfg.CE_DebugCh0[k].start_idx = pAd->CommonCfg.DFS_W[0][k].start_idx;
												pAd->CommonCfg.CE_DebugCh0[k].end_idx = pAd->CommonCfg.DFS_W[0][k].end_idx;
											}
											
											for (k = 0; k < NEW_DFS_MPERIOD_ENT_NUM; k++)
											{
												pAd->CommonCfg.CE_TCh0[k].idx = pAd->CommonCfg.DFS_T[0][k].idx;
												pAd->CommonCfg.CE_TCh0[k].width = pAd->CommonCfg.DFS_T[0][k].width;
												pAd->CommonCfg.CE_TCh0[k].idx2 = pAd->CommonCfg.DFS_T[0][k].idx2;
												pAd->CommonCfg.CE_TCh0[k].width2 = pAd->CommonCfg.DFS_T[0][k].width2;
												pAd->CommonCfg.CE_TCh0[k].period = pAd->CommonCfg.DFS_T[0][k].period;
											}
											
										}
                            	
#endif
													radar_detected = 1;
													printk("%d,%d,%d, w=%d %d\n", (unsigned int)T[2], (unsigned int)T[1], (unsigned int)T[0], (unsigned int)pCE_T->width, (unsigned int)pCE_T->width2);
													printk("*3* (%d), PRF = %d, %d, %d <%d>\n", (unsigned int)id, (unsigned int)PRF1, (unsigned int)PRF2, (unsigned int)PRF3, (unsigned int)Radar3PRF);
												}
												else
													printk("FDF: *** 3 PRF ***(%d), PRF = %d, %d, %d <%d>\n", (unsigned int)id, (unsigned int)PRF1, (unsigned int)PRF2, (unsigned int)PRF3, (unsigned int)Radar3PRF);
													
                            	
												return radar_detected;
											}
										}
										
                            	
									} // for (idx[5] = p6->start_idx; idx[5] <= p6->end_idx; idx[5]++)
                            	
								} //for (idx[4] = p5->start_idx; idx[4] <= p5->end_idx; idx[4]++)

							} // for (idx[3] = p4->start_idx; idx[3] <= p4->end_idx; idx[3]++)

						} // for (idx[2] = p3->start_idx; idx[2] <= p3->end_idx; idx[2]++)

					} // if (pAd->CommonCfg.ce_staggered_check)


				} // for (idx[1] = p2->start_idx; idx[1] <= p2->end_idx; idx[1]++)

			} // for (idx[0] = pCurrent->start_idx; idx[0] <= pCurrent->end_idx; idx[0]++)

		}
		
	} // for (i = end_idx; i < start_idx; i--)
	
	
	return radar_detected;
	
}

#endif // DFS_SUPPORT //

#endif // NEW_DFS //

#endif // #if defined(DFS_SUPPORT) || defined(CARRIER_DETECTION_SUPPORT)

#endif // CONFIG_AP_SUPPORT //
