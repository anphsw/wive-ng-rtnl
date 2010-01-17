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
/*
 * Broadcom Home Gateway Reference Design
 * Web Page Configuration Support Routines
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id: broadcom.c,v 1.1.1.1 2007/02/15 12:13:18 jiahao Exp $
 */

#ifdef WEBS
#include <webs.h>
#include <uemf.h>
#include <ej.h>
#else /* !WEBS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <httpd.h>
#endif /* WEBS */

#include <nvram/typedefs.h>
#include <proto/ethernet.h>
#include <nvram/bcmnvram.h>
#include <nvram/bcmutils.h>
#include <shutils.h>
#include <netconf.h>
#include <nvparse.h>
#include <ralink.h>
#include "iwlib.h"
#include "stapriv.h"
#include <semaphore_mfp.h>

#define wan_prefix(unit, prefix)	snprintf(prefix, sizeof(prefix), "wan%d_", unit)
static char * rfctime(const time_t *timep);
//static char * reltime(unsigned int seconds);
void reltime(unsigned int seconds, char *buf);

#if defined(linux)

#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/klog.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h>

typedef u_int64_t u64;
typedef u_int32_t u32;
typedef u_int16_t u16;
typedef u_int8_t u8;

#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <net/if_arp.h>

/******************************************************************************************************************************************/

#ifdef REMOVE
#ifndef WEBS

#define MIN_BUF_SIZE	4096

/* Upgrade from remote server or socket stream */
static int
sys_upgrade(char *url, FILE *stream, int *total)
{
	char upload_fifo[] = "/tmp/uploadXXXXXX";
	FILE *fifo = NULL;
	char *write_argv[] = { "write", upload_fifo, "linux", NULL };
	pid_t pid;
	char *buf = NULL;
	int count, ret = 0;
	long flags = -1;
	int size = BUFSIZ;

	if (url)
		return eval("write", url, "linux");

	/* Feed write from a temporary FIFO */
	if (!mktemp(upload_fifo) ||
	    mkfifo(upload_fifo, S_IRWXU) < 0||
	    (ret = _eval(write_argv, NULL, 0, &pid)) ||
	    !(fifo = fopen(upload_fifo, "w"))) {
		if (!ret)
			ret = errno;
		goto err;
	}

	/* Set nonblock on the socket so we can timeout */
	if ((flags = fcntl(fileno(stream), F_GETFL)) < 0 ||
	    fcntl(fileno(stream), F_SETFL, flags | O_NONBLOCK) < 0) {
		ret = errno;
		goto err;
	}

	/*
	* The buffer must be at least as big as what the stream file is
	* using so that it can read all the data that has been buffered 
	* in the stream file. Otherwise it would be out of sync with fn
	* select specially at the end of the data stream in which case
	* the select tells there is no more data available but there in 
	* fact is data buffered in the stream file's buffer. Since no
	* one has changed the default stream file's buffer size, let's
	* use the constant BUFSIZ until someone changes it.
	*/
	if (size < MIN_BUF_SIZE)
		size = MIN_BUF_SIZE;
	if ((buf = malloc(size)) == NULL) {
		ret = ENOMEM;
		goto err;
	}
	
	/* Pipe the rest to the FIFO */
	cprintf("Upgrading");
	while (total && *total) {
		if (waitfor(fileno(stream), 5) <= 0)
			break;
		count = safe_fread(buf, 1, size, stream);
		if (!count && (ferror(stream) || feof(stream)))
			break;
		*total -= count;
		safe_fwrite(buf, 1, count, fifo);
		cprintf(".");
	}
	fclose(fifo);
	fifo = NULL;

	/* Wait for write to terminate */
	waitpid(pid, &ret, 0);
	cprintf("done\n");

	/* Reset nonblock on the socket */
	if (fcntl(fileno(stream), F_SETFL, flags) < 0) {
		ret = errno;
		goto err;
	}

 err:
 	if (buf)
		free(buf);
	if (fifo)
		fclose(fifo);
	unlink(upload_fifo);
	return ret;
}
#endif	//WEBS

int 
sys_send_signal(char *pidfile, int sig)
{

	FILE *fp;
	pid_t pid;	    
	fp=fopen(pidfile,"r");	    
	if (fp!=NULL)
	{
	    	fscanf(fp, "%d", &pid);
	    	kill(pid, sig);
	    	fclose(fp);	 
		return 0;
	}
	return 1;
}

/* Dump firewall log */
static int
ej_dumplog(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[4096], *line, *next, *s;
	int len, ret = 0;

	time_t tm;
	char *verdict, *src, *dst, *proto, *spt, *dpt;

	if (klogctl(3, buf, 4096) < 0) {
		websError(wp, 400, "Insufficient memory\n");
		return -1;
	}

	for (next = buf; (line = strsep(&next, "\n"));) {
		if (!strncmp(line, "<4>DROP", 7))
			verdict = "denied";
		else if (!strncmp(line, "<4>ACCEPT", 9))
			verdict = "accepted";
		else
			continue;

		/* Parse into tokens */
		s = line;
		len = strlen(s);
		while (strsep(&s, " "));

		/* Initialize token values */
		time(&tm);
		src = dst = proto = spt = dpt = "n/a";

		/* Set token values */
		for (s = line; s < &line[len] && *s; s += strlen(s) + 1) {
			if (!strncmp(s, "TIME=", 5))
				tm = strtoul(&s[5], NULL, 10);
			else if (!strncmp(s, "SRC=", 4))
				src = &s[4];
			else if (!strncmp(s, "DST=", 4))
				dst = &s[4];
			else if (!strncmp(s, "PROTO=", 6))
				proto = &s[6];
			else if (!strncmp(s, "SPT=", 4))
				spt = &s[4];
			else if (!strncmp(s, "DPT=", 4))
				dpt = &s[4];
		}

		ret += websWrite(wp, "%s %s connection %s to %s:%s from %s:%s\n",
				 rfctime(&tm), proto, verdict, dst, dpt, src, spt);
		ret += websWrite(wp, "<br>");
	}

	return ret;
}

#ifndef NOUSB
static int
wan_restore_mac(webs_t wp)
{
	char tmp[50], tmp2[50], prefix[] = "wanXXXXXXXXXX_", *t2;
	int unit, errf = -1;
	char wan_ea[ETHER_ADDR_LEN];

	unit = atoi(websGetVar(wp, "wan_unit", NULL));
	if (unit >= 0)
	{
		strcpy(tmp2, nvram_safe_get("wan_ifname"));
		if (!strncmp(tmp2, "eth", 3))
		{
			sprintf(tmp, "et%dmacaddr", atoi(tmp2 + 3));
			t2 = nvram_safe_get(tmp);
			if (t2 && t2[0] != 0)
			{
				ether_atoe(t2, wan_ea);
				ether_etoa(wan_ea, tmp2);
				wan_prefix(unit, prefix);
				nvram_set("wan_hwaddr", tmp2);
				nvram_set(strcat_r(prefix, "hwaddr", tmp), tmp2);
				nvram_commit();
				errf = 0;
			}
		}
	}

	return errf;
}
#endif // NOUSB

#define sin_addr(s) (((struct sockaddr_in *)(s))->sin_addr)

