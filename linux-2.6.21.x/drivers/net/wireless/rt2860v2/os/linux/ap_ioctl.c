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
	ap_ioctl.c

    Abstract:
    IOCTL related subroutines

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/

#include "rt_config.h"
#include "rtmp.h"
#include <linux/wireless.h>


struct iw_priv_args ap_privtab[] = {
{ RTPRIV_IOCTL_SET, 
// 1024 --> 1024 + 512
/* larger size specific to allow 64 ACL MAC addresses to be set up all at once. */
  IW_PRIV_TYPE_CHAR | 1536, 0,
  "set"},  
{ RTPRIV_IOCTL_SHOW,
  IW_PRIV_TYPE_CHAR | 1024, 0,
  "show"},
{ RTPRIV_IOCTL_GSITESURVEY,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 ,
  "get_site_survey"}, 
{ RTPRIV_IOCTL_GET_MAC_TABLE,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 ,
  "get_mac_table"}, 
//#ifdef DBG		// by Jiahao for ASUS ATE
{ RTPRIV_IOCTL_BBP,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "bbp"},
{ RTPRIV_IOCTL_MAC,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "mac"},
{ RTPRIV_IOCTL_E2P,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "e2p"},
#ifdef RTMP_RF_RW_SUPPORT
{ RTPRIV_IOCTL_RF,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "rf"},
#endif // RTMP_RF_RW_SUPPORT //
//#endif // DBG //	// by Jiahao for ASUS ATE

#ifdef WSC_AP_SUPPORT
{ RTPRIV_IOCTL_WSC_PROFILE,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 ,
  "get_wsc_profile"},
#endif // WSC_AP_SUPPORT //
{ RTPRIV_IOCTL_QUERY_BATABLE,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 ,
  "get_ba_table"},
{ RTPRIV_IOCTL_STATISTICS,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "stat"}
};


#ifdef CONFIG_APSTA_MIXED_SUPPORT
const struct iw_handler_def rt28xx_ap_iw_handler_def =
{
#define	N(a)	(sizeof (a) / sizeof (a[0]))
	.private_args	= (struct iw_priv_args *) ap_privtab,
	.num_private_args	= N(ap_privtab),
#if IW_HANDLER_VERSION >= 7
	.get_wireless_stats = rt28xx_get_wireless_stats,
#endif 
};
#endif // CONFIG_APSTA_MIXED_SUPPORT //


INT rt28xx_ap_ioctl(
	IN	struct net_device	*net_dev, 
	IN	OUT	struct ifreq	*rq, 
	IN	INT					cmd)
{
	RTMP_ADAPTER	*pAd = NULL;
    struct iwreq	*wrq = (struct iwreq *) rq;
    INT				Status = NDIS_STATUS_SUCCESS;
    USHORT			subcmd, index;
	POS_COOKIE		pObj;
	UCHAR			apidx=0;

	pAd = net_dev->priv;
	pObj = (POS_COOKIE) pAd->OS_Cookie;

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

    //+ patch for SnapGear Request even the interface is down
    if(cmd== SIOCGIWNAME){
	    DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCGIWNAME\n"));
#ifdef RTMP_MAC_PCI
	    strcpy(wrq->u.name, "RT2860 SoftAP");
#endif // RTMP_MAC_PCI //
	    return Status;
    }//- patch for SnapGear
	
    if((net_dev->priv_flags == INT_MAIN) && !RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
    {
#ifdef CONFIG_APSTA_MIXED_SUPPORT
	if (wrq->u.data.pointer == NULL)
		return Status;

	if (cmd == RTPRIV_IOCTL_SET)
	{
		if (strstr(wrq->u.data.pointer, "OpMode") == NULL)
			return -ENETDOWN;
	}
	else
#endif // CONFIG_APSTA_MIXED_SUPPORT //
		return -ENETDOWN;
    }

    // determine this ioctl command is comming from which interface.
    if (net_dev->priv_flags == INT_MAIN)
    {
		pObj->ioctl_if_type = INT_MAIN;
        pObj->ioctl_if = MAIN_MBSSID;
//        DBGPRINT(RT_DEBUG_INFO, ("rt28xx_ioctl I/F(ra%d)(flags=%d): cmd = 0x%08x\n", pObj->ioctl_if, net_dev->priv_flags, cmd));
    }
    else if (net_dev->priv_flags == INT_MBSSID)
    {
		pObj->ioctl_if_type = INT_MBSSID;
//    	if (!RTMPEqualMemory(net_dev->name, pAd->net_dev->name, 3))  // for multi-physical card, no MBSSID
		if (strcmp(net_dev->name, pAd->net_dev->name) != 0) // sample
    	{
	        for (index = 1; index < pAd->ApCfg.BssidNum; index++)
	    	{
	    	    if (pAd->ApCfg.MBSSID[index].MSSIDDev == net_dev)
	    	    {
	    	        pObj->ioctl_if = index;
	    	        
//	    	        DBGPRINT(RT_DEBUG_INFO, ("rt28xx_ioctl I/F(ra%d)(flags=%d): cmd = 0x%08x\n", index, net_dev->priv_flags, cmd));
	    	        break;
	    	    }
	    	}
	        // Interface not found!
	        if(index == pAd->ApCfg.BssidNum)
	        {
//	        	DBGPRINT(RT_DEBUG_ERROR, ("rt28xx_ioctl can not find I/F\n"));
	            return -ENETDOWN;
	        }
	    }
	    else    // ioctl command from I/F(ra0)
	    {
	    	pAd= net_dev->priv;
    	    pObj->ioctl_if = MAIN_MBSSID;
//	        DBGPRINT(RT_DEBUG_ERROR, ("rt28xx_ioctl can not find I/F and use default: cmd = 0x%08x\n", cmd));
	    }
        MBSS_MR_APIDX_SANITY_CHECK(pObj->ioctl_if);
        apidx = pObj->ioctl_if;
    }
#ifdef WDS_SUPPORT
	else if (net_dev->priv_flags == INT_WDS)
	{
		pObj->ioctl_if_type = INT_WDS;
		for(index = 0; index < MAX_WDS_ENTRY; index++)
		{
			if (pAd->WdsTab.WdsEntry[index].dev == net_dev)
			{
				pObj->ioctl_if = index;

				break;
			}
			
			if(index == MAX_WDS_ENTRY)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("rt28xx_ioctl can not find wds I/F\n"));
				return -ENETDOWN;
			}
		}
	}
