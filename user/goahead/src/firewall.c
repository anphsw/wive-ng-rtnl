/*
 *	firewall.c -- Firewall Settings 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: firewall.c,v 1.29.2.1 2009-03-24 08:56:16 yy Exp $
 */

/*
 *	if  WAN or LAN ip changed, we must restart firewall.
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>

#include "utils.h"
#include "firewall.h"
#include "internet.h"

#define _PATH_PFW           "/etc"
#define _PATH_PFW_FILE      _PATH_PFW "/portforward"
#define _PATH_MACIP_FILE    _PATH_PFW "/macipfilter"
#define _PATH_WEBS_FILE     _PATH_PFW "/websfilter"

#define DD printf("---> %d\n", __LINE__);

static void websSysFirewall(webs_t wp, char_t *path, char_t *query);
char l7name[8192]; // export it for internet.c qos (The actual string is about 7200 bytes.)

int isMacValid(char *str)
{
	int i, len = strlen(str);
	if(len != 17)
		return 0;

	for(i=0; i<5; i++){
		if( (!isxdigit( str[i*3])) || (!isxdigit( str[i*3+1])) || (str[i*3+2] != ':') )
			return 0;
	}
	return (isxdigit(str[15]) && isxdigit(str[16])) ? 1: 0;
}

static int isIpValid(char *str)
{
	struct in_addr addr;	// for examination
	if( (! strcmp(T("any"), str)) || (! strcmp(T("any/0"), str)))
		return 1;

	if(! (inet_aton(str, &addr))){
		printf("isIpValid(): %s is not a valid IP address.\n", str);
		return 0;
	}
	return 1;
}

static int isNumOnly(char *str){
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if((str[i] >= '0' && str[i] <= '9'))
			continue;
		return 0;
	}
	return 1;
}

static int isAllNumAndSlash(char *str){
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if( (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '/' )
			continue;
		return 0;
	}
	return 1;
}

static int isOnlyOneSlash(char *str)
{
	int i, count=0;
	int len = strlen(str);
	for(i=0; i<len; i++)
		if( str[i] == '/')
			count++;
	return count <= 1 ? 1 : 0;
}

int isIpNetmaskValid(char *s)
{
	char str[32];
	char *slash;
	struct in_addr addr;    // for examination

	if (!s || !strlen(s))
		return 0;

	strncpy(str, s, sizeof(str));

	if( (!strcmp("any", str)) || (!strcmp("any/0", str)))
		return 1;

	if (!isAllNumAndSlash(str))
		return 0;

	if (!isOnlyOneSlash(str))
		return 0;

	slash = strchr(str, '/');
	if (slash)
	{
		int mask;

		*slash = '\0';
		slash++;
		if(!strlen(slash)){
			return 0;
		}

		if(!isNumOnly(slash)){
			return 0;
		}

		mask = atoi(slash);
		if(mask < 0 || mask > 32){
			return 0;
		}
	}

	if (! (inet_aton(str, &addr)))
	{
		printf("isIpNetmaskValid(): %s is not a valid IP address.\n", str);
		return 0;
	}
	return 1;
}

static int getDMZEnableASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, value;
	char *dmze = nvram_get(RT2860_NVRAM, "DMZEnable");

	if (dmze)
		value = atoi(dmze);
	else
		value = 0;

	if( ejArgs(argc, argv, T("%d"), &type) == 1){
		if(type == value)
			websWrite(wp, T("selected"));
		else
			websWrite(wp, T(" "));
		return 0;
	}
	return -1;
}

static int getPortForwardEnableASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, value;
	char *pfe = nvram_get(RT2860_NVRAM, "PortForwardEnable");

	if(pfe)
		value = atoi(pfe);
	else
		value = 0;

	if( ejArgs(argc, argv, T("%d"), &type) == 1){
		if(type == value)
			websWrite(wp, T("selected"));
		else
			websWrite(wp, T(" "));
		return 0;
	}
	return -1;
}

static int  getIPPortFilterEnableASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, value;
	char *pfe = nvram_get(RT2860_NVRAM, "IPPortFilterEnable");

	if(pfe)
		value = atoi(pfe);
	else
		value = 0;

	if( ejArgs(argc, argv, T("%d"), &type) == 1){
		if(type == value)
			websWrite(wp, T("selected"));
		else
			websWrite(wp, T(" "));
		return 0;
	}
	return -1;
}

static int getNums(char *value, char delimit)
{
	char *pos = value;
    int count=1;
    if(!pos)
    	return 0;
	while( (pos = strchr(pos, delimit))){
		pos = pos+1;
		count++;
	}
	return count;
}

/*
 *
 */
