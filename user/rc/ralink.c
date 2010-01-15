/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <stdio.h>
#include <fcntl.h>		//	for restore175C() from Ralink src
#include <flash_ioctl.h>	//	for restore175C() from Ralink src
#include <netconf.h>
#include <nvram/bcmnvram.h>
#include <unistd.h>
#include <string.h>

#include <ralink.h>
#include "iwlib.h"
#include "stapriv.h"
#include "wps.h"

typedef u_int64_t u64;
typedef u_int32_t u32;
typedef u_int16_t u16;
typedef u_int8_t u8;

#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <net/if_arp.h>
#include <shutils.h>
#include <rc.h>
#include "ra_ioctl.h"
#include <sys/signal.h>
#include <semaphore_mfp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_FRW 64 //2008.11 magic for FWRITE

int FRead(unsigned int *dst, unsigned int *src, unsigned int numBytes)
{
	struct flash_opt flash;
	int fd;

	if(numBytes > FLASH_MAX_RW_SIZE)
	{
		fprintf(stderr, "Too many bytes - %d > %d bytes\n",numBytes, FLASH_MAX_RW_SIZE);
		return 0;
	}

	flash.dest=dst;
	flash.src=src;
	flash.bytes=numBytes;

	fd = open("/dev/flash0", O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "Open flash pseudo device failed\n");
		return 0;
	}

	if(ioctl(fd, FLASH_IOCTL_READ, &flash)<0)
	{
		fprintf(stderr, "FLASH_API: ioctl error\n");
		close(fd);
		return -1;
	}

	close(fd);
	if(flash.result==OUT_OF_SCOPE)
		return -1;
	else
		return 0;
}

int FWrite(unsigned int *source, unsigned int *dest, unsigned int numBytes)
{
	struct flash_opt flash;
	int fd;

	if(numBytes > FLASH_MAX_RW_SIZE)
	{
		fprintf(stderr, "Too many bytes - %d > %d bytes\n",flash.bytes, FLASH_MAX_RW_SIZE);
		return 0;
	}

	flash.dest=dest;
	flash.src=source;
	flash.bytes=numBytes;

	fd = open("/dev/flash0", O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "Open flash pseudo device failed\n");
		return 0;
	}

	if(ioctl(fd, FLASH_IOCTL_WRITE, &flash)<0)
	{
		fprintf(stderr, "FLASH_API: ioctl error\n");
		close(fd);
		return -1;
	}

	close(fd);
	if(flash.result==OUT_OF_SCOPE)
		return -1;
	else
		return 0;
}

int
getMAC()
{
	unsigned char buffer[6];
	char macaddr[18];
	memset(buffer, 0, sizeof(buffer));
	memset(macaddr, 0, sizeof(macaddr));

	if(FRead((unsigned int *)buffer,0x40004, 6)<0)
		fprintf(stderr, "READ MAC address: Out of scope\n");
	else
	{
		ether_etoa(buffer, macaddr);
		puts(macaddr);
	}
	return 0;
}

char DEFAULT_MAC[]="00AABBCDEF77";
//char DEFAULT_MAC2[]="00:AA:BB:CD:EF:77";

char*
getMAC2(char *mac)
{
	unsigned char buffer[6];
	char macaddr[MACSIZE];
	memset(buffer, 0, sizeof(buffer));
	memset(macaddr, 0, MACSIZE);

	if(FRead((unsigned int *)buffer,0x40004, 6)<0)
		fprintf(stderr, "READ MAC address: Out of scope\n");
	else
	{
		ether_etoa2(buffer, macaddr);
		puts(macaddr);
		memset(mac, 0, MACSIZE);
		strcpy(mac, macaddr);
	}
	if(mac && (strncmp(mac, "000000000000", 12)!=0))
       		return mac;
	else
		return DEFAULT_MAC;
}

int
setMAC(const char *mac)
{
	char ea[ETHER_ADDR_LEN];
	if (ether_atoe(mac, ea))
	{
		FWrite(ea, 0x40004, 6);
		getMAC();
	}
	return 0;
}

int
getCountryCode()
{
	unsigned char CC[3];
	memset(CC, 0, sizeof(CC));
	FRead(CC, 0x4004e, 2);
	if (CC[0]==0xff && CC[1]==0xff)	// 0xffff is default
		;
	else
		puts(CC);
	return 0;
}

int
setCountryCode(const char *cc)
{
	char CC[3];

	if (strlen(cc)!=2)
		return 0;
	/* Please refer to ISO3166 code list for other countries and can be found at
	 * http://www.iso.org/iso/en/prods-services/iso3166ma/02iso-3166-code-lists/list-en1.html#sz
	 */
	else if (!strcasecmp(cc, "DB")) ;
	else if (!strcasecmp(cc, "AL")) ;
	else if (!strcasecmp(cc, "DZ")) ;
	else if (!strcasecmp(cc, "AR")) ;
	else if (!strcasecmp(cc, "AM")) ;
	else if (!strcasecmp(cc, "AU")) ;
	else if (!strcasecmp(cc, "AT")) ;
	else if (!strcasecmp(cc, "AZ")) ;
	else if (!strcasecmp(cc, "BH")) ;
	else if (!strcasecmp(cc, "BY")) ;
	else if (!strcasecmp(cc, "BE")) ;
	else if (!strcasecmp(cc, "BZ")) ;
	else if (!strcasecmp(cc, "BO")) ;
	else if (!strcasecmp(cc, "BR")) ;
	else if (!strcasecmp(cc, "BN")) ;
	else if (!strcasecmp(cc, "BG")) ;
	else if (!strcasecmp(cc, "CA")) ;
	else if (!strcasecmp(cc, "CL")) ;
	else if (!strcasecmp(cc, "CN")) ;
	else if (!strcasecmp(cc, "CO")) ;
	else if (!strcasecmp(cc, "CR")) ;
	else if (!strcasecmp(cc, "HR")) ;
	else if (!strcasecmp(cc, "CY")) ;
	else if (!strcasecmp(cc, "CZ")) ;
	else if (!strcasecmp(cc, "DK")) ;
	else if (!strcasecmp(cc, "DO")) ;
	else if (!strcasecmp(cc, "EC")) ;
	else if (!strcasecmp(cc, "EG")) ;
	else if (!strcasecmp(cc, "SV")) ;
	else if (!strcasecmp(cc, "EE")) ;
	else if (!strcasecmp(cc, "FI")) ;
	else if (!strcasecmp(cc, "FR")) ;
	else if (!strcasecmp(cc, "GE")) ;
	else if (!strcasecmp(cc, "DE")) ;
	else if (!strcasecmp(cc, "GR")) ;
	else if (!strcasecmp(cc, "GT")) ;
	else if (!strcasecmp(cc, "HN")) ;
	else if (!strcasecmp(cc, "HK")) ;
	else if (!strcasecmp(cc, "HU")) ;
	else if (!strcasecmp(cc, "IS")) ;
	else if (!strcasecmp(cc, "IN")) ;
	else if (!strcasecmp(cc, "ID")) ;
	else if (!strcasecmp(cc, "IR")) ;
	else if (!strcasecmp(cc, "IE")) ;
	else if (!strcasecmp(cc, "IL")) ;
	else if (!strcasecmp(cc, "IT")) ;
	else if (!strcasecmp(cc, "JP")) ;
	else if (!strcasecmp(cc, "JO")) ;
	else if (!strcasecmp(cc, "KZ")) ;
	else if (!strcasecmp(cc, "KP")) ;
	else if (!strcasecmp(cc, "KR")) ;
	else if (!strcasecmp(cc, "KW")) ;
	else if (!strcasecmp(cc, "LV")) ;
	else if (!strcasecmp(cc, "LB")) ;
	else if (!strcasecmp(cc, "LI")) ;
	else if (!strcasecmp(cc, "LT")) ;
	else if (!strcasecmp(cc, "LU")) ;
	else if (!strcasecmp(cc, "MO")) ;
	else if (!strcasecmp(cc, "MK")) ;
	else if (!strcasecmp(cc, "MY")) ;
	else if (!strcasecmp(cc, "MX")) ;
	else if (!strcasecmp(cc, "MC")) ;
	else if (!strcasecmp(cc, "MA")) ;
	else if (!strcasecmp(cc, "NL")) ;
	else if (!strcasecmp(cc, "NZ")) ;
	else if (!strcasecmp(cc, "NO")) ;
	else if (!strcasecmp(cc, "OM")) ;
	else if (!strcasecmp(cc, "PK")) ;
	else if (!strcasecmp(cc, "PA")) ;
	else if (!strcasecmp(cc, "PE")) ;
	else if (!strcasecmp(cc, "PH")) ;
	else if (!strcasecmp(cc, "PL")) ;
	else if (!strcasecmp(cc, "PT")) ;
	else if (!strcasecmp(cc, "PR")) ;
	else if (!strcasecmp(cc, "QA")) ;
	else if (!strcasecmp(cc, "RO")) ;
	else if (!strcasecmp(cc, "RU")) ;
	else if (!strcasecmp(cc, "SA")) ;
	else if (!strcasecmp(cc, "SG")) ;
	else if (!strcasecmp(cc, "SK")) ;
	else if (!strcasecmp(cc, "SI")) ;
	else if (!strcasecmp(cc, "ZA")) ;
	else if (!strcasecmp(cc, "ES")) ;
	else if (!strcasecmp(cc, "SE")) ;
	else if (!strcasecmp(cc, "CH")) ;
	else if (!strcasecmp(cc, "SY")) ;
	else if (!strcasecmp(cc, "TW")) ;
	else if (!strcasecmp(cc, "TH")) ;
	else if (!strcasecmp(cc, "TT")) ;
	else if (!strcasecmp(cc, "TN")) ;
	else if (!strcasecmp(cc, "TR")) ;
	else if (!strcasecmp(cc, "UA")) ;
	else if (!strcasecmp(cc, "AE")) ;
	else if (!strcasecmp(cc, "GB")) ;
	else if (!strcasecmp(cc, "US")) ;
	else if (!strcasecmp(cc, "UY")) ;
	else if (!strcasecmp(cc, "UZ")) ;
	else if (!strcasecmp(cc, "VE")) ;
	else if (!strcasecmp(cc, "VN")) ;
	else if (!strcasecmp(cc, "YE")) ;
	else if (!strcasecmp(cc, "ZW")) ;
	else
	{
		return 0;
	}

	memset(&CC[0], toupper(cc[0]), 1);
	memset(&CC[1], toupper(cc[1]), 1);
	memset(&CC[2], 0, 1);
	FWrite(CC, 0x4004e, 2);
	puts(CC);
	return 0;
}

//2008.11 magic{
int
atoh(const char *a, unsigned char *e)
{
	char *c = (char *) a;
	int i = 0;

	memset(e, 0, MAX_FRW);
	for (;;) {
		e[i++] = (unsigned char) strtoul(c, &c, 16);
		if (!*c++ || i == MAX_FRW)
			break;
	}
	return i;
}

int
htoa(const unsigned char *e, char *a, int len)
{
	char *c = a;
	int i;

	for (i = 0; i < len; i++) {
		if (i)
			*c++ = ':';
		c += sprintf(c, "%02X", e[i] & 0xff);
	}
	return a;
}

int
FREAD(unsigned int addr_sa, int len)
{
	unsigned char buffer[MAX_FRW];
	char buffer_h[128];
	memset(buffer, 0, sizeof(buffer));
	memset(buffer_h, 0, sizeof(buffer_h));

	if(FRead((unsigned int *)buffer, addr_sa, len)<0)
		fprintf(stderr, "FREAD: Out of scope\n");
	else
	{
		if (len > MAX_FRW)
			len = MAX_FRW;
		htoa(buffer, buffer_h, len);
		puts(buffer_h);
	}
	return 0;
}

int
FWRITE(char *da, char* str_hex)
{
	unsigned char ee[MAX_FRW];
	unsigned int addr_da;
	int len;
	int i;

	addr_da = strtoul(da, NULL, 16);
	if (addr_da && (len = atoh(str_hex, ee)))
	{
		FWrite(ee, addr_da, len);
		FREAD(addr_da, len);
	}
	return 0;
}
//2008.11 magic}


int
pincheck(const char *a)
{
	unsigned char *c = (char *) a;
	int i = 0;
	int v;

	for (;;) {
		if (*c>0x39 || *c<0x30)
			break;
		else
			i++;
		if (!*c++ || i == 8)
			break;
	}
	return (i == 8);
}

int
setPIN(const char *pin)
{
	if (pincheck(pin))
	{
		FWrite(pin, 0x40100, 8);
		char PIN[9];
		memset(PIN, 0, 9);
		memcpy(PIN, pin, 8);
		puts(PIN);
	}
	return 0;	
}

getBootVer()
{
	unsigned char btv[3];
	memset(btv, 0, sizeof(btv));
	FRead(btv, 0x4009c, 2);
	puts(btv);

	return 0;
}

setBootVer()
{
	char btv[2];
	btv[0] = 0x01;
	btv[1] = 0xa0;
	FWrite(btv, 0x4009c, 2);
	puts(btv);

	return 0;
}

