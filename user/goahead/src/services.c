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

#include	"utils.h"
#include	"internet.h"
#include	"management.h"
#include	"station.h"
#include	"firewall.h"

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
	    //close file
	    fclose(fd);
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
		sync();
		
		// Call rwfs to store data
		system("fs save &");
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
	if (strncmp(dhcp_tp, "SERVER", 7)==0)
	{
		if (inet_addr(dhcp_s) == -1)
		{
			websError(wp, 200, "invalid DHCP Start IP");
			return;
		}
		if (inet_addr(dhcp_e) == -1)
		{	
			websError(wp, 200, "invalid DHCP End IP");
			return;
		}
		if (inet_addr(dhcp_m) == -1)
		{
			websError(wp, 200, "invalid DHCP Subnet Mask");
			return;
		}
		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "dhcpStart", dhcp_s);
		nvram_bufset(RT2860_NVRAM, "dhcpEnd", dhcp_e);
		nvram_bufset(RT2860_NVRAM, "dhcpMask", dhcp_m);
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "1");
		nvram_bufset(RT2860_NVRAM, "dhcpPriDns", dhcp_pd);
		nvram_bufset(RT2860_NVRAM, "dhcpSecDns", dhcp_sd);
		nvram_bufset(RT2860_NVRAM, "dhcpGateway", dhcp_g);
		nvram_bufset(RT2860_NVRAM, "dhcpLease", dhcp_l);
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
		dhcpStoreAliases(static_leases);
	} else 	if (strncmp(dhcp_tp, "DISABLE", 8)==0)
		nvram_set(RT2860_NVRAM, "dhcpEnabled", "0");
	
	// Restart DHCP service
	doSystem("service dhcpd restart &");
}

typedef struct service_flag_t
{
	char_t *web;
	char   *nvram;
	char_t *deflt;
} service_flag_t;

const service_flag_t service_misc_flags[] =
{
	{ T("stpEnbl"), "stpEnabled", T("0") },
	{ T("lltdEnbl"), "lltdEnabled", T("0") },
	{ T("igmpEnbl"), "igmpEnabled", T("0") },
	{ T("upnpEnbl"), "upnpEnabled", T("0") },
	{ T("radvdEnbl"), "radvdEnabled", T("0") },
	{ T("pppoeREnbl"), "pppoeREnabled", T("0") },
	{ T("dnspEnbl"), "dnsPEnabled", T("0") },
	{ T("rmtHTTP"), "RemoteManagement", T("0") },
	{ T("rmtSSH"), "RemoteSSH", T("0") },
	{ T("udpxyMode"), "UDPXYMode", T("0") },
	{ T("watchdogEnable"), "WatchdogEnabled", T("0") },
	{ T("pingWANEnbl"), "WANPingFilter", T("0") },
	{ T("krnlPppoePass"), "pppoe_pass", T("0") },
	{ T("krnlIpv6Pass"), "ipv6_pass", T("0") },
	{ NULL, NULL, NULL } // Terminator
};

/* goform/setMiscServices */
static void setMiscServices(webs_t wp, char_t *path, char_t *query)
{
	const service_flag_t *p;

	nvram_init(RT2860_NVRAM);
	
	for (p = service_misc_flags; p->web != NULL; p++)
	{
		char_t *var = websGetVar(wp, p->web, p->deflt);
		if (var != NULL)
			nvram_bufset(RT2860_NVRAM, p->nvram, var);
	}
	
	nvram_close(RT2860_NVRAM);

	//restart some services instead full reload
	doSystem("services_restart.sh misc &");
}