static void makeIPPortFilterRule(char *buf, int len, char *iface, char *mac_address,
	char *sip, char *sim, int sprf_int, int sprt_int, 
	char *dip, char *dim, int dprf_int, int dprt_int, int proto, int action)
{
	int rc = 0;
	char *pos = buf;
	char *spifw = nvram_get(RT2860_NVRAM, "SPIFWEnabled");

	switch (action)
	{
		case ACTION_DROP:
			rc = (atoi(spifw) == 0) ? 
				snprintf(pos, len, "iptables -A %s ", IPPORT_FILTER_CHAIN) :
				snprintf(pos, len, "iptables -A %s -m state --state NEW,INVALID ", IPPORT_FILTER_CHAIN);
			break;
		case ACTION_ACCEPT:
			rc = snprintf(pos, len, "iptables -A %s ", IPPORT_FILTER_CHAIN);
			break;
	}
	pos += rc;
	len -= rc;
	
	if (iface != NULL)
	{
		rc = snprintf(pos, len, "-i %s ", iface);
		pos += rc;
		len -= rc;
	}

	// write mac address
	if ((mac_address!=NULL) && (strlen(mac_address)>0))
	{
		rc = snprintf(pos, len, "-m mac --mac-source %s ", mac_address);
		pos += rc;
		len -= rc;
	}

	// write source ip
	if ((sip == NULL) || (strlen(sip)<=0))
		sip = "any/0";

	if ((sim==NULL) || (strlen(sim)==0) || (strcmp(sim, "255.255.255.255")==0))
		rc = snprintf(pos, len, "-s %s ", sip);
	else
		rc = snprintf(pos, len, "-s %s/%s ", sip, sim);
	
	pos += rc;
	len -= rc;
	
	// write destination ip
	if ((dip == NULL) || (strlen(dip)<=0))
		dip = "any/0";

	if ((dim==NULL) || (strlen(dim)==0) || (strcmp(dim, "255.255.255.255")==0))
		rc = snprintf(pos, len, "-d %s ", dip);
	else
		rc = snprintf(pos, len, "-d %s/%s ", dip, dim);
	
	pos += rc;
	len -= rc;

	// write protocol type
	if (proto == PROTO_ICMP)
	{
		rc = snprintf(pos, len, "-p icmp ");
		pos += rc;
		len -= rc;
	}
	else if ((proto == PROTO_TCP) || (proto == PROTO_UDP))
	{
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else // UDP
			rc = snprintf(pos, len, "-p udp ");
		pos += rc;
		len -= rc;

		// write source port
		if (sprf_int>0)
		{
			if (sprt_int>0)
				rc = snprintf(pos, len, "--sport %d:%d ", sprf_int, sprt_int);
			else
				rc = snprintf(pos, len, "--sport %d ", sprf_int);
			pos += rc;
			len -= rc;
		}

		// write dest port
		if (dprf_int>0)
		{
			if (dprt_int>0)
				rc = snprintf(pos, len, "--dport %d:%d ", dprf_int, dprt_int);
			else
				rc = snprintf(pos, len, "--dport %d ", dprf_int);
			pos += rc;
			len -= rc;
		}
	}
	
	switch(action)
	{
		case ACTION_DROP:			// 1 == ENABLE--DROP mode
			rc = snprintf(pos, len, "-j DROP");
			break;
		case ACTION_ACCEPT:			// 2 == ENABLE--ACCEPT mode
			rc = snprintf(pos, len, "-j ACCEPT");
			break;
	}
	
	pos += rc;
	len -= rc;
	rc = snprintf(pos, len, "\n");
}

static void makePortForwardRule(char *buf, int len, char *wan_name, char *ip_address, int proto, int prf_int, int prt_int, int rprf_int, int rprt_int, int inat_loopback)
{
	int rc = 0;
	char *pos = buf;

	// Add nat loopback
	if (inat_loopback)
	{
		// write basic rules
		rc = snprintf(pos, len, "# iptables -t nat -I %s -s %s -d %s", PORT_FORWARD_POST_CHAIN, 
			"<адрес локалки куда смотрит роутер LANном>", "<адрес сервера в локалке для которого делаем форвард>" );
		pos += rc;
		len -= rc;
		
		// write protocol type
		if (proto == PROTO_TCP)
			rc = snprintf(pos, len, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len, "-p udp ");
		else if (proto == PROTO_TCP_UDP)
			rc = snprintf(pos, len, " ");
		pos += rc;
		len -= rc;

		// write src port
		if (prf_int != 0)
		{
			rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
			snprintf(pos, len, "--dport %d ", prf_int);
			pos += rc;
			len -= rc;
		}
		
		rc = snprintf(pos, len, "--jump SNAT --to-source %s\n", "<внешний адрес роутера>");
		pos += rc;
		len -= rc;
	}

	// Add forwarding rule
	rc = snprintf(pos, len, "iptables -t nat -A %s -j DNAT -i %s ", PORT_FORWARD_PRE_CHAIN, wan_name);
	pos += rc;
	len -= rc;

	// write protocol type
	if (proto == PROTO_TCP)
		rc = snprintf(pos, len, "-p tcp ");
	else if (proto == PROTO_UDP)
		rc = snprintf(pos, len, "-p udp ");
	else if (proto == PROTO_TCP_UDP)
		rc = snprintf(pos, len, " ");
	pos += rc;
	len -= rc;

	// write src port
	if (prf_int != 0)
	{
		rc = (prt_int != 0) ?
			snprintf(pos, len, "--dport %d:%d ", prf_int, prt_int) :
			snprintf(pos, len, "--dport %d ", prf_int);
		pos += rc;
		len -= rc;
	}

	// write remote ip
	rc = snprintf(pos, len, "--to %s", ip_address);
	pos += rc;
	len -= rc;
	
	// write dst port
	if (rprf_int != 0)
	{
		rc = (rprt_int != 0) ?
			snprintf(pos, len, ":%d-%d ", rprf_int, rprt_int) :
			snprintf(pos, len, ":%d ", rprf_int);
		pos += rc;
		len -= rc;
	}
	
	rc = snprintf(pos, len, "\n");
}