getPIN()
{
	unsigned char PIN[9];
	memset(PIN, 0, sizeof(PIN));
	FRead(PIN, 0x40100, 8);
	if (PIN[0]!=0xff)
		puts(PIN);
	return 0;
}
int gen_ralink_config()
{
	FILE *fp;
	char *str = NULL;
	int  i, ssid_num = 1;
	char wmm_enable[8];
	char wmm_noack[8];
	char macbuf[36];
	char list[2048];
	int flag_8021x=0;
	int wsc_configure = 0;
	int warning = 0;
	int num;

	printf("gen ralink config\n");
	system("mkdir -p /etc/Wireless/RT2860");
	if(!(fp=fopen("/etc/Wireless/RT2860/RT2860.dat", "w+")))
		return 0;

	fprintf(fp, "#The word of \"Default\" must not be removed\n");
	fprintf(fp, "Default\n");

	//CountryRegion
	str = nvram_safe_get("wl_country_code");
	if (str)
	{
		if (    (strcasecmp(str, "CA") == 0) || (strcasecmp(str, "CO") == 0) ||
			(strcasecmp(str, "DO") == 0) || (strcasecmp(str, "GT") == 0) ||
			(strcasecmp(str, "MX") == 0) || (strcasecmp(str, "NO") == 0) ||
			(strcasecmp(str, "PA") == 0) || (strcasecmp(str, "PR") == 0) ||
			(strcasecmp(str, "TW") == 0) || (strcasecmp(str, "US") == 0) ||
			(strcasecmp(str, "UZ") == 0))
			fprintf(fp, "CountryRegion=%d\n", 0);   // channel 1-11
		else if (/*strcasecmp(str, "JP") == 0 || */strcasecmp(str, "DB") == 0 || strcasecmp(str, "")==0)
			fprintf(fp, "CountryRegion=%d\n", 5);   // channel 1-14
		else
			fprintf(fp, "CountryRegion=%d\n", 1);   // channel 1-13
	}
	else
	{
		warning = 1;
		fprintf(fp, "CountryRegion=%d\n", 5);
	}

	//CountryRegion for A band
	str = nvram_safe_get("CountryRegionABand");
	if (str)
		fprintf(fp, "CountryRegionABand=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "CountryRegionABand=%d\n", 7);
	}

	//CountryCode
	str = nvram_safe_get("wl_country_code");
	if (str)
	{
		if (strcmp(str, "") == 0)
			fprintf(fp, "CountryCode=\n");
		else
			fprintf(fp, "CountryCode=%s\n", str);
	}
	else
	{
		warning = 1;
		fprintf(fp, "CountryCode=\n");
	}

	//SSID Num. [MSSID Only]
	str = nvram_safe_get("BssidNum");
	if (str)
	{
		fprintf(fp, "BssidNum=%d\n", atoi(str));
		ssid_num = atoi(str);
	}
	else
	{
		warning = 1;
		fprintf(fp, "BssidNum=%d\n", 1);
		ssid_num = 1;
	}

	//SSID
	str = nvram_safe_get("wl_ssid");
	if (str)
		fprintf(fp, "SSID1=%s\n", str);
	else
	{
		warning = 1;
		fprintf(fp, "SSID1=%s\n", "default");
	}

	fprintf(fp, "SSID2=\n");
	fprintf(fp, "SSID3=\n");
	fprintf(fp, "SSID4=\n");
	fprintf(fp, "SSID5=\n");
	fprintf(fp, "SSID6=\n");
	fprintf(fp, "SSID7=\n");
	fprintf(fp, "SSID8=\n");

	//Network Mode
	str = nvram_safe_get("wl_gmode");
	if (str)
	{
		if (atoi(str)==2)       // B,G,N
			fprintf(fp, "WirelessMode=%d\n", 9);
		else if (atoi(str)==1)  // B,G
			fprintf(fp, "WirelessMode=%d\n", 0);
		else if (atoi(str)==3)  // N
			fprintf(fp, "WirelessMode=%d\n", 6);
		else if (atoi(str)==4)  // G
			fprintf(fp, "WirelessMode=%d\n", 4);
		else if (atoi(str)==0)  // B
			fprintf(fp, "WirelessMode=%d\n", 1);
		else			// B,G,N
			fprintf(fp, "WirelessMode=%d\n", 9);
	}
	else
	{
		warning = 1;
		fprintf(fp, "WirelessMode=%d\n", 9);
	}

	fprintf(fp, "TxRate=%d\n", 0);

	//Channel
	if (nvram_invmatch("sw_mode_ex", "2") && nvram_invmatch("wl_channel", "0"))
	{
		str = nvram_safe_get("wl_channel");

		if (str)
			fprintf(fp, "Channel=%d\n", atoi(str));
		else
		{
			warning = 1;
			fprintf(fp, "Channel=%d\n", 0);
		}
	}

	//BasicRate
	str = nvram_safe_get("wl_rateset");
	if (str)
	{
		if (!strcmp(str, "default"))	// 1, 2, 5.5, 11
			fprintf(fp, "BasicRate=%d\n", 15);
		else if (!strcmp(str, "all"))	// 1, 2, 5.5, 6, 11, 12, 24
			fprintf(fp, "BasicRate=%d\n", 351);
		else if (!strcmp(str, "12"))	// 1, 2
			fprintf(fp, "BasicRate=%d\n", 3);
		else
			fprintf(fp, "BasicRate=%d\n", 15);
	}
	else
	{
		warning = 1;
		fprintf(fp, "BasicRate=%d\n", 15);
	}

	//BeaconPeriod
	str = nvram_safe_get("wl_bcn");
	if (str)
		fprintf(fp, "BeaconPeriod=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "BeaconPeriod=%d\n", 100);
	}

	//DTIM Period
	str = nvram_safe_get("wl_dtim");
	if (str)
		fprintf(fp, "DtimPeriod=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "DtimPeriod=%d\n", 1);
	}

	//TxPower
	str = nvram_safe_get("TxPower");
	if (str)
		fprintf(fp, "TxPower=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "TxPower=%d\n", 100);
	}

	//DisableOLBC
	str = nvram_safe_get("DisableOLBC");
	if (str)
		fprintf(fp, "DisableOLBC=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "DisableOLBC=%d\n", 0);
	}

	//BGProtection
	str = nvram_safe_get("wl_gmode_protection");
	if (str)
	{
		if (!strcmp(str, "auto"))
			fprintf(fp, "BGProtection=%d\n", 0);
		else
			fprintf(fp, "BGProtection=%d\n", 2);
	}
	else
	{
		warning = 1;
		fprintf(fp, "BGProtection=%d\n", 0);
	}

	//TxAntenna
	fprintf(fp, "TxAntenna=\n");
	//str = nvram_safe_get("TxAntenna");
	//fprintf(fp, "TxAntenna=%d\n", atoi(str));

	//RxAntenna
	fprintf(fp, "RxAntenna=\n");
	//str = nvram_safe_get("RxAntenna");
	//fprintf(fp, "RxAntenna=%d\n", atoi(str));

	//TxPreamble
	str = nvram_safe_get("TxPreamble");
	if (str)
		fprintf(fp, "TxPreamble=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "TxPreamble=%d\n", 0);
	}

	//RTSThreshold  Default=2347
	str = nvram_safe_get("wl_rts");
	if (str)
		fprintf(fp, "RTSThreshold=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "RTSThreshold=%d\n", 2347);
	}

	//FragThreshold  Default=2346
	str = nvram_safe_get("wl_frag");
	if (str)
		fprintf(fp, "FragThreshold=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "FragThreshold=%d\n", 2346);
	}

	//TxBurst
	str = nvram_safe_get("TxBurst");
	if (str)
		fprintf(fp, "TxBurst=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "TxBurst=%d\n", 1);
	}

	//PktAggregate
	str = nvram_safe_get("PktAggregate");
	if (str)
		fprintf(fp, "PktAggregate=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "PktAggregate=%d\n", 1);
	}

	fprintf(fp, "TurboRate=%d\n", 0);

	//WmmCapable
	bzero(wmm_enable, sizeof(char)*8);
//	for (i = 0; i < ssid_num; i++)
	{

		str = nvram_safe_get("wl_gmode");
		if (str && atoi(str)==3)	// always enable WMM in N only mode
			sprintf(wmm_enable+strlen(wmm_enable), "%d", 1);
		else
			sprintf(wmm_enable+strlen(wmm_enable), "%d", atoi(nvram_safe_get("wl_wme")));
//		sprintf(wmm_enable+strlen(wmm_enable), "%c", ';');
	}
//	wmm_enable[strlen(wmm_enable) - 1] = '\0';
	wmm_enable[1] = '\0';
	fprintf(fp, "WmmCapable=%s\n", wmm_enable);

	//APAifsn
	fprintf(fp, "APAifsn=%s\n", nvram_safe_get("APAifsn"));
	//APCwmin
	fprintf(fp, "APCwmin=%s\n", nvram_safe_get("APCwmin"));
	//APCwmax
	fprintf(fp, "APCwmax=%s\n", nvram_safe_get("APCwmax"));
	//APTxop
	fprintf(fp, "APTxop=%s\n", nvram_safe_get("APTxop"));
	//APACM
	fprintf(fp, "APACM=%s\n", nvram_safe_get("APACM"));
	//BSSAifsn
	fprintf(fp, "BSSAifsn=%s\n", nvram_safe_get("BSSAifsn"));
	//BSSCwmin
	fprintf(fp, "BSSCwmin=%s\n", nvram_safe_get("BSSCwmin"));
	//BSSCwmax
	fprintf(fp, "BSSCwmax=%s\n", nvram_safe_get("BSSCwmax"));
	//BSSTxop
	fprintf(fp, "BSSTxop=%s\n", nvram_safe_get("BSSTxop"));
	//BSSACM
	fprintf(fp, "BSSACM=%s\n", nvram_safe_get("BSSACM"));
	//AckPolicy
//	fprintf(fp, "AckPolicy=%s\n", nvram_safe_get("AckPolicy"));
	bzero(wmm_noack, sizeof(char)*8);
	for (i = 0; i < 4; i++)
	{
		sprintf(wmm_noack+strlen(wmm_noack), "%d", strcmp(nvram_safe_get("wl_wme_no_ack"), "on")? 0 : 1);
		sprintf(wmm_noack+strlen(wmm_noack), "%c", ';');
	}
	wmm_noack[strlen(wmm_noack) - 1] = '\0';
	fprintf(fp, "AckPolicy=%s\n", wmm_noack);

	//APSDCapable
	str = nvram_safe_get("APSDCapable");
	if (str)
		fprintf(fp, "APSDCapable=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "APSDCapable=%d\n", 0);
	}

	//DLSDCapable
	str = nvram_safe_get("DLSCapable");
	if (str)
		fprintf(fp, "DLSCapable=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "DLSCapable=%d\n", 0);
	}

	//NoForwarding pre SSID & NoForwardingBTNBSSID
	str = nvram_safe_get("wl_ap_isolate");
	if (str)
	{
		fprintf(fp, "NoForwarding=%d\n", atoi(str));
		fprintf(fp, "NoForwardingBTNBSSID=%d\n", atoi(str));
	}
	else
	{
		warning = 1;
		fprintf(fp, "NoForwarding=%d\n", 0);
		fprintf(fp, "NoForwardingBTNBSSID=%d\n", 0);
	}

	//HideSSID
	fprintf(fp, "HideSSID=%s\n", nvram_safe_get("wl_closed"));

	//ShortSlot
	str = nvram_safe_get("ShortSlot");
	if (str)
		fprintf(fp, "ShortSlot=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "ShortSlot=%d\n", 1);
	}

	//AutoChannelSelect
	{
		str = nvram_safe_get("wl_channel");
		if (nvram_match("sw_mode_ex", "2"))
			fprintf(fp, "AutoChannelSelect=%d\n", 1);
		else if (str)
		{
			if (atoi(str)==0)
				fprintf(fp, "AutoChannelSelect=%d\n", 1);
			else
				fprintf(fp, "AutoChannelSelect=%d\n", 0);
		}
		else
		{
			warning = 1;
			fprintf(fp, "AutoChannelSelect=%d\n", 0);
		}
	}

	//AuthMode
	str = nvram_safe_get("wl_auth_mode");
	if (str)
	{
		if (!strcmp(str, "open"))
		{
			//IEEE8021X
			fprintf(fp, "IEEE8021X=%d\n", 0);
			fprintf(fp, "IEEE80211H=%s\n", nvram_safe_get("IEEE80211H"));
			fprintf(fp, "CSPeriod=%s\n", nvram_safe_get("CSPeriod"));
			fprintf(fp, "RDRegion=\n");
			fprintf(fp, "PreAuth=%s\n", nvram_safe_get("PreAuth"));

			fprintf(fp, "AuthMode=%s\n", "OPEN");
		}
		else if (!strcmp(str, "shared"))
		{
			//IEEE8021X
			fprintf(fp, "IEEE8021X=%d\n", 0);
			fprintf(fp, "IEEE80211H=%s\n", nvram_safe_get("IEEE80211H"));
			fprintf(fp, "CSPeriod=%s\n", nvram_safe_get("CSPeriod"));
			fprintf(fp, "RDRegion=\n");
			fprintf(fp, "PreAuth=%s\n", nvram_safe_get("PreAuth"));

			fprintf(fp, "AuthMode=%s\n", "SHARED");
		}
		else if (!strcmp(str, "psk"))
		{
			//IEEE8021X
			fprintf(fp, "IEEE8021X=%d\n", 0);
			fprintf(fp, "IEEE80211H=%s\n", nvram_safe_get("IEEE80211H"));
			fprintf(fp, "CSPeriod=%s\n", nvram_safe_get("CSPeriod"));
			fprintf(fp, "RDRegion=\n");
			fprintf(fp, "PreAuth=%s\n", nvram_safe_get("PreAuth"));

			if(nvram_match("wl_wpa_mode", "0"))
			{
/*
				if (nvram_match("wl_crypto", "tkip"))
					fprintf(fp, "AuthMode=%s\n", "WPAPSK");
				else if (nvram_match("wl_crypto", "aes"))
					fprintf(fp, "AuthMode=%s\n", "WPA2PSK");
				else
*/
					fprintf(fp, "AuthMode=%s\n", "WPAPSKWPA2PSK");
			}
			else if(nvram_match("wl_wpa_mode", "1"))
				fprintf(fp, "AuthMode=%s\n", "WPAPSK");
			else if(nvram_match("wl_wpa_mode", "2"))
				fprintf(fp, "AuthMode=%s\n", "WPA2PSK");
		}
		else if (!strcmp(str, "wpa"))
		{
			//IEEE8021X
			flag_8021x=1;
			fprintf(fp, "IEEE8021X=%d\n", 0);
			fprintf(fp, "IEEE80211H=%s\n", nvram_safe_get("IEEE80211H"));
			fprintf(fp, "CSPeriod=%s\n", nvram_safe_get("CSPeriod"));
			fprintf(fp, "RDRegion=\n");
			fprintf(fp, "PreAuth=%s\n", nvram_safe_get("PreAuth"));

			//if(nvram_match("wl_wpa_mode", "0")) //2008.11 magic
			if(nvram_match("wl_wpa_mode", "4"))
			{
				if (nvram_match("wl_crypto", "tkip"))
					fprintf(fp, "AuthMode=%s\n", "WPA");
				else if (nvram_match("wl_crypto", "aes"))
					fprintf(fp, "AuthMode=%s\n", "WPA2");
				else
					fprintf(fp, "AuthMode=%s\n", "WPA1WPA2");
			}
		//2008.11 magic for new UI{
			//else if(nvram_match("wl_wpa_mode", "1"))
			//	fprintf(fp, "AuthMode=%s\n", "WPA");
			else if(nvram_match("wl_wpa_mode", "3"))
				fprintf(fp, "AuthMode=%s\n", "WPA");
			//else if(nvram_match("wl_wpa_mode", "2"))
			//	fprintf(fp, "AuthMode=%s\n", "WPA2");
			//2008.11 magic for new UI}
		}
		//2008.11 magic for new UI{
		else if (!strcmp(str, "wpa2"))
		{
			//IEEE8021X
			flag_8021x=1;
			fprintf(fp, "IEEE8021X=%d\n", 0);
			fprintf(fp, "IEEE80211H=%s\n", nvram_safe_get("IEEE80211H"));
			fprintf(fp, "CSPeriod=%s\n", nvram_safe_get("CSPeriod"));
			fprintf(fp, "RDRegion=\n");
			fprintf(fp, "PreAuth=%s\n", nvram_safe_get("PreAuth"));

			fprintf(fp, "AuthMode=%s\n", "WPA2");	
		}
		//2008.11 magic for new UI}
		else if (!strcmp(str, "radius"))
		{
			//IEEE8021X
			flag_8021x=1;
			fprintf(fp, "IEEE8021X=%d\n", 1);
			fprintf(fp, "IEEE80211H=%s\n", nvram_safe_get("IEEE80211H"));
			fprintf(fp, "CSPeriod=%s\n", nvram_safe_get("CSPeriod"));
			fprintf(fp, "RDRegion=\n");
			fprintf(fp, "PreAuth=%s\n", nvram_safe_get("PreAuth"));

			fprintf(fp, "AuthMode=%s\n", "OPEN");
		}
		else
		{
			//IEEE8021X
			fprintf(fp, "IEEE8021X=%d\n", 0);
			fprintf(fp, "IEEE80211H=%s\n", nvram_safe_get("IEEE80211H"));
			fprintf(fp, "CSPeriod=%s\n", nvram_safe_get("CSPeriod"));
			fprintf(fp, "RDRegion=\n");
			fprintf(fp, "PreAuth=%s\n", nvram_safe_get("PreAuth"));

			fprintf(fp, "AuthMode=%s\n", "OPEN");
		}
	}
	else
	{
		warning = 2;
		fprintf(fp, "AuthMode=%s\n", "OPEN");
	}

	//EncrypType
	if (	(nvram_match("wl_auth_mode", "open") && nvram_match("wl_wep_x", "0")) /*||
		(nvram_match("wl_auth_mode", "radius") && nvram_match("wl_wep_x", "0"))*/
	)
		fprintf(fp, "EncrypType=%s\n", "NONE");
	else if (       (nvram_match("wl_auth_mode", "open") && nvram_invmatch("wl_wep_x", "0")) ||
			nvram_match("wl_auth_mode", "shared") ||
			nvram_match("wl_auth_mode", "radius")/* ||
			(nvram_match("wl_auth_mode", "radius") && nvram_invmatch("wl_wep_x", "0"))*/
	)
		fprintf(fp, "EncrypType=%s\n", "WEP");
	else if (nvram_match("wl_crypto", "tkip"))
		fprintf(fp, "EncrypType=%s\n", "TKIP");
	else if (nvram_match("wl_crypto", "aes"))
		fprintf(fp, "EncrypType=%s\n", "AES");
	else if (nvram_match("wl_crypto", "tkip+aes"))
		fprintf(fp, "EncrypType=%s\n", "TKIPAES");
	else
	{
		warning = 3;
		fprintf(fp, "EncrypType=%s\n", "NONE");
	}

//	fprintf(fp, "RekeyInterval=%d\n", 3600);
//	fprintf(fp, "RekeyMethod=%s\n", "DISABLE");
//	fprintf(fp, "PMKCachePeriod=%d\n", 10);

	//RekeyInterval
	str = nvram_safe_get("wl_wpa_gtk_rekey");
	if (str)
	{
		fprintf(fp, "RekeyInterval=%d\n", atoi(str));
		if (atoi(str)==0)
			fprintf(fp, "RekeyMethod=%s\n", "DISABLE");
		else
			fprintf(fp, "RekeyMethod=%s\n", nvram_safe_get("RekeyMethod"));
	}
	else
	{
		warning = 4;
		fprintf(fp, "RekeyInterval=%d\n", 0);
		fprintf(fp, "RekeyMethod=%s\n", "DISABLE");
	}

	//PMKCachePeriod
	str = nvram_safe_get("PMKCachePeriod");
	if (str)
		fprintf(fp, "PMKCachePeriod=%d\n", atoi(str));
	else
	{
		warning = 5;
		fprintf(fp, "PMKCachePeriod=%d\n", 10);
	}

	//WPAPSK
	fprintf(fp, "WPAPSK1=%s\n", nvram_safe_get("wl_wpa_psk"));

	fprintf(fp, "WPAPSK2=\n");
	fprintf(fp, "WPAPSK3=\n");
	fprintf(fp, "WPAPSK4=\n");
	fprintf(fp, "WPAPSK5=\n");
	fprintf(fp, "WPAPSK6=\n");
	fprintf(fp, "WPAPSK7=\n");
	fprintf(fp, "WPAPSK8=\n");

	//DefaultKeyID
	fprintf(fp, "DefaultKeyID=%s\n", nvram_safe_get("wl_key"));

	//Key1Type(0 -> Hex, 1->Ascii)
	fprintf(fp, "Key1Type=%s\n", nvram_safe_get("wl_key_type"));
	//Key1Str
	fprintf(fp, "Key1Str1=%s\n", nvram_safe_get("wl_key1"));
	fprintf(fp, "Key1Str2=\n");
	fprintf(fp, "Key1Str3=\n");
	fprintf(fp, "Key1Str4=\n");
	fprintf(fp, "Key1Str5=\n");
	fprintf(fp, "Key1Str6=\n");
	fprintf(fp, "Key1Str7=\n");
	fprintf(fp, "Key1Str8=\n");

	//Key2Type
	fprintf(fp, "Key2Type=%s\n", nvram_safe_get("wl_key_type"));
	//Key2Str
	fprintf(fp, "Key2Str1=%s\n", nvram_safe_get("wl_key2"));
	fprintf(fp, "Key2Str2=\n");
	fprintf(fp, "Key2Str3=\n");
	fprintf(fp, "Key2Str4=\n");
	fprintf(fp, "Key2Str5=\n");
	fprintf(fp, "Key2Str6=\n");
	fprintf(fp, "Key2Str7=\n");
	fprintf(fp, "Key2Str8=\n");

	//Key3Type
	fprintf(fp, "Key3Type=%s\n", nvram_safe_get("wl_key_type"));
	//Key3Str
	fprintf(fp, "Key3Str1=%s\n", nvram_safe_get("wl_key3"));
	fprintf(fp, "Key3Str2=\n");
	fprintf(fp, "Key3Str3=\n");
	fprintf(fp, "Key3Str4=\n");
	fprintf(fp, "Key3Str5=\n");
	fprintf(fp, "Key3Str6=\n");
	fprintf(fp, "Key3Str7=\n");
	fprintf(fp, "Key3Str8=\n");

	//Key4Type
	fprintf(fp, "Key4Type=%s\n", nvram_safe_get("wl_key_type"));
	//Key4Str
	fprintf(fp, "Key4Str1=%s\n", nvram_safe_get("wl_key4"));
	fprintf(fp, "Key4Str2=\n");
	fprintf(fp, "Key4Str3=\n");
	fprintf(fp, "Key4Str4=\n");
	fprintf(fp, "Key4Str5=\n");
	fprintf(fp, "Key4Str6=\n");
	fprintf(fp, "Key4Str7=\n");
	fprintf(fp, "Key4Str8=\n");
/*
	fprintf(fp, "SecurityMode=%d\n", 0);
	fprintf(fp, "VLANEnable=%d\n", 0);
	fprintf(fp, "VLANName=\n");
	fprintf(fp, "VLANID=%d\n", 0);
	fprintf(fp, "VLANPriority=%d\n", 0);
*/
	fprintf(fp, "HSCounter=%d\n", 0);

	//HT_HTC
	str = nvram_safe_get("HT_HTC");
	if (str)
		fprintf(fp, "HT_HTC=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_HTC=%d\n", 1);
	}

	//HT_RDG
	str = nvram_safe_get("HT_RDG");
	if (str)
		fprintf(fp, "HT_RDG=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_RDG=%d\n", 1);
	}

	//HT_LinkAdapt
	str = nvram_safe_get("HT_LinkAdapt");
	if (str)
		fprintf(fp, "HT_LinkAdapt=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_LinkAdapt=%d\n", 0);
	}

	//HT_OpMode
	str = nvram_safe_get("HT_OpMode");
	if (str)
		fprintf(fp, "HT_OpMode=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_OpMode=%d\n", 0);
	}

	//HT_MpduDensity
	str = nvram_safe_get("HT_MpduDensity");
	if (str)
		fprintf(fp, "HT_MpduDensity=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_MpduDensity=%d\n", 5);
	}

	//HT_EXTCHA
	if (nvram_invmatch("sw_mode_ex", "2") && nvram_invmatch("wl_channel", "0"))
	{
		str = nvram_safe_get("HT_EXTCHA");
		if (str)
			fprintf(fp, "HT_EXTCHA=%d\n", atoi(str));
		else
		{
			warning = 1;
			fprintf(fp, "HT_EXTCHA=%d\n", 1);
		}
	}

	//HT_BW
	str = nvram_safe_get("HT_BW");
	if (nvram_match("sw_mode_ex", "2"))
		fprintf(fp, "HT_BW=%d\n", 1);
	else if (str)
		fprintf(fp, "HT_BW=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_BW=%d\n", 1);
	}

	//HT_AutoBA
	str = nvram_safe_get("HT_AutoBA");
	if (str)
		fprintf(fp, "HT_AutoBA=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_AutoBA=%d\n", 1);
	}

	//HT_AMSDU
	str = nvram_safe_get("HT_AMSDU");
	if (str)
		fprintf(fp, "HT_AMSDU=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_AMSDU=%d\n", 0);
	}

	//HT_BAWinSize
	str = nvram_safe_get("HT_BAWinSize");
	if (str)
		fprintf(fp, "HT_BAWinSize=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_BAWinSize=%d\n", 64);
	}

	//HT_GI
	str = nvram_safe_get("HT_GI");
	if (str)
		fprintf(fp, "HT_GI=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_GI=%d\n", 1);
	}

	//HT_STBC
	str = nvram_safe_get("HT_STBC");
	if (str)
		fprintf(fp, "HT_STBC=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_STBC=%d\n", 1);
	}

	//HT_MCS
	str = nvram_safe_get("HT_MCS");
	if (str)
		fprintf(fp, "HT_MCS=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_MCS=%d\n", 33);
	}

	//HT_TxStream
	str = nvram_safe_get("HT_TxStream");
	if (str)
		fprintf(fp, "HT_TxStream=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_TxStream=%d\n", 2);
	}

	//HT_RxStream
	str = nvram_safe_get("HT_RxStream");
	if (str)
		fprintf(fp, "HT_RxStream=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_RxStream=%d\n", 2);
	}

	//HT_BADecline
	str = nvram_safe_get("HT_BADecline");
	if (str)
		fprintf(fp, "HT_BADecline=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_BADecline=%d\n", 0);
	}

	//HT_PROTECT
	str = nvram_safe_get("HT_PROTECT");
	if (str)
		fprintf(fp, "HT_PROTECT=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_PROTECT=%d\n", 1);
	}