#endif // WDS_SUPPORT //
#ifdef APCLI_SUPPORT
	else if (net_dev->priv_flags == INT_APCLI)
	{
		pObj->ioctl_if_type = INT_APCLI;
		for (index = 0; index < MAX_APCLI_NUM; index++)
		{
			if (pAd->ApCfg.ApCliTab[index].dev == net_dev)
			{
				pObj->ioctl_if = index;

				break;
			}

			if(index == MAX_APCLI_NUM)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("rt28xx_ioctl can not find Apcli I/F\n"));
				return -ENETDOWN;
			}
		}
		APCLI_MR_APIDX_SANITY_CHECK(pObj->ioctl_if);
	}
#endif // APCLI_SUPPORT //
    else
    {
//    	DBGPRINT(RT_DEBUG_WARN, ("IOCTL is not supported in WDS interface\n"));
    	return -EOPNOTSUPP;
    }
		
	switch(cmd)
	{
#ifdef RALINK_ATE
#ifdef RALINK_28xx_QA
		case RTPRIV_IOCTL_ATE:
			{
				RtmpDoAte(pAd, wrq);
			}
			break;
#endif // RALINK_28xx_QA // 
#endif // RALINK_ATE //
        case SIOCGIFHWADDR:
			DBGPRINT(RT_DEBUG_TRACE, ("IOCTLIOCTLIOCTL::SIOCGIFHWADDR\n"));
			strcpy((PSTRING) wrq->u.name, (PSTRING) pAd->ApCfg.MBSSID[pObj->ioctl_if].Bssid);
			break;
		case SIOCGIWNAME:
			DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCGIWNAME\n"));
#ifdef RTMP_MAC_PCI
			strcpy(wrq->u.name, "RT2860 SoftAP");
#endif // RTMP_MAC_PCI //
			break;
		case SIOCSIWESSID:  //Set ESSID
			Status = -EOPNOTSUPP;
			break;
		case SIOCGIWESSID:  //Get ESSID
			{
				struct iw_point *erq = &wrq->u.essid;
				PCHAR pSsidStr = NULL;

				erq->flags=1;
              //erq->length = pAd->ApCfg.MBSSID[pObj->ioctl_if].SsidLen;
              
#ifdef APCLI_SUPPORT
				if (net_dev->priv_flags == INT_APCLI)
				{
					if (pAd->ApCfg.ApCliTab[pObj->ioctl_if].Valid == TRUE)
					{
						erq->length = pAd->ApCfg.ApCliTab[pObj->ioctl_if].SsidLen;
						pSsidStr = (PCHAR)&pAd->ApCfg.ApCliTab[pObj->ioctl_if].Ssid;
					}
					else {
						erq->length = 0;
						pSsidStr = NULL;
					}
				}
				else
#endif // APCLI_SUPPORT //
				{
				erq->length = pAd->ApCfg.MBSSID[apidx].SsidLen;
					pSsidStr = (PCHAR)pAd->ApCfg.MBSSID[apidx].Ssid;
				}

				if((erq->pointer) && (pSsidStr != NULL))
				{
					//if(copy_to_user(erq->pointer, pAd->ApCfg.MBSSID[pObj->ioctl_if].Ssid, erq->length))
					if(copy_to_user(erq->pointer, pSsidStr, erq->length))
					{
						Status = -EFAULT;
						break;
					}
				}
				DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCGIWESSID (Len=%d, ssid=%s...)\n", erq->length, (char *)erq->pointer));
			}
			break;
		case SIOCGIWNWID: // get network id 
		case SIOCSIWNWID: // set network id (the cell)
			Status = -EOPNOTSUPP;
			break;
		case SIOCGIWFREQ: // get channel/frequency (Hz)
			wrq->u.freq.m = pAd->CommonCfg.Channel;
			wrq->u.freq.e = 0;
			wrq->u.freq.i = 0;
			break; 
		case SIOCSIWFREQ: //set channel/frequency (Hz)
		case SIOCGIWNICKN:
		case SIOCSIWNICKN: //set node name/nickname
		case SIOCGIWRATE:  //get default bit rate (bps)
            {

				PHTTRANSMIT_SETTING		pHtPhyMode;
#ifdef APCLI_SUPPORT
				if (net_dev->priv_flags == INT_APCLI)
					pHtPhyMode = &pAd->ApCfg.ApCliTab[pObj->ioctl_if].HTPhyMode;
				else
#endif // APCLI_SUPPORT //
#ifdef WDS_SUPPORT
				if (net_dev->priv_flags == INT_WDS)
					pHtPhyMode = &pAd->WdsTab.WdsEntry[pObj->ioctl_if].HTPhyMode;
				else
#endif // WDS_SUPPORT //
					pHtPhyMode = &pAd->ApCfg.MBSSID[pObj->ioctl_if].HTPhyMode;


			RT28XX_IOCTL_MaxRateGet(pAd, pHtPhyMode, (UINT32 *)&wrq->u.bitrate.value);
			wrq->u.bitrate.disabled = 0;
            }
			break;
		case SIOCSIWRATE:  //set default bit rate (bps)
		case SIOCGIWRTS:  // get RTS/CTS threshold (bytes)
		case SIOCSIWRTS:  //set RTS/CTS threshold (bytes)
		case SIOCGIWFRAG:  //get fragmentation thr (bytes)
		case SIOCSIWFRAG:  //set fragmentation thr (bytes)
		case SIOCGIWENCODE:  //get encoding token & mode
		case SIOCSIWENCODE:  //set encoding token & mode
			Status = -EOPNOTSUPP;
			break;
		case SIOCGIWAP:  //get access point MAC addresses
			{
				PCHAR pBssidStr;

				wrq->u.ap_addr.sa_family = ARPHRD_ETHER;
				//memcpy(wrq->u.ap_addr.sa_data, &pAd->ApCfg.MBSSID[pObj->ioctl_if].Bssid, ETH_ALEN);
#ifdef APCLI_SUPPORT
				if (net_dev->priv_flags == INT_APCLI)
				{
					if (pAd->ApCfg.ApCliTab[pObj->ioctl_if].Valid == TRUE)
						pBssidStr = (PCHAR)&APCLI_ROOT_BSSID_GET(pAd, pAd->ApCfg.ApCliTab[pObj->ioctl_if].MacTabWCID);
					else
						pBssidStr = NULL;
				}
				else
#endif // APCLI_SUPPORT //
				{
					pBssidStr = (PCHAR) &pAd->ApCfg.MBSSID[pObj->ioctl_if].Bssid[0];
				}

				if (pBssidStr != NULL)
				{
					memcpy(wrq->u.ap_addr.sa_data, pBssidStr, ETH_ALEN);
					DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCGIWAP(=%02x:%02x:%02x:%02x:%02x:%02x)\n",
						pBssidStr[0],pBssidStr[1],pBssidStr[2], pBssidStr[3],pBssidStr[4],pBssidStr[5]));
				}
				else
				{
					memset(wrq->u.ap_addr.sa_data, 0, ETH_ALEN);
				}
			}
			break;
		case SIOCGIWMODE:  //get operation mode
			wrq->u.mode = IW_MODE_INFRA;   //SoftAP always on INFRA mode.
			break;
		case SIOCSIWAP:  //set access point MAC addresses
		case SIOCSIWMODE:  //set operation mode
		case SIOCGIWSENS:   //get sensitivity (dBm)
		case SIOCSIWSENS:	//set sensitivity (dBm)
		case SIOCGIWPOWER:  //get Power Management settings
		case SIOCSIWPOWER:  //set Power Management settings
		case SIOCGIWTXPOW:  //get transmit power (dBm)
		case SIOCSIWTXPOW:  //set transmit power (dBm)
		//case SIOCGIWRANGE:	//Get range of parameters
		case SIOCGIWRETRY:	//get retry limits and lifetime
		case SIOCSIWRETRY:	//set retry limits and lifetime
			Status = -EOPNOTSUPP;
			break;
		case SIOCGIWRANGE:	//Get range of parameters
		    {
		        struct iw_range range;
				UINT32 len;

                        memset(&range, 0, sizeof(range));
	                range.we_version_compiled = WIRELESS_EXT;
	                range.we_version_source = 14;
				len = copy_to_user(wrq->u.data.pointer, &range, sizeof(range));
		    }
		    break;
		    
		case RT_PRIV_IOCTL:
			subcmd = wrq->u.data.flags;
			if (subcmd & OID_GET_SET_TOGGLE)
				Status = RTMPAPSetInformation(pAd, wrq,  (INT)subcmd);
			else
				Status = RTMPAPQueryInformation(pAd, wrq, (INT)subcmd);
			break;
		
		case SIOCGIWPRIV:
			if (wrq->u.data.pointer) 
			{
				if ( access_ok(VERIFY_WRITE, wrq->u.data.pointer, sizeof(ap_privtab)) != TRUE)
					break;
				wrq->u.data.length = sizeof(ap_privtab) / sizeof(ap_privtab[0]);
				if (copy_to_user(wrq->u.data.pointer, ap_privtab, sizeof(ap_privtab)))
					Status = -EFAULT;
			}
			break;
		case RTPRIV_IOCTL_SET:
			{
				if( access_ok(VERIFY_READ, wrq->u.data.pointer, wrq->u.data.length) == TRUE)
					Status = RTMPAPPrivIoctlSet(pAd, wrq);
			}
			break;
		    
		case RTPRIV_IOCTL_SHOW:
			{
				if( access_ok(VERIFY_READ, wrq->u.data.pointer, wrq->u.data.length) == TRUE)
					Status = RTMPAPPrivIoctlShow(pAd, wrq);
			}
			break;		    

		case RTPRIV_IOCTL_GET_MAC_TABLE:
			RTMPIoctlGetMacTable(pAd,wrq);
		    break;

		case RTPRIV_IOCTL_GSITESURVEY:
			RTMPIoctlGetSiteSurvey(pAd,wrq);
			break;

		case RTPRIV_IOCTL_STATISTICS:
			RTMPIoctlStatistics(pAd, wrq);
			break;

		case RTPRIV_IOCTL_RADIUS_DATA:
		    RTMPIoctlRadiusData(pAd, wrq);
		    break;

		case RTPRIV_IOCTL_ADD_WPA_KEY:
		    RTMPIoctlAddWPAKey(pAd, wrq);
		    break;

		case RTPRIV_IOCTL_ADD_PMKID_CACHE:
		    RTMPIoctlAddPMKIDCache(pAd, wrq);
		    break;

		case RTPRIV_IOCTL_STATIC_WEP_COPY:
		    RTMPIoctlStaticWepCopy(pAd, wrq);
		    break;