static void iptablesIPPortFilterBuildScript(void)
{
	int i=0;
	int mode, sprf_int, sprt_int, proto, action, dprf_int, dprt_int;
	char rec[256];
	char cmd[1024];
	char sprf[8], sprt[8], protocol[8], iface[8];
	char dprf[8], dprt[8], wan_name[16];
	char mac_address[32], action_str[4];
	char sip[32], dip[32], sim[32], dim[32];
	char *rule, *c_if;
	char *firewall_enable, *default_policy;
	
	// Check that IP/port filter is enabled
	firewall_enable = nvram_get(RT2860_NVRAM, "IPPortFilterEnable");
	if (!firewall_enable)
	{
		printf("Warning: can't find \"IPPortFilterEnable\" in flash.\n");
		return;
	}
	mode = atoi(firewall_enable);
	if(!mode)
		return;

	rule = nvram_get(RT2860_NVRAM, "IPPortFilterRules");
	if(!rule){
		printf("Warning: can't find \"IPPortFilterRules\" in flash.\n");
		return;
	}

	default_policy = nvram_get(RT2860_NVRAM, "DefaultFirewallPolicy");
	// add the default policy to the end of FORWARD chain
	if (default_policy == NULL)
		default_policy = "0";
	
	// get wan name
	strncpy(wan_name, getWanIfNamePPP(), sizeof(wan_name)-1);

	//Generate portforward script file
	FILE *fd = fopen(_PATH_MACIP_FILE, "w");
	if (fd != NULL)
	{
		fputs("#!/bin/sh\n\n", fd);
		fputs("iptables -t filter -N macipport_filter\n", fd);
		fputs("iptables -t filter -A FORWARD -j macipport_filter\n\n", fd);
		
		while ( (getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1) )
		{
			// Get interface
			if ((getNthValueSafe(0, rec, ',', iface, sizeof(iface)) == -1))
				continue;
			
			if (strcmp(iface, "LAN")==0)
				c_if = "br0";
			else if (strcmp(iface, "VPN")==0)
				c_if = "ppp+";
			else
				c_if = wan_name;
		
			// get protocol
			if ((getNthValueSafe(1, rec, ',', protocol, sizeof(protocol)) == -1))
				continue;
		
			proto = atoi(protocol);
			switch(proto)
			{
				case PROTO_TCP:
				case PROTO_UDP:
				case PROTO_NONE:
				case PROTO_ICMP:
					break;
				default:
					continue;
			}

			// get mac address
			if ((getNthValueSafe(2, rec, ',', mac_address, sizeof(mac_address)) == -1))
				continue;
			
			if (strlen(mac_address) > 0)
			{
				if (!isMacValid(mac_address))
					continue;
			}

			// get source ip
			if ((getNthValueSafe(3, rec, ',', sip, sizeof(sip)) == -1))
				continue;
			if (!isIpNetmaskValid(sip))
				sip[0] = '\0';
		
			// get source ip mask
			if ((getNthValueSafe(4, rec, ',', sim, sizeof(sim)) == -1))
				continue;
			if (!isIpNetmaskValid(sim))
				sim[0] = '\0';

			// get source port range "from"
			if ((getNthValueSafe(5, rec, ',', sprf, sizeof(sprf)) == -1))
				continue;
			if ((sprf_int = atoi(sprf)) > 65535)
				continue;

			// get source port range "to"
			if ((getNthValueSafe(6, rec, ',', sprt, sizeof(sprt)) == -1))
				continue;
			if ((sprt_int = atoi(sprt)) > 65535)
				continue;

			// get destination ip
			if ((getNthValueSafe(7, rec, ',', dip, sizeof(dip)) == -1))
				continue;
			if (!isIpNetmaskValid(dip))
				dip[0] = '\0';
		
			// get destination ip mask
			if ((getNthValueSafe(8, rec, ',', dim, sizeof(dim)) == -1))
				continue;
			if (!isIpNetmaskValid(dim))
				dim[0] = '\0';

			// get destination port range "from"
			if ((getNthValueSafe(9, rec, ',', dprf, sizeof(dprf)) == -1))
				continue;
			if ((dprf_int = atoi(dprf)) > 65535)
				continue;

			// get destination port range "to"
			if ((getNthValueSafe(10, rec, ',', dprt, sizeof(dprt)) == -1))
				continue;
			if ((dprt_int = atoi(dprt)) > 65535)
				continue;

			// get action / policy
			if ((getNthValueSafe(11, rec, ',', action_str, sizeof(action_str)) == -1))
				continue;

			action = atoi(action_str);

			makeIPPortFilterRule(cmd, sizeof(cmd), c_if, mac_address, sip, sim, sprf_int, sprt_int, dip, dim, dprf_int, dprt_int, proto, action);
			fputs(cmd, fd);
		}

		//close file
		fclose(fd);
	}
}

static int checkNatLoopback(char *rule)
{
	char nat_loopback[8];
	char rec[256];
	int inat_loopback, i=0;
	
	while ( (getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1) )
	{
		// get Nat Loopback enable flag
		if ((getNthValueSafe(7, rec, ',', nat_loopback, sizeof(nat_loopback)) == -1))
			continue;
		
		if (strlen(nat_loopback) > 0)
			inat_loopback = atoi(nat_loopback);
		else
			inat_loopback = 0;
		
		if (inat_loopback)
			return 1;
	}
	
	return 0;
}