/* Return WAN link state */
static int
ej_wan_link(int eid, webs_t wp, int argc, char_t **argv)
{
	char *wan_ifname;
	int s;
	struct ifreq ifr;
	struct ethtool_cmd ecmd;
	FILE *fp;
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";

	if ((unit = atoi(nvram_safe_get("wan_unit"))) < 0)
		unit = 0;
	wan_prefix(unit, prefix);

	/* non-exist and disabled */
	if (nvram_match(strcat_r(prefix, "proto", tmp), "") ||
	    nvram_match(strcat_r(prefix, "proto", tmp), "disabled")) {
		return websWrite(wp, "N/A");
	}
	/* PPPoE connection status */
	else if (nvram_match(strcat_r(prefix, "proto", tmp), "pppoe")) {
		wan_ifname = nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp));
		if ((fp = fopen(strcat_r("/tmp/ppp/link.", wan_ifname, tmp), "r"))) {
			fclose(fp);
			return websWrite(wp, "Connected");
		} else
			return websWrite(wp, "Disconnected");
	}
	/* Get real interface name */
	else
		wan_ifname = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

	/* Open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return websWrite(wp, "N/A");

	/* Check for hardware link */
	strncpy(ifr.ifr_name, wan_ifname, IFNAMSIZ);
	ifr.ifr_data = (void *) &ecmd;
	ecmd.cmd = ETHTOOL_GSET;
	if (ioctl(s, SIOCETHTOOL, &ifr) < 0) {
		close(s);
		return websWrite(wp, "Unknown");
	}
	if (!ecmd.speed) {
		close(s);
		return websWrite(wp, "Disconnected");
	}

	/* Check for valid IP address */
	strncpy(ifr.ifr_name, wan_ifname, IFNAMSIZ);
	if (ioctl(s, SIOCGIFADDR, &ifr) < 0) {
		close(s);
		return websWrite(wp, "Connecting");
	}

	/* Otherwise we are probably configured */
	close(s);
	return websWrite(wp, "Connected");
}

/* Display IP Address lease */
static int
ej_wan_lease(int eid, webs_t wp, int argc, char_t **argv)
{
	unsigned long expires = 0;
	int ret = 0;
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";

	if ((unit = atoi(nvram_safe_get("wan_unit"))) < 0)
		unit = 0;
	wan_prefix(unit, prefix);
	
	if (nvram_match(strcat_r(prefix, "proto", tmp), "dhcp")) {
		char *str;
		time_t now;

		snprintf(tmp, sizeof(tmp), "/tmp/udhcpc%d.expires", unit); 
		if ((str = file2str(tmp))) {
			expires = atoi(str);
			free(str);
		}
		time(&now);
		if (expires <= now)
			ret += websWrite(wp, "Expired");
		else
		{
                	char lease_buf[128];
                	memset(lease_buf, 0, sizeof(lease_buf));
                	reltime(expires - now, lease_buf);
			ret += websWrite(wp, "%s", lease_buf);
		}
	} else
		ret += websWrite(wp, "N/A");

	return ret;
}


/* Return a list of wan interfaces (eth0/eth1/eth2/eth3) */
static int
ej_wan_iflist(int eid, webs_t wp, int argc, char_t **argv)
{
	char name[IFNAMSIZ], *next;
	int ret = 0;
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	char ea[64];
	int s;
	struct ifreq ifr;

	/* current unit # */
	if ((unit = atoi(nvram_safe_get("wan_unit"))) < 0)
		unit = 0;
	wan_prefix(unit, prefix);
	
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return errno;
	
	/* build wan interface name list */
	foreach(name, nvram_safe_get("wan_ifnames"), next) {
		strncpy(ifr.ifr_name, name, IFNAMSIZ);
		if (ioctl(s, SIOCGIFHWADDR, &ifr))
			continue;
		ret += websWrite(wp, "<option value=\"%s\" %s>%s (%s)</option>", name,
				 nvram_match(strcat_r(prefix, "ifname", tmp), name) ? "selected" : "",
				 name, ether_etoa(ifr.ifr_hwaddr.sa_data, ea));
	}

	close(s);

	return ret;
}

#endif	// REMOVE

/******************************************************************************************************************************************/

void
sys_refresh_lease(void)
{
	//fprintf(stderr, "run sys_refresh_lease\n");
	char sigusr1[] = "-XX";

	/* Write out leases file */
	sprintf(sigusr1, "-%d", SIGUSR1);
	eval("killall", sigusr1, "udhcpd");
}

struct lease_t {
	unsigned char chaddr[16];
	u_int32_t yiaddr;
	u_int32_t expires;
	char hostname[64];
};

/* Dump leases in <tr><td>hostname</td><td>MAC</td><td>IP</td><td>expires</td></tr> format */
int
ej_lan_leases(int eid, webs_t wp, int argc, char_t **argv)
{
	fprintf(stderr, "run ej_lan_leases\n");
	FILE *fp = NULL;
	struct lease_t lease;
	int i;
	struct in_addr addr;
	unsigned long expires = 0;
	int ret = 0;

        ret += websWrite(wp, "Host Name       Mac Address       IP Address      Lease\n");
			                                                  
	/* Write out leases file */
	if (!(fp = fopen("/tmp/udhcpd-br0.leases", "r")))
		return ret;

	while (fread(&lease, sizeof(lease), 1, fp)) {
		/* Do not display reserved leases */
		if (ETHER_ISNULLADDR(lease.chaddr))
		{
			continue;
		}

		ret += websWrite(wp, "%-16s", lease.hostname);

		for (i = 0; i < 6; i++) {
			ret += websWrite(wp, "%02X", lease.chaddr[i]);
			if (i != 5) ret += websWrite(wp, ":");
		}

		addr.s_addr = lease.yiaddr;
		ret += websWrite(wp, " %-15s ", inet_ntoa(addr));
		expires = ntohl(lease.expires);

		if (expires==0xffffffff) 
		{
			ret += websWrite(wp, "Manual\n");
		}
		else if (!expires)
		{ 
			ret += websWrite(wp, "Expired\n");
		}
		else
		{ 
                        char lease_buf[128];
                        memset(lease_buf, 0, sizeof(lease_buf));
                        reltime(expires, lease_buf);
			ret += websWrite(wp, "%s\n", lease_buf);
		}
	}
	printf("lease test 3\n");	// tmp test
	fclose(fp);

#ifdef GUEST_ACCOUNT
	printf("GUEST ACCOUNT\n");	// tmp test
	if(nvram_invmatch("wl_guest_enable", "1")) return ret;

	/* Write out leases file */
	if (!(fp = fopen("/tmp/udhcpd-br1.leases", "r")))
		return ret;

	while (fread(&lease, sizeof(lease), 1, fp)) {
		/* Do not display reserved leases */
		if (ETHER_ISNULLADDR(lease.chaddr))
			continue;

		//printf("lease: %s %d\n", lease.hostname, strlen(lease.hostname));
		ret += websWrite(wp, "%-16s", lease.hostname);
		for (i = 0; i < 6; i++) {
			ret += websWrite(wp, "%02X", lease.chaddr[i]);
			if (i != 5) ret += websWrite(wp, ":");
		}
		addr.s_addr = lease.yiaddr;
		ret += websWrite(wp, " %-15s ", inet_ntoa(addr));
		expires = ntohl(lease.expires);

		if (expires==0xffffffff) ret += websWrite(wp, "Manual\n");
		else if (!expires) ret += websWrite(wp, "Expired\n");
		else
		{
                        char lease_buf[128];
                        memset(lease_buf, 0, sizeof(lease_buf));
                        reltime(expires, lease_buf);
			ret += websWrite(wp, "%s\n", lease_buf);
		}
	}
	fclose(fp);
#endif

	return ret;
}

/* Renew lease */
int
sys_renew(void)
{
	int unit;
	char tmp[100];
	char *str;
	int pid;

	if ((unit = atoi(nvram_safe_get("wan_unit"))) < 0)
		unit = 0;

	printf("sys renew\n");	// tmp test
#ifdef REMOVE	
	snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", unit);
	if ((str = file2str(tmp))) {
		pid = atoi(str);
		free(str);
		return kill(pid, SIGUSR1);
	}	
	return -1;
#else
	snprintf(tmp, sizeof(tmp), "wan_connect,%d", unit);
	nvram_set("rc_service", tmp);
	kill(1, SIGUSR1);
#endif
}