/*
	//HT_MIMOPSMode
	str = nvram_safe_get("HT_MIMOPSMode");
	if (str)
		fprintf(fp, "HT_MIMOPSMode=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_MIMOPSMode=%d\n", 3);
	}

	//HT_40MHZ_INTOLERANT
	str = nvram_safe_get("HT_40MHZ_INTOLERANT");
	if (str)
		fprintf(fp, "HT_40MHZ_INTOLERANT=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "HT_40MHZ_INTOLERANT=%d\n", 0);
	}
*/
	//GreenAP

/*	// disable for tmp
	str = nvram_safe_get("GreenAP");
	if (nvram_match("sw_mode_ex", "2"))
		fprintf(fp, "GreenAP=%d\n", 0);
	else if (str)
		fprintf(fp, "GreenAP=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "GreenAP=%d\n", 1);
	}
*/
	fprintf(fp, "GreenAP=0\n");

/*
	fprintf(fp, "NintendoCapable=%d\n", 0);

	//WscConfMode
	str = nvram_safe_get("WscModeOption");
	if (str)
		fprintf(fp, "WscConfMode=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "WscConfMode=%d\n", 0);
	}
*/

	//WscConfStatus
//	str = nvram_safe_get("WscConfigured");
	str = nvram_safe_get("wsc_config_state");
	if (str)
		wsc_configure = atoi(str);
	else
	{
		warning = 1;
		wsc_configure = 0;
	}
	
	if (wsc_configure == 0)
	{
		fprintf(fp, "WscConfMode=%d\n", 0);
		fprintf(fp, "WscConfStatus=%d\n", 1);
	}
	else
	{
		fprintf(fp, "WscConfMode=%d\n", 0);
		fprintf(fp, "WscConfStatus=%d\n", 2);
	}

	
	fprintf(fp, "WscVendorPinCode=%s\n", nvram_safe_get("secret_code"));
//	fprintf(fp, "ApCliWscPinCode=%s\n", nvram_safe_get("secret_code"));	// removed from SDK 3.3.0.0

/*
	fprintf(fp, "WscAKMP=%d\n", 1);
	fprintf(fp, "WscActionIndex=%d\n", 9);
	fprintf(fp, "WscPinCode=\n");
	fprintf(fp, "WscRegResult=%d\n", 1);
	fprintf(fp, "WscUseUPnP=%d\n", 1);
	fprintf(fp, "WscUseUFD=%d\n", 0);
	fprintf(fp, "WscSSID=%s\n", "RalinkInitialAP");
	fprintf(fp, "WscKeyMGMT=%s\n", "WPA-EAP");
	fprintf(fp, "WscConfigMethod=%d\n", 138);
	fprintf(fp, "WscAuthType=%d\n", 1);
	fprintf(fp, "WscEncrypType=%d\n", 1);
	fprintf(fp, "WscNewKey=%s\n", "scaptest");
*/	

	//AccessPolicy0
	str = nvram_safe_get("wl_macmode");
	if (str)
	{
		if (!strcmp(str, "disabled"))
			fprintf(fp, "AccessPolicy0=%d\n", 0);
		else if (!strcmp(str, "allow"))
			fprintf(fp, "AccessPolicy0=%d\n", 1);
		else if (!strcmp(str, "deny"))
			fprintf(fp, "AccessPolicy0=%d\n", 2);
		else
			fprintf(fp, "AccessPolicy0=%d\n", 0);
	}
	else
	{
		warning = 1;
		fprintf(fp, "AccessPolicy0=%d\n", 0);
	}

	list[0]=0;
	list[1]=0;
	if (nvram_invmatch("wl_macmode", "disabled"))
	{
		num = atoi(nvram_safe_get("wl_macnum_x"));
		for(i=0;i<num;i++)
			sprintf(list, "%s;%s", list, mac_conv("wl_maclist_x", i, macbuf));
	}

	//AccessControlLis0t
	fprintf(fp, "AccessControlList0=%s\n", list+1);

	//AccessPolicy1
	str = nvram_safe_get("AccessPolicy1");
	if (str)
		fprintf(fp, "AccessPolicy1=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "AccessPolicy1=%d\n", 0);
	}

	//AccessControlList1
	fprintf(fp, "AccessControlList1=%s\n", nvram_safe_get("AccessControlList1"));

	//AccessPolicy2
	str = nvram_safe_get("AccessPolicy2");
	if (str)
		fprintf(fp, "AccessPolicy2=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "AccessPolicy2=%d\n", 0);
	}

	//AccessControlList2
	fprintf(fp, "AccessControlList2=%s\n", nvram_safe_get("AccessControlList2"));

	//AccessPolicy3
	str = nvram_safe_get("AccessPolicy3");
	if (str)
		fprintf(fp, "AccessPolicy3=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "AccessPolicy3=%d\n", 0);
	}

	//AccessControlList3
	fprintf(fp, "AccessControlList3=%s\n", nvram_safe_get("AccessControlList3"));

	fprintf(fp, "AccessPolicy4=%d\n", 0);
	fprintf(fp, "AccessControlList4=\n");
	fprintf(fp, "AccessPolicy5=%d\n", 0);
	fprintf(fp, "AccessControlList5=\n");
	fprintf(fp, "AccessPolicy6=%d\n", 0);
	fprintf(fp, "AccessControlList6=\n");
	fprintf(fp, "AccessPolicy7=%d\n", 0);
	fprintf(fp, "AccessControlList7=\n");

	//WdsEnable
	str = nvram_safe_get("wl_mode_x");
	if (str)
	{
		if (nvram_match("wl_auth_mode", "open") && nvram_invmatch("sw_mode_ex", "2"))
		{
			if (atoi(str)==0)
				fprintf(fp, "WdsEnable=%d\n", 0);
			else if (atoi(str)==1)
				fprintf(fp, "WdsEnable=%d\n", 2);
			else if (atoi(str)==2)
			{
				if (nvram_match("wl_lazywds", "1"))
					fprintf(fp, "WdsEnable=%d\n", 4);
				else
					fprintf(fp, "WdsEnable=%d\n", 3);
			}
		}
		else
			fprintf(fp, "WdsEnable=%d\n", 0);
	}
	else
	{
		warning = 6;
		fprintf(fp, "WdsEnable=%d\n", 0);
	}

	//WdsEncrypType
	if (nvram_match("wl_auth_mode", "open") && nvram_match("wl_wep_x", "0"))
		fprintf(fp, "WdsEncrypType=%s\n", "NONE");
	else if (nvram_match("wl_auth_mode", "open") && nvram_invmatch("wl_wep_x", "0"))
		fprintf(fp, "WdsEncrypType=%s\n", "WEP");
	else
		fprintf(fp, "WdsEncrypType=%s\n", "NONE");

	//WdsPhyMode
	str = nvram_safe_get("wl_mode_x");
	if (str)
	{
		if (atoi(str)==1)
			fprintf(fp, "WdsPhyMode=%s\n", "HTMIX");
	}

	list[0]=0;
	list[1]=0;
	if (nvram_invmatch("wl_mode_x", "0") && nvram_match("wl_auth_mode", "open") && nvram_invmatch("sw_mode_ex", "2"))
	{
		num = atoi(nvram_safe_get("wl_wdsnum_x"));
		for(i=0;i<num;i++)
			sprintf(list, "%s;%s", list, mac_conv("wl_wdslist_x", i, macbuf));
	}

	//WdsList
	fprintf(fp, "WdsList=%s\n", list+1);

	//WdsKey
	fprintf(fp, "WdsKey=\n");
/*
	if (nvram_match("wl_auth_mode", "psk"))
		fprintf(fp, "WdsKey=%s\n", nvram_safe_get("wl_wpa_psk"));
	else
		fprintf(fp, "WdsKey=\n");
*/

//	fprintf(fp, "WirelessEvent=%d\n", 0);

	//RADIUS_Server
	if (!strcmp(nvram_safe_get("wl_radius_ipaddr"), ""))
		fprintf(fp, "RADIUS_Server=0;0;0;0;0;0;0;0\n");
	else
		fprintf(fp, "RADIUS_Server=%s;0;0;0;0;0;0;0\n", nvram_safe_get("wl_radius_ipaddr"));

	//RADIUS_Port
	str = nvram_safe_get("wl_radius_port");
	if (str)
		fprintf(fp, "RADIUS_Port=%d;%d;%d;%d;%d;%d;%d;%d\n",	atoi(str),
									atoi(str),
									atoi(str),
									atoi(str),
									atoi(str),
									atoi(str),
									atoi(str),
									atoi(str));
	else
	{
		warning = 8;
		fprintf(fp, "RADIUS_Port=%d;%d;%d;%d;%d;%d;%d;%d\n", 1812, 1812, 1812, 1812, 1812, 1812, 1812, 1812);
	}

	//RADIUS_Key
	fprintf(fp, "RADIUS_Key=%s;%s;%s;%s;%s;%s;%s;%s\n", 	nvram_safe_get("wl_radius_key"),
								nvram_safe_get("wl_radius_key"),
								nvram_safe_get("wl_radius_key"),
								nvram_safe_get("wl_radius_key"),
								nvram_safe_get("wl_radius_key"),
								nvram_safe_get("wl_radius_key"),
								nvram_safe_get("wl_radius_key"),
								nvram_safe_get("wl_radius_key")
	);

	//RADIUS_Acct_Server
	fprintf(fp, "RADIUS_Acct_Server=%s\n", nvram_safe_get("RADIUS_Acct_Server"));

	//RADIUS_Acct_Port
	str = nvram_safe_get("RADIUS_Acct_Port");
	if (str)
		fprintf(fp, "RADIUS_Acct_Port=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "RADIUS_Acct_Port=%d\n", 1813);
	}

	//RADIUS_Acct_Key
	fprintf(fp, "RADIUS_Acct_Key=%s\n", nvram_safe_get("RADIUS_Acct_Key"));

	//own_ip_addr
	if (flag_8021x==1)
		fprintf(fp, "own_ip_addr=%s\n", nvram_safe_get("lan_ipaddr"));
	else
		fprintf(fp, "own_ip_addr=\n");

	//Ethifname