static void iptablesPortForwardBuildScript(void)
{
	char rec[256];
	char cmd[1024];
	char wan_name[16];
	char ip_address[32], prf[8], prt[8], rprf[9], rprt[8], protocol[8], interface[8], nat_loopback[8];
	char *rule, *c_if;
	char *firewall_enable;
	int i=0, prf_int, prt_int, rprf_int, rprt_int, proto, inat_loopback;

	//Remove portforward script
	firewall_enable = nvram_get(RT2860_NVRAM, "PortForwardEnable");
	if (!firewall_enable)
	{
		printf("Warning: can't find \"PortForwardEnable\" in flash\n");
		return;
	}

	if (atoi(firewall_enable))
	{
		rule = nvram_get(RT2860_NVRAM, "PortForwardRules");
		if(!rule)
		{
			printf("Warning: can't find \"PortForwardRules\" in flash\n");
			return;
		}
	}
	else
		return;

	// get wan name
	strncpy(wan_name, getWanIfNamePPP(), sizeof(wan_name)-1);

	// Generate portforward script file
	FILE *fd = fopen(_PATH_PFW_FILE, "w");

	if (fd != NULL)
	{
		fputs("#!/bin/sh\n\n", fd);
		fprintf(fd, 
			"iptables -t nat -N %s\n"
			"iptables -t nat -A PREROUTING -j %s\n\n",
			PORT_FORWARD_PRE_CHAIN, PORT_FORWARD_PRE_CHAIN);

		// Check if nat loopback is enabled
		if (checkNatLoopback(rule))
			fprintf(fd, 
				"iptables -t nat -N %s\n"
				"iptables -t nat -A POSTROUTING -j %s\n\n",
				PORT_FORWARD_POST_CHAIN, PORT_FORWARD_POST_CHAIN);

		while( (getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1) )
		{
			// get interface
			if ((getNthValueSafe(0, rec, ',', interface, sizeof(interface)) == -1))
				continue;
		
			// get protocol
			if ((getNthValueSafe(1, rec, ',', protocol, sizeof(protocol)) == -1))
				continue;

			proto = atoi(protocol);
			switch(proto)
			{
				case PROTO_TCP:
				case PROTO_UDP:
				case PROTO_TCP_UDP:
					break;
				default:
					continue;
			}
		
			// get port range "from"
			if ((getNthValueSafe(2, rec, ',', prf, sizeof(prf)) == -1))
				continue;

			if (strlen(prf) > 0)
			{
				if ((prf_int = atoi(prf)) == 0 || prf_int > 65535)
					continue;
			}
			else
				prf_int = 0;

			// get port range "to"
			if ((getNthValueSafe(3, rec, ',', prt, sizeof(prt)) == -1))
				continue;

			if (prt > 0)
			{
				if ((prt_int = atoi(prt)) > 65535)
					continue;
			}
			else
				prt_int = 0;
		
			// get ip address
			if ((getNthValueSafe(4, rec, ',', ip_address, sizeof(ip_address)) == -1))
				continue;

			if (!isIpValid(ip_address))
				continue;

			// get forward port range "from"
			if ((getNthValueSafe(5, rec, ',', rprf, sizeof(rprf)) == -1))
				continue;

			if (strlen(rprf) > 0)
			{
				rprf_int = atoi(rprf);
				if (rprf_int > 65535)
					continue;
			}
			else
				rprf_int = 0;

			// get port range "to"
			if ((getNthValueSafe(6, rec, ',', rprt, sizeof(rprt)) == -1))
				continue;

			if (strlen(rprt) > 0)
			{
				if ((rprt_int = atoi(rprt)) > 65535)
					continue;
			}
			else
				rprt_int = 0;
			
			// get Nat Loopback enable flag
			if ((getNthValueSafe(7, rec, ',', nat_loopback, sizeof(nat_loopback)) == -1))
				continue;
			
			if (strlen(nat_loopback) > 0)
				inat_loopback = atoi(nat_loopback);
			else
				inat_loopback = 0;

			
			// Patch interface
			if (strcmp(interface, "LAN")==0)
				c_if = "br0";
			else if (strcmp(interface, "VPN")==0)
				c_if = "ppp+";
			else
				c_if = wan_name;
			
			switch(proto)
			{
				case PROTO_TCP:
				case PROTO_UDP:
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, proto, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd);
					break;
				
				case PROTO_TCP_UDP:
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, PROTO_TCP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd);
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, PROTO_UDP, prf_int, prt_int, rprf_int, rprt_int, inat_loopback);
					fputs(cmd, fd);
					break;
			}
		}
		
		//close file
		fclose(fd);
	}
}

inline int getRuleNums(char *rules)
{
	return getNums(rules, ';');
}

static int checkIfUnderBridgeModeASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char *mode = nvram_get(RT2860_NVRAM, "OperationMode");

	if(!mode)
		return -1;	// fatal error, make ASP engine complained.
	if(atoi(mode) == 0)	// bridge mode
		websWrite(wp, T(HTML_NO_FIREWALL_UNDER_BRIDGE_MODE));
	return 0;
}

/*
 * ASP function
 */
static int getPortForwardRules(int eid, webs_t wp, int argc, char_t **argv)
{
	int first=1, i=0;
	int prf_int, prt_int, rprf_int, rprt_int, proto, inat_loopback;
	char ip_address[32], prf[8], prt[8], rprf[8], rprt[8], comment[64], protocol[8], interface[8], nat_loopback[8];
	char rec[128];
	char *rules = nvram_get(RT2860_NVRAM, "PortForwardRules");

	if (rules == NULL)
		return 0;
	if (strlen(rules) == 0)
		return 0;

	/* format is :
	 * [interface],[protocol],[src_port],[dst_port],[ip_address],[redirect_src_port],[redirect_dst_port],[nat_loopback],[comment];
	 */
	while (getNthValueSafe(i++, rules, ';', rec, sizeof(rec)) != -1 )
	{
		// get interface
		if ((getNthValueSafe(0, rec, ',', interface, sizeof(interface)) == -1))
			continue;
		
		// get protocol
		if ((getNthValueSafe(1, rec, ',', protocol, sizeof(protocol)) == -1))
			continue;

		proto = atoi(protocol);
		switch(proto)
		{
			case PROTO_TCP:
			case PROTO_UDP:
			case PROTO_TCP_UDP:
				break;
			default:
				continue;
		}
		
		// get port range "from"
		if ((getNthValueSafe(2, rec, ',', prf, sizeof(prf)) == -1))
			continue;

		if (strlen(prf) > 0)
		{
			if ((prf_int = atoi(prf)) == 0 || prf_int > 65535)
				continue;
		}

		// get port range "to"
		if ((getNthValueSafe(3, rec, ',', prt, sizeof(prt)) == -1))
			continue;

		if (prt > 0)
		{
			if ((prt_int = atoi(prt)) > 65535)
				continue;
		}
		
		// get ip address
		if ((getNthValueSafe(4, rec, ',', ip_address, sizeof(ip_address)) == -1))
			continue;

		if (!isIpValid(ip_address))
			continue;

		// get forward port range "from"
		if ((getNthValueSafe(5, rec, ',', rprf, sizeof(rprf)) == -1))
			continue;

		if (strlen(rprf) > 0)
		{
			if ((rprf_int = atoi(rprf)) == 0 || rprf_int > 65535)
				continue;
		}

		// get port range "to"
		if ((getNthValueSafe(6, rec, ',', rprt, sizeof(rprt)) == -1))
			continue;

		if (rprt > 0)
		{
			if ((rprt_int = atoi(rprt)) > 65535)
				continue;
		}

		// get Nat Loopback enable flag
		if ((getNthValueSafe(7, rec, ',', nat_loopback, sizeof(nat_loopback)) == -1))
			continue;
		
		if (strlen(nat_loopback) > 0)
			inat_loopback = atoi(nat_loopback);
		else
			inat_loopback = 0;


		// Get comment
		if ((getNthValueSafe(8, rec, ',', comment, sizeof(comment)) == -1))
			continue;

		// Output data
		websWrite(wp, T("%s[ '%s', %d, '%s', '%s', '%s', '%s', '%s', %d, '%s' ]"),
				(first) ? "" : ",\n\t",
				interface,
				proto,
				prf, prt,
				ip_address,
				rprf, rprt,
				inat_loopback,
				comment
			);
		
		first = 0;
	}
	
	return 0;
}