/* Release lease */
int
sys_release(void)
{
	int unit;
	char tmp[100];
	char *str;
	int pid;

	if ((unit = atoi(nvram_safe_get("wan_unit"))) < 0)
		unit = 0;
	
	//printf("sys release\n");	// tmp test
#ifdef REMOVE
	snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", unit);
	if ((str = file2str(tmp))) {
		pid = atoi(str);
		free(str);
		return kill(pid, SIGUSR2);
	}	
	return -1;
#else	
	//printf("enter kill\n");	// tmp test
	snprintf(tmp, sizeof(tmp), "wan_disconnect,%d", unit);
	nvram_set("rc_service", tmp);
	kill(1, SIGUSR1);
#endif
}

#endif	// defined(linux)

#ifdef REMOVE

#ifndef NOUSB
static const char * const apply_header =
"<head>"
"<title>Broadcom Home Gateway Reference Design: Apply</title>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
"<style type=\"text/css\">"
"body { background: white; color: black; font-family: arial, sans-serif; font-size: 9pt }"
".title	{ font-family: arial, sans-serif; font-size: 13pt; font-weight: bold }"
".subtitle { font-family: arial, sans-serif; font-size: 11pt }"
".label { color: #306498; font-family: arial, sans-serif; font-size: 7pt }"
"</style>"
"</head>"
"<body>"
"<p>"
"<span class=\"title\">APPLY</span><br>"
"<span class=\"subtitle\">This screen notifies you of any errors "
"that were detected while changing the router's settings.</span>"
"<form method=\"get\" action=\"apply.cgi\">"
"<p>"
;
static const char * const apply_footer =
"<p>"
"<input type=\"button\" name=\"action\" value=\"Continue\" OnClick=\"document.location.href='%s';\">"
"</form>"
"<p class=\"label\">&#169;2001-2004 Broadcom Corporation. All rights reserved.</p>"
"</body>"
;
#endif

