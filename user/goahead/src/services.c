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
#include	"helpers.h"

#define _PATH_DHCP_ALIAS_FILE "/etc/dhcpd_static.conf"
#define _PATH_IPT_ACCOUNTING_FILE "/proc/net/ipt_account/mynetwork"
#define IPT_SHORT_ACCOUNT

/*** Busybox leases.h ***/

static inline uint64_t hton64(uint64_t v)
{
        return (((uint64_t)htonl(v)) << 32) | htonl(v >> 32);
}

#define ntoh64(v) hton64(v)
typedef uint32_t leasetime_t;
typedef int32_t signed_leasetime_t;

struct dyn_lease {
	/* "nip": IP in network order */
	/* Unix time when lease expires. Kept in memory in host order.
	 * When written to file, converted to network order
	 * and adjusted (current time subtracted) */
	leasetime_t expires;
	uint32_t lease_nip;
	/* We use lease_mac[6], since e.g. ARP probing uses
	 * only 6 first bytes anyway. We check received dhcp packets
	 * that their hlen == 6 and thus chaddr has only 6 significant bytes
	 * (dhcp packet has chaddr[16], not [6])
	 */
	uint8_t lease_mac[6];
	char hostname[20];
	uint8_t pad[2];
	/* total size is a multiply of 4 */
} __attribute__((__packed__));

static void setDhcp(webs_t wp, char_t *path, char_t *query);
static int getDhcpCliList(int eid, webs_t wp, int argc, char_t **argv);

static void setSamba(webs_t wp, char_t *path, char_t *query);
static void setMiscServices(webs_t wp, char_t *path, char_t *query);
static void formIptAccounting(webs_t wp, char_t *path, char_t *query);
static int getDhcpStaticList(int eid, webs_t wp, int argc, char_t **argv);
static int iptStatList(int eid, webs_t wp, int argc, char_t **argv);

void formDefineServices(void)
{
	// Define forms
	websFormDefine(T("setDhcp"), setDhcp);
	websFormDefine(T("formSamba"), setSamba);
	websFormDefine(T("setMiscServices"), setMiscServices);
	websFormDefine(T("formIptAccounting"), formIptAccounting);

	// Define functions
	websAspDefine(T("getDhcpCliList"), getDhcpCliList);
	websAspDefine(T("getDhcpStaticList"), getDhcpStaticList);
	websAspDefine(T("iptStatList"), iptStatList);
}


/*
 * description: write DHCP client list
 */