static void getRulesPacketCount(webs_t wp, char_t *path, char_t *query)
{
	FILE *fp;
	int i, step_in_chains=0;
	char buf[1024], *default_policy;
	int default_drop_flag;
	int index=0, pkt_count;
	int *result;

	// check if the default policy is "drop" 
	default_policy = nvram_get(RT2860_NVRAM, "DefaultFirewallPolicy");
	if(!default_policy)
		default_policy = "0";
	default_drop_flag = atoi(default_policy);

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	result = (int *)malloc(sizeof(int) * 128);
	if(!result)
		goto error;

	fp = popen("iptables -t filter -L -v", "r");
	if(!fp)
	{
		free(result);
		goto error;
	}

	while(fgets(buf, 1024, fp) && index < 128)
	{
		if (step_in_chains)
		{
			if(buf[0] == '\n')
				break;
			if(buf[0] == ' ' && buf[1] == 'p' && buf[2] == 'k' && buf[3] == 't' )
				continue;
			// Skip the first one rule if default policy is drop.
			if(default_drop_flag)
			{
				default_drop_flag = 0;
				continue;
			}
			sscanf(buf, "%d ", &pkt_count);
			result[index++] = pkt_count;
		}

		if(strstr(buf, "Chain " IPPORT_FILTER_CHAIN))
			step_in_chains = 1;
	}
	pclose(fp);

	if(index > 0)
		websWrite(wp, "%d", result[0]);
	for(i=1; i<index; i++)
		websWrite(wp, " %d", result[i]);

	free(result);
error:
	websDone(wp, 200);
	return;
}


/*
 * ASP function
 */
static int getPortFilteringRules(int eid, webs_t wp, int argc, char_t **argv)
{
	int i = 0, first = 1;
	int sprf_int, sprt_int, proto, dprf_int, dprt_int;
	char mac_address[32];
	char sip[32], sprf[8], sprt[8], comment[16], protocol[8], action[4];
	char dip[32], dprf[8], dprt[8], iface[8], sim[32], dim[32];
	char rec[256];
	char *rules = nvram_get(RT2860_NVRAM, "IPPortFilterRules");

	if (rules == NULL)
		return 0;

	while (getNthValueSafe(i++, rules, ';', rec, sizeof(rec)) != -1 && strlen(rec))
	{
		// Get interface
		if ((getNthValueSafe(0, rec, ',', iface, sizeof(iface)) == -1))
			continue;
		
		// get protocol
		if ((getNthValueSafe(1, rec, ',', protocol, sizeof(protocol)) == -1))
			continue;
		
		proto = atoi(protocol);
		switch(proto)
		{
			case PROTO_TCP:
			case PROTO_UDP:
			case PROTO_NONE:
			case PROTO_ICMP:
				break;
			default:
				continue;
		}

		// get mac address
		if ((getNthValueSafe(2, rec, ',', mac_address, sizeof(mac_address)) == -1))
			continue;

		// get source ip
		if ((getNthValueSafe(3, rec, ',', sip, sizeof(sip)) == -1))
			continue;
		if (!isIpNetmaskValid(sip))
			sip[0] = '\0';
		
		// get source ip mask
		if ((getNthValueSafe(4, rec, ',', sim, sizeof(sim)) == -1))
			continue;
		if (!isIpNetmaskValid(sim))
			sim[0] = '\0';

		// get source port range "from"
		if ((getNthValueSafe(5, rec, ',', sprf, sizeof(sprf)) == -1))
			continue;
		if ((sprf_int = atoi(sprf)) > 65535)
			continue;

		// get source port range "to"
		if ((getNthValueSafe(6, rec, ',', sprt, sizeof(sprt)) == -1))
			continue;
		if ((sprt_int = atoi(sprt)) > 65535)
			continue;

		// get destination ip
		if ((getNthValueSafe(7, rec, ',', dip, sizeof(dip)) == -1))
			continue;
		if (!isIpNetmaskValid(dip))
			dip[0] = '\0';
		
		// get destination ip mask
		if ((getNthValueSafe(8, rec, ',', dim, sizeof(dim)) == -1))
			continue;
		if (!isIpNetmaskValid(dim))
			dim[0] = '\0';

		// get destination port range "from"
		if ((getNthValueSafe(9, rec, ',', dprf, sizeof(dprf)) == -1))
			continue;
		if ((dprf_int = atoi(dprf)) > 65535)
			continue;

		// get destination port range "to"
		if ((getNthValueSafe(10, rec, ',', dprt, sizeof(dprt)) == -1))
			continue;
		if ((dprt_int = atoi(dprt)) > 65535)
			continue;

		// get action / policy
		if ((getNthValueSafe(11, rec, ',', action, sizeof(action)) == -1))
			continue;

		// get comment
		if ((getNthValueSafe(12, rec, ',', comment, sizeof(comment)) == -1))
			continue;

		// Output data
		websWrite(wp, T("%s[ '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s' ]"),
				(first) ? "" : ",\n\t",
				iface,
				proto,
				mac_address,
				sip, sim,
				sprf, sprt,
				dip, dim,
				dprf, dprt,
				action,
				comment
			);
		
		first = 0;
	}

	return 0;
}

static int showDMZIPAddressASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char *DMZIPAddress = nvram_get(RT2860_NVRAM, "DMZIPAddress");

	if(!DMZIPAddress)
		return 0;
	if(!strlen(DMZIPAddress))
		return 0;

	websWrite(wp, T("%s"), DMZIPAddress);
	return 0;
}

