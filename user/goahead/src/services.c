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

static void setDhcp(webs_t wp, char_t *path, char_t *query);
static void setMiscServices(webs_t wp, char_t *path, char_t *query);

void formDefineServices(void)
{
	websFormDefine(T("setDhcp"), setDhcp);
	websFormDefine(T("setMiscServices"), setMiscServices);
}

/* goform/setDhcp */
static void setDhcp(webs_t wp, char_t *path, char_t *query)
{
	char_t	*dhcp_tp;
	char_t  *dhcp_s, *dhcp_e, *dhcp_m, *dhcp_pd, *dhcp_sd, *dhcp_g, *dhcp_l;
	char_t	*dhcp_sl1, *dhcp_sl2, *dhcp_sl3;

	dhcp_tp = websGetVar(wp, T("lanDhcpType"), T("DISABLE"));
	dhcp_s = websGetVar(wp, T("dhcpStart"), T(""));
	dhcp_e = websGetVar(wp, T("dhcpEnd"), T(""));
	dhcp_m = websGetVar(wp, T("dhcpMask"), T(""));
	dhcp_pd = websGetVar(wp, T("dhcpPriDns"), T(""));
	dhcp_sd = websGetVar(wp, T("dhcpSecDns"), T(""));
	dhcp_g = websGetVar(wp, T("dhcpGateway"), T(""));
	dhcp_l = websGetVar(wp, T("dhcpLease"), T("86400"));
	dhcp_sl1 = websGetVar(wp, T("dhcpStatic1"), T(""));
	dhcp_sl2 = websGetVar(wp, T("dhcpStatic2"), T(""));
	dhcp_sl3 = websGetVar(wp, T("dhcpStatic3"), T(""));

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
		nvram_bufset(RT2860_NVRAM, "dhcpStatic1", dhcp_sl1);
		nvram_bufset(RT2860_NVRAM, "dhcpStatic2", dhcp_sl2);
		nvram_bufset(RT2860_NVRAM, "dhcpStatic3", dhcp_sl3);
	}
	
	// Commit settings
	nvram_commit(RT2860_NVRAM);

	// Restart DHCP service
	doSystem("service dhcpd restart");
}

/* goform/setMiscServices */
static void setMiscServices(webs_t wp, char_t *path, char_t *query)
{
	char_t	*stp_en, *lltd_en, *igmp_en, *upnp_en, *radvd_en, *pppoer_en, *dnsp_en;

	stp_en = websGetVar(wp, T("stpEnbl"), T("0"));
	lltd_en = websGetVar(wp, T("lltdEnbl"), T("0"));
	igmp_en = websGetVar(wp, T("igmpEnbl"), T("0"));
	upnp_en = websGetVar(wp, T("upnpEnbl"), T("0"));
	radvd_en = websGetVar(wp, T("radvdEnbl"), T("0"));
	pppoer_en = websGetVar(wp, T("pppoeREnbl"), T("0"));
	dnsp_en = websGetVar(wp, T("dnspEnbl"), T("0"));

	nvram_bufset(RT2860_NVRAM, "stpEnabled", stp_en);
	nvram_bufset(RT2860_NVRAM, "lltdEnabled", lltd_en);
	nvram_bufset(RT2860_NVRAM, "igmpEnabled", igmp_en);
	nvram_bufset(RT2860_NVRAM, "upnpEnabled", upnp_en);
	nvram_bufset(RT2860_NVRAM, "radvdEnabled", radvd_en);
	nvram_bufset(RT2860_NVRAM, "pppoeREnabled", pppoer_en);
	nvram_bufset(RT2860_NVRAM, "dnsPEnabled", dnsp_en);

	// Commit settings
	nvram_commit(RT2860_NVRAM);

	initInternet();
}