static int getDhcpCliList(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	struct dyn_lease lease;

	int i, rownum = 0;
	struct in_addr addr;
	int64_t written_at, curr, expired_abs;

	//if DHCP is disabled - just exit
	char* dhcpEnabled = nvram_get(RT2860_NVRAM, "dhcpEnabled");
	if (!strncmp(dhcpEnabled, "0", 2))
		return 0;

	doSystem("killall -q -USR1 udhcpd");

	fp = fopen("/var/udhcpd.leases", "r");
	if (!fp)
		return websWrite(wp, T(""));

	/* Read header of dhcpleases */
	if ( fread(&written_at, 1, sizeof(written_at), fp) != sizeof(written_at) )
	{
		fclose(fp);
		return 0;
	}
		
	written_at = ntoh64(written_at);
	curr = time(NULL);
	if (curr < written_at)
		written_at = curr; /* lease file from future! :) */

	/* Output leases file */
	while (fread(&lease, 1, sizeof(lease), fp) == sizeof(lease))
	{
		// Output structure
		// Host
		websWrite(wp, T("<tr><td>%s</td>"), lease.hostname);
		// MAC
		websWrite(wp, T("<td id=\"dhclient_row%d_mac\">%02X"), rownum, lease.lease_mac[0]);
		for (i = 1; i < 6; i++)
			websWrite(wp, T(":%02X"), lease.lease_mac[i]);
		// IP
		addr.s_addr = lease.lease_nip;
		websWrite(wp, T("</td><td id=\"dhclient_row%d_ip\">%s</td><td>"), rownum, inet_ntoa(addr));

		// Expire Date
		expired_abs = ntohl(lease.expires) + written_at;
		if (expired_abs > curr)
		{
			leasetime_t expires = expired_abs - curr;
			unsigned d = expires / (24*60*60);
			expires %= (24*60*60);
			unsigned h = expires / (60*60);
			expires %= (60*60);
			unsigned m = expires / 60;
			expires %= 60;

			if (d>0)
				websWrite(wp, T("%u days "), d);
			websWrite(wp, T("%02u:%02u:%02u</td>"), h, m, (unsigned)expires);
		}
		else
			websWrite(wp, T("expired</td>"));
		websWrite(wp, "<td><input id=\"dhclient_row%d\" type=\"checkbox\" onchange=\"toggleDhcpTable(this);\"></td>", rownum);
		websWrite(wp, "</tr>\n");

		rownum++;
	}

	fclose(fp);
	return 0;
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
	char_t	*dhcp_s, *dhcp_e, *dhcp_m, *dhcp_pd, *dhcp_sd, *dhcp_g, *dhcp_l, *dhcp_domain;
	char_t	*static_leases;

	dhcp_tp = websGetVar(wp, T("lanDhcpType"), T("DISABLE"));
	dhcp_s = websGetVar(wp, T("dhcpStart"), T(""));
	dhcp_e = websGetVar(wp, T("dhcpEnd"), T(""));
	dhcp_m = websGetVar(wp, T("dhcpMask"), T(""));
	dhcp_pd = websGetVar(wp, T("dhcpPriDns"), T(""));
	dhcp_sd = websGetVar(wp, T("dhcpSecDns"), T(""));
	dhcp_g = websGetVar(wp, T("dhcpGateway"), T(""));
	dhcp_l = websGetVar(wp, T("dhcpLease"), T("86400"));
	dhcp_domain = websGetVar(wp, T("dhcpDomain"), T("localdomain"));
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
		nvram_bufset(RT2860_NVRAM, "dhcpDomain", dhcp_domain);
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
		dhcpStoreAliases(static_leases);
	}
	else if (strncmp(dhcp_tp, "DISABLE", 8)==0)
		nvram_set(RT2860_NVRAM, "dhcpEnabled", "0");

	// Restart DHCP service
	doSystem("service dhcpd restart");

	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (submitUrl != NULL)
		websRedirect(wp, submitUrl);
	else
		websDone(wp, 200);
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
	{ T("dhcpSwReset"), "dhcpSwReset", T("0") },
	{ T("natFastpath"), "natFastpath", T("0") },
	{ T("bridgeFastpath"), "bridgeFastpath", T("1") },
	{ T("CrondEnable"), "CrondEnable", T("0") },
	{ T("ForceRenewDHCP"), "ForceRenewDHCP", T("1") },
	{ T("arpPT"), "parproutedEnabled", T("0") },
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

	char_t *nat_fp = nvram_bufget(RT2860_NVRAM, "natFastpath");
	if ((nat_fp != NULL) && ((strcmp(nat_fp, "2") == 0) || (strcmp(nat_fp, "3") == 0)))
	{
		char_t *nat_th = websGetVar(wp, "hwnatThreshold", "30");
		if (nat_th != NULL)
			nvram_bufset(RT2860_NVRAM, "hw_nat_bind", nat_th);
	}

	nvram_close(RT2860_NVRAM);

	//restart some services instead full reload
	doSystem("services_restart.sh misc &");

	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (submitUrl != NULL)
		websRedirect(wp, submitUrl);
	else
		websDone(wp, 200);
}

//------------------------------------------------------------------------------
// Samba/CIFS setup
const service_flag_t service_samba_flags[] =
{
	{ T("WorkGroup"), "WorkGroup", T("") },
	{ T("SmbNetBIOS"), "SmbNetBIOS", T("") },
	{ T("SmbString"), "SmbString", T("") },
	{ T("SmbOsLevel"), "SmbOsLevel", T("") },
	{ NULL, NULL, NULL } // Terminator
};