static int
ej_wl_parse_str(int eid, webs_t wp, int argc, char_t **argv) 
{
	char *var, *match, *next;
	int unit, val = 0;
	char tmp[100], prefix[] = "wlXXXXXXXXXX_";
	char *name;
	char str[100];

	if (ejArgs(argc, argv, "%s %s", &var, &match) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if ((unit = atoi(nvram_safe_get("wl_unit"))) < 0)
		return -1;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

	if (wl_get_val(name, var, (void *)tmp, 100))
		return -1;

	foreach(str, tmp, next) {
		if (strncmp(str, match, sizeof(str)) == 0) {
			val = 1;
			break;
		}
	}

	return websWrite(wp, "%u", val);
}
#endif

/* Dump NAT table <tr><td>destination</td><td>MAC</td><td>IP</td><td>expires</td></tr> format */
int
ej_nat_table(int eid, webs_t wp, int argc, char_t **argv)
{
    	int needlen = 0, listlen, i, ret;
    	netconf_nat_t *nat_list = 0;
	netconf_nat_t **plist, *cur;
	char line[256], tstr[32];

	ret += websWrite(wp, "Destination     Proto.  Port Range  Redirect to\n");

    	netconf_get_nat(NULL, &needlen);

    	if (needlen > 0) 
	{

		nat_list = (netconf_nat_t *) malloc(needlen);
		if (nat_list) {
	    		memset(nat_list, 0, needlen);
	    		listlen = needlen;
	    		if (netconf_get_nat(nat_list, &listlen) == 0 && needlen == listlen) {
				listlen = needlen/sizeof(netconf_nat_t);

				for(i=0;i<listlen;i++)
				{				
				//printf("%d %d %d\n", nat_list[i].target,
			        //		nat_list[i].match.ipproto,
				//		nat_list[i].match.dst.ipaddr.s_addr);	
				if (nat_list[i].target==NETCONF_DNAT)
				{
					if (nat_list[i].match.dst.ipaddr.s_addr==0)
					{
						sprintf(line, "%-15s", "all");
					}
					else
					{
						sprintf(line, "%-15s", inet_ntoa(nat_list[i].match.dst.ipaddr));
					}


					if (ntohs(nat_list[i].match.dst.ports[0])==0)	
						sprintf(line, "%s %-7s", line, "ALL");
					else if (nat_list[i].match.ipproto==IPPROTO_TCP)
						sprintf(line, "%s %-7s", line, "TCP");
					else sprintf(line, "%s %-7s", line, "UDP");

					if (nat_list[i].match.dst.ports[0] == nat_list[i].match.dst.ports[1])
					{
						if (ntohs(nat_list[i].match.dst.ports[0])==0)	
						sprintf(line, "%s %-11s", line, "ALL");
						else
						sprintf(line, "%s %-11d", line, ntohs(nat_list[i].match.dst.ports[0]));
					}
					else 
					{
						sprintf(tstr, "%d:%d", ntohs(nat_list[i].match.dst.ports[0]),
						ntohs(nat_list[i].match.dst.ports[1]));
						sprintf(line, "%s %-11s", line, tstr);					
					}	
					sprintf(line, "%s %s\n", line, inet_ntoa(nat_list[i].ipaddr));
					ret += websWrite(wp, line);
				
				}
				}
	    		}
	    		free(nat_list);
		}
    	}
	return ret;
}

int
ej_route_table(int eid, webs_t wp, int argc, char_t **argv)
{
	char buff[256];
	int  nl = 0 ;
	struct in_addr dest;
	struct in_addr gw;
	struct in_addr mask;
	int flgs, ref, use, metric, ret;
	char flags[4];
	unsigned long int d,g,m;
	char sdest[16], sgw[16];
	FILE *fp;

        ret += websWrite(wp, "Destination     Gateway         Genmask         Flags Metric Ref    Use Iface\n");

	if (!(fp = fopen("/proc/net/route", "r"))) return 0;

	while(fgets(buff, sizeof(buff), fp) != NULL ) 
	{
		if(nl) 
		{
			int ifl = 0;
			while(buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0')
				ifl++;
			buff[ifl]=0;    /* interface */
			if(sscanf(buff+ifl+1, "%lx%lx%d%d%d%d%lx",
			   &d, &g, &flgs, &ref, &use, &metric, &m)!=7) {
				//error_msg_and_die( "Unsuported kernel route format\n");
				//continue;
			}

			ifl = 0;        /* parse flags */
			if(flgs&1)
				flags[ifl++]='U';
			if(flgs&2)
				flags[ifl++]='G';
			if(flgs&4)
				flags[ifl++]='H';
			flags[ifl]=0;
			dest.s_addr = d;
			gw.s_addr   = g;
			mask.s_addr = m;
			strcpy(sdest,  (dest.s_addr==0 ? "default" :
					inet_ntoa(dest)));
			strcpy(sgw,    (gw.s_addr==0   ? "*"       :
					inet_ntoa(gw)));
			if(nvram_match("wan_proto","pppoe") && (strstr(buff, "eth0")))
				continue;
			if (strstr(buff, "br0") || strstr(buff, "wl0"))
			{
				ret += websWrite(wp, "%-16s%-16s%-16s%-6s%-6d %-2d %7d LAN\n",
				sdest, sgw,
				inet_ntoa(mask),
				flags, metric, ref, use);
			}
			else if(!strstr(buff, "lo"))
			{
				ret += websWrite(wp, "%-16s%-16s%-16s%-6s%-6d %-2d %7d WAN\n",
				sdest, sgw,
				inet_ntoa(mask),
				flags, metric, ref, use);
			}
		}
		nl++;
	}
	fclose(fp);
}

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
int	iw_ignore_version_sp = 0;

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

#define RTPRIV_IOCTL_SHOW		SIOCIWFIRSTPRIV + 0x11
#define RTPRIV_IOCTL_GET_MAC_TABLE	SIOCIWFIRSTPRIV + 0x0F

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

int
ej_wl_status(int eid, webs_t wp, int argc, char_t **argv)
{	int ret = 0;

	int channel;
	struct iw_range	range;
	double freq;
	struct iwreq wrq0;
	struct iwreq wrq1;
	struct iwreq wrq2;
	struct iwreq wrq3;
	unsigned long phy_mode;

	if(nvram_match("sw_radio", "0"))
	{
		ret+=websWrite(wp, "Radio is disabled\n");
		return ret;
	}

	if(wl_ioctl(WIF, SIOCGIWAP, &wrq0) < 0)
	{
		ret+=websWrite(wp, "Radio is disabled\n");
		return ret;
	}

	wrq0.u.ap_addr.sa_family = ARPHRD_ETHER;
	ret+=websWrite(wp, "MAC address	: %02X:%02X:%02X:%02X:%02X:%02X\n",
			(unsigned char)wrq0.u.ap_addr.sa_data[0],
			(unsigned char)wrq0.u.ap_addr.sa_data[1],
			(unsigned char)wrq0.u.ap_addr.sa_data[2],
			(unsigned char)wrq0.u.ap_addr.sa_data[3],
			(unsigned char)wrq0.u.ap_addr.sa_data[4],
			(unsigned char)wrq0.u.ap_addr.sa_data[5]);

	if(wl_ioctl(WIF, SIOCGIWFREQ, &wrq1) < 0)
		return ret;

	char buffer[sizeof(iwrange) * 2];
	bzero(buffer, sizeof(buffer));
	wrq2.u.data.pointer = (caddr_t) buffer;
	wrq2.u.data.length = sizeof(buffer);
	wrq2.u.data.flags = 0;

	if(wl_ioctl(WIF, SIOCGIWRANGE, &wrq2) < 0)
		return ret;

	if(ralink_get_range_info(&range, buffer, wrq2.u.data.length) < 0)
		return ret;

	bzero(buffer, sizeof(unsigned long));
	wrq2.u.data.length = sizeof(unsigned long);
	wrq2.u.data.pointer = (caddr_t) buffer;
	wrq2.u.data.flags = RT_OID_GET_PHY_MODE;

	if(wl_ioctl(WIF, RT_PRIV_IOCTL, &wrq2) < 0)
		return ret;
	else
		phy_mode=wrq2.u.mode;

	freq = iw_freq2float(&(wrq1.u.freq));
	if(freq < KILO)
		channel = (int) freq;
	else
	{
		channel = iw_freq_to_channel(freq, &range);
		if(channel < 0)
			return ret;
	}

	if (nvram_match("WL_mode", "ap"))
	{
		if (nvram_match("WL_lazywds", "1") || nvram_match("WL_wdsapply_x", "1"))
			ret+=websWrite(wp, "OP Mode		: Hybrid\n");
		else
			ret+=websWrite(wp, "OP Mode		: AP\n");
	}
	else if (nvram_match("WL_mode", "wds"))
	{
		ret+=websWrite(wp, "OP Mode		: WDS Only\n");
	}
/*
	else if (nvram_match("WL_mode", "wet"))
	{
		ret+=websWrite(wp, "Mode		: Ethernet Bridge\n");
		ret+=websWrite(wp, "Channel		: %d\n", channel);
		ret+=ej_wl_sta_status(eid, wp, WIF);
		return ret;
	}

	else if (nvram_match("WL_mode", "sta"))
	{
		ret+=websWrite(wp, "Mode		: Stations\n");
		ret+=websWrite(wp, "Channel		: %d\n", channel);
		ret+=ej_wl_sta_status(eid, wp, WIF);
		return ret;
	}
*/

	if (phy_mode==PHY_11BG_MIXED)
		ret+=websWrite(wp, "Phy Mode	: 11b/g\n");
	else if (phy_mode==PHY_11B)
		ret+=websWrite(wp, "Phy Mode	: 11b\n");
	else if (phy_mode==PHY_11A)
		ret+=websWrite(wp, "Phy Mode	: 11a\n");
	else if (phy_mode==PHY_11ABG_MIXED)
		ret+=websWrite(wp, "Phy Mode	: 11a/b/g\n");
	else if (phy_mode==PHY_11G)
		ret+=websWrite(wp, "Phy Mode	: 11g\n");
	else if (phy_mode==PHY_11ABGN_MIXED)
		ret+=websWrite(wp, "Phy Mode	: 11a/b/g/n\n");
	else if (phy_mode==PHY_11N)
		ret+=websWrite(wp, "Phy Mode	: 11n\n");
	else if (phy_mode==PHY_11GN_MIXED)
		ret+=websWrite(wp, "Phy Mode	: 11g/n\n");
	else if (phy_mode==PHY_11AN_MIXED)
		ret+=websWrite(wp, "Phy Mode	: 11a/n\n");
	else if (phy_mode==PHY_11BGN_MIXED)
		ret+=websWrite(wp, "Phy Mode	: 11b/g/n\n");
	else if (phy_mode==PHY_11AGN_MIXED)
		ret+=websWrite(wp, "Phy Mode	: 11a/g/n\n");

	ret+=websWrite(wp, "Channel		: %d\n", channel);

	char data[2048];
	memset(data, 0, 2048);
	wrq3.u.data.pointer = data;
	wrq3.u.data.length = 2048;
	wrq3.u.data.flags = 0;

	if(wl_ioctl(WIF, RTPRIV_IOCTL_GET_MAC_TABLE, &wrq3) < 0)
		return ret;

	RT_802_11_MAC_TABLE* mp=(RT_802_11_MAC_TABLE*)wrq3.u.data.pointer;
	int i;

//	ret+=websWrite(wp, "\n%-4s%-20s%-4s%-11s%-11s%-11s\n", "AID", "MAC_Address", "PSM", "LastTime", "RxByte", "TxByte");

	websWrite(wp, "\n\n");
	websWrite(wp, "Stations List                           \n");
	websWrite(wp, "----------------------------------------\n");

	for(i=0;i<mp->Num;i++)
	{
		ret+=websWrite(wp, "%02X:%02X:%02X:%02X:%02X:%02X\n",
				mp->Entry[i].Addr[0], mp->Entry[i].Addr[1],
				mp->Entry[i].Addr[2], mp->Entry[i].Addr[3],
				mp->Entry[i].Addr[4], mp->Entry[i].Addr[5]
		);
	}

	return ret;
}

int
ej_getclientlist(int eid, webs_t wp, int argc, char_t **argv)
{
	int i, ret = 0;

	struct iw_range	range;
	struct iwreq wrq0;
	struct iwreq wrq1;
	double freq;
	int channel;

	if(wl_ioctl(WIF, SIOCGIWAP, &wrq0) < 0)
	{
		ret+=websWrite(wp, "Radio is disabled\n");
		return ret;
	}

	char data[2048];
	memset(data, 0, 2048);
	wrq1.u.data.pointer = data;
	wrq1.u.data.length = 2048;
	wrq1.u.data.flags = 0;
	char MAC_asus[13];
	char MAC[18];
	memset(MAC_asus, 0, 13);
	memset(MAC, 0 ,18);

	if(wl_ioctl(WIF, RTPRIV_IOCTL_GET_MAC_TABLE, &wrq1) < 0)
		return ret;

	RT_802_11_MAC_TABLE* mp=(RT_802_11_MAC_TABLE*)wrq1.u.data.pointer;

	for(i=0;i<mp->Num;i++)
	{
		sprintf(MAC_asus, "%02X%02X%02X%02X%02X%02X",
				mp->Entry[i].Addr[0], mp->Entry[i].Addr[1],
				mp->Entry[i].Addr[2], mp->Entry[i].Addr[3],
				mp->Entry[i].Addr[4], mp->Entry[i].Addr[5]
		);
	sprintf(MAC, "%02X:%02X:%02X:%02X:%02X:%02X",
				mp->Entry[i].Addr[0], mp->Entry[i].Addr[1],
				mp->Entry[i].Addr[2], mp->Entry[i].Addr[3],
				mp->Entry[i].Addr[4], mp->Entry[i].Addr[5]
		);
		ret+=websWrite(wp, "<option class=\"content_input_fd\" value=\"%s\">%s</option>", MAC_asus, MAC);
	}

	return ret;
}

void char_to_ascii(char *output, char *input)
{
	int i;
	char tmp[10];
	char *ptr;

	ptr = output;

	for( i=0; i<strlen(input); i++ )
	{
		if((input[i]>='0' && input[i] <='9')
		   ||(input[i]>='A' && input[i]<='Z')
		   ||(input[i] >='a' && input[i]<='z')
		   || input[i] == '!' || input[i] == '*'
		   || input[i] == '(' || input[i] == ')'
		   || input[i] == '_' || input[i] == '-'
		   || input[i] == "'" || input[i] == '.')
		{
			*ptr = input[i];
			ptr++;
		}
		else
		{
			sprintf(tmp, "%%%.02X", input[i]);
			strcpy(ptr, tmp);
			ptr+=3;
		}
	}

	*ptr = '\0';                                                                                                              
}

/* remove space in the end of string */
char *trim_r(char *str)
{
	int i;

	i=strlen(str);

	while(i>=1)
	{
		if (*(str+i-1) == ' ' || *(str+i-1) == 0x0a || *(str+i-1) == 0x0d) *(str+i-1)=0x0;
		else break;
		i--;
	}
	return(str);
}

char *toupperstr(char *str)
{
	int i;
	for(i=0;i<strlen(str);i++)
		str[i]=toupper(str[i]);
	return(str);
}

int
ej_getSiteSurvey(int eid, webs_t wp, int argc, char_t **argv)
{
	int retval = 0, i = 0, apCount = 0;
	char data[8192];
	char ssid_str[256];
	char header[128];
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

	nvram_set("ap_selecting", "1");
	fprintf(stderr, "Please wait");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".\n\n");
	nvram_set("ap_selecting", "0");

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
//	sprintf(header, "%-3s%-33s%-18s%-8s%-15s%-9s%-8s%-2s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode", "NT");
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
//			fprintf(stderr, "%-3s%-33s%-18s%-8s%-15s%-9s%-8s%-2s\n",
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

	retval += websWrite(wp, "<input type=\"hidden\" name=\"apinfo_n\" value=\"%d\" readonly=\"1\">", apCount);
	if (apCount==0)
	{
		retval += websWrite(wp, "<tr><td class='content_header_td_ap2'>&nbsp;</td>");
		retval += websWrite(wp, "<td class='content_header_td_ap2'><font color='#FF0000'>Finds no AP!</font></td>");
		retval += websWrite(wp, "<td class='content_header_td_ap2'>&nbsp;</td>");
		retval += websWrite(wp, "<td class='content_header_td_ap2'>&nbsp;</td>");
		retval += websWrite(wp, "<td class='content_header_td_ap2'>&nbsp;</td></tr>");
	}
	else
	for (i=0;i<apCount;i++)
	{
		retval += websWrite(wp, "<tr><td class='content_header_td_ap2'><a href=\"javascript:copyBSSID('%s')\">%s</a>", ssap->SiteSurvey[i].bssid, ssap->SiteSurvey[i].bssid);
		if (strlen(ssap->SiteSurvey[i].ssid)==0)
		{
			retval += websWrite(wp, "<td nowarp=\"nowarp\" class='content_header_td_ap2'>&nbsp;</td><input type=\"hidden\" class=\"content_input_fd\" name=\"RSSID\" value=\"\" readonly=\"1\"><input type=\"hidden\" name=\"RSSIDE\" value=\"\" readonly=\"1\">");
		}
		else
		{
			retval += websWrite(wp, "<td nowarp=\"nowarp\" class='content_header_td_ap2'><input type=\"text\" class=\"content_input_fd\" name=\"RSSID\" value=\"\" readonly=\"1\">");
			memset(ssid_str, 0, sizeof(ssid_str));
			char_to_ascii(ssid_str, trim_r(ssap->SiteSurvey[i].ssid));
			retval += websWrite(wp, "<input type=\"hidden\" name=\"RSSIDE\" value=\"%s\" readonly=\"1\">", ssid_str);
		}

		if (strncmp(ssap->SiteSurvey[i].authmode, "OPEN", 4)==0 && strncmp(ssap->SiteSurvey[i].encryption, "NONE", 4)==0)
			retval += websWrite(wp, "<td class='content_header_td_ap2'>Open System</td>");
		else
			retval += websWrite(wp, "<td class='content_header_td_ap2'>%s (%s)</td>", trim_r(ssap->SiteSurvey[i].authmode), trim_r(ssap->SiteSurvey[i].encryption));

		retval += websWrite(wp, "<td class='content_header_td_ap2' align=\"center\">%s</td>", trim_r(ssap->SiteSurvey[i].channel));
		retval += websWrite(wp, "<td class='content_header_td_ap2' align=\"center\">%s</td></tr>", trim_r(ssap->SiteSurvey[i].signal));
	}

	return retval;
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

void
get_apcli_address(char* output_address)
{
	int skfd;
	int rc;
	struct wireless_info info;
	char buffer[128];

	if (!output_address)
		return;

	/* Create a channel to the NET kernel. */
	if((skfd = iw_sockets_open()) < 0)
	{
		perror("socket");
		return;
	}

	rc = get_info(skfd, "apcli0", &info);

	/* Close the socket. */
  	close(skfd);

	if (!rc && info.b.has_essid && info.b.essid_on && info.has_ap_addr)
	{
		strcpy(output_address, iw_sawap_ntop(&info.ap_addr, buffer));
		if (	!strcmp(output_address, "Not-Associated") ||
			!strcmp(output_address, "Invalid") ||
			!strcmp(output_address, "None"))
			strcpy(output_address, "");
	}
	else
		strcpy(output_address, "");
}

int
ej_SiteSurvey(int eid, webs_t wp, int argc, char_t **argv)
{
	int retval = 0, i = 0, apCount = 0;
	char data[8192];
	char ssid_str[256];
	char header[128];
	struct iwreq wrq;
	SSA *ssap;
	char apcli_mac[18];
	int channellistnum;
	int commonchannel;
	int centralchannel;
	int ht_extcha;
	char *value;

	if (	nvram_invmatch("sw_mode_ex", "2") ||
		nvram_match("apcli_workaround", "1"))	// WPS PBC is proceeding
	{
		retval += websWrite(wp, "[");
		retval += websWrite(wp, "];");
		return retval;
	}

/*	value = nvram_safe_get("wl_country_code");
	if (	(strcasecmp(value, "CA") == 0) || (strcasecmp(value, "CO") == 0) ||
		(strcasecmp(value, "DO") == 0) || (strcasecmp(value, "GT") == 0) ||
		(strcasecmp(value, "MX") == 0) || (strcasecmp(value, "NO") == 0) ||
		(strcasecmp(value, "PA") == 0) || (strcasecmp(value, "PR") == 0) ||
		(strcasecmp(value, "TW") == 0) || (strcasecmp(value, "US") == 0) ||
		(strcasecmp(value, "UZ") == 0))
	{
		channellistnum = 11;
	}
	else if (!value ||
	{
		channellistnum = 14;
	}
	else
	{
		channellistnum = 13;
	}
*/
	channellistnum = 14;

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

	nvram_set("ap_selecting", "1");
	fprintf(stderr, "Please wait (web hook) ");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".");
	sleep(1);
	fprintf(stderr, ".\n\n");
	nvram_set("ap_selecting", "0");

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

	memset(apcli_mac, 0x0, 18);
	get_apcli_address(apcli_mac);

	memset(header, 0, sizeof(header));
//	sprintf(header, "%-3s%-33s%-18s%-8s%-15s%-9s%-8s%-2s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode", "NT");
	sprintf(header, "%-3s%-33s%-18s%-9s%-16s%-9s%-8s%-2s%-3s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode", "NT", " CC");
//	fprintf(stderr, "\n%s", header);
	fprintf(stderr, "\n%-3s%-33s%-18s%-9s%-16s%-9s%-8s%-2s%-3s%-3s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode", "NT", " CC", " EC");

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
/*
//			fprintf(stderr, "%-3s%-33s%-18s%-8s%-15s%-9s%-8s%-2s\n",
			fprintf(stderr, "%-3s%-33s%-18s%-8s%-15s%-9s%-8s%-2s %-2s\n",
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
*/
			commonchannel = atoi(trim_r(ssap->SiteSurvey[i].channel));
			centralchannel = atoi(trim_r(ssap->SiteSurvey[i].centralchannel));
		
			if (strstr(ssap->SiteSurvey[i].bsstype, "n") && (commonchannel != centralchannel))
			{
				if (commonchannel <= 4)
					ht_extcha = 1;
				else if (commonchannel > 4 && commonchannel < 8)
				{
					if (centralchannel < commonchannel)
						ht_extcha = 0;
					else
						ht_extcha = 1;
				}
				else if (commonchannel >= 8)
				{
					if ((channellistnum - commonchannel) < 4)
						ht_extcha = 0;
					else
					{
						if (centralchannel < commonchannel)
							ht_extcha = 0;
						else
							ht_extcha = 1;
					}
				}

//				fprintf(stderr, "%-3s%-33s%-18s%-8s%-15s%-9s%-8s%-2s\n",
				fprintf(stderr, "%-3s%-33s%-18s%-9s%-16s%-9s%-8s%-2s %-2s %d\n",
					ssap->SiteSurvey[i].channel,
					(char*)ssap->SiteSurvey[i].ssid,
					ssap->SiteSurvey[i].bssid,
					ssap->SiteSurvey[i].encryption,
					ssap->SiteSurvey[i].authmode,
					ssap->SiteSurvey[i].signal,
					ssap->SiteSurvey[i].wmode,
					ssap->SiteSurvey[i].bsstype,
					ssap->SiteSurvey[i].centralchannel,
					ht_extcha
				);
			}
			else
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
		}

		fprintf(stderr, "\n");
	}

	retval += websWrite(wp, "[");
	if (apCount > 0)
	for (i=0;i<apCount;i++)
	{
		retval += websWrite(wp, "[");

		if (strlen(ssap->SiteSurvey[i].ssid)==0)
			retval += websWrite(wp, "\"\", ");
		else
		{
			memset(ssid_str, 0, sizeof(ssid_str));
			char_to_ascii(ssid_str, trim_r(ssap->SiteSurvey[i].ssid));
			retval += websWrite(wp, "\"%s\", ", ssid_str);
		}

		retval += websWrite(wp, "\"%s\", ", trim_r(ssap->SiteSurvey[i].channel));
		if (strncmp(ssap->SiteSurvey[i].authmode, "OPEN", 4)==0 && strncmp(ssap->SiteSurvey[i].encryption, "NONE", 4)==0)
			retval += websWrite(wp, "\"%s\", ", "Open System");
		else if (strncmp(ssap->SiteSurvey[i].authmode, "WPAPSK", 6)==0)
			retval += websWrite(wp, "\"%s\", ", "WPA-PSK");
		else if (strncmp(ssap->SiteSurvey[i].authmode, "WPA2PSK", 7)==0)
			retval += websWrite(wp, "\"%s\", ", "WPA2-PSK");
		else
			retval += websWrite(wp, "\"%s\", ", trim_r(ssap->SiteSurvey[i].authmode));
		retval += websWrite(wp, "\"%s\", ", trim_r(ssap->SiteSurvey[i].encryption));
		retval += websWrite(wp, "\"%s\", ", trim_r(ssap->SiteSurvey[i].signal));
		retval += websWrite(wp, "\"%s\", ", trim_r(toupperstr(ssap->SiteSurvey[i].bssid)));
		retval += websWrite(wp, "\"%s\", ", trim_r(ssap->SiteSurvey[i].bsstype));
		if (strcmp(trim_r(ssap->SiteSurvey[i].wmode), "11b") == 0)
			retval += websWrite(wp, "\"%s\", ", "b");
		else if (strcmp(trim_r(ssap->SiteSurvey[i].wmode), "11a") == 0)
			retval += websWrite(wp, "\"%s\", ", "a");
		else if (strcmp(trim_r(ssap->SiteSurvey[i].wmode), "11a/n") == 0)
			retval += websWrite(wp, "\"%s\", ", "an");
		else if (strcmp(trim_r(ssap->SiteSurvey[i].wmode), "11b/g") == 0)
			retval += websWrite(wp, "\"%s\", ", "bg");
		else if (strcmp(trim_r(ssap->SiteSurvey[i].wmode), "11b/g/n") == 0)
			retval += websWrite(wp, "\"%s\", ", "bgn");
		else
			retval += websWrite(wp, "\"%s\", ", "");

		if (nvram_invmatch("sta_ssid", "") && strcmp(nvram_safe_get("sta_ssid"), trim_r((char*)ssap->SiteSurvey[i].ssid)))
		{
			if (strcmp(trim_r((char*)ssap->SiteSurvey[i].ssid), ""))
				retval += websWrite(wp, "\"%s\"", "0");				// none
			else if (!strcmp(apcli_mac, trim_r(toupperstr(ssap->SiteSurvey[i].bssid))))
			{									// hidden AP (null SSID)
				if (nvram_match("sta_auth_mode", "psk"))
				{
					if (	nvram_match("sta_connected", "1") &&
						(nvram_match("sta_authorized", "1") || nvram_match("sta_authorized", "2")) )
						retval += websWrite(wp, "\"%s\"", "4");		// in profile, connected
					else
						retval += websWrite(wp, "\"%s\"", "5");		// in profile, connecting
				}
				else
				{
					if (nvram_match("sta_connected", "1"))
						retval += websWrite(wp, "\"%s\"", "4");		// in profile, connected
					else
						retval += websWrite(wp, "\"%s\"", "5");		// in profile, connecting
				}
			}
			else									// hidden AP (null SSID)
				retval += websWrite(wp, "\"%s\"", "0");				// none
		}
		else if (nvram_invmatch("sta_ssid", "") && !strcmp(nvram_safe_get("sta_ssid"), trim_r((char*)ssap->SiteSurvey[i].ssid)))
		{
			if (!strlen(apcli_mac))
				retval += websWrite(wp, "\"%s\", ", "1");			// in profile, disconnected
			else if (!strcmp(apcli_mac, trim_r(toupperstr(ssap->SiteSurvey[i].bssid))))
			{
				if (nvram_match("sta_auth_mode", "psk"))
				{
					if (	nvram_match("sta_connected", "1") &&
						(nvram_match("sta_authorized", "1") || nvram_match("sta_authorized", "2")) )
						retval += websWrite(wp, "\"%s\"", "2");		// in profile, connected
					else
						retval += websWrite(wp, "\"%s\"", "3");		// in profile, connecting
				}
				else
				{
					if (nvram_match("sta_connected", "1"))
						retval += websWrite(wp, "\"%s\"", "2");		// in profile, connected
					else
						retval += websWrite(wp, "\"%s\"", "3");		// in profile, connecting
				}
			}
			else
				retval += websWrite(wp, "\"%s\"", "0");				// impossible...
		}
		else
			retval += websWrite(wp, "\"%s\"", "0");					// sta_ssid == ""

		if (i == apCount - 1)
			retval += websWrite(wp, "]\n");
		else
			retval += websWrite(wp, "],\n");
	}
	retval += websWrite(wp, "];");

	return retval;
}