//	fprintf(fp, "Ethifname=%s\n", nvram_safe_get("Ethifname"));
	fprintf(fp, "Ethifname=\n");

	//EAPifname
	if (flag_8021x==1)
		fprintf(fp, "EAPifname=%s\n", nvram_safe_get("EAPifname"));
	else
		fprintf(fp, "EAPifname=\n");

	//PreAuthifname
	if (flag_8021x==1)
		fprintf(fp, "PreAuthifname=%s\n", nvram_safe_get("PreAuthifname"));
	else
		fprintf(fp, "PreAuthifname=\n");

	//session_timeout_interval
	str = nvram_safe_get("session_timeout_interval");
	if (str)
		fprintf(fp, "session_timeout_interval=%d\n", atoi(str));
	else
	{
		warning = 9;
		fprintf(fp, "session_timeout_interval=%d\n", 0);
	}

	//idle_timeout_interval
	str = nvram_safe_get("idle_timeout_interval");
	if (str)
		fprintf(fp, "idle_timeout_interval=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "idle_timeout_interval=%d\n", 0);
	}

	fprintf(fp, "SSID=\n");
	fprintf(fp, "WPAPSK=\n");
	fprintf(fp, "Key1Str=\n");
	fprintf(fp, "Key2Str=\n");
	fprintf(fp, "Key3Str=\n");
	fprintf(fp, "Key4Str=\n");

	if (nvram_match("sw_mode_ex", "2") && nvram_invmatch("sta_ssid", ""))
	{
		int flag_wep;

		fprintf(fp, "ApCliEnable=1\n");
		fprintf(fp, "ApCliSsid=%s\n", nvram_safe_get("sta_ssid"));
		fprintf(fp, "ApCliBssid=\n");

		str = nvram_safe_get("sta_auth_mode");
		if (str)
		{
			if (!strcmp(str, "open") && nvram_match("sta_wep_x", "0"))
			{
				fprintf(fp, "ApCliAuthMode=%s\n", "OPEN");
				fprintf(fp, "ApCliEncrypType=%s\n", "NONE");
			}
			else if (!strcmp(str, "open") || !strcmp(str, "shared"))
			{
				flag_wep = 1;
				fprintf(fp, "ApCliAuthMode=%s\n", "WEPAUTO");
				fprintf(fp, "ApCliEncrypType=%s\n", "WEP");
			}
			else if (!strcmp(str, "psk"))
			{
				if(nvram_match("sta_wpa_mode", "1"))
					fprintf(fp, "ApCliAuthMode=%s\n", "WPAPSK");
				else if(nvram_match("sta_wpa_mode", "2"))
					fprintf(fp, "ApCliAuthMode=%s\n", "WPA2PSK");

				//EncrypType
				if (nvram_match("sta_crypto", "tkip"))
					fprintf(fp, "ApCliEncrypType=%s\n", "TKIP");
				else if (nvram_match("sta_crypto", "aes"))
					fprintf(fp, "ApCliEncrypType=%s\n", "AES");

				//WPAPSK
				fprintf(fp, "ApCliWPAPSK=%s\n", nvram_safe_get("sta_wpa_psk"));
			}
			else
			{
				fprintf(fp, "ApCliAuthMode=%s\n", "OPEN");
				fprintf(fp, "ApCliEncrypType=%s\n", "NONE");
			}
		}
		else
		{
			fprintf(fp, "ApCliAuthMode=%s\n", "OPEN");
			fprintf(fp, "ApCliEncrypType=%s\n", "NONE");
		}

		//EncrypType
		if (flag_wep)
		{
			//DefaultKeyID
			fprintf(fp, "ApCliDefaultKeyID=%s\n", nvram_safe_get("sta_key"));

			//KeyType (0 -> Hex, 1->Ascii)
			fprintf(fp, "ApCliKey1Type=%s\n", nvram_safe_get("sta_key_type"));
			fprintf(fp, "ApCliKey2Type=%s\n", nvram_safe_get("sta_key_type"));
			fprintf(fp, "ApCliKey3Type=%s\n", nvram_safe_get("sta_key_type"));
			fprintf(fp, "ApCliKey4Type=%s\n", nvram_safe_get("sta_key_type"));

			//KeyStr
			fprintf(fp, "ApCliKey1Str=%s\n", nvram_safe_get("sta_key1"));
			fprintf(fp, "ApCliKey2Str=%s\n", nvram_safe_get("sta_key2"));
			fprintf(fp, "ApCliKey3Str=%s\n", nvram_safe_get("sta_key3"));
			fprintf(fp, "ApCliKey4Str=%s\n", nvram_safe_get("sta_key4"));
		}
		else
		{
			fprintf(fp, "ApCliDefaultKeyID=0\n");
			fprintf(fp, "ApCliKey1Type=0\n");
			fprintf(fp, "ApCliKey1Str=\n");
			fprintf(fp, "ApCliKey2Type=0\n");
			fprintf(fp, "ApCliKey2Str=\n");
			fprintf(fp, "ApCliKey3Type=0\n");
			fprintf(fp, "ApCliKey3Str=\n");
			fprintf(fp, "ApCliKey4Type=0\n");
			fprintf(fp, "ApCliKey4Str=\n");
		}
	}
	else
	{
		fprintf(fp, "ApCliEnable=0\n");
		fprintf(fp, "ApCliSsid=\n");
		fprintf(fp, "ApCliBssid=\n");
		fprintf(fp, "ApCliAuthMode=\n");
		fprintf(fp, "ApCliEncrypType=\n");
		fprintf(fp, "ApCliWPAPSK=\n");
		fprintf(fp, "ApCliDefaultKeyID=0\n");
		fprintf(fp, "ApCliKey1Type=0\n");
		fprintf(fp, "ApCliKey1Str=\n");
		fprintf(fp, "ApCliKey2Type=0\n");
		fprintf(fp, "ApCliKey2Str=\n");
		fprintf(fp, "ApCliKey3Type=0\n");
		fprintf(fp, "ApCliKey3Str=\n");
		fprintf(fp, "ApCliKey4Type=0\n");
		fprintf(fp, "ApCliKey4Str=\n");
	}

	//WiFiTest
	str = nvram_safe_get("WiFiTest");
	if (str)
		fprintf(fp, "WiFiTest=%d\n", atoi(str));
	else
		fprintf(fp, "WiFiTest=0\n");

	//TGnWifiTest
	str = nvram_safe_get("TGnWifiTest");
	if (str)
		fprintf(fp, "TGnWifiTest=%d\n", atoi(str));
	else
		fprintf(fp, "TGnWifiTest=0\n");

	//IgmpSnEnable
	str = nvram_safe_get("IgmpSnEnable");
	if (str)
		fprintf(fp, "IgmpSnEnable=%d\n", atoi(str));
	else
	{
		warning = 1;
		fprintf(fp, "IgmpSnEnable=%d\n", 1);
	}

	//McastPhyMode, PHY mode for Multicast frames
	//McastMcs, MCS for Multicast frames, ranges from 0 to 7
	/*	MODE=1, MCS=0: Legacy CCK 1Mbps
	 *	MODE=1, MCS=1: Legacy CCK 2Mbps
	 *	MODE=1, MCS=2: Legacy CCK 5.5Mbps
	 *	MODE=1, MCS=3: Legacy CCK 11Mbps
	 *	MODE=2, MCS=0: Legacy OFDM 6Mbps
	 *	MODE=2, MCS=1: Legacy OFDM 9Mbps
	 *	MODE=2, MCS=2: Legacy OFDM 12Mbps
	 *	MODE=2, MCS=3: Legacy OFDM 18Mbps
	 *	MODE=2, MCS=4: Legacy OFDM 24Mbps
	 * 	MODE=2, MCS=5: Legacy OFDM 36Mbps
	 *	MODE=2, MCS=6: Legacy OFDM 48Mbps
	 *	MODE=2, MCS=7: Legacy OFDM 54Mbps
	 **/
	str = nvram_safe_get("wl_mrate");
	if (str)
	{
		if (atoi(str)==0)	// Disable => Auto
		{
//			fprintf(fp, "McastPhyMode=%d\n", 0);
//			fprintf(fp, "McastMcs=%d\n", 0);
		}
		else if (atoi(str)==1)	// Legacy CCK 1Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 1);
			fprintf(fp, "McastMcs=%d\n", 0);
		}
		else if (atoi(str)==2)	// Legacy CCK 2Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 1);
			fprintf(fp, "McastMcs=%d\n", 1);
		}
		else if (atoi(str)==3)	// Legacy CCK 5.5Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 1);
			fprintf(fp, "McastMcs=%d\n", 2);
		}
		else if (atoi(str)==4)	// Legacy CCK 11Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 1);
			fprintf(fp, "McastMcs=%d\n", 3);
		}
		else if (atoi(str)==5)	// Legacy OFDM 6Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 2);
			fprintf(fp, "McastMcs=%d\n", 0);
		}
		else if (atoi(str)==6)	// Legacy OFDM 9Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 2);
			fprintf(fp, "McastMcs=%d\n", 1);
		}
		else if (atoi(str)==7)	// Legacy OFDM 12Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 2);
			fprintf(fp, "McastMcs=%d\n", 2);
		}
		else if (atoi(str)==8)	// Legacy OFDM 18Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 2);
			fprintf(fp, "McastMcs=%d\n", 3);
		}
		else if (atoi(str)==9)	// Legacy OFDM 24Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 2);
			fprintf(fp, "McastMcs=%d\n", 4);
		}
		else if (atoi(str)==10)	// Legacy OFDM 36Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 2);
			fprintf(fp, "McastMcs=%d\n", 5);
		}
		else if (atoi(str)==11)	// Legacy OFDM 48Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 2);
			fprintf(fp, "McastMcs=%d\n", 6);
		}
		else if (atoi(str)==12)	// Legacy OFDM 54Mbps
		{
			fprintf(fp, "McastPhyMode=%d\n", 2);
			fprintf(fp, "McastMcs=%d\n", 7);
		}
		else			// Disable => Auto
		{
//			fprintf(fp, "McastPhyMode=%d\n", 0);
//			fprintf(fp, "McastMcs=%d\n", 0);
		}
	}
	else
	{
		warning = 1;
	}

	if (warning)
	{
		printf("%d!!!!\n", warning);
		printf("Miss some configuration, please check!!!!\n");
	}

	fclose(fp);

	return 0;
}

int
wl_ioctl(const char *ifname, int cmd, struct iwreq *pwrq)
{
	int ret = 0;
 	int s;

	/* open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return errno;
	}

	/* do it */
	strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);
	if ((ret = ioctl(s, cmd, pwrq)) < 0)
		perror(pwrq->ifr_name);

	/* cleanup */
	close(s);
	return ret;
}

PAIR_CHANNEL_FREQ_ENTRY ChannelFreqTable[] = {
	//channel Frequency
	{1,     2412000},
	{2,     2417000},
	{3,     2422000},
	{4,     2427000},
	{5,     2432000},
	{6,     2437000},
	{7,     2442000},
	{8,     2447000},
	{9,     2452000},
	{10,    2457000},
	{11,    2462000},
	{12,    2467000},
	{13,    2472000},
	{14,    2484000},
	{34,    5170000},
	{36,    5180000},
	{38,    5190000},
	{40,    5200000},
	{42,    5210000},
	{44,    5220000},
	{46,    5230000},
	{48,    5240000},
	{52,    5260000},
	{56,    5280000},
	{60,    5300000},
	{64,    5320000},
	{100,   5500000},
	{104,   5520000},
	{108,   5540000},
	{112,   5560000},
	{116,   5580000},
	{120,   5600000},
	{124,   5620000},
	{128,   5640000},
	{132,   5660000},
	{136,   5680000},
	{140,   5700000},
	{149,   5745000},
	{153,   5765000},
	{157,   5785000},
	{161,   5805000},
};

char G_bRadio = 1;
int G_nChanFreqCount = sizeof (ChannelFreqTable) / sizeof(PAIR_CHANNEL_FREQ_ENTRY);

/************************ CONSTANTS & MACROS ************************/

/*
 * Constants fof WE-9->15
 */
#define IW15_MAX_FREQUENCIES	16
#define IW15_MAX_BITRATES	8
#define IW15_MAX_TXPOWER	8
#define IW15_MAX_ENCODING_SIZES	8
#define IW15_MAX_SPY		8
#define IW15_MAX_AP		8

/****************************** TYPES ******************************/

/*
 *	Struct iw_range up to WE-15
 */
struct	iw15_range
{
	__u32		throughput;
	__u32		min_nwid;
	__u32		max_nwid;
	__u16		num_channels;
	__u8		num_frequency;
	struct iw_freq	freq[IW15_MAX_FREQUENCIES];
	__s32		sensitivity;
	struct iw_quality	max_qual;
	__u8		num_bitrates;
	__s32		bitrate[IW15_MAX_BITRATES];
	__s32		min_rts;
	__s32		max_rts;
	__s32		min_frag;
	__s32		max_frag;
	__s32		min_pmp;
	__s32		max_pmp;
	__s32		min_pmt;
	__s32		max_pmt;
	__u16		pmp_flags;
	__u16		pmt_flags;
	__u16		pm_capa;
	__u16		encoding_size[IW15_MAX_ENCODING_SIZES];
	__u8		num_encoding_sizes;
	__u8		max_encoding_tokens;
	__u16		txpower_capa;
	__u8		num_txpower;
	__s32		txpower[IW15_MAX_TXPOWER];
	__u8		we_version_compiled;
	__u8		we_version_source;
	__u16		retry_capa;
	__u16		retry_flags;
	__u16		r_time_flags;
	__s32		min_retry;
	__s32		max_retry;
	__s32		min_r_time;
	__s32		max_r_time;
	struct iw_quality	avg_qual;
};

/*
 * Union for all the versions of iwrange.
 * Fortunately, I mostly only add fields at the end, and big-bang
 * reorganisations are few.
 */
union	iw_range_raw
{
	struct iw15_range	range15;	/* WE 9->15 */
	struct iw_range		range;		/* WE 16->current */
};

/*
 * Offsets in iw_range struct
 */
#define iwr15_off(f)	( ((char *) &(((struct iw15_range *) NULL)->f)) - \
			  (char *) NULL)
#define iwr_off(f)	( ((char *) &(((struct iw_range *) NULL)->f)) - \
			  (char *) NULL)

/* Disable runtime version warning in ralink_get_range_info() */
int iw_ignore_version_sp = 0;

/*------------------------------------------------------------------*/
/*
 * Get the range information out of the driver
 */