static void portForward(webs_t wp, char_t *path, char_t *query)
{
	char *pfe               = websGetVar(wp, T("portForwardEnabled"), T(""));
	char *PortForwardRules  = websGetVar(wp, T("portForwardRules"), T(""));

	if ((pfe==NULL) || (strcmp(pfe, "1")!=0))
		pfe = "0";

	// Commit
	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "PortForwardEnable", pfe);
	if (strcmp(pfe, "1") == 0)
		nvram_bufset(RT2860_NVRAM, "PortForwardRules", PortForwardRules);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	websHeader(wp);
	websWrite(wp, T("portForwardEnabled: %s<br>\n"), pfe);
	websFooter(wp);
	websDone(wp, 200);

	// call iptables
	firewall_rebuild();
}

static void portFiltering(webs_t wp, char_t *path, char_t *query)
{
	char *firewall_enable   = websGetVar(wp, T("portFilterEnabled"), T(""));
	char *default_policy    = websGetVar(wp, T("defaultFirewallPolicy"), T("0"));
	char *firewall_rules    = websGetVar(wp, T("portFilteringRules"), T(""));
	
	if ((firewall_enable == NULL) || (strcmp(firewall_enable, "1") != 0))
		firewall_enable = "0";
	if ((default_policy == NULL) || (strcmp(default_policy, "1") != 0))
		default_policy = "0";

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "IPPortFilterEnable", firewall_enable);
	if (strcmp(firewall_enable, "1") == 0)
	{
		// Store default firewall policy & rules
		nvram_bufset(RT2860_NVRAM, "DefaultFirewallPolicy", default_policy);
		nvram_bufset(RT2860_NVRAM, "IPPortFilterRules", firewall_rules);
	}

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	websHeader(wp);
	websWrite(wp, T("portFilteringEnabled: %s<br>\n"), firewall_enable);
	websWrite(wp, T("default_policy: %s<br>\n"), default_policy);
	websFooter(wp);
	websDone(wp, 200);

	// Call iptables
	firewall_rebuild();
}

static void DMZ(webs_t wp, char_t *path, char_t *query)
{
	char *dmzE, *ip_address;

	dmzE = websGetVar(wp, T("DMZEnabled"), T(""));
	ip_address = websGetVar(wp, T("DMZIPAddress"), T(""));

	// someone use malform page.....
	if(!dmzE && !strlen(dmzE))
		return;

	// we dont trust user input, check all things before doing changes
	if(atoi(dmzE) && !isIpValid(ip_address))	// enable && invalid mac address
		return;

	if(atoi(dmzE) == 0){		// disable
		nvram_set(RT2860_NVRAM, "DMZEnable", "0");
	}else{					// enable
		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "DMZEnable", "1");
		if(strlen(ip_address)){
			nvram_bufset(RT2860_NVRAM, "DMZIPAddress", ip_address);
		}
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	
	websHeader(wp);
	websWrite(wp, T("DMZEnabled: %s<br>\n"), dmzE);
	websWrite(wp, T("ip_address: %s<br>\n"), ip_address);
	websFooter(wp);
	websDone(wp, 200);

	// Call iptables
	firewall_rebuild();
}

static void websSysFirewall(webs_t wp, char_t *path, char_t *query)
{
	char *wpfE = websGetVar(wp, T("pingFrmWANFilterEnabled"), T(""));

	// someone use malform page.....
	if(!wpfE || !strlen(wpfE))
		return;

	
	websHeader(wp);
	websWrite(wp, T("WANPingFilter: %s<br>\n"), wpfE);
	websFooter(wp);
	websDone(wp, 200);

	// call iptables
	firewall_rebuild();
}


/* Same as the file "linux/netfilter_ipv4/ipt_webstr.h" */
#define BLK_JAVA                0x01
#define BLK_ACTIVE              0x02
#define BLK_COOKIE              0x04
#define BLK_PROXY               0x08
void iptablesWebsFilterRun(void)
{
    int i, content_filter = 0;
    char entry[256], buff[4096]; //need long buffer for utf domain name encoding support 
    char *proxy		= nvram_get(RT2860_NVRAM, "websFilterProxy");
    char *java		= nvram_get(RT2860_NVRAM, "websFilterJava");
    char *activex	= nvram_get(RT2860_NVRAM, "websFilterActivex");
    char *cookies	= nvram_get(RT2860_NVRAM, "websFilterCookies");
    char *url_filter	= nvram_get(RT2860_NVRAM, "websURLFilters");
    char *host_filter	= nvram_get(RT2860_NVRAM, "websHostFilters");
    
    if ((url_filter && strlen(url_filter) && getRuleNums(url_filter)) || 
	(host_filter && strlen(host_filter) && getRuleNums(host_filter)) || 
		atoi(proxy) || atoi(java) || atoi(activex) || atoi(cookies)) {

	// Content filter
	if(atoi(java))
	    content_filter += BLK_JAVA;
	if(atoi(activex))
	    content_filter += BLK_ACTIVE;
	if(atoi(cookies))
	    content_filter += BLK_COOKIE;
	if(atoi(proxy))
	    content_filter += BLK_PROXY;

	//Generate portforward script file
	FILE *fd = fopen(_PATH_WEBS_FILE, "w");
	if (fd != NULL) {
	    fputs("#!/bin/sh\n\n", fd);
	    fputs("iptables -t filter -N web_filter\n", fd);
	    fputs("iptables -t filter -A FORWARD -j web_filter\n", fd);

	    if (content_filter) {
		// Why only 3 ports are inspected?(This idea is from CyberTAN source code)
		// TODO: use layer7 to inspect HTTP
		sprintf(buff, "iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp --dport 80   -m webstr --content %d -j REJECT --reject-with tcp-reset\n", content_filter);
		fputs(buff, fd);
		sprintf(buff, "iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp --dport 3128 -m webstr --content %d -j REJECT --reject-with tcp-reset\n", content_filter);
		fputs(buff, fd);
		sprintf(buff, "iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp --dport 8080 -m webstr --content %d -j REJECT --reject-with tcp-reset\n", content_filter);
		fputs(buff, fd);
	    }

	    // URL filter
	    i=0;
	    while ((i < getRuleNums(url_filter)) && (getNthValueSafe(i, url_filter, ';', entry, sizeof(entry)) != -1)) {
		if (strlen(entry)) {
		    if(!strncasecmp(entry, "http://", strlen("http://")))
			strcpy(entry, entry + strlen("http://"));
		    sprintf(buff, "iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp -m webstr --url  %s -j REJECT --reject-with tcp-reset\n", entry);
		    fputs(buff, fd);
		}
	    i++;
	    }

	    // HOST(Keyword) filter
	    i=0;
	    while ((i < getRuleNums(host_filter)) && (getNthValueSafe(i, host_filter, ';', entry, sizeof(entry)) != -1)) {
		if (strlen(entry)) {
		    sprintf(buff, "iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp -m webstr --host %s -j REJECT --reject-with tcp-reset\n", entry);
		    fputs(buff, fd);
		}
	    i++;
	    }
	    //closefile
	    fclose(fd);
	}
    } else {
	printf("Content filter disabled.\n");
	return;
    }
  return;
}


