#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/autoconf.h>

#include "nvram.h"

#define DEFAULT_FLASH_ZONE_NAME "2860"

int set_usage(char *aout)
{
	int i;

	printf("Usage example: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr 1.2.3.4\n");
	}
	return -1;
}

int get_usage(char *aout)
{
	int i;

	printf("Usage: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr\n");
	}
	return -1;
}

int buf_get_usage(char *aout)
{
	int i;

	printf("Usage: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr wan_ipaddr ...\n");
	}
	return -1;
}

int ra_nv_set(int argc,char **argv)
{
	int index, rc;
	char *fz = "", *key = "", *value = "";

	if (argc == 1 || argc > 5)
		return set_usage(argv[0]);

	if (argc == 2) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
		value = "";
	} else if (argc == 3) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
		value = argv[2];
	} else {
		fz = argv[1];
		key = argv[2];
		value = argv[3];
	}

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return set_usage(argv[0]);
	}

	rc = nvram_set(index, key, value);
    return rc;
}

int ra_nv_get(int argc, char *argv[])
{
	char *fz;
	char *key;
	char *rc;

	int index, ret;

	if (argc != 3 && argc != 2)
		return get_usage(argv[0]);

	if (argc == 2) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
	} else {
		fz = argv[1];
		key = argv[2];
	}

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return get_usage(argv[0]);
	}

	rc = nvram_get(index, key);
	if (rc) {
	    printf("%s\n", rc);
	    ret = 0;
	} else {
	    fprintf(stderr, "nvram_get return error or not return data!\n");
	    ret = -1;
	}

    return (ret);
}

int ra_nv_buf_get(int argc, char *argv[])
{
	char *fz, *key, *rc;
	int i, index, ret=0;

	if (argc < 3)
		return buf_get_usage(argv[0]);

	fz = argv[1];
	key = argv[2];

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return get_usage(argv[0]);
	}

	for (i = 2; i < argc; i++) {
	    rc = nvram_bufget(index, argv[i]);
	    if (rc) {
		printf("%s=\"%s\"\n",argv[i], rc);
	    } else {
		fprintf(stderr, "nvram_buf_get return error or not return data!\n");
		ret = -1;
		break;
	    }
	}

    return (ret);
}

static int isMacValid(char *str)
{
	int i, len = strlen(str);
	if(len != 17)
		return 0;

	for ( i = 0; i < 5; i++ )
	{
		if( (!isxdigit( str[i*3])) || (!isxdigit( str[i*3+1])) || (str[i*3+2] != ':') )
			return 0;
	}
	return (isxdigit(str[15]) && isxdigit(str[16])) ? 1: 0;
}

static int nvram_load_default(void)
{
	/* default macs is OK */
	int mac_ok=1;

	printf("Store MACS...\n");
	char *WLAN_MAC_ADDR	= nvram_get(RT2860_NVRAM, "WLAN_MAC_ADDR");
        char *WAN_MAC_ADDR	= nvram_get(RT2860_NVRAM, "WAN_MAC_ADDR");
        char *LAN_MAC_ADDR	= nvram_get(RT2860_NVRAM, "LAN_MAC_ADDR");
        char *CHECKMAC		= nvram_get(RT2860_NVRAM, "CHECKMAC");

	printf("Clear nvram...\n");
	nvram_clear(RT2860_NVRAM);
	printf("Load defaults nvram...\n");
	renew_nvram(RT2860_NVRAM, "/etc/default/nvram_default");

	/* reinit nvram before commit */
	if ( nvram_init(RT2860_NVRAM) == -1 )
		return -1;

	printf("Restore old macs...\n");
	if ((strlen(WLAN_MAC_ADDR) > 0) && isMacValid(WLAN_MAC_ADDR))
	    nvram_bufset(RT2860_NVRAM, "WLAN_MAC_ADDR", WLAN_MAC_ADDR);
	else
	    mac_ok=0;
	if ((strlen(WAN_MAC_ADDR) > 0) && isMacValid(WAN_MAC_ADDR))
    	    nvram_bufset(RT2860_NVRAM, "WAN_MAC_ADDR",  WAN_MAC_ADDR);
	else
	    mac_ok=0;
	if ((strlen(LAN_MAC_ADDR) > 0) && isMacValid(LAN_MAC_ADDR))
    	    nvram_bufset(RT2860_NVRAM, "LAN_MAC_ADDR",  LAN_MAC_ADDR);
	else
	    mac_ok=0;

	/* all restore ok ? */
	if ( mac_ok == 1 ) {
	    printf("Restore checkmac atribute.\n");
    	    nvram_bufset(RT2860_NVRAM, "CHECKMAC", CHECKMAC);
	} else {
	    printf("Set checkmac atribute.\n");
    	    nvram_bufset(RT2860_NVRAM, "CHECKMAC", "YES");
	}

	/* set wive flag */
        nvram_bufset(RT2860_NVRAM, "IS_WIVE", "YES");
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);
	sync();
    return 0;
}