int
ralink_get_range_info(iwrange *	range, char* buffer, int length)
{
  union iw_range_raw *	range_raw;

  /* Point to the buffer */
  range_raw = (union iw_range_raw *) buffer;

  /* For new versions, we can check the version directly, for old versions
   * we use magic. 300 bytes is a also magic number, don't touch... */
  if(length < 300)
    {
      /* That's v10 or earlier. Ouch ! Let's make a guess...*/
      range_raw->range.we_version_compiled = 9;
    }

  /* Check how it needs to be processed */
  if(range_raw->range.we_version_compiled > 15)
    {
      /* This is our native format, that's easy... */
      /* Copy stuff at the right place, ignore extra */
      memcpy((char *) range, buffer, sizeof(iwrange));
    }
  else
    {
      /* Zero unknown fields */
      bzero((char *) range, sizeof(struct iw_range));

      /* Initial part unmoved */
      memcpy((char *) range,
	     buffer,
	     iwr15_off(num_channels));
      /* Frequencies pushed futher down towards the end */
      memcpy((char *) range + iwr_off(num_channels),
	     buffer + iwr15_off(num_channels),
	     iwr15_off(sensitivity) - iwr15_off(num_channels));
      /* This one moved up */
      memcpy((char *) range + iwr_off(sensitivity),
	     buffer + iwr15_off(sensitivity),
	     iwr15_off(num_bitrates) - iwr15_off(sensitivity));
      /* This one goes after avg_qual */
      memcpy((char *) range + iwr_off(num_bitrates),
	     buffer + iwr15_off(num_bitrates),
	     iwr15_off(min_rts) - iwr15_off(num_bitrates));
      /* Number of bitrates has changed, put it after */
      memcpy((char *) range + iwr_off(min_rts),
	     buffer + iwr15_off(min_rts),
	     iwr15_off(txpower_capa) - iwr15_off(min_rts));
      /* Added encoding_login_index, put it after */
      memcpy((char *) range + iwr_off(txpower_capa),
	     buffer + iwr15_off(txpower_capa),
	     iwr15_off(txpower) - iwr15_off(txpower_capa));
      /* Hum... That's an unexpected glitch. Bummer. */
      memcpy((char *) range + iwr_off(txpower),
	     buffer + iwr15_off(txpower),
	     iwr15_off(avg_qual) - iwr15_off(txpower));
      /* Avg qual moved up next to max_qual */
      memcpy((char *) range + iwr_off(avg_qual),
	     buffer + iwr15_off(avg_qual),
	     sizeof(struct iw_quality));
    }

  /* We are now checking much less than we used to do, because we can
   * accomodate more WE version. But, there are still cases where things
   * will break... */
  if(!iw_ignore_version_sp)
    {
      /* We don't like very old version (unfortunately kernel 2.2.X) */
      if(range->we_version_compiled <= 10)
	{
	  fprintf(stderr, "Warning: Driver for device %s has been compiled with an ancient version\n", WIF);
	  fprintf(stderr, "of Wireless Extension, while this program support version 11 and later.\n");
	  fprintf(stderr, "Some things may be broken...\n\n");
	}

      /* We don't like future versions of WE, because we can't cope with
       * the unknown */
      if(range->we_version_compiled > WE_MAX_VERSION)
	{
	  fprintf(stderr, "Warning: Driver for device %s has been compiled with version %d\n", WIF, range->we_version_compiled);
	  fprintf(stderr, "of Wireless Extension, while this program supports up to version %d.\n", WE_VERSION);
	  fprintf(stderr, "Some things may be broken...\n\n");
	}

      /* Driver version verification */
      if((range->we_version_compiled > 10) &&
	 (range->we_version_compiled < range->we_version_source))
	{
	  fprintf(stderr, "Warning: Driver for device %s recommend version %d of Wireless Extension,\n", WIF, range->we_version_source);
	  fprintf(stderr, "but has been compiled with version %d, therefore some driver features\n", range->we_version_compiled);
	  fprintf(stderr, "may not be available...\n\n");
	}
      /* Note : we are only trying to catch compile difference, not source.
       * If the driver source has not been updated to the latest, it doesn't
       * matter because the new fields are set to zero */
    }

  /* Don't complain twice.
   * In theory, the test apply to each individual driver, but usually
   * all drivers are compiled from the same kernel. */
  iw_ignore_version_sp = 1;

  return(0);
}
#if 0
/*------------------------------------------------------------------*/
/*
 * Convert our internal representation of frequencies to a floating point.
 */
double
iw_freq2float(const iwfreq *	in)
{
  /* Version without libm : slower */
  int		i;
  double	res = (double) in->m;
  for(i = 0; i < in->e; i++)
    res *= 10;
  return(res);
}

/*------------------------------------------------------------------*/
/*
 * Convert a frequency to a channel (negative -> error)
 */
int
iw_freq_to_channel(double			freq,
		   const struct iw_range *	range)
{
  double	ref_freq;
  int		k;

  /* Check if it's a frequency or not already a channel */
  if(freq < KILO)
    return(-1);

  /* We compare the frequencies as double to ignore differences
   * in encoding. Slower, but safer... */
  for(k = 0; k < range->num_frequency; k++)
    {
      ref_freq = iw_freq2float(&(range->freq[k]));
      if(freq == ref_freq)
	return(range->freq[k].i);
    }
  /* Not found */
  return(-2);
}
#endif
int
getSSID()
{
	struct iwreq wrq;
	wrq.u.data.flags = 0;
	char buffer[33];
	bzero(buffer, sizeof(buffer));
	wrq.u.essid.pointer = (caddr_t) buffer;
	wrq.u.essid.length = IW_ESSID_MAX_SIZE + 1;
	wrq.u.essid.flags = 0;

	if(wl_ioctl(WIF, SIOCGIWESSID, &wrq) < 0)
	{
		fprintf(stderr, "!!!\n");
		return 0;
	}

	if (wrq.u.essid.length>0)
	{
		unsigned char SSID[33];
		memset(SSID, 0, sizeof(SSID));
		memcpy(SSID, wrq.u.essid.pointer, wrq.u.essid.length);
		puts(SSID);
	}

	return 0;
}

int
getChannel()
{
	int channel;
	struct iw_range	range;
	double freq;
	struct iwreq wrq1;
	struct iwreq wrq2;
	char ch_str[3];

	if(wl_ioctl(WIF, SIOCGIWFREQ, &wrq1) < 0)
		return 0;

	char buffer[sizeof(iwrange) * 2];
	bzero(buffer, sizeof(buffer));
	wrq2.u.data.pointer = (caddr_t) buffer;
	wrq2.u.data.length = sizeof(buffer);
	wrq2.u.data.flags = 0;

	if(wl_ioctl(WIF, SIOCGIWRANGE, &wrq2) < 0)
		return 0;

	if(ralink_get_range_info(&range, buffer, wrq2.u.data.length) < 0)
		return 0;

	freq = iw_freq2float(&(wrq1.u.freq));
	if(freq < KILO)
		channel = (int) freq;
	else
	{
		channel = iw_freq_to_channel(freq, &range);
		if(channel < 0)
			return 0;
	}

	memset(ch_str, 0, sizeof(ch_str));
	sprintf(ch_str, "%d", channel);
	puts(ch_str);
	return 0;
}

int
getSiteSurvey()
{
	int retval = 0, i = 0, apCount = 0;
	char data[8192];
	char ssid_str[256];
	char header[128];
	char tmp_wmode[8];
	char tmp_bsstype[4];
	struct iwreq wrq;
	SSA *ssap;

	memset(data, 0x00, 255);
	strcpy(data, "SiteSurvey=1"); 
	wrq.u.data.length = strlen(data)+1; 
	wrq.u.data.pointer = data; 
	wrq.u.data.flags = 0; 

	spinlock_lock(SPINLOCK_SiteSurvey);
	if(wl_ioctl(WIF, RTPRIV_IOCTL_SET, &wrq) < 0)
	{
		spinlock_unlock(0);

		fprintf(stderr, "Site Survey fails\n");
		return 0;
	}
	spinlock_unlock(SPINLOCK_SiteSurvey);

	fprintf(stderr, "Please wait");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".\n\n");

	memset(data, 0, 8192);
	strcpy(data, "");
	wrq.u.data.length = 8192;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	if(wl_ioctl(WIF, RTPRIV_IOCTL_GSITESURVEY, &wrq) < 0)
	{
		fprintf(stderr, "errors in getting site survey result\n");
		return 0;
	}

	memset(header, 0, sizeof(header));
	//sprintf(header, "%-3s%-33s%-18s%-8s%-15s%-9s%-8s%-2s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode", "NT");
	sprintf(header, "%-3s%-33s%-18s%-9s%-16s%-9s%-8s%-2s%-3s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode", "NT", " CC");
	fprintf(stderr, "\n%s", header);

	if(wrq.u.data.length > 0)
	{
		ssap=(SSA *)(wrq.u.data.pointer+strlen(header)+1);
		int len = strlen(wrq.u.data.pointer+strlen(header))-1;
		char *sp, *op;
 		op = sp = wrq.u.data.pointer+strlen(header)+1;

		while(*sp && ((len - (sp-op)) >= 0))
		{
			ssap->SiteSurvey[i].channel[2] = '\0';
			ssap->SiteSurvey[i].ssid[32] = '\0';
			ssap->SiteSurvey[i].bssid[17] = '\0';
			ssap->SiteSurvey[i].encryption[8] = '\0';
			ssap->SiteSurvey[i].authmode[15] = '\0';
			ssap->SiteSurvey[i].signal[8] = '\0';
			ssap->SiteSurvey[i].wmode[7] = '\0';
			ssap->SiteSurvey[i].bsstype[2] = '\0';
			ssap->SiteSurvey[i].centralchannel[2] = '\0';

			sp+=strlen(header);
			apCount=++i;
		}

		for (i=0;i<apCount;i++)
		{
			fprintf(stderr, "%-3s%-33s%-18s%-9s%-16s%-9s%-8s%-2s %-2s\n",
				ssap->SiteSurvey[i].channel,
				(char*)ssap->SiteSurvey[i].ssid,
				ssap->SiteSurvey[i].bssid,
				ssap->SiteSurvey[i].encryption,
				ssap->SiteSurvey[i].authmode,
				ssap->SiteSurvey[i].signal,
				ssap->SiteSurvey[i].wmode,
				ssap->SiteSurvey[i].bsstype,
				ssap->SiteSurvey[i].centralchannel
			);
		}
		fprintf(stderr, "\n");
	}

	return 0;
}

int OidQueryInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	strcpy(wrq.ifr_name, DeviceName);
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode;

#ifdef WIRELESS_EXT > 17
	if ( OidQueryCode == OID_802_11_BSSID_LIST )
		wrq.u.data.length = 8192;
#endif

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}

int OidSetInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	strcpy(wrq.ifr_name, DeviceName);
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode | OID_GET_SET_TOGGLE;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}

unsigned int ConvertRssiToSignalQuality(long RSSI)
{
    unsigned int signal_quality;
    if (RSSI >= -50)
	signal_quality = 100;
    else if (RSSI >= -80)    // between -50 ~ -80dbm
	signal_quality = (unsigned int)(24 + (RSSI + 80) * 2.6);
    else if (RSSI >= -90)   // between -80 ~ -90dbm
	signal_quality = (unsigned int)((RSSI + 90) * 2.6);
    else    // < -84 dbm
	signal_quality = 0;

    return signal_quality;
}
#if 0
char *ConfigMethodTranslate(unsigned short num)
{
	char result[128];
	result[0] = '\0';

	if(num & 0x0001)
		strcat(result, "USBA(Flash Drive),");
	if(num & 0x0002)
		strcat(result, "Ethernet,");
	if(num & 0x0004)
		strcat(result, "Label,");
	if(num & 0x0008)
		strcat(result, "Display,");
	if(num & 0x0010)
		strcat(result, "External NFC Token,");
	if(num & 0x0020)
		strcat(result, "Integrated NFC Token,");
	if(num & 0x0040)
		strcat(result, "NFC Interface,");
	if(num & 0x0080)
		strcat(result, "Push Button,");
	if(num & 0x0100)
		strcat(result, "Keypad");

	return result;
}

char *DevicePasswordIDTranslate(unsigned short num)
{
	char result[32];
	if(num == 0x0000)
		strncpy(result, "PIN (default)", 32);
	else if(num == 0x0001)
		strncpy(result, "User-specified", 32);
	else if(num == 0x0002)
		strncpy(result, "Machine-specified", 32);
	else if(num == 0x0003)
		strncpy(result, "Rekey", 32);
	else if(num == 0x0004)
		strncpy(result, "Push Button", 32);
	else if(num == 0x0005)
		strncpy(result, "Registrar-specified", 32);
	else
		strncpy(result, "Reserved or unknown", 32);
	return result;
}

char *RFBandTranslate(unsigned char rfband)
{
	char result[16];
	strncpy(result, "Unknown", 16);
	switch(rfband){
	case 1:
		strncpy(result, "2.4G", 16);
	case 2:
		strncpy(result, "5G", 16);
	case 3:
		strncpy(result, "2.4G/5G", 16);
	}
	return result;
}

char *PrimaryDeviceTypeTranslate(unsigned short category, unsigned short sub_cat)
{
	char result[32];
	snprintf(result, 32, "Unknown:%d,%d", category, sub_cat);
	switch(category){
	case 1:
		if(sub_cat == 1)
			strncpy(result, "PC", 32);
		if(sub_cat == 2)
			strncpy(result, "Server", 32);
		if(sub_cat == 3)
			strncpy(result, "Media Center", 32);
		break;
	case 2:
		strncpy(result, "Input Device", 32);
		break;
	case 3:
		if(sub_cat == 1)
			strncpy(result, "Printer", 32);
		if(sub_cat == 2)
			strncpy(result, "Scanner", 32);
		break;
	case 4:
		strncpy(result, "Digital Still Camera", 32);
	case 5:
		strncpy(result, "Network Storage", 32);
	case 6:
		if(sub_cat == 1)
			strncpy(result, "Access Point", 32);
		if(sub_cat == 2)
			strncpy(result, "Router", 32);
		if(sub_cat == 3)
			strncpy(result, "Switch", 32);
		break;
	case 7:
		if(sub_cat == 1)
			strncpy(result, "TV", 32);
		if(sub_cat == 2)
			strncpy(result, "Eletronic Picture Frame", 32);
		if(sub_cat == 3)
			strncpy(result, "Projector", 32);
		break;
	case 8:
		if(sub_cat == 1)
			strncpy(result, "Access Point", 32);
		if(sub_cat == 2)
			strncpy(result, "Router", 32);
		if(sub_cat == 3)
			strncpy(result, "Switch", 32);
		break;
	case 9:
		if(sub_cat == 1)
			strncpy(result, "DAR", 32);
		if(sub_cat == 2)
			strncpy(result, "PVR", 32);
		if(sub_cat == 3)
			strncpy(result, "MCX", 32);
		if(sub_cat == 4)
			strncpy(result, "DMR", 32);
		break;
	case 10:
		strncpy(result, "Windows Mobile", 32);

	}
	return result;
}
#endif

// Added new types for OFDM 5G and 2.4G (modified from source/linux-2.6.21.x/drivers/net/wireless/rt2860v2/include/oid.h)
typedef enum _NDIS_802_11_NETWORK_TYPE_COPY
{
	Ndis802_11FH_COPY,
	Ndis802_11DS_COPY,
	Ndis802_11OFDM5_COPY,
	Ndis802_11OFDM5_N_COPY,
	Ndis802_11OFDM24_COPY,
	Ndis802_11OFDM24_N_COPY,
	Ndis802_11Automode_COPY,
	Ndis802_11NetworkTypeMax_COPY	// not a real type, defined as an upper bound
};

// BSS Type definitions (from source/linux-2.6.21.x/drivers/net/wireless/rt2860v2/include/rtmp_def.h)
#define BSS_ADHOC	0	// = Ndis802_11IBSS
#define BSS_INFRA	1	// = Ndis802_11Infrastructure
#define BSS_ANY		2	// = Ndis802_11AutoUnknown
#define BSS_MONITOR	3	// = Ndis802_11Monitor

int getWPSAP()
{
	int s, ret, retry;
	unsigned int lBufLen = 65536; // 64K
	PNDIS_802_11_BSSID_LIST_EX pBssidList;
	PNDIS_WLAN_BSSID_EX pBssid;
	unsigned int ConnectStatus = 0;
	unsigned char BssidQuery[6];

	s = socket(AF_INET, SOCK_DGRAM, 0);
	pBssidList = (PNDIS_802_11_BSSID_LIST_EX) malloc(65536);  //64k
	memset(pBssidList, 0x00, sizeof(char)*65536);

	//step 1
	if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, "ra0", &ConnectStatus, sizeof(ConnectStatus)) < 0) {
		fprintf(stderr, "Query OID_GEN_MEDIA_CONNECT_STATUS failed!");
		free(pBssidList); close(s);
		return -1;
	}

	//step 2
	if (OidQueryInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0) {
		fprintf(stderr, "Query RT_OID_802_11_RADIO failed!");
		free(pBssidList); close(s);
		return -1;
	}

	if (ConnectStatus == 1 && G_bRadio) {
		// for handshake
		memset(&BssidQuery, 0x00, sizeof(BssidQuery));
		OidQueryInformation(OID_802_11_BSSID, s, "ra0", &BssidQuery, sizeof(BssidQuery));
	}

	//step 3
	if (OidSetInformation(OID_802_11_BSSID_LIST_SCAN, s, "ra0", 0, 0) < 0) {
		fprintf(stderr, "Set OID_802_11_BSSID_LIST_SCAN failed!");
		free(pBssidList); close(s);
		return -1;
	}
	// wait a few seconds to get all AP.
	sleep(2);

	for (retry = 0; retry < 5; retry++) {
		ret = OidQueryInformation(OID_802_11_BSSID_LIST, s, "ra0", pBssidList, lBufLen);
		if (ret < 0) {
			if (retry < 4) {
				sleep(1);
				continue;
			}
			fprintf(stderr, "Query OID_802_11_BSSID_LIST error! return=%d", ret);
			free(pBssidList);
			pBssidList = NULL;
			close(s);
			return -1;
		}
		break;
	}

	unsigned char tmpRadio[188], tmpBSSIDII[18], tmpSSID[64+NDIS_802_11_LENGTH_SSID],/* tmpRSSI[16], tmpChannel[16],*/ tmpAuth[32], tmpEncry[20];
	unsigned char tmpSSIDII[NDIS_802_11_LENGTH_SSID];
	unsigned char version = 0x0, wsc_state=0, ap_setup_lock=0, selected_registrar=0, uuid_e[16], rf_band=0;
	unsigned short device_password_id=0, selected_registrar_config_method=0, primary_device_type_category=0, primary_device_type_subcategory=0;
	unsigned int IEFlags = 0;
	unsigned char extend[1024];