int
ej_urelease(int eid, webs_t wp, int argc, char_t **argv)
{
	int retval = 0;

	if (	nvram_match("sw_mode_ex", "2") &&
		nvram_invmatch("lan_ipaddr_new", "") &&
		nvram_invmatch("lan_netmask_new", "") &&
		nvram_invmatch("lan_gateway_new", "") &&
		nvram_match("apcli_workaround", "0"))
	{
		retval += websWrite(wp, "[");
		retval += websWrite(wp, "\"%s\", ", nvram_safe_get("lan_ipaddr_new"));
		retval += websWrite(wp, "\"%s\", ", nvram_safe_get("lan_netmask_new"));
		retval += websWrite(wp, "\"%s\"", nvram_safe_get("lan_gateway_new"));
		retval += websWrite(wp, "];");

		kill_pidfile_s("/var/run/apcli_monitor.pid", SIGUSR1);
	}
	else
	{
		retval += websWrite(wp, "[");
		retval += websWrite(wp, "\"\", ");
		retval += websWrite(wp, "\"\", ");
		retval += websWrite(wp, "\"\"");
		retval += websWrite(wp, "];");
	}

	return retval;
}

typedef struct PACKED _WSC_CONFIGURED_VALUE {
    unsigned short WscConfigured;	// 1 un-configured; 2 configured
    unsigned char WscSsid[32 + 1];
    unsigned short WscAuthMode;		// mandatory, 0x01: open, 0x02: wpa-psk, 0x04: shared, 0x08:wpa, 0x10: wpa2, 0x
    unsigned short WscEncrypType;	// 0x01: none, 0x02: wep, 0x04: tkip, 0x08: aes
    unsigned char DefaultKeyIdx;
    unsigned char WscWPAKey[64 + 1];
} WSC_CONFIGURED_VALUE;

