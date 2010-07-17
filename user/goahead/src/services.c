/* vi: set sw=4 ts=4 sts=4 fdm=marker: */
/*
 *	services.c -- services
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: services.c,v 1.167.2.8 2009-04-22 01:31:35 chhung Exp $
 */

#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<net/if.h>
#include	<net/route.h>
#include	<string.h>
#include	<dirent.h>
#include	"internet.h"
#include	"nvram.h"
#include	"webs.h"
#include	"utils.h"
#include	"management.h"
#include	"station.h"

#include	"linux/autoconf.h"  //kernel config
#include	"config/autoconf.h" //user config

#define _PATH_DHCP_ALIAS_FILE "/etc/dhcpd_static.conf"

static void setDhcp(webs_t wp, char_t *path, char_t *query);
static void setMiscServices(webs_t wp, char_t *path, char_t *query);
static int getDhcpStaticList(int eid, webs_t wp, int argc, char_t **argv);

void formDefineServices(void)
{
	websFormDefine(T("setDhcp"), setDhcp);
	websFormDefine(T("setMiscServices"), setMiscServices);
	websAspDefine(T("getDhcpStaticList"), getDhcpStaticList);
}

static int getDhcpStaticList(int eid, webs_t wp, int argc, char_t **argv)
{
	// Get values
	FILE *fd = fopen(_PATH_DHCP_ALIAS_FILE, "r");
	
	if (fd != NULL)
	{
		char line[256];
		char ip[64], mac[64];
		int args, first = 1;
		
		while (fgets(line, 255, fd) != NULL)
		{
			// Read routing line
			args = sscanf(line, "%s %s", ip, mac);

			if (args >= 2)
			{
				if (!first)
					websWrite(wp, T(",\n"));
				else
					first = 0;

				websWrite(wp, T("\t[ '%s', '%s' ]"), ip, mac );
			}
		}
	}
	
	websWrite(wp, T("\n"));
	
	return 0;
}

void dhcpStoreAliases(const char *dhcp_config)
{
	// Open file
	FILE *fd = fopen(_PATH_DHCP_ALIAS_FILE, "w+");
	
	if (fd != NULL)
	{
		// Output routing table to file
		fwrite(dhcp_config, strlen(dhcp_config), 1, fd);
		fclose(fd);
		
		// Call rwfs to store data
		system("fs save");
	}
	else
		printf("Failed to open file %s\n", _PATH_DHCP_ALIAS_FILE);
}

/* goform/setDhcp */
static void setDhcp(webs_t wp, char_t *path, char_t *query)
{
	char_t	*dhcp_tp;
	char_t	*dhcp_s, *dhcp_e, *dhcp_m, *dhcp_pd, *dhcp_sd, *dhcp_g, *dhcp_l;
	char_t	*static_leases;

	dhcp_tp = websGetVar(wp, T("lanDhcpType"), T("DISABLE"));
	dhcp_s = websGetVar(wp, T("dhcpStart"), T(""));
	dhcp_e = websGetVar(wp, T("dhcpEnd"), T(""));
	dhcp_m = websGetVar(wp, T("dhcpMask"), T(""));
	dhcp_pd = websGetVar(wp, T("dhcpPriDns"), T(""));
	dhcp_sd = websGetVar(wp, T("dhcpSecDns"), T(""));
	dhcp_g = websGetVar(wp, T("dhcpGateway"), T(""));
	dhcp_l = websGetVar(wp, T("dhcpLease"), T("86400"));
	static_leases = websGetVar(wp, T("dhcpAssignIP"), T(""));

	// configure gateway and dns (WAN) at bridge mode
	if (strncmp(dhcp_tp, "DISABLE", 8)==0)
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "0");
	else if (strncmp(dhcp_tp, "SERVER", 7)==0)
	{
		if (inet_addr(dhcp_s) == -1)
		{
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid DHCP Start IP");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpStart", dhcp_s);
		if (inet_addr(dhcp_e) == -1)
		{
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid DHCP End IP");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpEnd", dhcp_e);
		if (inet_addr(dhcp_m) == -1)
		{
			nvram_commit(RT2860_NVRAM);
			websError(wp, 200, "invalid DHCP Subnet Mask");
			return;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpMask", dhcp_m);
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "1");
		nvram_bufset(RT2860_NVRAM, "dhcpPriDns", dhcp_pd);
		nvram_bufset(RT2860_NVRAM, "dhcpSecDns", dhcp_sd);
		nvram_bufset(RT2860_NVRAM, "dhcpGateway", dhcp_g);
		nvram_bufset(RT2860_NVRAM, "dhcpLease", dhcp_l);
		dhcpStoreAliases(static_leases);
//		nvram_bufset(RT2860_NVRAM, "dhcpStatic1", static_leases);
	}
	
	// Commit settings
	nvram_commit(RT2860_NVRAM);

	// Restart DHCP service
	doSystem("service dhcpd restart &");
}

/* goform/setMiscServices */
static void setMiscServices(webs_t wp, char_t *path, char_t *query)
{
	char_t  *stp_en, *lltd_en, *igmp_en, *upnp_en, *radvd_en;
	char_t  *pppoer_en, *dnsp_en, *rmt_http, *rmt_ssh, *udpxy_mode;

	stp_en = websGetVar(wp, T("stpEnbl"), T("0"));
	lltd_en = websGetVar(wp, T("lltdEnbl"), T("0"));
	igmp_en = websGetVar(wp, T("igmpEnbl"), T("0"));
	upnp_en = websGetVar(wp, T("upnpEnbl"), T("0"));
	radvd_en = websGetVar(wp, T("radvdEnbl"), T("0"));
	pppoer_en = websGetVar(wp, T("pppoeREnbl"), T("0"));
	dnsp_en = websGetVar(wp, T("dnspEnbl"), T("0"));
	rmt_http = websGetVar(wp, T("rmtHTTP"), T("0"));
	rmt_ssh = websGetVar(wp, T("rmtSSH"), T("0"));
	udpxy_mode = websGetVar(wp, T("udpxyMode"), T("0"));

	nvram_bufset(RT2860_NVRAM, "stpEnabled", stp_en);
	nvram_bufset(RT2860_NVRAM, "lltdEnabled", lltd_en);
	nvram_bufset(RT2860_NVRAM, "igmpEnabled", igmp_en);
	nvram_bufset(RT2860_NVRAM, "upnpEnabled", upnp_en);
	nvram_bufset(RT2860_NVRAM, "radvdEnabled", radvd_en);
	nvram_bufset(RT2860_NVRAM, "pppoeREnabled", pppoer_en);

	nvram_bufset(RT2860_NVRAM, "dnsPEnabled", dnsp_en);
	nvram_bufset(RT2860_NVRAM, "RemoteManagement", rmt_http);
	nvram_bufset(RT2860_NVRAM, "RemoteSSH", rmt_ssh);
	nvram_bufset(RT2860_NVRAM, "UDPXYMode", udpxy_mode);
	
	printf("dnsPEnabled = %s, RemoteManagement = %s, RemoteSSH = %s, UDPXYMode = %s\n",
			dnsp_en, rmt_http, rmt_ssh, udpxy_mode);

	// Commit settings
	nvram_commit(RT2860_NVRAM);

	//restart some services instead full reload
	doSystem("services_restart.sh misc");
}