#define TESTBIT(x) (x & IEFlags)

	int i=0, j=0;
	unsigned int nSigQua;
	int nChannel = 1;
	unsigned char radiocheck[8];
	int wps_ap_flag;
	int connected;

	fprintf(stderr, "%-3s%-33s%-18s%-9s%-16s%-9s%-8s%-3s%-4s%-5s%-3s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode", "NT", "WPS", "Conf", "PW");

	pBssid = (PNDIS_WLAN_BSSID_EX) pBssidList->Bssid;
	for (i = 0; i < pBssidList->NumberOfItems; i++)
	{
		memset(radiocheck, 0x00, sizeof(radiocheck));
		memset(tmpRadio, 0x00, sizeof(tmpRadio));
//		memset(tmpRSSI, 0x00, sizeof(tmpRSSI));
		memset(tmpSSID, 0x00, sizeof(tmpSSID));
//		memset(tmpChannel, 0x00, sizeof(tmpChannel));
		memset(tmpAuth, 0x00, sizeof(tmpAuth));
		memset(tmpEncry, 0x00, sizeof(tmpEncry));
		memset(tmpBSSIDII, 0x00, sizeof(tmpBSSIDII));
		memset(tmpSSIDII, 0x00, sizeof(tmpSSIDII));
		memset(extend, 0x00, sizeof(extend));
		wps_ap_flag = 0;
		connected = 0;

		// compare BSSID with connected bssid
		if (memcmp(BssidQuery, pBssid->MacAddress, 6) == 0)
			connected = 1;

/*
		if (strcmp((char *)pBssid->Ssid.Ssid, "") == 0)
			sprintf((char *)tmpSSID, "%s", "&nbsp;");
		else*/ {
			int i = 0;
			do {/*
				if (pBssid->Ssid.Ssid[i] < 32 || pBssid->Ssid.Ssid[i] > 126 || pBssid->Ssid.Ssid[i] == 13) // 13 is string end of Dos
				{
					strncpy(tmpSSID, "&nbsp;", sizeof(tmpSSID));
					break;
				}else if(pBssid->Ssid.Ssid[i] == '\\'){
					snprintf(tmpSSID, sizeof(tmpSSID), "%s%s", tmpSSID, "\\\\");
				}else*/
					snprintf(tmpSSID, sizeof(tmpSSID), "%s%c", tmpSSID, pBssid->Ssid.Ssid[i]);
				i++;
			} while(i < pBssid->Ssid.SsidLength);
			tmpSSID[pBssid->Ssid.SsidLength] = '\0';
		}

		sprintf((char *)tmpBSSIDII, "%02X:%02X:%02X:%02X:%02X:%02X", pBssid->MacAddress[0], pBssid->MacAddress[1], pBssid->MacAddress[2], pBssid->MacAddress[3], pBssid->MacAddress[4], pBssid->MacAddress[5]);

		nSigQua = ConvertRssiToSignalQuality(pBssid->Rssi);
//		sprintf((char *)tmpRSSI,"%d", nSigQua);

		nChannel = -1;	
		for(j = 0; j < G_nChanFreqCount; j++){
			if (pBssid->Configuration.DSConfig == ChannelFreqTable[j].lFreq) {
				nChannel = ChannelFreqTable[j].lChannel;
				break;
			}
		}
		if (nChannel == -1)
			continue;
//		sprintf((char *)tmpChannel, "%u", nChannel);
		
		// work with NDIS_WLAN_BSSID_EX
		unsigned char bTKIP = FALSE;
		unsigned char bAESWRAP = FALSE;
		unsigned char bAESCCMP = FALSE;
		unsigned char bWPA = FALSE;
		unsigned char bWPAPSK = FALSE;
		unsigned char bWPANONE = FALSE;
		unsigned char bWPA2 = FALSE;
		unsigned char bWPA2PSK = FALSE;
		unsigned char bWPA2NONE = FALSE;
		unsigned char bCCKM = FALSE; // CCKM for Cisco, add by candy 2006.11.24

		version = 0x0;
		wsc_state = 0;
		ap_setup_lock = 0xff;
		selected_registrar = 0xff;
		memset(uuid_e, 16, 0);
		rf_band = 0xff;
		device_password_id = 0xffff;
		selected_registrar_config_method = 0xffff;
		IEFlags = 0;

		if ((pBssid->Length > sizeof(NDIS_WLAN_BSSID)) && (pBssid->IELength > sizeof(NDIS_802_11_FIXED_IEs)))
		{
			unsigned int lIELoc = 0;
			PNDIS_802_11_FIXED_IEs pFixIE = (PNDIS_802_11_FIXED_IEs)pBssid->IEs;
			PNDIS_802_11_VARIABLE_IEs pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pFixIE + sizeof(NDIS_802_11_FIXED_IEs));
			lIELoc += sizeof(NDIS_802_11_FIXED_IEs);

			while (pBssid->IELength > (lIELoc + sizeof(NDIS_802_11_VARIABLE_IEs)))
			{
				/* WPS section  WPS OUI: 00 50 F2 04 , little endian form */

				if ((pVarIE->ElementID == 221) && *((unsigned int*)((char*)pVarIE->data)) == 0x04f25000)
				{
					unsigned char *pos, *data_head;
					unsigned short len_in_hdr;
					unsigned char tot_len =  pVarIE->Length;

					data_head = pos = (unsigned char*)pVarIE->data + 4;
					
					/* TLV:  T=2bytes, L=2bytes, V=... */
					while(pos < data_head + tot_len - 4)
					{
						len_in_hdr = ntohs(*(unsigned short *)(pos+2));

						if(tot_len - (pos - data_head) < len_in_hdr){
							printf("Error: crafted WSC packet? tot_len = %d, len_in_hdr = %d\n", tot_len, len_in_hdr);
							break;
						}

						switch( ntohs(*(unsigned short *)pos)){
							case WSC_ID_VERSION:
								if(len_in_hdr != WSC_ID_VERSION_LEN) {
									printf("Error: crafted WSC packet? WSC_ID_VERSION.\n");
									goto WSC_FAILED;
								}
								IEFlags |= ntohs(*(unsigned short *)pos);
								version = *(unsigned char *)(pos + 2 + 2);
								break;

							case WSC_ID_SC_STATE:
								if(len_in_hdr != WSC_ID_SC_STATE_LEN) {
									printf("Error: crafted WSC packet? WSC_ID_SC_STATE.\n");
									goto WSC_FAILED;
								}
								IEFlags |= ntohs(*(unsigned short *)pos);
								wsc_state = *(unsigned char *)(pos + 2 + 2);
								break;
							
							case WSC_ID_AP_SETUP_LOCKED:
								if(len_in_hdr != WSC_ID_AP_SETUP_LOCKED_LEN) {
									printf("Error: crafted WSC packet? WSC_ID_AP_SETUP_LOCKED.\n");
									goto WSC_FAILED;
								}
								IEFlags |= ntohs(*(unsigned short *)pos);
								ap_setup_lock = *(unsigned char *)(pos + 2 + 2);
								break;

							case WSC_ID_SEL_REGISTRAR:
								if(len_in_hdr != WSC_ID_SEL_REGISTRAR_LEN) {
									printf("Error: crafted WSC packet? WSC_ID_SEL_REGISTRAR.\n");
									goto WSC_FAILED;
								}
								IEFlags |= ntohs(*(unsigned short *)pos);
								selected_registrar = *(unsigned char *)(pos + 2 + 2);
								break;

							case WSC_ID_DEVICE_PWD_ID:
								if(len_in_hdr != WSC_ID_DEVICE_PWD_ID_LEN) {
									printf("Error: crafted WSC packet? WSC_ID_DEVICE_PWD_ID.\n");
									goto WSC_FAILED;
								}
								IEFlags |= ntohs(*(unsigned short *)pos);
								device_password_id = ntohs(*(unsigned short *)(pos + 2 + 2));
								break;

							case WSC_ID_SEL_REG_CFG_METHODS:
								if(len_in_hdr != WSC_ID_SEL_REG_CFG_METHODS_LEN) {
									printf("Error: crafted WSC packet? WSC_ID_SEL_REG_CFG_METHODS.\n");
									goto WSC_FAILED;
								}
								IEFlags |= ntohs(*(unsigned short *)pos);
								selected_registrar_config_method = ntohs(*(unsigned short *)(pos + 2 + 2));
								break;

							case WSC_ID_UUID_E:
								if(len_in_hdr != WSC_ID_UUID_E_LEN) {
									printf("Error: crafted WSC packet? WSC_ID_UUID_E.\n");
									goto WSC_FAILED;
								}
								IEFlags |= ntohs(*(unsigned short *)pos);
								memcpy(uuid_e, (unsigned char *)pos + 2 + 2, 16);
								break;

							case WSC_ID_RF_BAND:
								if(len_in_hdr != WSC_ID_RF_BAND_LEN) {
									printf("Error: crafted WSC packet? WSC_ID_RF_BAND.\n");
									goto WSC_FAILED;
								}
								IEFlags |= ntohs(*(unsigned short *)pos);
								rf_band = *(unsigned char *)(pos + 2 + 2);
								break;

							case WSC_ID_PRIMARY_DEVICE_TYPE:
								if(len_in_hdr != WSC_ID_PRIMARY_DEVICE_TYPE_LEN) {
									printf("Error: crafted WSC packet? WSC_ID_PRIMARY_DEVICE_TYPE.\n");
									goto WSC_FAILED;
								}
								IEFlags |= ntohs(*(unsigned short *)pos);
								primary_device_type_category =  *(unsigned short *)(pos + 2 + 2);
								primary_device_type_subcategory =  *(unsigned short *)(pos + 2 + 2 + 2 + 2);
							default:
//								printf("unknown tlv:%04x\n", ntohs(*(unsigned short *)pos));
								break;

						}
						pos = pos+2+2+ len_in_hdr;
					}

					if( ! TESTBIT(WSC_ID_VERSION) || ! TESTBIT( WSC_ID_SC_STATE)){
						printf("No Version and WPS setup state\n.");
						break;
					}

					wps_ap_flag = 1;
				}
WSC_FAILED:

				if ((pVarIE->ElementID == 221) && (pVarIE->Length >= 16))
				{
					//UINT* pOUI = (UINT*)((char*)pVarIE + 2);
					unsigned int* pOUI = (unsigned int*)((char*)pVarIE->data);
					//fprintf(stderr, "pOUI=0x%08x\n", pOUI);
					if (*pOUI != WPA_OUI_TYPE)
						break;
					unsigned int* plGroupKey; 
					unsigned short* pdPairKeyCount;
					unsigned int* plPairwiseKey=NULL;
					unsigned int* plAuthenKey=NULL;
					unsigned short* pdAuthenKeyCount;
					plGroupKey = (unsigned int*)((char*)pVarIE + 8);
			
					unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
					//fprintf(stderr, "lGroupKey=%d\n", lGroupKey);
					if (lGroupKey == WPA_OUI) {
						lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
						if (lGroupKey == 2)
							bTKIP = TRUE;
						else if (lGroupKey == 3)
							bAESWRAP = TRUE;
						else if (lGroupKey == 4)
							bAESCCMP = TRUE;
					}
					else
						break;
			
					pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
					plPairwiseKey = (unsigned int*) ((char*)pdPairKeyCount + 2);
					unsigned short k = 0;
					for (k = 0; k < *pdPairKeyCount; k++) {
						unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
						if (lPairKey == WPA_OUI )//|| (lPairKey & 0xffffff00) == WPA_OUI_1)
						{
							lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
							if (lPairKey == 2)
								bTKIP = TRUE;
							else if (lPairKey == 3)
								bAESWRAP = TRUE;
							else if (lPairKey == 4)
								bAESCCMP = TRUE;
						}
						else
							break;
						
						plPairwiseKey++;
					}
			
					pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * (*pdPairKeyCount));
					plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);

					for(k = 0; k < *pdAuthenKeyCount; k++)
					{
						unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
						if (lAuthenKey == CISCO_OUI) {
							bCCKM = TRUE; // CCKM for Cisco
						}
						else if (lAuthenKey == WPA_OUI) {
							lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;

							if (lAuthenKey == 1)
								bWPA = TRUE;
							else if (lAuthenKey == 0 || lAuthenKey == 2) {
								if (pBssid->InfrastructureMode)
									bWPAPSK = TRUE;
								else
									bWPANONE = TRUE;
							}
						}					
						plAuthenKey++;
					}
				//break;
				}
				else if (pVarIE->ElementID == 48 && pVarIE->Length >= 12)
				{
					unsigned int* plGroupKey; 
					unsigned int* plPairwiseKey; 
					unsigned short* pdPairKeyCount;
					unsigned int* plAuthenKey; 
					unsigned short* pdAuthenKeyCount;
					plGroupKey = (unsigned int*)((char*)pVarIE + 4);

					unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
					if (lGroupKey == WPA2_OUI) {
						lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
						if (lGroupKey == 2)
							bTKIP = TRUE;
						else if (lGroupKey == 3)
							bAESWRAP = TRUE;
						else if (lGroupKey == 4)
							bAESCCMP = TRUE;
					}
					else
						break;

					pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
					plPairwiseKey = (unsigned int*)((char*)pdPairKeyCount + 2);
					unsigned short k = 0;

					for (k = 0; k < *pdPairKeyCount; k++)
					{
						unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
						if (lPairKey == WPA2_OUI) {
							lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
							if (lPairKey == 2)
								bTKIP = TRUE;
							else if (lPairKey == 3)
								bAESWRAP = TRUE;
							else if (lPairKey == 4)
								bAESCCMP = TRUE;
						}
						else
							break;
						plPairwiseKey++;
					}
		
					pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * *pdPairKeyCount);
					plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);
					for (k = 0; k < *pdAuthenKeyCount; k++)
					{
						unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
						if (lAuthenKey == CISCO_OUI) {
							bCCKM = TRUE; // CCKM for Cisco
						}
						else if (lAuthenKey == WPA2_OUI) {
							lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;
							if (lAuthenKey == 1)
								bWPA2 = TRUE;
							else if (lAuthenKey == 0 || lAuthenKey == 2) {
								if (pBssid->InfrastructureMode)
									bWPA2PSK = TRUE;
								else
									bWPA2NONE = TRUE;
							}
						}					
						plAuthenKey++;
					}
				}
		
				lIELoc += pVarIE->Length;
				lIELoc += 2;
				pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

				if (pVarIE->Length <= 0)
					break;
			}
		}
		
		char strAuth[32], strEncry[32];
		memset( strAuth, 0x00, sizeof(strAuth) );
		memset( strEncry, 0x00, sizeof(strEncry) );
/*
		if (bCCKM)
			strcpy(strAuth, "CCKM; ");
		if (bWPA)
			strcpy(strAuth, "WPA; ");
		if (bWPAPSK)
			strcat(strAuth, "WPA-PSK; ");
		if (bWPANONE)
			strcat(strAuth, "WPA-NONE; ");
		if (bWPA2)
			strcat(strAuth, "WPA2; ");
		if (bWPA2PSK)
			strcat(strAuth, "WPA2-PSK; ");
		if (bWPA2NONE)
			strcat(strAuth, "WPA2-NONE; ");
		
		if (strlen(strAuth) > 0) {
			strncpy((char *)tmpAuth, strAuth, strlen(strAuth) - 2);
			strcpy(strAuth, (char *)tmpAuth);
		}else {
			strcpy((char *)strAuth, "Unknown");
		}
*/
		if (bWPA2)
			strcat(strAuth, "WPA2");
		else if (bWPA)
			strcpy(strAuth, "WPA");
		else if (bWPA2PSK)
			strcat(strAuth, "WPA2-PSK");
		else if (bWPAPSK)
			strcat(strAuth, "WPA-PSK");
		else if (bCCKM)
			strcpy(strAuth, "CCKM");
		else if (bWPA2NONE)
			strcat(strAuth, "WPA2-NONE");
		else if (bWPANONE)
			strcat(strAuth, "WPA-NONE");
		else {
			strcpy((char *)strAuth, "Unknown");
		}