void getWPSAuthMode(WSC_CONFIGURED_VALUE *result, char *ret_str)
{
	if(result->WscAuthMode & 0x1)
		strcat(ret_str, "Open System");
	if(result->WscAuthMode & 0x2)
		strcat(ret_str, "WPA-Personal");
	if(result->WscAuthMode & 0x4)
		strcat(ret_str, "Shared Key");
	if(result->WscAuthMode & 0x8)
		strcat(ret_str, "WPA-Enterprise");
	if(result->WscAuthMode & 0x10)
		strcat(ret_str, "WPA2-Enterprise");
	if(result->WscAuthMode & 0x20)
		strcat(ret_str, "WPA2-Personal");
}

void getWPSEncrypType(WSC_CONFIGURED_VALUE *result, char *ret_str)
{
	if(result->WscEncrypType & 0x1)
		strcat(ret_str, "None");
	if(result->WscEncrypType & 0x2)
		strcat(ret_str, "WEP");
	if(result->WscEncrypType & 0x4)
		strcat(ret_str, "TKIP");
	if(result->WscEncrypType & 0x8)
		strcat(ret_str, "AES");
}

/*
 * these definitions are from rt2860v2 driver include/wsc.h 
 */
char *getWscStatusStr(int status)
{
	switch(status){
	case 0:
		return "Not used";
	case 1:
		return "Idle";
	case 2:
		return "WPS Fail(Ignore this if Intel/Marvell registrar used)";
	case 3:
		return "Start WPS Process";
	case 4:
		return "Received EAPOL-Start";
	case 5:
		return "Sending EAP-Req(ID)";
	case 6:
		return "Receive EAP-Rsp(ID)";
	case 7:
		return "Receive EAP-Req with wrong WPS SMI Vendor Id";
	case 8:
		return "Receive EAP-Req with wrong WPS Vendor Type";
	case 9:
		return "Sending EAP-Req(WPS_START)";
	case 10:
		return "Send M1";
	case 11:
		return "Received M1";
	case 12:
		return "Send M2";
	case 13:
		return "Received M2";
	case 14:
		return "Received M2D";
	case 15:
		return "Send M3";
	case 16:
		return "Received M3";
	case 17:
		return "Send M4";
	case 18:
		return "Received M4";
	case 19:
		return "Send M5";
	case 20:
		return "Received M5";
	case 21:
		return "Send M6";
	case 22:
		return "Received M6";
	case 23:
		return "Send M7";
	case 24:
		return "Received M7";
	case 25:
		return "Send M8";
	case 26:
		return "Received M8";
	case 27:
		return "Processing EAP Response (ACK)";
	case 28:
		return "Processing EAP Request (Done)";
	case 29:
		return "Processing EAP Response (Done)";
	case 30:
		return "Sending EAP-Fail";
	case 31:
		return "WPS_ERROR_HASH_FAIL";
	case 32:
		return "WPS_ERROR_HMAC_FAIL";
	case 33:
		return "WPS_ERROR_DEV_PWD_AUTH_FAIL";
	case 34:
		return "Configured";
	case 35:
		return "SCAN AP";
	case 36:
		return "EAPOL START SENT";
	case 37:
		return "WPS_EAP_RSP_DONE_SENT";
	case 38:
		return "WAIT PINCODE";
	case 39:
		return "WSC_START_ASSOC";
	case 0x101:
		return "PBC:TOO MANY AP";
	case 0x102:
		return "PBC:NO AP";
	case 0x103:
		return "EAP_FAIL_RECEIVED";
	case 0x104:
		return "EAP_NONCE_MISMATCH";
	case 0x105:
		return "EAP_INVALID_DATA";
	case 0x106:
		return "PASSWORD_MISMATCH";
	case 0x107:
		return "EAP_REQ_WRONG_SMI";
	case 0x108:
		return "EAP_REQ_WRONG_VENDOR_TYPE";
	case 0x109:
		return "PBC_SESSION_OVERLAP";
	default:
		return "Unknown";
	}
}