static void setSamba(webs_t wp, char_t *path, char_t *query)
{
	const service_flag_t *p;

	char_t *smb_enabled = websGetVar(wp, T("SmbEnabled"), T("0"));
	if (smb_enabled == NULL)
		smb_enabled = "0";

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "SmbEnabled", smb_enabled);

	if (strcmp(smb_enabled, "1")==0)
	{
		for (p = service_samba_flags; p->web != NULL; p++)
		{
			char_t *var = websGetVar(wp, p->web, p->deflt);
			if (var != NULL)
				nvram_bufset(RT2860_NVRAM, p->nvram, var);
		}
	}

	nvram_close(RT2860_NVRAM);

	//restart some services instead full reload
	doSystem("service sysctl restart");
	doSystem("service dhcpd restart");
	doSystem("service samba restart");

	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (submitUrl != NULL)
		websRedirect(wp, submitUrl);
	else
		websDone(wp, 200);
}

//------------------------------------------------------------------------------
// IPT Accounting
void formIptAccounting(webs_t wp, char_t *path, char_t *query)
{
	char_t *strValue;
	int reset_ipt = 0;

	strValue = websGetVar(wp, T("iptEnable"), T("0"));	//reset stats
	if ((strValue != NULL) && (strcmp(strValue, "0")==0))
		reset_ipt = 1;

	nvram_set(RT2860_NVRAM, "ipt_account", strValue);

	strValue = websGetVar(wp, T("reset"), T("0"));	//reset stats
	if ((strValue != NULL) && (strcmp(strValue, "1")))
		reset_ipt = 1;

	// Reset IPT
	if (reset_ipt)
	{
		FILE *fd = fopen(_PATH_IPT_ACCOUNTING_FILE, "w");
		if (fd != NULL)
		{
			fputs("reset", fd);
			fclose(fd);
		}
	}

	doSystem("modprobe -q ipt_account");
	doSystem("service iptables restart");

	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (submitUrl != NULL)
		websRedirect(wp, submitUrl);
	else
		websDone(wp, 200);
}

#ifndef IPT_SHORT_ACCOUNT
const char *iptProtocolNames[]=
{
	"All", "TCP", "UDP", "ICMP", "Others"
};
#endif /* !IPT_SHORT_ACCOUNT */