/*
		if (bTKIP)
			strcpy(strEncry, "TKIP; ");
		if (bAESWRAP || bAESCCMP)
			strcat(strEncry, "AES; ");

		if (strlen(strEncry) > 0) {
			strncpy((char *)tmpEncry, strEncry, strlen(strEncry) - 2);
			strcpy(strEncry, (char *)tmpEncry);
		}else {
			if (pBssid->Privacy)  // privacy value is on/of
				strcpy(strEncry, "WEP");
			else {
				strcpy(strEncry, "Not Use");
				strcpy(strAuth, "OPEN");
			}
		}
*/

		if (bAESWRAP || bAESCCMP)
			strcat(strEncry, "AES");
		else if (bTKIP)
			strcpy(strEncry, "TKIP");
		else {
			if (pBssid->Privacy)  // privacy value is on/of
				strcpy(strEncry, "WEP");
			else {
				strcpy(strEncry, "Not Use");
				strcpy(strAuth, "OPEN");
			}
		}

		sprintf((char *)tmpAuth, "%s", strAuth);
		sprintf((char *)tmpEncry, "%s", strEncry);

		strcpy((char *)tmpSSIDII, pBssid->Ssid.Ssid);

		/*
		 *  "extend" will become a literal part of Javascript, so we use "\\n" instead of "\n".
		 */
/*		 
		if( TESTBIT( WSC_ID_SEL_REG_CFG_METHODS)){
			strncat(extend, "Config Method:", sizeof(extend));
			snprintf(extend, sizeof(extend), "%s%d\\n", extend, selected_registrar_config_method);
		}
*/
		if( TESTBIT( WSC_ID_DEVICE_PWD_ID)){
			strncat(extend, "Device Password:", sizeof(extend));
//			snprintf(extend, sizeof(extend), "%s%s\\n", extend, DevicePasswordIDTranslate(device_password_id));
			snprintf(extend, sizeof(extend), "%s%d", extend, device_password_id);
		}
/*
		if( TESTBIT( WSC_ID_SEL_REGISTRAR)){
			strncat(extend, "Seleted Registrar:", sizeof(extend));
			snprintf(extend, sizeof(extend), "%s%d\\n", extend, selected_registrar);
		}

		if( TESTBIT( WSC_ID_AP_SETUP_LOCKED)){
			strncat(extend, "AP Locked:", sizeof(extend));
			snprintf(extend, sizeof(extend), "%s%d\\n", extend, ap_setup_lock);
		}

		if( TESTBIT( WSC_ID_UUID_E)){
			int pos;
			char uuid_str[WSC_ID_UUID_E_LEN * 2 + 1];
			memset(uuid_str, 0, WSC_ID_UUID_E_LEN * 2 + 1);
			strncat(extend, "UUID:", sizeof(extend));
			for(pos=0; pos< WSC_ID_UUID_E_LEN; pos++){
				snprintf(uuid_str, WSC_ID_UUID_E_LEN * 2 + 1, "%s%02x", uuid_str, uuid_e[pos]);
			}
			snprintf(extend, sizeof(extend), "%s%s\\n", extend, uuid_str);
		}

		if( TESTBIT( WSC_ID_PRIMARY_DEVICE_TYPE )){
			strncat(extend, "Primary Device Type:", sizeof(extend));
			snprintf(extend, sizeof(extend), "%s%s", extend, PrimaryDeviceTypeTranslate(primary_device_type_category, primary_device_type_subcategory));
		}
*/
		/* This space is important for seperating from two possible closed '\n' characters. */
		strncat(extend, " ", sizeof(extend));

		if(!wps_ap_flag || (TESTBIT( WSC_ID_RF_BAND) && rf_band!=2)){
//			fprintf(stderr, "%s,%s,%s,%s,%s,%s,%d,%d,%s\n", tmpSSID, tmpBSSIDII, tmpRSSI, tmpChannel, tmpAuth, tmpEncry, connected, wsc_state, extend);

			if (connected)
				fprintf(stderr, "%-2d*", nChannel);
			else
				fprintf(stderr, "%-3d", nChannel);
			fprintf(stderr, "%-33s", tmpSSID);
			fprintf(stderr, "%s ", tmpBSSIDII);
			fprintf(stderr, "%-9s", tmpEncry);
			fprintf(stderr, "%-16s", tmpAuth);
			fprintf(stderr, "%-9d", nSigQua);
			if (pBssid->NetworkTypeInUse == Ndis802_11FH_COPY || pBssid->NetworkTypeInUse == Ndis802_11DS_COPY)
				fprintf(stderr, "%-8s", "11b");
			else if (pBssid->NetworkTypeInUse == Ndis802_11OFDM5_COPY)
				fprintf(stderr, "%-8s", "11a");
			else if (pBssid->NetworkTypeInUse == Ndis802_11OFDM5_N_COPY)
				fprintf(stderr, "%-8s", "11a/n");
			else if (pBssid->NetworkTypeInUse == Ndis802_11OFDM24_COPY)
				fprintf(stderr, "%-8s", "11b/g");
			else if (pBssid->NetworkTypeInUse == Ndis802_11OFDM24_N_COPY)
				fprintf(stderr, "%-8s", "11b/g/n");
			else
				fprintf(stderr, "%-8s", "unknown");	
			if (pBssid->InfrastructureMode == BSS_ADHOC)
				fprintf(stderr, "%-3s", "Ad");
			else
				fprintf(stderr, "%-3s", "In");
			if (wps_ap_flag)
			{
				fprintf(stderr, "%-4s", "Yes");

				if (wsc_state == 0x1)
					fprintf(stderr, "%-5s", "No");
				else if (wsc_state == 0x2)
					fprintf(stderr, "%-5s", "Yes");
				else
					fprintf(stderr, "%-5s", "?");

				if (device_password_id == 0x0)
					fprintf(stderr, "%-3s", "PIN");
				else if (device_password_id == 0x4)
					fprintf(stderr, "%-3s", "PBC");
				else
					fprintf(stderr, "%-3s", "");
			}
			else
				fprintf(stderr, "%-4s", "No");

			fprintf(stderr, "\n");
		}

		pBssid = (PNDIS_WLAN_BSSID_EX)((char *)pBssid + pBssid->Length);
	}
	free(pBssidList);
	close(s);

	return 0;
}

int is_URE()
{
	DIR *dir_to_open = NULL;

	dir_to_open = opendir("/sys/class/net/apcli0");
	if (dir_to_open)
	{
		closedir(dir_to_open);
		return 1;
	}
		return 0;
}

int getBSSID()	// get AP's BSSID
{
	unsigned char data[MACSIZE];
	char macaddr[18];
	struct iwreq wrq;

	memset(data, 0x00, MACSIZE);
	wrq.u.data.length = MACSIZE;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = OID_802_11_BSSID;

	if(wl_ioctl(WIF, RT_PRIV_IOCTL, &wrq) < 0)
	{
		fprintf(stderr, "errors in getting bssid!\n");
		return -1;
	}
	else
	{
		ether_etoa(data, macaddr);
		puts(macaddr);
		return 0;
	}
}

int setDisassociate()
{
	struct iwreq wrq;

	wrq.u.data.length = 0;
	wrq.u.data.pointer = NULL;
	wrq.u.data.flags = OID_802_11_DISASSOCIATE | OID_GET_SET_TOGGLE;

	if(wl_ioctl(WIF, RT_PRIV_IOCTL, &wrq) < 0)
	{
		fprintf(stderr, "errors in DISASSOCIATE!\n");
		return -1;
	}
	else
	{
		fprintf(stderr, "DISASSOCIATE succeeds!\n");
		return 0;
	}
}

int getCurrentAddress()		// get STA's MAC
{
	unsigned char CurrentAddress[MACSIZE];
	char macaddr[18];
	struct iwreq wrq;

	memset(CurrentAddress, 0, MACSIZE);
	wrq.u.data.length = MACSIZE;
	wrq.u.data.pointer = CurrentAddress;
	wrq.u.data.flags = OID_802_3_CURRENT_ADDRESS;

	if(wl_ioctl(WIF, RT_PRIV_IOCTL, &wrq) < 0)
	{
		fprintf(stderr, "errors in getting current address!\n");
		return -1;
	}
	else
	{
		ether_etoa(CurrentAddress, macaddr);
		puts(macaddr);
		return 0;
	}
}

int getStaConnectionSSID()
{
	int ConnectStatus = 0;
	unsigned char Connection_flag = 0;
	NDIS_802_11_SSID SsidQuery;
	unsigned char BssidQuery[6];
	char strSSID[NDIS_802_11_LENGTH_SSID + 1];
	int  s, ret;

	s = socket(AF_INET, SOCK_DGRAM, 0);

	//step 1
	if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, "ra0", &ConnectStatus, sizeof(ConnectStatus)) < 0) {
		fprintf(stderr, "Query OID_GEN_MEDIA_CONNECT_STATUS error!");
		close(s);
		return -1;
	}

	//step 2
	if (OidQueryInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0) {
		fprintf(stderr, "Query RT_OID_802_11_RADIO error!");
		close(s);
		return -1;
	}

	if (ConnectStatus == 1 && G_bRadio) {
		memset(&SsidQuery, 0x00, sizeof(SsidQuery));
		OidQueryInformation(OID_802_11_SSID, s, "ra0", &SsidQuery, sizeof(SsidQuery));

		if (SsidQuery.SsidLength == 0) {
			memset(&BssidQuery, 0x00, sizeof(BssidQuery));
			ret = OidQueryInformation(OID_802_11_BSSID, s, "ra0", &BssidQuery, sizeof(BssidQuery));
			fprintf(stderr, "Connected <--> [%02X:%02X:%02X:%02X:%02X:%02X]\n",
					BssidQuery[0], BssidQuery[1], BssidQuery[2],
					BssidQuery[3], BssidQuery[4], BssidQuery[5]);
		}
		else {
			memset(strSSID, 0x00, NDIS_802_11_LENGTH_SSID + 1);
			memcpy(strSSID, SsidQuery.Ssid, SsidQuery.SsidLength);
			fprintf(stderr, "Connected <--> %s\n", strSSID);
		}
		Connection_flag = 1;
	}
	else if (G_bRadio) {
		fprintf(stderr, "Disconnected!\n");
		Connection_flag = 0;
	}
	else {
		Connection_flag = 0;
		fprintf(stderr, "Radio Off!\n");
	}
	close(s);

	return Connection_flag;
}

int
asuscfe(const char *PwqV)
{
	if (strcmp(PwqV, "stat")==0)
	{
		eval("iwpriv", WIF, "stat");
	}
	else if (strcmp(PwqV, "ATESHOW=1")==0)
	{
		ateshow();
	}
	else if (strcmp(PwqV, "ATEHELP=1")==0)
	{
		atehelp();
	}
	else if (strstr(PwqV, "=") && strstr(PwqV, "=")!=PwqV)
	{
		eval("iwpriv", WIF, "set", PwqV);
		puts("success");
	}
	return 0;
}

#define RTPRIV_IOCTL_GATESHOW (SIOCIWFIRSTPRIV + 0x18)
#define RTPRIV_IOCTL_GATEHELP (SIOCIWFIRSTPRIV + 0x19)

int
ateshow()
{
	int retval=0;
	char data[1024];
	struct iwreq wrq;

	memset(data, 0x00, 1024);
	wrq.u.data.length = 1024;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	if(wl_ioctl(WIF, RTPRIV_IOCTL_GATESHOW, &wrq) < 0)
	{
		fprintf(stderr, "errors in getting ATESHOW result\n");
		return 0;
	}

	if(wrq.u.data.length > 0)
	{
		puts(wrq.u.data.pointer);
	}

	return 0;
}

int
atehelp()
{
	int retval=0;
	char data[1536];
	struct iwreq wrq;

	memset(data, 0x00, 1536);
	wrq.u.data.length = 1536;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	if(wl_ioctl(WIF, RTPRIV_IOCTL_GATEHELP, &wrq) < 0)
	{
		fprintf(stderr, "errors in getting ATEHELP result\n");
		return 0;
	}

	if(wrq.u.data.length > 0)
	{
		puts(wrq.u.data.pointer);
	}

	return 0;
}

int
wps_pin(int pincode)
{
	if (	nvram_match("WL_auth_mode", "shared") ||
		nvram_match("WL_auth_mode", "wpa") ||
		nvram_match("WL_auth_mode", "wpa2") ||
		nvram_match("WL_auth_mode", "radius") ||
		nvram_invmatch("sw_mode_ex", "1"))
		return 0;

//	fprintf(stderr, "WPS: PIN\n");
	eval("iwpriv", WIF, "set", "WscMode=1");

	char tmpstr[20];
	sprintf(tmpstr, "WscPinCode=%d", pincode);

	if (pincode == 0)
		eval("iwpriv", WIF, "set", "WscGetConf=1");
	else
	{
		eval("iwpriv", WIF, "set", tmpstr);
#ifdef W7_LOGO
		eval("iwpriv", WIF, "set", "WscGetConf=1");
#endif
	}

	return 0;
}

int
wps_pbc()
{
	if (	nvram_match("WL_auth_mode", "shared") ||
		nvram_match("WL_auth_mode", "wpa") ||
		nvram_match("WL_auth_mode", "wpa2") ||
		nvram_match("WL_auth_mode", "radius") ||
		nvram_invmatch("sw_mode_ex", "1"))
		return 0;

//	fprintf(stderr, "WPS: PBC\n");
	eval("iwpriv", WIF, "set", "WscMode=2");
	eval("iwpriv", WIF, "set", "WscGetConf=1");

	return 0;
}

void
wps_oob()
{
	if (nvram_match("lan_ipaddr_t", "") && nvram_match("lan_ipaddr", ""))
		return;

	if (	nvram_match("WL_auth_mode", "shared") ||
		nvram_match("WL_auth_mode", "wpa") ||
		nvram_match("WL_auth_mode", "wpa2") ||
		nvram_match("WL_auth_mode", "radius") ||
		nvram_invmatch("sw_mode_ex", "1"))
		return;

	nvram_set("wsc_config_state", "0");
	nvram_set("wl_ssid", "ASUSInitialAP");
	nvram_set("wl_auth_mode", "psk");
	nvram_set("wl_wep_x", "0");
	nvram_set("wl_wpa_mode", "1");
	nvram_set("wl_crypto", "tkip+aes");
	nvram_set("wl_key", "2");
	nvram_set("wl_wpa_psk", "12345678");
	nvram_commit();

	nvram_set("x_Setting", "0");

	eval("iwpriv", WIF, "set", "WscConfStatus=1");			// AP is unconfigured
#ifdef W7_LOGO
	eval("iwpriv", WIF, "set", "WscStatus=0");
#else
	eval("iwpriv", WIF, "set", "WscStatus=3");
#endif
	eval("iwpriv", WIF, "set", "SSID=ASUSInitialAP");
	eval("iwpriv", WIF, "set", "AuthMode=WPAPSK");
	eval("iwpriv", WIF, "set", "EncrypType=TKIPAES");
	eval("iwpriv", WIF, "set", "DefaultKeyID=2");
	eval("iwpriv", WIF, "set", "WPAPSK=12345678");

#ifdef W7_LOGO
	if (nvram_match("wps_mode", "1"))
	{
		fprintf(stderr, "WPS: PIN\n");
		eval("iwpriv", WIF, "set", "WscMode=1");
	}
	else
	{
		fprintf(stderr, "WPS: PBC\n");
		eval("iwpriv", WIF, "set", "WscMode=2");
	}
//	char tmpstr[20];
//	sprintf(tmpstr, "WscPinCode=%s", nvram_safe_get("secret_code"));
//	eval("iwpriv", WIF, "set", tmpstr);
	eval("iwpriv", WIF, "set", "WscGetConf=1");
#endif
}