int getWscStatus()
{
	int socket_id;
	int data = 0;
	struct iwreq wrq;
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;

	if(wl_ioctl(WIF, RT_PRIV_IOCTL, &wrq) < 0)
		fprintf(stderr, "errors in getting WSC status\n");

	return data;
}

unsigned int getAPPIN()
{
	unsigned int data = 0;
	struct iwreq wrq;
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_PIN_CODE;

	if(wl_ioctl(WIF, RT_PRIV_IOCTL, &wrq) < 0)
		fprintf(stderr, "errors in getting AP PIN\n");

	return data;
}

void updateWPS()
{
	int i;
	char tmpstr[128];
	WSC_CONFIGURED_VALUE result;
	struct iwreq wrq;
	wrq.u.data.length = sizeof(WSC_CONFIGURED_VALUE);
	wrq.u.data.pointer = (caddr_t) &result;
	wrq.u.data.flags = 0;
	strcpy((char *)&result, "get_wsc_profile");

	if(wl_ioctl(WIF, RTPRIV_IOCTL_WSC_PROFILE, &wrq) < 0)
	{
		fprintf(stderr, "errors in getting WSC profile\n");
		return;
	}

	//1. WPSConfigured
	if (result.WscConfigured==2)
		nvram_set("wps_conf", "Yes");
	else
		nvram_set("wps_conf", "No");
	
	//2. WPSSSID
	memset(tmpstr, 0, sizeof(tmpstr));
	sprintf(tmpstr, "%s", result.WscSsid);
	nvram_set("wps_ssid", tmpstr);
	memset(tmpstr, 0, sizeof(tmpstr));
	char_to_ascii(tmpstr, result.WscSsid);
	nvram_set("wps_ssid2", tmpstr);

	//3. WPSAuthMode
	memset(tmpstr, 0, sizeof(tmpstr));
	getWPSAuthMode(&result, tmpstr);
	nvram_set("wps_auth_mode", tmpstr);

	//4. EncrypType
	memset(tmpstr, 0, sizeof(tmpstr));
	getWPSEncrypType(&result, tmpstr);
	nvram_set("wps_crypto", tmpstr);

	//5. DefaultKeyIdx
	memset(tmpstr, 0, sizeof(tmpstr));
	sprintf(tmpstr, "%d", result.DefaultKeyIdx);
	nvram_set("wps_key", tmpstr);

	//6. WPAKey
	memset(tmpstr, 0, sizeof(tmpstr));
	for(i=0; i<64; i++)	// WPA key default length is 64 (defined & hardcode in driver) 
	{
		sprintf(tmpstr, "%s%c", tmpstr, result.WscWPAKey[i]);
	}
	nvram_set("wps_wpa_psk", tmpstr);

	//7. WSC Status
	nvram_set("wps_status_current", getWscStatusStr(getWscStatus()));

	//8. AP PIN Code
	memset(tmpstr, 0, sizeof(tmpstr));
	sprintf(tmpstr, "%d", getAPPIN());
	nvram_set("wps_pin_ap", tmpstr);

	return;
}