// Output IP Accounting statistics
int iptStatList(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fd;
	char_t ip[32], line[256];
	long long b_src[5], p_src[5], b_dst[5], p_dst[5], time;
	int lines = 0;

	// Do not show anything if nat_fastpath is set
	char* nat_fp = nvram_get(RT2860_NVRAM, "natFastpath");
	if (nat_fp != NULL)
	{
		if (strcmp(nat_fp, "0") != 0)
		{
			websWrite(wp, T("<tr><td>No IPT accounting allowed</td></tr>\n"));
			return 0;
		}
	}

	websWrite(wp, T("<tr><td class=\"title\" colspan=\"%d\">IP Accounting table</td></tr>\n"),
#ifdef IPT_SHORT_ACCOUNT
				6
#else
				7
#endif
	);

	if ((fd = fopen(_PATH_IPT_ACCOUNTING_FILE, "r"))!=NULL)
	{
		// Output table header
#ifdef IPT_SHORT_ACCOUNT
		websWrite(wp, T(
			"<tr>\n"
			"<th width=\"30%%\" align=\"center\">IP addr</th>\n"
			"<th width=\"15%%\" align=\"center\">Tx bts</th>\n"
			"<th width=\"15%%\" align=\"center\">Tx pkt</th>\n"
			"<th width=\"15%%\" align=\"center\">Rx bts</th>\n"
			"<th width=\"15%%\" align=\"center\">Rx pkt</th>\n"
			"<th width=\"10%%\" align=\"center\">Time</th>\n"
			"</tr>\n"));
#else
		websWrite(wp, T(
			"<tr>\n"
			"<th width=\"30%%\" align=\"center\">IP addr</th>\n"
			"<th width=\"10%%\" align=\"center\">Proto</th>\n"
			"<th width=\"13%%\" align=\"center\">Tx bts</th>\n"
			"<th width=\"13%%\" align=\"center\">Tx pkt</th>\n"
			"<th width=\"13%%\" align=\"center\">Rx bts</th>\n"
			"<th width=\"13%%\" align=\"center\">Rx pkt</th>\n"
			"<th width=\"8%%\" align=\"center\">Time</th>\n"
			"</tr>\n"));

#endif /* IPT_SHORT_ACCOUNT */

		while (fgets(line, 255, fd)!=NULL)
		{
			lines++;
#ifdef IPT_SHORT_ACCOUNT
			sscanf(line,
				"%*s %*s %s "   // IP
				"%*s %*s %lld "  // bytes_src
				"%*s %*s %lld "  // packets_src
				"%*s %*s %lld "  // bytes_dst
				"%*s %*s %lld "  // packets_dst
				"%*s %*s %lld ", // time
				ip, &b_src[0], &p_src[0], &b_dst[0], &p_dst[0], &time);

			websWrite(wp, T(
				"<tr class=\"grey\">\n"
				"<td width=\"30%%\" align=\"center\">%s</td>\n"),
				ip);

			const char *src_sz = normalizeSize(&b_src[0]);
			const char *dst_sz = normalizeSize(&b_dst[0]);

			websWrite(wp, T(
				"<td width=\"15%%\" align=\"center\">%ld %s</td>\n"
				"<td width=\"15%%\" align=\"center\">%ld</td>\n"
				"<td width=\"15%%\" align=\"center\">%ld %s</td>\n"
				"<td width=\"15%%\" align=\"center\">%ld</td>\n"
				),
				(long)b_src[0], src_sz, (long)p_src[0], (long)b_dst[0], dst_sz, (long)p_dst[0]
				);

			websWrite(wp, T(
				"<td width=\"10%%\" align=\"center\">%ld</td>\n"
				"</tr>\n"),
				(long)time);
#else
			sscanf(line,
				"%*s %*s %s "   // IP
				"%*s %*s %lld %lld %lld %lld %lld "  // bytes_src
				"%*s %*s %lld %lld %lld %lld %lld "  // packets_src
				"%*s %*s %lld %lld %lld %lld %lld "  // bytes_dst
				"%*s %*s %lld %lld %lld %lld %lld "  // packets_dst
				"%*s %*s %lld ", // time
				ip,
				&b_src[0], &b_src[1], &b_src[2], &b_src[3], &b_src[4],
				&p_src[0], &p_src[1], &p_src[2], &p_src[3], &p_src[4],
				&b_dst[0], &b_dst[1], &b_dst[2], &b_dst[3], &b_dst[4],
				&p_dst[0], &p_dst[1], &p_dst[2], &p_dst[3], &p_dst[4],
				&time);

			websWrite(wp, T(
				"<tr class=\"grey\">\n"
				"<td width=\"30%%\" align=\"center\">%s</td>\n"),
				ip);

			int i;
			for (i=0; i<5; i++)
			{
				if (i>0)
					websWrite(wp, T("<tr class=\"grey\">\n"));

				const char *src_sz = normalizeSize(&b_src[i]);
				const char *dst_sz = normalizeSize(&b_dst[i]);
				websWrite(wp, T(
					"<td width=\"10%%\" align=\"center\">%s</font></td>\n"
					"<td width=\"13%%\" align=\"center\">%lld %s</td>\n"
					"<td width=\"13%%\" align=\"center\">%lld</td>\n"
					"<td width=\"13%%\" align=\"center\">%lld %s</td>\n"
					"<td width=\"13%%\" align=\"center\">%lld</td>\n"),
					iptProtocolNames[i], b_src[i], src_sz, p_src[i], b_dst[i], dst_sz, p_dst[i]);

				if (i==0)
				{
					websWrite(wp, T(
						"<td width=\"10%%\" align=\"center\">%lld</td>\n"),
						time);
				}
				websWrite(wp, T("</tr>\n"));
			}
#endif /* IPT_SHORT_ACCOUNT */
		}

		fclose(fd);
	}

	if (lines<=0)
		websWrite(wp, T("<tr><td align=\"left\" colspan=\"%d\">No statistics available now</td></tr>\n"),
#ifdef IPT_SHORT_ACCOUNT
				6
#else
				7
#endif
			);
	return 0;
}