void
start_wsc()
{
	if (nvram_match("lan_ipaddr_t", "") && nvram_match("lan_ipaddr", ""))
		return;

	if (	nvram_match("WL_auth_mode", "shared") ||
		nvram_match("WL_auth_mode", "wpa") ||
		nvram_match("WL_auth_mode", "wpa2") ||
		nvram_match("WL_auth_mode", "radius") ||
		nvram_invmatch("sw_mode_ex", "1"))
		return;

	system("route delete 239.255.255.250 1>/dev/null 2>&1");
	system("killall wscd 1>/dev/null 2>&1");
	system("killall -9 wscd 1>/dev/null 2>&1");

	fprintf(stderr, "start wsc\n");

	eval("iwpriv", WIF, "set", "WscConfMode=0");			// WPS disable
	eval("iwpriv", WIF, "set", "WscConfMode=7");
	if (atoi(nvram_safe_get("wsc_config_state")) == 0)
		eval("iwpriv", WIF, "set", "WscConfStatus=1");

	system("route add -host 239.255.255.250 dev br0 1>/dev/null 2>&1");
	char str_lan_ipaddr[16];
	if (nvram_match("lan_ipaddr_t", ""))
		strcpy(str_lan_ipaddr, nvram_safe_get("lan_ipaddr"));
	else
		strcpy(str_lan_ipaddr, nvram_safe_get("lan_ipaddr_t"));
	char *argv[] = {"wscd", "-m", "1", "-a", str_lan_ipaddr, "-i", WIF, NULL};
	pid_t pid;
	_eval(argv, NULL, 0, &pid);

/*
	if (nvram_match("wsc_config_state", "1"))
		eval("iwpriv", WIF, "set", "WscConfStatus=2");		// AP is configured
	else
		eval("iwpriv", WIF, "set", "WscConfStatus=1");
*/
#ifdef W7_LOGO
	if (nvram_match("wps_mode", "1"))
	{
		fprintf(stderr, "WPS: PIN\n");				// PIN method
		eval("iwpriv", WIF, "set", "WscMode=1");
	}
	else
	{
		fprintf(stderr, "WPS: PBC\n");				// PBC method
		eval("iwpriv", WIF, "set", "WscMode=2");
	}

	eval("iwpriv", WIF, "set", "WscGetConf=1");
#endif
}

void
start_wsc_pbc()
{
	if (nvram_match("lan_ipaddr_t", "") && nvram_match("lan_ipaddr", ""))
		return;

	if (	nvram_match("WL_auth_mode", "shared") ||
		nvram_match("WL_auth_mode", "wpa") ||
		nvram_match("WL_auth_mode", "wpa2") ||
		nvram_match("WL_auth_mode", "radius") ||
		nvram_invmatch("sw_mode_ex", "1"))
		return;

	fprintf(stderr, "start wsc\n");
	if (nvram_match("wps_enable", "0"))
	{
		system("route add -host 239.255.255.250 dev br0 1>/dev/null 2>&1");
		system("killall wscd 1>/dev/null 2>&1");
		system("killall -9 wscd 1>/dev/null 2>&1");
		
		char str_lan_ipaddr[16];
		if (nvram_match("lan_ipaddr_t", ""))
			strcpy(str_lan_ipaddr, nvram_safe_get("lan_ipaddr"));
		else
			strcpy(str_lan_ipaddr, nvram_safe_get("lan_ipaddr_t"));
		char *argv[] = {"wscd", "-m", "1", "-a", str_lan_ipaddr, "-i", WIF, NULL};
		pid_t pid;
		_eval(argv, NULL, 0, &pid);
		eval("iwpriv", WIF, "set", "WscConfMode=7");
	}

	fprintf(stderr, "WPS: PBC\n");
	eval("iwpriv", WIF, "set", "WscMode=2");
	eval("iwpriv", WIF, "set", "WscGetConf=1");

	nvram_set("wps_enable", "1");
	nvram_set("wps_mode", "2");
}

void
start_wsc_pin_enrollee()
{
	if (nvram_match("lan_ipaddr_t", "") && nvram_match("lan_ipaddr", ""))
	{
		nvram_set("wps_enable", "0");
		nvram_set("wps_start_flag", "0");
		return;
	}

	if (	nvram_match("WL_auth_mode", "shared") ||
		nvram_match("WL_auth_mode", "wpa") ||
		nvram_match("WL_auth_mode", "wpa2") ||
		nvram_match("WL_auth_mode", "radius") ||
		nvram_invmatch("sw_mode_ex", "1"))
		return;

	system("route add -host 239.255.255.250 dev br0 1>/dev/null 2>&1");
	system("killall wscd 1>/dev/null 2>&1");
	system("killall -9 wscd 1>/dev/null 2>&1");

	fprintf(stderr, "start wsc\n");

	char str_lan_ipaddr[16];
	if (nvram_match("lan_ipaddr_t", ""))
		strcpy(str_lan_ipaddr, nvram_safe_get("lan_ipaddr"));
	else
		strcpy(str_lan_ipaddr, nvram_safe_get("lan_ipaddr_t"));
	char *argv[] = {"wscd", "-m", "1", "-a", str_lan_ipaddr, "-i", WIF, NULL};
	pid_t pid;
	_eval(argv, NULL, 0, &pid);
	eval("iwpriv", WIF, "set", "WscConfMode=7");			// Enrollee + Proxy
/*
	if (nvram_match("wsc_config_state", "1"))
		eval("iwpriv", WIF, "set", "WscConfStatus=2");		// AP is configured
	else
		eval("iwpriv", WIF, "set", "WscConfStatus=1");
*/
	fprintf(stderr, "WPS: PIN\n");					// PIN method
	eval("iwpriv", WIF, "set", "WscMode=1");
#ifdef W7_LOGO
	eval("iwpriv", WIF, "set", "WscGetConf=1");
#endif
	nvram_set("wps_start_flag", "1");
}

void
stop_wsc()
{
	if (	nvram_match("WL_auth_mode", "shared") ||
		nvram_match("WL_auth_mode", "wpa") ||
		nvram_match("WL_auth_mode", "wpa2") ||
		nvram_match("WL_auth_mode", "radius") ||
		nvram_invmatch("sw_mode_ex", "1"))
		return;

	system("route delete 239.255.255.250 1>/dev/null 2>&1");
	system("killall wscd 1>/dev/null 2>&1");
	system("killall -9 wscd 1>/dev/null 2>&1");

	eval("iwpriv", WIF, "set", "WscConfMode=0");
	eval("iwpriv", WIF, "set", "WscStatus=0");
}

/*------------------------------------------------------------------*/
/*
 * Get wireless informations & config from the device driver
 * We will call all the classical wireless ioctl on the driver through
 * the socket to know what is supported and to get the settings...
 */
int
get_info(int			skfd,
	 char *			ifname,
	 struct wireless_info *	info)
{
  struct iwreq		wrq;

  memset((char *) info, 0, sizeof(struct wireless_info));

  /* Get basic information */
  if(iw_get_basic_config(skfd, ifname, &(info->b)) < 0)
    {
      /* If no wireless name : no wireless extensions */
      /* But let's check if the interface exists at all */
      struct ifreq ifr;

      strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
      if(ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
	return(-ENODEV);
      else
	return(-ENOTSUP);
    }

  /* Get AP address */
  if(iw_get_ext(skfd, ifname, SIOCGIWAP, &wrq) >= 0)
    {
      info->has_ap_addr = 1;
      memcpy(&(info->ap_addr), &(wrq.u.ap_addr), sizeof (sockaddr));
    }
  else
    return -1;

  return(0);
}

/*------------------------------------------------------------------*/
/*
 * Print on the screen in a neat fashion all the info we have collected
 * on a device.
 */
void
display_info(struct wireless_info *	info,
	     char *			ifname)
{
  char		buffer[128];	/* Temporary buffer */

  /* Display ESSID (extended network), if any */
  if(info->b.has_essid)
    {
      if(info->b.essid_on)
	{
	  /* Does it have an ESSID index ? */
	  if((info->b.essid_on & IW_ENCODE_INDEX) > 1)
	    printf("ESSID:\"%s\" [%d]  ", info->b.essid,
		   (info->b.essid_on & IW_ENCODE_INDEX));
	  else
	    printf("ESSID:\"%s\"  ", info->b.essid);
	}
      else
	printf("ESSID:off/any  ");
    }

  /* Display the address of the current Access Point */
  if(info->has_ap_addr)
    {
      /* Oups ! No Access Point in Ad-Hoc mode */
      if((info->b.has_mode) && (info->b.mode == IW_MODE_ADHOC))
	printf("Cell:");
      else
	printf("Access Point:");
      printf(" %s", iw_sawap_ntop(&info->ap_addr, buffer));
    }

  printf("\n");
}

int
getApCliInfo()
{
	int skfd;		/* generic raw socket desc.	*/
	int rc;
	struct wireless_info info;

	/* Create a channel to the NET kernel. */
	if((skfd = iw_sockets_open()) < 0)
	{
		perror("socket");
		return -1;
	}

	rc = get_info(skfd, URE, &info);

	/* Close the socket. */
  	close(skfd);

	if (!rc)
      		display_info(&info, URE);
      	else
      		fprintf(stderr, "errors in getting apcli0 info!\n");

	return rc;
}

int
get_channel()
{
	int channel;
	struct iw_range	range;
	double freq;
	struct iwreq wrq1;
	struct iwreq wrq2;

	if(wl_ioctl(WIF, SIOCGIWFREQ, &wrq1) < 0)
		return 0;

	char buffer[sizeof(iwrange) * 2];
	bzero(buffer, sizeof(buffer));
	wrq2.u.data.pointer = (caddr_t) buffer;
	wrq2.u.data.length = sizeof(buffer);
	wrq2.u.data.flags = 0;

	if(wl_ioctl(WIF, SIOCGIWRANGE, &wrq2) < 0)
		return 0;

	if(ralink_get_range_info(&range, buffer, wrq2.u.data.length) < 0)
		return 0;

	freq = iw_freq2float(&(wrq1.u.freq));
	if(freq < KILO)
		channel = (int) freq;
	else
	{
		channel = iw_freq_to_channel(freq, &range);
		if(channel < 0)
			return 0;
	}

	return channel;
}

int
apcli_set(const char *pv_pair)
{
	struct iwreq wrq;
	char data[256];

	memset(data, 0x0, 256); 
	strcpy(data, pv_pair);
	wrq.u.data.pointer = data;
	wrq.u.data.length = strlen(data);
	wrq.u.data.flags = 0;

	fprintf(stderr, "set %s\n", pv_pair);

	if(wl_ioctl(URE, RTPRIV_IOCTL_SET, &wrq) < 0)
	{
		return 0;
	}
	else
		return 1;
}

int
ap_set(const char *pv_pair)
{
	struct iwreq wrq;
	char data[256];

	memset(data, 0x0, 256); 
	strcpy(data, pv_pair);
	wrq.u.data.pointer = data;
	wrq.u.data.length = strlen(data);
	wrq.u.data.flags = 0;

	fprintf(stderr, "set %s\n", pv_pair);

	if(wl_ioctl(WIF, RTPRIV_IOCTL_SET, &wrq) < 0)
	{
		return 0;
	}
	else
		return 1;
}

int
proc_check(const char *pid_file)
{
	char pid_buf[10], proc_path[32];
	int fd, pid;
	struct stat f_st;

	if((fd=open(pid_file, O_RDONLY)) <= 0)
		return 0;

	memset(pid_buf, '\0', sizeof(pid_buf));
	read(fd, pid_buf, sizeof(pid_buf));
	close(fd);

	if((pid = atoi(pid_buf)) <= 0)
		return 0;

	memset(proc_path, '\0', sizeof(proc_path));
	sprintf(proc_path, "/proc/%d", pid);

	lstat(proc_path, &f_st);
	if(!S_ISDIR(f_st.st_mode))
		return 0;

	return 1;
}

int
sta_wps_pbc()
{
	unsigned char ssid_fake[2];
	char tmpstr[8];

	if (is_URE() && (nvram_match("sw_mode_ex", "3") || nvram_match("apcli_workaround", "0")))
	{
		nvram_set("apcli_workaround", "1");

		eval("brctl", "delif", "br0", WIF);
		apcli_set("ApCliEnable=0");
		ifconfig(URE, 0, NULL, NULL);
		ifconfig(WIF, 0, NULL, NULL);
		eval("rmmod", "rt2860v2_ap");
		eval("insmod", "rt2860v2_sta");
		ifconfig(WIF, IFUP, NULL, NULL);
	}
	else
	{
		fprintf(stderr, "sta_wps_pbc: Not in AP(-Client) mode!\n");
		return 1;
	}

	eval("iwpriv", WIF, "set", "WirelessMode=9");	// B,G,N
	ssid_fake[0] = 0x13;				// 13 is string end of Dos
	ssid_fake[0] = 0x00;
	sprintf(tmpstr, "SSID=%s", ssid_fake);
	eval("iwpriv", WIF, "set", tmpstr);

	fprintf(stderr, "STA WPS: PBC\n");
	eval("iwpriv", WIF, "wsc_cred_count", "0");
	eval("iwpriv", WIF, "wsc_conf_mode", "1");	// Enrollee
	eval("iwpriv", WIF, "wsc_mode", "2");		// PBC
	eval("iwpriv", WIF, "wsc_start");

	return 0;
}

int
get_pid_from_file(char *pidfile)
{
	FILE *fp = fopen(pidfile, "r");
	char buf[256];
	int pid;

	if (fp && fgets(buf, sizeof(buf), fp))
	{
		pid = strtoul(buf, NULL, 0);
		fclose(fp);
	}

	return pid;
}

int
sta_wps_stop()
{	
	fprintf(stderr, "STA WPS: stop...\n");

	if (!is_URE())
	{
		eval("iwpriv", WIF, "wsc_stop");
		eval("iwpriv", WIF, "wsc_conf_mode", "0");      // Disable WPS

		ifconfig(WIF, 0, NULL, NULL);
		eval("rmmod", "rt2860v2_sta");
		eval("insmod", "rt2860v2_ap");

		gen_ralink_config();
		ifconfig(WIF, IFUP, NULL, NULL);
		if (nvram_invmatch("sta_ssid", ""))
			ifconfig(URE, IFUP, NULL, NULL);

		char tmpstr[32];
		sprintf(tmpstr, "ApcliMonitorPid=%d", get_pid_from_file("/var/run/apcli_monitor.pid"));
		eval("iwpriv", WIF, "set", tmpstr);
		eval("brctl", "addif", "br0", WIF);
		if (nvram_invmatch("sta_ssid", ""))
			eval("brctl", "addif", "br0", URE);

		fprintf(stderr, "sending SIGTSTP in sta_wps_stop()!\n");
		nvram_set("ui_triggered", "2");
		nvram_unset("sta_check_ha");
		kill_pidfile_s("/var/run/apcli_monitor.pid", SIGTSTP);
	}
	else
		fprintf(stderr, "sta_wps_stop: Already in AP(-Client) mode!\n");

	return 0;
}

int
ap_wps_stop()
{	
	fprintf(stderr, "STA WPS: stop...\n");

	if (!is_URE())
	{
		eval("iwpriv", WIF, "wsc_stop");
		eval("iwpriv", WIF, "wsc_conf_mode", "0");	// Disable WPS

		ifconfig(WIF, 0, NULL, NULL);
		eval("rmmod", "rt2860v2_sta");
		eval("insmod", "rt2860v2_ap");
/*
		nvram_set("wl_ssid", nvram_safe_get("sta_ssid"));
		nvram_set("wl_auth_mode", nvram_safe_get("sta_auth_mode"));
		nvram_set("wl_wep_x", nvram_safe_get("sta_wep_x"));
		nvram_set("wl_crypto", nvram_safe_get("sta_crypto"));
		nvram_set("wl_wpa_mode", nvram_safe_get("sta_wpa_mode"));
		nvram_set("wl_wpa_psk", nvram_safe_get("sta_wpa_psk"));
		nvram_set("wl_key", nvram_safe_get("sta_key"));
		nvram_set("wl_key_type", nvram_safe_get("sta_key_type"));
		nvram_set("wl_key1", nvram_safe_get("sta_key1"));
		nvram_set("wl_key2", nvram_safe_get("sta_key2"));
		nvram_set("wl_key3", nvram_safe_get("sta_key3"));
		nvram_set("wl_key4", nvram_safe_get("sta_key4"));

		{
			char buff[100];
			memset(buff, 0x0, 100);
			char_to_ascii(buff, nvram_safe_get("sta_ssid"));
			nvram_set("wl_ssid2", buff);
			nvram_commit();
		} 
*/
		gen_ralink_config();

		ifconfig(WIF, IFUP, NULL, NULL);
		eval("brctl", "addif", "br0", WIF);
	}
	else
		fprintf(stderr, "ap_wps_stop: Already in AP(-Client) mode!\n");

	return 0;
}