/*
int
ej_wps_info(int eid, webs_t wp, int argc, char_t **argv)
{
	int i;
	char tmpstr[128];
	WSC_CONFIGURED_VALUE result;
	int retval=0;
	struct iwreq wrq;
	wrq.u.data.length = sizeof(WSC_CONFIGURED_VALUE);
	wrq.u.data.pointer = (caddr_t) &result;
	wrq.u.data.flags = 0;
	strcpy((char *)&result, "get_wsc_profile");

	if(wl_ioctl(WIF, RTPRIV_IOCTL_WSC_PROFILE, &wrq) < 0)
	{
		fprintf(stderr, "errors in getting WSC profile\n");
		return;
	}

	retval += websWrite(wp, "<wps>");

	//0. WSC Status
	retval += websWrite(wp, "<wps_info>%s</wps_info>", getWscStatusStr(getWscStatus()));

	//1. WPSConfigured
	if (result.WscConfigured==2)
		retval += websWrite(wp, "<wps_info>%s</wps_info>", "Yes");
	else
		retval += websWrite(wp, "<wps_info>%s</wps_info>", "No");
	
	//2. WPSSSID
	memset(tmpstr, 0, sizeof(tmpstr));
	char_to_ascii(tmpstr, result.WscSsid);
	retval += websWrite(wp, "<wps_info>%s</wps_info>", tmpstr);

	//3. WPSAuthMode
	memset(tmpstr, 0, sizeof(tmpstr));
	getWPSAuthMode(&result, tmpstr);
	retval += websWrite(wp, "<wps_info>%s</wps_info>", tmpstr);

	//4. EncrypType
	memset(tmpstr, 0, sizeof(tmpstr));
	getWPSEncrypType(&result, tmpstr);
	retval += websWrite(wp, "<wps_info>%s</wps_info>", tmpstr);

	//5. DefaultKeyIdx
	memset(tmpstr, 0, sizeof(tmpstr));
	sprintf(tmpstr, "%d", result.DefaultKeyIdx);
	retval += websWrite(wp, "<wps_info>%s</wps_info>", tmpstr);

	//6. WPAKey
	memset(tmpstr, 0, sizeof(tmpstr));
	for(i=0; i<64; i++)	// WPA key default lengtnvram_set("wps_key", tmpstr);h is 64 (defined & hardcode in driver) 
	{
		sprintf(tmpstr, "%s%c", tmpstr, result.WscWPAKey[i]);
	}
	if (strlen(tmpstr)==0)
		retval += websWrite(wp, "<wps_info>None</wps_info>");
	else
		retval += websWrite(wp, "<wps_info>%s</wps_info>", tmpstr);

	//7. AP PIN Code
	memset(tmpstr, 0, sizeof(tmpstr));
	sprintf(tmpstr, "%d", getAPPIN());
	retval += websWrite(wp, "<wps_info>%s</wps_info>", tmpstr);

	//8. Saved WPAKey
	retval += websWrite(wp, "<wps_info>%s</wps_info>", nvram_safe_get("WL_wpa_psk"));

	//9. WPS enable?
	retval += websWrite(wp, "<wps_info>%s</wps_info>", nvram_safe_get("wps_enable"));

	//A. WPS mode
	retval += websWrite(wp, "<wps_info>%s</wps_info>", nvram_safe_get("wps_mode"));

	retval += websWrite(wp, "</wps>");

	return retval;
}*/
int
ej_wps_info(int eid, webs_t wp, int argc, char_t **argv)
{
	int i;
	char tmpstr[128];
	WSC_CONFIGURED_VALUE result;
	int retval=0;
	struct iwreq wrq;
	wrq.u.data.length = sizeof(WSC_CONFIGURED_VALUE);
	wrq.u.data.pointer = (caddr_t) &result;
	wrq.u.data.flags = 0;
	strcpy((char *)&result, "get_wsc_profile");

	if(wl_ioctl(WIF, RTPRIV_IOCTL_WSC_PROFILE, &wrq) < 0)
	{
		fprintf(stderr, "errors in getting WSC profile\n");
		return 0;
	}

	retval += websWrite(wp, "<wps>\n");

	//0. WSC Status
	retval += websWrite(wp, "<wps_info>%s</wps_info>\n", getWscStatusStr(getWscStatus()));

	//1. WPSConfigured
	if (result.WscConfigured==2)
		retval += websWrite(wp, "<wps_info>%s</wps_info>\n", "Yes");
	else
		retval += websWrite(wp, "<wps_info>%s</wps_info>\n", "No");
	
	//2. WPSSSID
	memset(tmpstr, 0, sizeof(tmpstr));
	char_to_ascii(tmpstr, result.WscSsid);
	retval += websWrite(wp, "<wps_info>%s</wps_info>\n", tmpstr);

	//3. WPSAuthMode
	memset(tmpstr, 0, sizeof(tmpstr));
	getWPSAuthMode(&result, tmpstr);
	retval += websWrite(wp, "<wps_info>%s</wps_info>\n", tmpstr);

	//4. EncrypType
	memset(tmpstr, 0, sizeof(tmpstr));
	getWPSEncrypType(&result, tmpstr);
	retval += websWrite(wp, "<wps_info>%s</wps_info>\n", tmpstr);

	//5. DefaultKeyIdx
	memset(tmpstr, 0, sizeof(tmpstr));
	sprintf(tmpstr, "%d", result.DefaultKeyIdx);
	retval += websWrite(wp, "<wps_info>%s</wps_info>\n", tmpstr);

	//6. WPAKey
	memset(tmpstr, 0, sizeof(tmpstr));
	for(i=0; i<64; i++)	// WPA key default length is 64 (defined & hardcode in driver) 
	{
		sprintf(tmpstr, "%s%c", tmpstr, result.WscWPAKey[i]);
	}
	if (strlen(tmpstr)==0)
		retval += websWrite(wp, "<wps_info>None</wps_info>\n");
	else
		retval += websWrite(wp, "<wps_info>%s</wps_info>\n", tmpstr);

	//7. AP PIN Code
	memset(tmpstr, 0, sizeof(tmpstr));
	sprintf(tmpstr, "%d", getAPPIN());
	retval += websWrite(wp, "<wps_info>%s</wps_info>\n", tmpstr);

	//8. Saved WPAKey
	retval += websWrite(wp, "<wps_info>%s</wps_info>\n", nvram_safe_get("WL_wpa_psk"));

	//9. WPS enable?
	retval += websWrite(wp, "<wps_info>%s</wps_info>\n", nvram_safe_get("wps_enable"));

	//A. WPS mode
	retval += websWrite(wp, "<wps_info>%s</wps_info>\n", nvram_safe_get("wps_mode"));
	
	//B. current auth mode
	retval += websWrite(wp, "<wps_info>%s</wps_info>\n", nvram_safe_get("WL_auth_mode"));

	retval += websWrite(wp, "</wps>");

	return retval;
}