#ifdef WSC_AP_SUPPORT
		case RTPRIV_IOCTL_WSC_PROFILE:
		    RTMPIoctlWscProfile(pAd, wrq);
		    break;
#endif // WSC_AP_SUPPORT //
#ifdef DOT11_N_SUPPORT
		case RTPRIV_IOCTL_QUERY_BATABLE:
		    RTMPIoctlQueryBaTable(pAd, wrq);
		    break;
#endif // DOT11_N_SUPPORT //
//#ifdef DBG		// by Jiahao for ASUS ATE
		case RTPRIV_IOCTL_BBP:
			RTMPAPIoctlBBP(pAd, wrq);
			break;
			
		case RTPRIV_IOCTL_MAC:
			RTMPAPIoctlMAC(pAd, wrq);
			break;

		case RTPRIV_IOCTL_E2P:
			RTMPAPIoctlE2PROM(pAd, wrq);
			break;

#ifdef RTMP_RF_RW_SUPPORT
		case RTPRIV_IOCTL_RF:
			RTMPAPIoctlRF(pAd, wrq);
			break;
#endif // RTMP_RF_RW_SUPPORT //
//#endif // DBG //	// by Jiahao for ASUS ATE

		default:
//			DBGPRINT(RT_DEBUG_ERROR, ("IOCTL::unknown IOCTL's cmd = 0x%08x\n", cmd));
			Status = -EOPNOTSUPP;
			break;
	}

	return Status;
}