static int gen_wifi_config(int getmode)
{
	FILE *fp = NULL;
	int  i, ssid_num = 1, inic = 0, mode = getmode;
	char tx_rate[32], wmm_enable[32];

	if (mode == RT2860_NVRAM) {
		system("mkdir -p /etc/Wireless/RT2860");
		fp = fopen("/etc/Wireless/RT2860/RT2860.dat", "w+");
		printf("Build config for fist WiFi module.\n");
#if defined(CONFIG_RT3090_AP) || defined(CONFIG_RT3090_AP_MODULE)
	} else if (mode == RTINIC_NVRAM) {
		system("mkdir -p /etc/Wireless/iNIC");
		fp = fopen("/etc/Wireless/iNIC/RT2860AP.dat", "w+");
		/* after select file for write back to native 2860 mode */
		inic = 1;
		mode = RT2860_NVRAM;
		printf("Build config for second WiFi module.\n");
#endif
	} else {
		printf("gen_wifi_config: mode unknown...\n");
		return 0;
	}

	if (!fp){
		printf("gen_wifi_config: fopen failed.\n");
		return 0;
	}

	if (nvram_init(mode) == -1) {
		printf("gen_wifi_config: nvram init failed...\n");
		fclose(fp);
		return -1;
	}

	fprintf(fp, "#The word of \"Default\" must not be removed\n");
	fprintf(fp, "Default\n");

#define FPRINT_NUM(x) fprintf(fp, #x"=%d\n", atoi(nvram_bufget(mode, #x)));
#define FPRINT_STR(x) fprintf(fp, #x"=%s\n", nvram_bufget(mode, #x));

	if (RT2860_NVRAM == mode) {
		if (!inic) {
		    FPRINT_NUM(WirelessMode);
		    FPRINT_NUM(staWirelessMode);
		    FPRINT_NUM(Channel);
		    FPRINT_STR(SSID1);
		} else {
		    fprintf(fp, "WirelessMode=%d\n", atoi(nvram_bufget(mode, "WirelessModeINIC")));
		    fprintf(fp, "staWirelessMode=%d\n", atoi(nvram_bufget(mode, "staWirelessModeINIC")));
		    fprintf(fp, "Channel=%d\n", atoi(nvram_bufget(mode, "ChannelINIC")));
		    fprintf(fp, "SSID1=%s\n", nvram_bufget(mode, "SSID1INIC"));
		}

		//TxRate(FixedRate)
		ssid_num = atoi(nvram_bufget(mode, "BssidNum"));
		if (ssid_num > 16)
			ssid_num = 16;
		else if (ssid_num < 0)
			ssid_num = 0;
		bzero(tx_rate, sizeof(tx_rate));
		for (i = 0; i < ssid_num; i++)
		{
			snprintf(tx_rate+strlen(tx_rate), 1, "%d", atoi(nvram_bufget(mode, "TxRate")));
			snprintf(tx_rate+strlen(tx_rate), 1, "%c", ';');
		}
		fprintf(fp, "TxRate=%s\n", tx_rate);

		//WmmCapable
		bzero(wmm_enable, sizeof(wmm_enable));
		for (i = 0; i < ssid_num; i++)
		{
			snprintf(wmm_enable+strlen(wmm_enable), 1, "%d", atoi(nvram_bufget(mode, "WmmCapable")));
			snprintf(wmm_enable+strlen(wmm_enable), 1, "%c", ';');
		}
		fprintf(fp, "WmmCapable=%s\n", wmm_enable);

		//WscConfStatus
		if (atoi(nvram_bufget(mode, "WscConfigured")) == 0)
			fprintf(fp, "WscConfStatus=%d\n", 1);
		else
			fprintf(fp, "WscConfStatus=%d\n", 2);

		if (strcmp(nvram_bufget(mode, "WscVendorPinCode"), "") != 0)
			FPRINT_STR(WscVendorPinCode);

		FPRINT_NUM(WirelessEvent);
		FPRINT_NUM(CountryRegion);
		FPRINT_NUM(CountryRegionABand);
		FPRINT_STR(CountryCode);
#ifdef CONFIG_RT2860V2_EXT_CHANNEL_LIST
		FPRINT_NUM(ChannelGeography);
#endif
		FPRINT_NUM(AutoChannelSelect);

#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
		FPRINT_NUM(BssidNum);
		FPRINT_STR(SSID2);
		FPRINT_STR(SSID3);
		FPRINT_STR(SSID4);
		FPRINT_STR(SSID5);
		FPRINT_STR(SSID6);
		FPRINT_STR(SSID7);
		FPRINT_STR(SSID8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(SSID9);
		FPRINT_STR(SSID10);
		FPRINT_STR(SSID11);
		FPRINT_STR(SSID12);
		FPRINT_STR(SSID13);
		FPRINT_STR(SSID14);
		FPRINT_STR(SSID15);
		FPRINT_STR(SSID16);
#endif
#endif
		FPRINT_NUM(AutoConnect);
		FPRINT_NUM(FastConnect);
		FPRINT_NUM(HiPower);
		FPRINT_NUM(AutoRoaming);
		FPRINT_STR(FixedTxMode);
		FPRINT_NUM(BasicRate);
		FPRINT_NUM(BeaconPeriod);
		FPRINT_NUM(DtimPeriod);
		FPRINT_NUM(TxPower);
		FPRINT_NUM(DisableOLBC);
		FPRINT_NUM(BGProtection);
		fprintf(fp, "TxAntenna=\n");
		fprintf(fp, "RxAntenna=\n");
		FPRINT_NUM(TxPreamble);
		FPRINT_NUM(RTSThreshold);
		FPRINT_NUM(FragThreshold);
		FPRINT_NUM(TxBurst);
		FPRINT_NUM(BurstMode);
		FPRINT_NUM(PktAggregate);
		FPRINT_NUM(FreqDelta);
		fprintf(fp, "TurboRate=0\n");
#if defined (CONFIG_RT2860V2_AP_VIDEO_TURBINE) || defined (CONFIG_RT2860V2_STA_VIDEO_TURBINE)
		FPRINT_NUM(VideoTurbine);
		FPRINT_NUM(VideoClassifierEnable);
		FPRINT_NUM(VideoHighTxMode);
		FPRINT_NUM(VideoTxLifeTimeMode);
#endif
		FPRINT_NUM(M2UEnabled);
		FPRINT_NUM(IgmpSnEnable);
		FPRINT_NUM(McastPhyMode);
		FPRINT_NUM(McastMcs);
		FPRINT_STR(APAifsn);
		FPRINT_STR(APCwmin);
		FPRINT_STR(APCwmax);
		FPRINT_STR(APTxop);
		FPRINT_STR(APACM);
		FPRINT_STR(BSSAifsn);
		FPRINT_STR(BSSCwmin);
		FPRINT_STR(BSSCwmax);
		FPRINT_STR(BSSTxop);
		FPRINT_STR(BSSACM);
		FPRINT_STR(AckPolicy);
		FPRINT_STR(APSDCapable);
		FPRINT_STR(DLSCapable);
		FPRINT_STR(NoForwarding);
		FPRINT_NUM(NoForwardingBTNBSSID);
		FPRINT_STR(HideSSID);
		FPRINT_NUM(ShortSlot);
		FPRINT_STR(IEEE8021X);
		FPRINT_NUM(IEEE80211H);
#if defined (CONFIG_RT2860V2_AP_CARRIER) || defined (CONFIG_RT2860V2_STA_CARRIER)
		FPRINT_NUM(CarrierDetect);
#endif
		FPRINT_STR(PreAntSwitch);
		FPRINT_NUM(PhyRateLimit);
		FPRINT_NUM(DebugFlags);
		FPRINT_NUM(FineAGC);
		FPRINT_NUM(StreamMode);
		FPRINT_STR(StreamModeMac0);
		FPRINT_STR(StreamModeMac1);
		FPRINT_STR(StreamModeMac2);
		FPRINT_STR(StreamModeMac3);
		FPRINT_NUM(CSPeriod);
		FPRINT_STR(RDRegion);
		FPRINT_STR(StationKeepAlive);
#if defined (CONFIG_RT2860V2_AP_DFS) || defined (CONFIG_RT2860V2_STA_DFS)
		FPRINT_NUM(ITxBfEn);
		FPRINT_NUM(ETxBfEnCond);
		FPRINT_NUM(ITxBfTimeout);
		FPRINT_NUM(ETxBfTimeout);
		FPRINT_NUM(ETxBfNoncompress);
		FPRINT_NUM(ETxBfIncapable);
		FPRINT_NUM(DfsLowerLimit);
		FPRINT_NUM(DfsUpperLimit);
		FPRINT_NUM(DfsIndoor);
		FPRINT_NUM(DFSParamFromConfig);
		FPRINT_STR(FCCParamCh0);
		FPRINT_STR(FCCParamCh1);
		FPRINT_STR(FCCParamCh2);
		FPRINT_STR(FCCParamCh3);
		FPRINT_STR(CEParamCh0);
		FPRINT_STR(CEParamCh1);
		FPRINT_STR(CEParamCh2);
		FPRINT_STR(CEParamCh3);
		FPRINT_STR(JAPParamCh0);
		FPRINT_STR(JAPParamCh1);
		FPRINT_STR(JAPParamCh2);
		FPRINT_STR(JAPParamCh3);
		FPRINT_STR(JAPW53ParamCh0);
		FPRINT_STR(JAPW53ParamCh1);
		FPRINT_STR(JAPW53ParamCh2);
		FPRINT_STR(JAPW53ParamCh3);
		FPRINT_NUM(FixDfsLimit);
		FPRINT_NUM(LongPulseRadarTh);
		FPRINT_NUM(AvgRssiReq);
		FPRINT_NUM(DFS_R66);
		FPRINT_STR(blockch);
#endif
#ifdef CONFIG_RT2860V2_AP_GREENAP
		FPRINT_NUM(GreenAP);
#endif
		FPRINT_STR(PreAuth);
		FPRINT_STR(AuthMode);
		FPRINT_STR(EncrypType);
    		FPRINT_STR(RekeyMethod);
		FPRINT_NUM(RekeyInterval);
		FPRINT_STR(PMKCachePeriod);
#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
#if defined(CONFIG_RT2860V2_STA_WAPI) || defined(CONFIG_RT2860V2_AP_WAPI)
		/*kurtis: WAPI*/
		FPRINT_STR(WapiPsk1);
		FPRINT_STR(WapiPsk2);
		FPRINT_STR(WapiPsk3);
		FPRINT_STR(WapiPsk4);
		FPRINT_STR(WapiPsk5);
		FPRINT_STR(WapiPsk6);
		FPRINT_STR(WapiPsk7);
		FPRINT_STR(WapiPsk8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(WapiPsk9);
		FPRINT_STR(WapiPsk10);
		FPRINT_STR(WapiPsk11);
		FPRINT_STR(WapiPsk12);
		FPRINT_STR(WapiPsk13);
		FPRINT_STR(WapiPsk14);
		FPRINT_STR(WapiPsk15);
		FPRINT_STR(WapiPsk16);
#endif
#endif
		FPRINT_STR(WapiPskType);
		FPRINT_STR(Wapiifname);
		FPRINT_STR(WapiAsCertPath);
		FPRINT_STR(WapiUserCertPath);
		FPRINT_STR(WapiAsIpAddr);
		FPRINT_STR(WapiAsPort);
#endif
#if defined(CONFIG_RT2860V2_STA_MESH) || defined(CONFIG_RT2860V2_AP_MESH)
		FPRINT_NUM(MeshAutoLink);
		FPRINT_STR(MeshAuthMode);
		FPRINT_STR(MeshEncrypType);
		FPRINT_NUM(MeshDefaultkey);
		FPRINT_STR(MeshWEPKEY);
		FPRINT_STR(MeshWPAKEY);
		FPRINT_STR(MeshId);
#endif
#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
		//WPAPSK
		FPRINT_STR(WPAPSK1);
		FPRINT_STR(WPAPSK2);
		FPRINT_STR(WPAPSK3);
		FPRINT_STR(WPAPSK4);
		FPRINT_STR(WPAPSK5);
		FPRINT_STR(WPAPSK6);
		FPRINT_STR(WPAPSK7);
		FPRINT_STR(WPAPSK8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(WPAPSK9);
		FPRINT_STR(WPAPSK10);
		FPRINT_STR(WPAPSK11);
		FPRINT_STR(WPAPSK12);
		FPRINT_STR(WPAPSK13);
		FPRINT_STR(WPAPSK14);
		FPRINT_STR(WPAPSK15);
		FPRINT_STR(WPAPSK16);
#endif
#endif
		FPRINT_STR(DefaultKeyID);
#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
		FPRINT_STR(Key1Type);
		FPRINT_STR(Key1Str1);
		FPRINT_STR(Key1Str2);
		FPRINT_STR(Key1Str3);
		FPRINT_STR(Key1Str4);
		FPRINT_STR(Key1Str5);
		FPRINT_STR(Key1Str6);
		FPRINT_STR(Key1Str7);
		FPRINT_STR(Key1Str8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(Key1Str9);
		FPRINT_STR(Key1Str10);
		FPRINT_STR(Key1Str11);
		FPRINT_STR(Key1Str12);
		FPRINT_STR(Key1Str13);
		FPRINT_STR(Key1Str14);
		FPRINT_STR(Key1Str15);
		FPRINT_STR(Key1Str16);
#endif

		FPRINT_STR(Key2Type);
		FPRINT_STR(Key2Str1);
		FPRINT_STR(Key2Str2);
		FPRINT_STR(Key2Str3);
		FPRINT_STR(Key2Str4);
		FPRINT_STR(Key2Str5);
		FPRINT_STR(Key2Str6);
		FPRINT_STR(Key2Str7);
		FPRINT_STR(Key2Str8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(Key2Str9);
		FPRINT_STR(Key2Str10);
		FPRINT_STR(Key2Str11);
		FPRINT_STR(Key2Str12);
		FPRINT_STR(Key2Str13);
		FPRINT_STR(Key2Str14);
		FPRINT_STR(Key2Str15);
		FPRINT_STR(Key2Str16);
#endif
		FPRINT_STR(Key3Type);
		FPRINT_STR(Key3Str1);
		FPRINT_STR(Key3Str2);
		FPRINT_STR(Key3Str3);
		FPRINT_STR(Key3Str4);
		FPRINT_STR(Key3Str5);
		FPRINT_STR(Key3Str6);
		FPRINT_STR(Key3Str7);
		FPRINT_STR(Key3Str8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(Key3Str9);
		FPRINT_STR(Key3Str10);
		FPRINT_STR(Key3Str11);
		FPRINT_STR(Key3Str12);
		FPRINT_STR(Key3Str13);
		FPRINT_STR(Key3Str14);
		FPRINT_STR(Key3Str15);
		FPRINT_STR(Key3Str16);
#endif
		FPRINT_STR(Key4Type);
		FPRINT_STR(Key4Str1);
		FPRINT_STR(Key4Str2);
		FPRINT_STR(Key4Str3);
		FPRINT_STR(Key4Str4);
		FPRINT_STR(Key4Str5);
		FPRINT_STR(Key4Str6);
		FPRINT_STR(Key4Str7);
		FPRINT_STR(Key4Str8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(Key4Str9);
		FPRINT_STR(Key4Str10);
		FPRINT_STR(Key4Str11);
		FPRINT_STR(Key4Str12);
		FPRINT_STR(Key4Str13);
		FPRINT_STR(Key4Str14);
		FPRINT_STR(Key4Str15);
		FPRINT_STR(Key4Str16);
#endif
#endif
		//MIMO
		FPRINT_NUM(HT_HTC);
		FPRINT_NUM(HT_RDG);
		FPRINT_NUM(HT_OpMode);
		FPRINT_NUM(HT_MpduDensity);
		FPRINT_NUM(HT_EXTCHA);
		FPRINT_NUM(HT_BW);
		FPRINT_NUM(HT_AutoBA);
		FPRINT_NUM(HT_BADecline);
		FPRINT_NUM(HT_AMSDU);
		FPRINT_NUM(HT_BAWinSize);
		FPRINT_NUM(HT_GI);
		FPRINT_NUM(HT_STBC);
		FPRINT_STR(HT_MCS);
		FPRINT_NUM(HT_TxStream);
		FPRINT_NUM(HT_RxStream);
		FPRINT_NUM(HT_PROTECT);
		FPRINT_NUM(HT_DisallowTKIP);
		FPRINT_NUM(HT_40MHZ_INTOLERANT);
		FPRINT_NUM(HT_MIMOPSMode);
		FPRINT_NUM(HT_MIMOPS);
		FPRINT_NUM(HSCounter);
		FPRINT_NUM(WscConfMode);
		FPRINT_NUM(WCNTest);
#ifdef CONFIG_RT2860V2_AP_80211N_DRAFT3
		FPRINT_NUM(HT_BSSCoexistence);
		FPRINT_NUM(HT_BSSCoexApCntThr);
#endif
#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
		FPRINT_NUM(AccessPolicy0);
		FPRINT_STR(AccessControlList0);
		FPRINT_NUM(AccessPolicy1);
		FPRINT_STR(AccessControlList1);
		FPRINT_NUM(AccessPolicy2);
		FPRINT_STR(AccessControlList2);
		FPRINT_NUM(AccessPolicy3);
		FPRINT_STR(AccessControlList3);
		FPRINT_NUM(AccessPolicy4);
		FPRINT_STR(AccessControlList4);
		FPRINT_NUM(AccessPolicy5);
		FPRINT_STR(AccessControlList5);
		FPRINT_NUM(AccessPolicy6);
		FPRINT_STR(AccessControlList6);
		FPRINT_NUM(AccessPolicy7);
		FPRINT_STR(AccessControlList7);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_NUM(AccessPolicy8);
		FPRINT_STR(AccessControlList8);
		FPRINT_NUM(AccessPolicy9);
		FPRINT_STR(AccessControlList9);
		FPRINT_NUM(AccessPolicy10);
		FPRINT_STR(AccessControlList10);
		FPRINT_NUM(AccessPolicy11);
		FPRINT_STR(AccessControlList11);
		FPRINT_NUM(AccessPolicy12);
		FPRINT_STR(AccessControlList12);
		FPRINT_NUM(AccessPolicy13);
		FPRINT_STR(AccessControlList13);
		FPRINT_NUM(AccessPolicy14);
		FPRINT_STR(AccessControlList14);
		FPRINT_NUM(AccessPolicy15);
		FPRINT_STR(AccessControlList15);
#endif
#endif

		FPRINT_NUM(WdsEnable);
		FPRINT_STR(WdsPhyMode);
		FPRINT_STR(WdsTxMcs);
		FPRINT_STR(WdsEncrypType);
		FPRINT_STR(WdsList);
		FPRINT_STR(Wds0Key);
		FPRINT_STR(Wds1Key);
		FPRINT_STR(Wds2Key);
		FPRINT_STR(Wds3Key);
		FPRINT_STR(RADIUS_Server);
		FPRINT_STR(RADIUS_Port);
#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
		FPRINT_STR(RADIUS_Key1);
		FPRINT_STR(RADIUS_Key2);
		FPRINT_STR(RADIUS_Key3);
		FPRINT_STR(RADIUS_Key4);
		FPRINT_STR(RADIUS_Key5);
		FPRINT_STR(RADIUS_Key6);
		FPRINT_STR(RADIUS_Key7);
		FPRINT_STR(RADIUS_Key8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(RADIUS_Key9);
		FPRINT_STR(RADIUS_Key10);
		FPRINT_STR(RADIUS_Key11);
		FPRINT_STR(RADIUS_Key12);
		FPRINT_STR(RADIUS_Key13);
		FPRINT_STR(RADIUS_Key14);
		FPRINT_STR(RADIUS_Key15);
		FPRINT_STR(RADIUS_Key16);
#endif
#endif

		FPRINT_STR(own_ip_addr);
		FPRINT_STR(EAPifname);
		FPRINT_STR(PreAuthifname);
		FPRINT_STR(PSMode);
		FPRINT_NUM(session_timeout_interval);
		FPRINT_NUM(quiet_interval);
		FPRINT_NUM(TGnWifiTest);

#ifdef CONFIG_RT2860V2_AP_APCLI
		//AP Client parameters
		FPRINT_NUM(ApCliEnable);
		FPRINT_STR(ApCliSsid);
		FPRINT_STR(ApCliBssid);
		FPRINT_STR(ApCliAuthMode);
		FPRINT_STR(ApCliEncrypType);
		FPRINT_STR(ApCliDefaultKeyID);
		FPRINT_STR(ApCliWPAPSK);
		FPRINT_NUM(ApCliKey1Type);
		FPRINT_STR(ApCliKey1Str);
		FPRINT_NUM(ApCliKey2Type);
		FPRINT_STR(ApCliKey2Str);
		FPRINT_NUM(ApCliKey3Type);
		FPRINT_STR(ApCliKey3Str);
		FPRINT_NUM(ApCliKey4Type);
		FPRINT_STR(ApCliKey4Str);
		FPRINT_NUM(ApCliTxMode);
		FPRINT_NUM(ApCliTxMcs);
#endif
#if defined(CONFIG_RT2860V2_AP_IDS) || defined(CONFIG_RT2860V2_STA_IDS)
		FPRINT_NUM(IdsEnable);
		FPRINT_NUM(AuthFloodThreshold);
		FPRINT_NUM(AssocReqFloodThreshold);
		FPRINT_NUM(ReassocReqFloodThreshold);
		FPRINT_NUM(ProbeReqFloodThreshold);
		FPRINT_NUM(DisassocFloodThreshold);
		FPRINT_NUM(DeauthFloodThreshold);
		FPRINT_NUM(EapReqFooldThreshold);
#endif
		FPRINT_NUM(NintendoCapable);
		FPRINT_NUM(UseNewRateAdapt);
		FPRINT_NUM(IdleTimeout);

#ifdef CONFIG_RT2860V2_AP_INTERFERENCE_REDUCE
		FPRINT_NUM(MO_FalseCCATh);
		FPRINT_NUM(MO_LowFalseCCATh);
		FPRINT_NUM(DyncVgaEnable);
#endif

		//Radio On/Off
		if (atoi(nvram_bufget(mode, "RadioOff")) == 1)
			fprintf(fp, "RadioOn=0\n");
		else
			fprintf(fp, "RadioOn=1\n");

		/*
		 * There are no SSID/WPAPSK/Key1Str/Key2Str/Key3Str/Key4Str anymore since driver1.5 , but 
		 * STA WPS still need these entries to show the WPS result(That is the only way i know to get WPAPSK key) and
		 * so we create empty entries here.   --YY
		 */
		fprintf(fp, "SSID=\nWPAPSK=\nKey1Str=\nKey2Str=\nKey3Str=\nKey4Str=\n");
	}

    fclose(fp);
    nvram_close(mode);
    sync();
    return 0;
}

void usage(char *cmd)
{
#ifndef CONFIG_RT2860V2_USER_MEMORY_OPTIMIZATION
	printf("Usage:\n");
	printf("  %s <command> [<platform>] [<file>]\n\n", cmd);
	printf("command:\n");
	printf("  rt2860_nvram_show - display rt2860 values in nvram\n");
#ifdef CONFIG_DUAL_IMAGE
	printf("  uboot_nvram_show - display uboot parameter values\n");
#endif
	printf("  show    - display values in nvram for <platform>\n");
	printf("  gen     - generate config file from nvram for <platform>\n");
	printf("  renew   - replace nvram values for <platform> with <file>\n");
	printf("  clear	  - clear all entries in nvram for <platform>\n");
	printf("  default - load default for <platform>\n");
	printf("platform:\n");
	printf("  2860    - rt2860\n");
#ifdef CONFIG_DUAL_IMAGE
	printf("  uboot    - uboot parameter\n");
#endif
	printf("file:\n");
	printf("          - file name for renew command\n");
#endif
	exit(0);
}

int main(int argc, char *argv[])
{
	char *cmd = "";
	int index;

	if (argc < 2)
		usage(argv[0]);

	//call nvram_get, nvram_set or nvram_show
	if (cmd == strrchr(argv[0], '/'))
		cmd++;
	else
		cmd = argv[0];

	if (!strncmp(cmd, "nvram_get", 10))
		return ra_nv_get(argc, argv);
	if (!strncmp(cmd, "nvram_buf_get", 14))
		return ra_nv_buf_get(argc, argv);
	else if (!strncmp(cmd, "nvram_set", 10))
		return ra_nv_set(argc, argv);
	else if (!strncmp(cmd, "nvram_show", 11))
		return nvram_show(RT2860_NVRAM);
	else if (!strncmp(cmd, "nvram_default", 14))
		return nvram_load_default();
	else if (!strncmp(cmd, "nvram_clear", 12))
		return nvram_clear(RT2860_NVRAM);

	if (argc == 2) {
		if (!strncmp(argv[1], "rt2860_nvram_show", 18))
			nvram_show(RT2860_NVRAM);
#ifdef CONFIG_DUAL_IMAGE
		else if (!strncmp(argv[1], "uboot_nvram_show", 17))
			nvram_show(UBOOT_NVRAM);
#endif
		else
			usage(argv[0]);
	} else if (argc == 3) {
		/* TODO: <cmd> gen 2860ap */
		if (!strncasecmp(argv[1], "gen", 4) ||
		    !strncasecmp(argv[1], "make_wireless_config", 21)) {
			if (!strncmp(argv[2], "2860", 5) ||
			    !strncasecmp(argv[2], "rt2860", 7)) { //b-compatible
				gen_wifi_config(RT2860_NVRAM);
#if defined(CONFIG_RT3090_AP) || defined(CONFIG_RT3090_AP_MODULE)
				gen_wifi_config(RTINIC_NVRAM);
#endif
			}
#ifdef CONFIG_DUAL_IMAGE
			else if (!strncasecmp(argv[2], "uboot", 6))
				fprintf(stderr,"No support of gen command of uboot parameter.\n");
#endif
			else
				usage(argv[0]);
		} else if (!strncasecmp(argv[1], "show", 5)) {
			if (!strncmp(argv[2], "2860", 5) ||
			    !strncasecmp(argv[2], "rt2860", 7)) //b-compatible
				nvram_show(RT2860_NVRAM);
#ifdef CONFIG_DUAL_IMAGE
			else if (!strncasecmp(argv[2], "uboot", 6))
				nvram_show(UBOOT_NVRAM);
#endif
			else {
				if ((index = getNvramIndex(argv[2])) == -1) {
					fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], argv[2]);
					usage(argv[0]);
				} else
					nvram_show(index);
			}

		} else if(!strncasecmp(argv[1], "clear", 6)) {
			if (!strncmp(argv[2], "2860", 5) || 
			    !strncasecmp(argv[2], "rt2860", 7)) //b-compatible
				nvram_clear(RT2860_NVRAM);
#ifdef CONFIG_DUAL_IMAGE
			else if (!strncasecmp(argv[2], "uboot", 6))
				nvram_clear(UBOOT_NVRAM);
#endif
			else
				usage(argv[0]);
		} else
			usage(argv[0]);
	} else if (argc == 4) {
		if (!strncasecmp(argv[1], "renew", 6)) {
			if (!strncmp(argv[2], "2860", 5) ||
			    !strncasecmp(argv[2], "rt2860", 7)) //b-compatible
				renew_nvram(RT2860_NVRAM, argv[3]);
#ifdef CONFIG_DUAL_IMAGE
			else if (!strncasecmp(argv[2], "uboot", 6))
				fprintf(stderr,"No support of renew command of uboot parameter.\n");
#endif
		} else
			usage(argv[0]);
	} else
		usage(argv[0]);
	return 0;
}