static void websURLFilterDelete(webs_t wp, char_t *path, char_t *query)
{
	int i, j, rule_count;
	char_t name_buf[16];
	char_t *value;
	int *deleArray;

	char *rules = nvram_get(RT2860_NVRAM, "websURLFilters");
	if(!rules || !strlen(rules) )
		return;

	rule_count = getRuleNums(rules);
	if(!rule_count)
		return;

	deleArray = (int *)malloc(rule_count * sizeof(int));

	for (i=0, j=0; i< rule_count; i++)
	{
		snprintf(name_buf, 16, "DR%d", i);
		value = websGetVar(wp, name_buf, NULL);
		if (value)
			deleArray[j++] = i;
	}
	if (!j)
	{
		websHeader(wp);
		websWrite(wp, T("You didn't select any rules to delete.<br>\n"));
		websFooter(wp);
		websDone(wp, 200);
		return;
	}

	deleteNthValueMulti(deleArray, rule_count, rules, ';');
	free(deleArray);

	nvram_set(RT2860_NVRAM, "websURLFilters", rules);
	

	websHeader(wp);
	websWrite(wp, T("Delete : success<br>\n") );
	websFooter(wp);
	websDone(wp, 200);

	//call iptables
	firewall_rebuild();

	return;
}

static void websHostFilterDelete(webs_t wp, char_t *path, char_t *query)
{
	int i, j, rule_count;
	char_t name_buf[16];
	char_t *value;
	int *deleArray;

	char *rules = nvram_get(RT2860_NVRAM, "websHostFilters");
	if(!rules || !strlen(rules) )
		return;

	rule_count = getRuleNums(rules);
	if(!rule_count)
		return;

	deleArray = (int *)malloc(rule_count * sizeof(int));

	for(i=0, j=0; i< rule_count; i++){
		snprintf(name_buf, 16, "DR%d", i);
		value = websGetVar(wp, name_buf, NULL);
		if(value){
			deleArray[j++] = i;
		}
	}
	if(!j){
	    websHeader(wp);
	    websWrite(wp, T("You didn't select any rules to delete.<br>\n"));
	    websFooter(wp);
	    websDone(wp, 200);		
		return;
	}

	deleteNthValueMulti(deleArray, rule_count, rules, ';');
	free(deleArray);

	nvram_set(RT2860_NVRAM, "websHostFilters", rules);
	

	websHeader(wp);
	websWrite(wp, T("Delete : success<br>\n") );
	websFooter(wp);
	websDone(wp, 200);

	//call iptables
	firewall_rebuild();

	return;
}

static void webContentFilter(webs_t wp, char_t *path, char_t *query)
{
	char *proxy = websGetVar(wp, T("websFilterProxy"), T(""));
	char *java = websGetVar(wp, T("websFilterJava"), T(""));
	char *activex = websGetVar(wp, T("websFilterActivex"), T(""));
	char *cookies = websGetVar(wp, T("websFilterCookies"), T(""));

	// someone use malform page.....
	if(!proxy || !java || !activex || !cookies)
		return;

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "websFilterProxy",   atoi(proxy)   ? "1" : "0" );
	nvram_bufset(RT2860_NVRAM, "websFilterJava",    atoi(java)    ? "1" : "0" );
	nvram_bufset(RT2860_NVRAM, "websFilterActivex", atoi(activex) ? "1" : "0" );
	nvram_bufset(RT2860_NVRAM, "websFilterCookies", atoi(cookies) ? "1" : "0" );
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);
	
	websHeader(wp);
	websWrite(wp, T("Proxy: %s<br>\n"),  atoi(proxy) ? "enable" : "disable");
	websWrite(wp, T("Java: %s<br>\n"),   atoi(java) ? "enable" : "disable");
	websWrite(wp, T("Activex: %s<br>\n"), atoi(activex) ? "enable" : "disable");
	websWrite(wp, T("Cookies: %s<br>\n"), atoi(cookies) ? "enable" : "disable");
	websFooter(wp);
	websDone(wp, 200);

	//call iptables
	firewall_rebuild();
}

static void websURLFilter(webs_t wp, char_t *path, char_t *query)
{
	char *urlfilters = nvram_get(RT2860_NVRAM, "websURLFilters");
	char *rule = websGetVar(wp, T("addURLFilter"), T(""));
	char *new_urlfilters;
	if(!rule)
		return;
	if(strchr(rule, ';'))
		return;

	if(!urlfilters || !strlen(urlfilters))
		nvram_set(RT2860_NVRAM, "websURLFilters", rule);
	else
	{
		if(! (new_urlfilters = (char *)malloc(sizeof(char) * (strlen(urlfilters)+strlen(rule)+1))))
			return;
		new_urlfilters[0] = '\0';
		strcat(new_urlfilters, urlfilters);
		strcat(new_urlfilters, ";");
		strcat(new_urlfilters, rule);
		nvram_set(RT2860_NVRAM, "websURLFilters", new_urlfilters);
		free(new_urlfilters);
	}
	


	websHeader(wp);
	websWrite(wp, T("add URL filter: %s<br>\n"), rule);
	websFooter(wp);
	websDone(wp, 200);

	//call iptables
	firewall_rebuild();
}

static void websHostFilter(webs_t wp, char_t *path, char_t *query)
{
	char *hostfilters = nvram_get(RT2860_NVRAM, "websHostFilters");
	char *rule = websGetVar(wp, T("addHostFilter"), T(""));
	char *new_hostfilters;
	if(!rule)
		return;
	if(strchr(rule, ';'))
		return;

	if(!hostfilters || !strlen(hostfilters))
		nvram_set(RT2860_NVRAM, "websHostFilters", rule);
	else{
		if(! (new_hostfilters = (char *)malloc(sizeof(char) * (strlen(hostfilters)+strlen(rule)+1))))
			return;
		new_hostfilters[0] = '\0';
		strcat(new_hostfilters, hostfilters);
		strcat(new_hostfilters, ";");
		strcat(new_hostfilters, rule);
		nvram_set(RT2860_NVRAM, "websHostFilters", new_hostfilters);
		free(new_hostfilters);
	}
	
	websHeader(wp);
	websWrite(wp, T("add Host filter: %s<br>\n"), rule);
	websFooter(wp);
	websDone(wp, 200);

	//call iptables
	firewall_rebuild();
}

char *getNameIntroFromPat(char *filename)
{
	static char result[512];
	char buf[512], *begin, *end, *desh;
	char path_filename[512];
	char *rc;
	FILE *fp;

	sprintf(path_filename, "%s/%s", "/etc_ro/l7-protocols", filename);
	if(! (fp = fopen(path_filename, "r")))
		return NULL;
	result[0] = '\0';
	rc = fgets(buf, sizeof(buf), fp);
	if (rc)
	{
		// find name
		begin = buf + 2;
		if(! ( desh = strchr(buf, '-'))){
			printf("warning: can't find %s name.\n", filename);
			fclose(fp);
			return "N/A#N/A";
		}
		end = desh;
		if(*(end-1) == ' ')
			end--;
		*end = '\0';
		strncat(result, begin, sizeof(result));
		strncat(result, "#", sizeof(result));

		// find intro
		if(!(end = strchr(desh+1, '\n'))){
			printf("warning: can't find %s intro.\n", filename);
			fclose(fp);
			return "N/A#N/A";
		}
		*end = '\0';
		strncat(result, desh + 2 , sizeof(result));
	}
	else
	{
		printf("warning: can't read %s intro.\n", filename);
		fclose(fp);
		return "N/A#N/A";
	}

	fclose(fp);
	return result;
}


void LoadLayer7FilterName(void)
{
	char *delim;
	struct dirent *dir;
	DIR *d;
	char *intro;

	l7name[0] = '\0';
	if(!(d = opendir("/etc_ro/l7-protocols")))
		return;
	
	while ((dir = readdir(d)))
	{
		if(dir->d_name[0] == '.')
			continue;
		if(!(delim = strstr(dir->d_name, ".pat")) )
			continue;
		
		intro = getNameIntroFromPat(dir->d_name);

		*delim = '\0';
		if(l7name[0] == '\0'){
			strncat(l7name, dir->d_name, sizeof(l7name));
			strncat(l7name, "#", sizeof(l7name));
			strncat(l7name, intro, sizeof(l7name));
		}else{
			strncat(l7name, ";", sizeof(l7name));
			strncat(l7name, dir->d_name, sizeof(l7name));
			strncat(l7name, "#", sizeof(l7name));
			strncat(l7name, intro, sizeof(l7name));
		}
	}
	closedir(d);
}

static int getLayer7FiltersASP(int eid, webs_t wp, int argc, char_t **argv)
{
	websLongWrite(wp, l7name);
	return 0;
}

void formDefineFirewall(void)
{
	websFormDefine(T("portFiltering"), portFiltering);

	websAspDefine(T("getPortFilteringRules"), getPortFilteringRules);
	websFormDefine(T("getRulesPacketCount"), getRulesPacketCount);

	websFormDefine(T("DMZ"), DMZ);
	websAspDefine(T("getDMZEnableASP"), getDMZEnableASP);
	websAspDefine(T("showDMZIPAddressASP"), showDMZIPAddressASP);

	websAspDefine(T("getPortForwardRules"), getPortForwardRules);
	websFormDefine(T("portForward"), portForward);

	websFormDefine(T("websSysFirewall"), websSysFirewall);
	websFormDefine(T("webContentFilter"), webContentFilter);
	websFormDefine(T("websURLFilterDelete"), websURLFilterDelete);
	websFormDefine(T("websHostFilterDelete"), websHostFilterDelete);
	websFormDefine(T("websHostFilter"), websHostFilter);
	websFormDefine(T("websURLFilter"), websURLFilter);

	websAspDefine(T("getLayer7FiltersASP"), getLayer7FiltersASP);

	websAspDefine(T("checkIfUnderBridgeModeASP"), checkIfUnderBridgeModeASP);
}

void firewall_rebuild_etc(void)
{
	//rebuild firewall scripts in etc
	
	// Port forwarding
	char *pfw_enable = nvram_get(RT2860_NVRAM, "PortForwardEnable");
	if (pfw_enable == NULL)
		pfw_enable = "0";
	
	doSystem("rm -f " _PATH_PFW_FILE);
	if (strcmp(pfw_enable, "1") == 0) // Turned on?
		iptablesPortForwardBuildScript();
	
	// IP/Port/MAC filtering
	char *ipf_enable = nvram_get(RT2860_NVRAM, "IPPortFilterEnable");
	if (ipf_enable == NULL)
		ipf_enable = "0";
	
	doSystem("rm -f " _PATH_MACIP_FILE);
	if (strcmp(ipf_enable, "1") == 0) // Turned on?
		iptablesIPPortFilterBuildScript();

	// Web filtering
	doSystem("rm -f " _PATH_WEBS_FILE);
	iptablesWebsFilterRun();
	
	// Sync unwritten buffers to disk
	sync();
}

void firewall_rebuild(void)
{
	//rebuild firewall scripts in etc
	firewall_rebuild_etc();
	//no backgroudn it!!!!
	doSystem("service iptables restart");
	///-----Load L7 filters rules----////
	LoadLayer7FilterName();
}
