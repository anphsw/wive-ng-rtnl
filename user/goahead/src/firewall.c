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
#include "nvram.h"
#include "utils.h"
#include "webs.h"
#include "firewall.h"
#include "internet.h"

#define _PATH_PFW           "/etc"
#define _PATH_PFW_FILE      _PATH_PFW "/portforward"
#define _PATH_MACIP_FILE    _PATH_PFW "/macipfilter"

#define DD printf("---> %d\n", __LINE__);

static void websSysFirewall(webs_t wp, char_t *path, char_t *query);


char l7name[8192];						// export it for internet.c qos
										// (The actual string is about 7200 bytes.)

int getGoAHeadServerPort(void);

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
	const char *dmze = nvram_bufget(RT2860_NVRAM, "DMZEnable");
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
	const char *pfe = nvram_bufget(RT2860_NVRAM, "PortForwardEnable");

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
	const char *pfe = nvram_bufget(RT2860_NVRAM, "IPPortFilterEnable");

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
char *sip_1, char *sip_2, int sprf_int, int sprt_int, 
char *dip_1, char *dip_2, int dprf_int, int dprt_int, int proto, int action)
{
	int rc = 0;
	char *pos = buf;
	const char *spifw = nvram_bufget(RT2860_NVRAM, "SPIFWEnabled");

	switch(action)
	{
		case ACTION_DROP:
			if (atoi(spifw) == 0)
				rc = snprintf(pos, len-rc, "iptables -A %s ", IPPORT_FILTER_CHAIN);
			else
				rc = snprintf(pos, len-rc, "iptables -A %s -m state --state NEW,INVALID ", IPPORT_FILTER_CHAIN);
			break;
		case ACTION_ACCEPT:
			rc = snprintf(pos, len-rc, 
				"iptables -A %s ", IPPORT_FILTER_CHAIN);
			break;
	}
	pos = pos + rc;
	
	if (iface != NULL)
	{
		rc = snprintf(pos, len-rc, "-i %s ", iface);
		pos = pos + rc;
	}

	// write mac address
	if(mac_address && strlen(mac_address))
	{
		rc = snprintf(pos, len-rc, "-m mac --mac-source %s ", mac_address);
		pos = pos+rc;
	}

	// write source ip
	rc = snprintf(pos, len-rc, "-s %s ", sip_1);
	pos = pos+rc;
	
	// write dest ip
	rc = snprintf(pos, len-rc, "-d %s ", dip_1);
	pos = pos+rc;

	// write protocol type
	if (proto == PROTO_NONE)
	{
		rc = snprintf(pos, len-rc, " ");
		pos = pos + rc;
	}
	else if(proto == PROTO_ICMP)
	{
		rc = snprintf(pos, len-rc, "-p icmp ");
		pos = pos + rc;
	}
	else
	{
		if(proto == PROTO_TCP)
			rc = snprintf(pos, len-rc, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len-rc, "-p udp ");
		pos = pos + rc;

		// write source port
		if(sprf_int)
		{
			if(sprt_int)
				rc = snprintf(pos, len-rc, "--sport %d:%d ", sprf_int, sprt_int);
			else
				rc = snprintf(pos, len-rc, "--sport %d ", sprf_int);
			pos = pos+rc;
		}

		// write dest port
		if (dprf_int)
		{
			if(dprt_int)
				rc = snprintf(pos, len-rc, "--dport %d:%d ", dprf_int, dprt_int);
			else
				rc = snprintf(pos, len-rc, "--dport %d ", dprf_int);
			pos = pos+rc;
		}
	}

	switch(action)
	{
		case ACTION_DROP:			// 1 == ENABLE--DROP mode
			rc = snprintf(pos, len-rc, "-j DROP");
			break;
		case ACTION_ACCEPT:			// 2 == ENABLE--ACCEPT mode
			rc = snprintf(pos, len-rc, "-j ACCEPT");
			break;
	}
	
	pos = pos + rc;
	rc = snprintf(pos, len-rc, "\n");
}

static void makePortForwardRule(char *buf, int len, char *wan_name, char *ip_address, int proto, int prf_int, int prt_int)
{
	int rc = 0;
	char *pos = buf;

	rc = snprintf(pos, len-rc, "iptables -t nat -A %s -j DNAT -i %s ", PORT_FORWARD_CHAIN, wan_name);
	pos = pos + rc;

	// write protocol type
	if(proto == PROTO_TCP)
		rc = snprintf(pos, len-rc, "-p tcp ");
	else if (proto == PROTO_UDP)
		rc = snprintf(pos, len-rc, "-p udp ");
	else if (proto == PROTO_TCP_UDP)
		rc = snprintf(pos, len-rc, " ");
	pos = pos + rc;

	// write port
	if(prt_int != 0)
		rc = snprintf(pos, len-rc, "--dport %d:%d ", prf_int, prt_int);
	else
		rc = snprintf(pos, len-rc, "--dport %d ", prf_int);
	pos = pos + rc;

	// write remote ip
	rc = snprintf(pos, len-rc, "--to %s \n", ip_address);
}

static void iptablesIPPortFilterBuildScript(void)
{
	int i=0;
	char rec[256];
	char cmd[1024];
	int sprf_int, sprt_int, proto, action;
	int dprf_int, dprt_int;
	char sprf[8], sprt[8], protocol[8], iface[8];
	char dprf[8], dprt[8], wan_name[16];
	char mac_address[32];
	char sip_1[32], sip_2[32], action_str[4];
	char dip_1[32], dip_2[32];
	char *firewall_enable, *default_policy, *rule, *c_if;
	char *spifw = nvram_bufget(RT2860_NVRAM, "SPIFWEnabled");
	int mode;
	
	printf("iptablesIPPortFilterBuildScript\n");
	
	firewall_enable = nvram_bufget(RT2860_NVRAM, "IPPortFilterEnable");
	if (!firewall_enable)
	{
		printf("Warning: can't find \"IPPortFilterEnable\" in flash.\n");
		return;
	}
	mode = atoi(firewall_enable);
	if(!mode)
		return;

	rule = nvram_bufget(RT2860_NVRAM, "IPPortFilterRules");
	if(!rule){
		printf("Warning: can't find \"IPPortFilterRules\" in flash.\n");
		return;
	}

	default_policy = nvram_bufget(RT2860_NVRAM, "DefaultFirewallPolicy");
	// add the default policy to the end of FORWARD chain
	if(!default_policy)
		default_policy = "0";
	
	// get wan name
	strncpy(wan_name, getWanIfNamePPP(), sizeof(wan_name)-1);

	//Generate portforward script file
	FILE *fd = fopen(_PATH_MACIP_FILE, "w");
	if (fd != NULL)
	{
		printf("iptablesIPPortFilterBuildScript opened file\n");
		
		fputs("#!/bin/sh\n\n", fd);
		fputs("iptables -t filter -N macipport_filter\n", fd);
		fputs("iptables -t filter -A FORWARD -j macipport_filter\n", fd);
		
		while ( (getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1) )
		{
			// get interface
			if ((getNthValueSafe(0, rec, ',', iface, sizeof(iface)) == -1))
				continue;

			// get sip 1
			if ((getNthValueSafe(1, rec, ',', sip_1, sizeof(sip_1)) == -1))
				continue;

			if (!isIpNetmaskValid(sip_1))
				continue;

			// get source port range "from"
			if ((getNthValueSafe(3, rec, ',', sprf, sizeof(sprf)) == -1))
				continue;

			if ((sprf_int = atoi(sprf)) > 65535)
				continue;

			// get dest port range "to"
			if ((getNthValueSafe(4, rec, ',', sprt, sizeof(sprt)) == -1))
				continue;

			if( (sprt_int = atoi(sprt)) > 65535)
				continue;

			// Destination Part
			if ((getNthValueSafe(5, rec, ',', dip_1, sizeof(dip_1)) == -1))
				continue;

			if (!isIpNetmaskValid(dip_1))
				continue;

			// get source port range "from"
			if ((getNthValueSafe(7, rec, ',', dprf, sizeof(dprf)) == -1))
				continue;

			if( (dprf_int = atoi(dprf)) > 65535)
				continue;

			// get dest port range "to"
			if ((getNthValueSafe(8, rec, ',', dprt, sizeof(dprt)) == -1))
				continue;

			if ((dprt_int = atoi(dprt)) > 65535)
				continue;

			// get protocol
			if ((getNthValueSafe(9, rec, ',', protocol, sizeof(protocol)) == -1))
				continue;
			proto = atoi(protocol);

			// get action
			if ((getNthValueSafe(10, rec, ',', action_str, sizeof(action_str)) == -1))
				continue;

			action = atoi(action_str);

			// get mac address
			if ((getNthValueSafe(12, rec, ',', mac_address, sizeof(mac_address)) == -1))
				continue;

			if (strlen(mac_address))
			{
				if (!isMacValid(mac_address))
					continue;
			}
			
			if (strcmp(iface, "LAN")==0)
				c_if = "br0";
			else if (strcmp(iface, "VPN")==0)
				c_if = "ppp+";
			else
				c_if = wan_name;

			makeIPPortFilterRule(cmd, sizeof(cmd), c_if, mac_address, sip_1, sip_2, sprf_int, sprt_int, dip_1, dip_2, dprf_int, dprt_int, proto, action);
			fputs(cmd, fd);
		}

		//close file
		fclose(fd);
	}
}

static void iptablesPortForwardBuildScript(void)
{
	char rec[256];
	char cmd[1024];
	char wan_name[16];
	char ip_address[32], prf[8], prt[8], protocol[8], iface[8];
	char *firewall_enable, *rule, *c_if;
	int i=0,prf_int, prt_int, proto;

	firewall_enable = nvram_bufget(RT2860_NVRAM, "PortForwardEnable");
	if(!firewall_enable)
	{
		printf("Warning: can't find \"PortForwardEnable\" in flash\n");
		return;
	}

	if(atoi(firewall_enable))
	{
		rule = nvram_bufget(RT2860_NVRAM, "PortForwardRules");
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
		fputs("iptables -t nat -N port_forward\n", fd);
		fputs("iptables -t nat -A PREROUTING -j port_forward\n", fd);

		while( (getNthValueSafe(i++, rule, ';', rec, sizeof(rec)) != -1) )
		{
			// get interface
			if ((getNthValueSafe(0, rec, ',', iface, sizeof(iface)) == -1))
			{
				printf("iface = %s\n", iface);
				continue;
			}
			
			// get ip address
			if ((getNthValueSafe(1, rec, ',', ip_address, sizeof(ip_address)) == -1))
			{
				fprintf(fd, "ip_addr = %s\n", prf);
				continue;
			}
			if (!isIpValid(ip_address))
				continue;

			// get port range "from"
			if ((getNthValueSafe(2, rec, ',', prf, sizeof(prf)) == -1))
			{
				fprintf(fd, "prf = %s\n", prf);
				continue;
			}
			if( (prf_int = atoi(prf)) == 0 || prf_int > 65535)
				continue;

			// get port range "to"
			if ((getNthValueSafe(3, rec, ',', prt, sizeof(prt)) == -1))
			{
				printf("prt = %s\n", prt);
				continue;
			}
			if ((prt_int = atoi(prt)) > 65535)
				continue;

			// get protocol
			if ((getNthValueSafe(4, rec, ',', protocol, sizeof(protocol)) == -1))
				continue;
			
			// Patch interface
			if (strcmp(iface, "LAN")==0)
				c_if = "br0";
			else if (strcmp(iface, "VPN")==0)
				c_if = "ppp+";
			else
				c_if = wan_name;
			
			proto = atoi(protocol);
			switch(proto)
			{
				case PROTO_TCP:
				case PROTO_UDP:
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, proto, prf_int, prt_int);
					fputs(cmd, fd);
					break;
				
				case PROTO_TCP_UDP:
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, PROTO_TCP, prf_int, prt_int);
					fputs(cmd, fd);
					makePortForwardRule(cmd, sizeof(cmd), c_if, ip_address, PROTO_UDP, prf_int, prt_int);
					fputs(cmd, fd);
					break;
				default:
					continue;
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

static int getDefaultFirewallPolicyASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int value;
	char *p = nvram_bufget(RT2860_NVRAM, "DefaultFirewallPolicy");
	int default_policy;
	if(!p)
		default_policy = 0;
	else
		default_policy = atoi(p);

	if( ejArgs(argc, argv, T("%d"), &value) != 1){
		return -1;
	}

	if(default_policy == value )
		websWrite(wp, T(" selected "));
	return 0;
}

static int checkIfUnderBridgeModeASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char *mode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	if(!mode)
		return -1;	// fatal error, make ASP engine complained.
	if(atoi(mode) == 0)	// bridge mode
		websWrite(wp, T(HTML_NO_FIREWALL_UNDER_BRIDGE_MODE));
	return 0;
}

/*
 * ASP function
 */
static int getPortForwardRuleNumsASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char *rules = nvram_bufget(RT2860_NVRAM, "PortForwardRules");
	
	if (!rules || !strlen(rules))
	{
		websWrite(wp, T("0"));
		return 0;
	}

	websWrite(wp, T("%d"), getRuleNums(rules));
	return 0;
}

/*
 * ASP function
 */
static int getIPPortRuleNumsASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char *rules = nvram_bufget(RT2860_NVRAM, "IPPortFilterRules");
	
	if(!rules || !strlen(rules) )
	{
		websWrite(wp, T("0"));
		return 0;
	}

	websWrite(wp, T("%d"), getRuleNums(rules));
	return 0;
}

/*
 * ASP function
 */
static int showPortForwardRulesASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int i=0;
	int prf_int, prt_int, proto;
	char ip_address[32], prf[8], prt[8], comment[16], protocol[8], interface[8];
	char rec[128];

	char *rules = nvram_bufget(RT2860_NVRAM, "PortForwardRules");
	if(!rules)
		return 0;
	if(!strlen(rules))
		return 0;

	/* format is :
	 * [ip],[port_from],[port_to],[protocol],[comment],;
	 */
	while (getNthValueSafe(i++, rules, ';', rec, sizeof(rec)) != -1 )
	{
		// get interface
		if ((getNthValueSafe(0, rec, ',', interface, sizeof(interface)) == -1))
			continue;
		
		// get ip address
		if ((getNthValueSafe(1, rec, ',', ip_address, sizeof(ip_address)) == -1))
			continue;

		if(!isIpValid(ip_address))
			continue;

		// get port range "from"
		if((getNthValueSafe(2, rec, ',', prf, sizeof(prf)) == -1))
			continue;

		if( (prf_int = atoi(prf)) == 0 || prf_int > 65535)
			continue;

		// get port range "to"
		if((getNthValueSafe(3, rec, ',', prt, sizeof(prt)) == -1))
			continue;

		if( (prt_int = atoi(prt)) > 65535)
			continue;

		// get protocol
		if ((getNthValueSafe(4, rec, ',', protocol, sizeof(protocol)) == -1))
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

		if ((getNthValueSafe(5, rec, ',', comment, sizeof(comment)) == -1))
			continue;

		websWrite(wp, T("<tr>\n"));
		// output No.
		websWrite(wp, T("<td>%d&nbsp;<input type=\"checkbox\" name=\"delRule%d\"></td>"), i, i-1 );

		// output IP address
		websWrite(wp, T("<td align=\"center\">%s</td>"), ip_address);
		
		// Interface
		websWrite(wp, T("<td align=\"center\">%s</td>"), interface);

		// output Port Range
		if(prt_int)
			websWrite(wp, T("<td align=\"center\">%d&nbsp;-&nbsp;%d</td>"), prf_int, prt_int);
		else
			websWrite(wp, T("<td align=\"center\">%d</td>"), prf_int);

		// output Protocol
		switch (proto)
		{
			case PROTO_TCP:
				websWrite(wp, T("<td align=\"center\">TCP</td>"));
				break;
			case PROTO_UDP:
				websWrite(wp, T("<td align=\"center\">UDP</td>"));
				break;
			case PROTO_TCP_UDP:
				websWrite(wp, T("<td align=\"center\">TCP + UDP</td>"));
				break;
		}

		// output Comment
		if(strlen(comment))
			websWrite(wp, T("<td align=\"center\">%s</td>"), comment);
		else
			websWrite(wp, T("<td align=\"center\">&nbsp;</td>"));
		websWrite(wp, T("</tr>\n"));
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
	default_policy = nvram_bufget(RT2860_NVRAM, "DefaultFirewallPolicy");
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
static int showIPPortFilterRulesASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int i;
	int sprf_int, sprt_int, proto;
	char mac_address[32];
	char sip_1[32], sip_2[32], sprf[8], sprt[8], comment[16], protocol[8], action[4];
	char dip_1[32], dip_2[32], dprf[8], dprt[8], iface[8];
	int dprf_int, dprt_int;
	char rec[256];
	char *default_policy;
	char *rules = nvram_bufget(RT2860_NVRAM, "IPPortFilterRules");
	if(!rules)
		return 0;

	default_policy = nvram_bufget(RT2860_NVRAM, "DefaultFirewallPolicy");
	// add the default policy to the end of FORWARD chain
	if(!default_policy)
		return 0;
	if(!strlen(default_policy))
		return 0;

	i=0;
	while(getNthValueSafe(i, rules, ';', rec, sizeof(rec)) != -1 && strlen(rec))
	{
		printf("i=%d, rec=%s, strlen(rec)=%d\n", i, rec, strlen(rec));
		
		// Get interface
		if ((getNthValueSafe(0, rec, ',', iface, sizeof(iface)) == -1))
		{
			i++;
			continue;
		}
		
		// get ip 1
		if ((getNthValueSafe(1, rec, ',', sip_1, sizeof(sip_1)) == -1))
		{
			i++;
			continue;
		}
		if (!isIpNetmaskValid(sip_1))
		{
			i++;
			continue;
		}
		// translate "any/0" to "any" for readable reason
		if (!strcmp(sip_1, "any/0"))
			strcpy(sip_1, "-");

		// get ip 2
		// get ip address
		if ((getNthValueSafe(2, rec, ',', sip_2, sizeof(sip_2)) == -1))
		{
			i++;
			continue;
		}
		// dont verify cause we dont have ip range support
		//if(!isIpValid(sip_2))
		//	continue;

		// get port range "from"
		if ((getNthValueSafe(3, rec, ',', sprf, sizeof(sprf)) == -1))
		{
			i++;
			continue;
		}
		if ((sprf_int = atoi(sprf)) > 65535)
		{
			i++;
			continue;
		}

		// get port range "to"
		if ((getNthValueSafe(4, rec, ',', sprt, sizeof(sprt)) == -1))
		{
			i++;
			continue;
		}
		if ((sprt_int = atoi(sprt)) > 65535)
		{
			i++;
			continue;
		}

		// get ip 1
		if ((getNthValueSafe(5, rec, ',', dip_1, sizeof(dip_1)) == -1))
		{
			i++;
			continue;
		}
		if (!isIpNetmaskValid(dip_1))
		{
			i++;
			continue;
		}
		// translate "any/0" to "any" for readable reason
		if (!strcmp(dip_1, "any/0"))
			strcpy(dip_1, "-");
		
		// get ip 2
		if ((getNthValueSafe(6, rec, ',', dip_2, sizeof(dip_2)) == -1))
		{
			i++;
			continue;
		}
		// dont verify cause we dont have ip range support
		//if(!isIpValid(dip_2))
		//	continue;

		// get protocol
		if ((getNthValueSafe(9, rec, ',', protocol, sizeof(protocol)) == -1))
		{
			i++;
			continue;
		}
		
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

		// get port range "from"
		if ((getNthValueSafe(7, rec, ',', dprf, sizeof(dprf)) == -1))
		{
			i++;
			continue;
		}
		if ((dprf_int = atoi(dprf)) > 65535)
		{
			i++;
			continue;
		}

		// get port range "to"
		if ((getNthValueSafe(8, rec, ',', dprt, sizeof(dprt)) == -1))
		{
			i++;
			continue;
		}
		if ( (dprt_int = atoi(dprt)) > 65535)
		{
			i++;
			continue;
		}

		// get action
		if ((getNthValueSafe(10, rec, ',', action, sizeof(action)) == -1)){
			i++;
			continue;
		}

		// get comment
		if ((getNthValueSafe(11, rec, ',', comment, sizeof(comment)) == -1))
		{
			i++;
			continue;
		}

		// get mac address
		if ((getNthValueSafe(12, rec, ',', mac_address, sizeof(mac_address)) == -1))
		{
			i++;
			continue;
		}
		if (!strlen(mac_address))
			gstrcpy(mac_address, T("-"));

		websWrite(wp, T("<tr>\n"));
		// output No.
		websWrite(wp, T("<td> %d&nbsp; <input type=\"checkbox\" name=\"delRule%d\"> </td>"), i+1, i );

		// output Mac address
		websWrite(wp, T("<td align=\"center\"> %s </td>"), mac_address);
		
		// output Interface
		websWrite(wp, T("<td align=\"center\"> %s </td>"), iface);

		// output DIP
		websWrite(wp, T("<td align=\"center\"> %s </td>"), dip_1);
		// we dont support ip range 
		// websWrite(wp, T("<td align=center> %s-%s </td>"), ip_1, ip_2);

		// output SIP
		websWrite(wp, T("<td align=\"center\"> %s </td>"), sip_1);
		// we dont support ip range 
		// websWrite(wp, T("<td align=center> %s-%s </td>"), ip_1, ip_2);

		// output Protocol
		switch(proto)
		{
			case PROTO_TCP:
				websWrite(wp, T("<td align=\"center\"> TCP </td>"));
				break;
			case PROTO_UDP:
				websWrite(wp, T("<td align=\"center\"> UDP </td>"));
				break;
			case PROTO_ICMP:
				websWrite(wp, T("<td align=\"center\"> ICMP </td>"));
				break;
			case PROTO_NONE:
				websWrite(wp, T("<td align=\"center\"> - </td>"));
				break;
		}

		// output dest Port Range
		if(dprt_int)
			websWrite(wp, T("<td align=\"center\"> %d - %d </td>"), dprf_int, dprt_int);
		else
		{
			// we re-descript the port number here because 
			// "any" word is more meanful than "0"
			if (!dprf_int)
				websWrite(wp, T("<td align=\"center\"> - </td>"), dprf_int);
			else
				websWrite(wp, T("<td align=\"center\"> %d </td>"), dprf_int);
		}

		// output Source Port Range
		if (sprt_int)
			websWrite(wp, T("<td align=\"center\"> %d - %d </td>"), sprf_int, sprt_int);
		else
		{
			// we re-descript the port number here because 
			// "any" word is more meanful than "0"
			if (!sprf_int)
				websWrite(wp, T("<td align=\"center\"> - </td>"), sprf_int);
			else
				websWrite(wp, T("<td align=\"center\"> %d </td>"), sprf_int);
		}

		// output action
		switch(atoi(action))
		{
			case ACTION_DROP:
				websWrite(wp, T("<td align=\"center\" id=\"portFilterActionDrop%d\"> Drop </td>"), i);
				break;
			case ACTION_ACCEPT:
				websWrite(wp, T("<td align=\"center\" id=\"portFilterActionAccept%d\"> Accept </td>"), i);
				break;
		}

		// output Comment
		if(strlen(comment))
			websWrite(wp, T("<td align=\"center\"> %s</td>"), comment);
		else
			websWrite(wp, T("<td align=\"center\"> &nbsp; </td>"));

		// output the id of "packet count"
		websWrite(wp, T("<td align=\"center\" id=\"pktCnt\"%d>-</td>"), i);

		websWrite(wp, T("</tr>\n"));

		i++;
	}

	switch(atoi(default_policy))
	{
		case 0:
			websWrite(wp, T("<tr><td align=\"center\" colspan=\"10\" id=\"portCurrentFilterDefaultAccept\"> Others would be accepted.</td><td align=\"center\" id=\"pktCnt%d\">-</td></tr>"), i);
			break;
		case 1:
			websWrite(wp, T("<tr><td align=\"center\" colspan=\"10\" id=\"portCurrentFilterDefaultDrop\"> Others would be dropped.</td><td align=\"center\" id=\"pktCnt%d\">-</td></tr>"), i);
			break;
	}

	return 0;
}

static int showDMZIPAddressASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char *DMZIPAddress = nvram_bufget(RT2860_NVRAM, "DMZIPAddress");
	if(!DMZIPAddress)
		return 0;
	if(!strlen(DMZIPAddress))
		return 0;

	websWrite(wp, T("%s"), DMZIPAddress);
	return 0;
}

static void ipportFilterDelete(webs_t wp, char_t *path, char_t *query)
{
	int i, j, rule_count;
	char_t name_buf[16];
	char_t *value;
	int *deleArray;

	char *rules = nvram_bufget(RT2860_NVRAM, "IPPortFilterRules");
	if (!rules || !strlen(rules))
		return;

	rule_count = getRuleNums(rules);
	if(!rule_count)
		return;

	deleArray = (int *)malloc(rule_count * sizeof(int));

	for (i=0, j=0; i< rule_count; i++)
	{
		snprintf(name_buf, 16, "delRule%d", i);
		value = websGetVar(wp, name_buf, NULL);
		if (value)
			deleArray[j++] = i;
	}
	if(!j)
	{
		websHeader(wp);
		websWrite(wp, T("You didn't select any rules to delete.<br>\n"));
		websFooter(wp);
		websDone(wp, 200);
		return;
	}

	deleteNthValueMulti(deleArray, rule_count, rules, ';');
	free(deleArray);

	nvram_set(RT2860_NVRAM, "IPPortFilterRules", rules);
	nvram_commit(RT2860_NVRAM);

	websHeader(wp);
	websWrite(wp, T("s<br>\n") );
	websWrite(wp, T("fromPort: <br>\n"));
	websWrite(wp, T("toPort: <br>\n"));
	websWrite(wp, T("protocol: <br>\n"));
	websWrite(wp, T("comment: <br>\n"));
	websFooter(wp);
	websDone(wp, 200);

	//generate and save rules
	iptablesIPPortFilterBuildScript();
	// Call rwfs to store data
	doSystem("fs save &");
	// call iptables
	firewall_rebuild();

	return;
}

static void portForwardDelete(webs_t wp, char_t *path, char_t *query)
{
	int i, j, rule_count;
	char_t name_buf[16];
	char_t *value;
	int *deleArray;
	char *firewall_enable;

	char *rules = nvram_bufget(RT2860_NVRAM, "PortForwardRules");
	if(!rules || !strlen(rules) )
		return;

	rule_count = getRuleNums(rules);
	if(!rule_count)
		return;

	deleArray = (int *)malloc(rule_count * sizeof(int));

	for (i=0, j=0; i< rule_count; i++)
	{
		snprintf(name_buf, 16, "delRule%d", i);
		value = websGetVar(wp, T(name_buf), NULL);
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

	nvram_set(RT2860_NVRAM, "PortForwardRules", rules);
	nvram_commit(RT2860_NVRAM);

	websHeader(wp);
	websWrite(wp, T("s<br>\n") );
	websWrite(wp, T("fromPort: <br>\n"));
	websWrite(wp, T("toPort: <br>\n"));
	websWrite(wp, T("protocol: <br>\n"));
	websWrite(wp, T("comment: <br>\n"));
	websFooter(wp);
	websDone(wp, 200);

	// restart iptables if it is running
	firewall_enable = nvram_bufget(RT2860_NVRAM, "PortForwardEnable");

	if (firewall_enable)
	{
		if (atoi(firewall_enable))
		{
			//generate and save rules
			iptablesPortForwardBuildScript();
			// Call iptables
			firewall_rebuild();
		}
	}

	return;
}


static void ipportFilter(webs_t wp, char_t *path, char_t *query)
{
	char rule[8192];
	char *mac_address;
	char *sip_1, *sip_2, *sprf, *sprt, *protocol, *action_str, *comment;
	char *dip_1, *dip_2, *dprf, *dprt, *iface;
	char *IPPortFilterRules;
	
	int sprf_int, sprt_int, dprf_int, dprt_int, proto, action;

	mac_address = websGetVar(wp, T("mac_address"), T(""));

	sip_1 = websGetVar(wp, T("sip_address"), T("any"));
	sip_2 = websGetVar(wp, T("sip_address2"), T(""));
	sprf = websGetVar(wp, T("sFromPort"), T("0"));
	sprt = websGetVar(wp, T("sToPort"), T(""));

	dip_1 = websGetVar(wp, T("dip_address"), T("any"));
	dip_2 = websGetVar(wp, T("dip_address2"), T(""));
	dprf = websGetVar(wp, T("dFromPort"), T("0"));
	dprt = websGetVar(wp, T("dToPort"), T(""));

	protocol = websGetVar(wp, T("protocol"), T(""));
	action_str = websGetVar(wp, T("action"), T(""));
	comment = websGetVar(wp, T("comment"), T(""));
	iface = websGetVar(wp, T("fltIface"), T(""));

	if(!mac_address || !sip_1 || !dip_1 || !sprf || !dprf || !iface)
		return;

	if(!strlen(mac_address) && !strlen(sip_1) && !strlen(dip_1) && !strlen(sprf) && !strlen(dprf) && !strlen(iface))
		return;

	// we dont trust user input.....
	if(strlen(mac_address))
	{
		if (!isMacValid(mac_address))
			return;
	}

	if(strlen(sip_1)){
		if(!isIpNetmaskValid(sip_1))
			return;
	}else
		sip_1 = T("any/0");

	if(strlen(dip_1))
	{
		if(!isIpNetmaskValid(dip_1))
			return;
	}
	else
		dip_1 = T("any/0");

	sip_2 = dip_2 = "0";

	if (!strcmp(protocol, T("TCP")))
		proto = PROTO_TCP;
	else if( !strcmp(protocol, T("UDP")))
		proto = PROTO_UDP;
	else if( !strcmp(protocol, T("None")))
		proto = PROTO_NONE;
	else if( !strcmp(protocol, T("ICMP")))
		proto = PROTO_ICMP;
	else
		return;

	if (!strlen(sprf) || proto == PROTO_NONE || proto == PROTO_ICMP)
		sprf_int = 0;
	else
	{
		sprf_int = atoi(sprf);
		if (sprf_int == 0 || sprf_int > 65535)
			return;
	}

	if (!strlen(sprt) || proto == PROTO_NONE || proto == PROTO_ICMP)
		sprt_int = 0;
	else
	{
		sprt_int = atoi(sprt);
		if(sprt_int ==0 || sprt_int > 65535)
			return;
	}

	if(!strlen(dprf) || proto == PROTO_NONE || proto == PROTO_ICMP)
		dprf_int = 0;
	else
	{
		dprf_int = atoi(dprf);
		if (dprf_int ==0 || dprf_int > 65535)
			return;
	}

	if (!strlen(dprt) || proto == PROTO_NONE || proto == PROTO_ICMP)
		dprt_int = 0;
	else
	{
		dprt_int = atoi(dprt);
		if(dprt_int ==0 || dprt_int > 65535)
			return;
	}

	if (!(strcmp(action_str, T("Accept"))))
		action = ACTION_ACCEPT;
	else if (!(strcmp(action_str, T("Drop"))))
		action = ACTION_DROP;
	else
		return;

	if (strlen(comment) > 32)
		return;
	// i know you will try to break our box... ;) 
	if (strchr(comment, ';') || strchr(comment, ','))
		return;

	if (( IPPortFilterRules = nvram_bufget(RT2860_NVRAM, "IPPortFilterRules")) && strlen(IPPortFilterRules))
		snprintf(rule, sizeof(rule), "%s;%s,%s,%s,%d,%d,%s,%s,%d,%d,%d,%d,%s,%s", IPPortFilterRules, iface, sip_1, sip_2, sprf_int, sprt_int, dip_1, dip_2, dprf_int, dprt_int, proto, action, comment, mac_address);
	else
		snprintf(rule, sizeof(rule), "%s,%s,%s,%d,%d,%s,%s,%d,%d,%d,%d,%s,%s", iface, sip_1, sip_2, sprf_int, sprt_int, dip_1, dip_2, dprf_int, dprt_int, proto, action, comment, mac_address);

	nvram_set(RT2860_NVRAM, "IPPortFilterRules", rule);
	nvram_commit(RT2860_NVRAM);

	websHeader(wp);
	websWrite(wp, T("mac: %s<br>\n"), mac_address);
	websWrite(wp, T("iface: %s<br>\n"), iface);
	websWrite(wp, T("sip1: %s<br>\n"), sip_1);
	websWrite(wp, T("sip2: %s<br>\n"), sip_2);
	websWrite(wp, T("sFromPort: %s<br>\n"), sprf);
	websWrite(wp, T("sToPort: %s<br>\n"), sprt);
	websWrite(wp, T("dip1: %s<br>\n"), dip_1);
	websWrite(wp, T("dip2: %s<br>\n"), dip_2);
	websWrite(wp, T("dFromPort: %s<br>\n"), dprf);
	websWrite(wp, T("dToPort: %s<br>\n"), dprt);
	websWrite(wp, T("protocol: %s<br>\n"), protocol);
	websWrite(wp, T("action: %s<br>\n"), action_str);
	websWrite(wp, T("comment: %s<br>\n"), comment);

	websFooter(wp);
	websDone(wp, 200);
	
	// generate and save rules
	iptablesIPPortFilterBuildScript();
	// Call rwfs to store data
	doSystem("fs save &");
	// call iptables
	firewall_rebuild();
}

static void portForward(webs_t wp, char_t *path, char_t *query)
{
	char rule[8192];
	char *ip_address, *pfe, *prf, *prt, *protocol, *comment, *iface;
	char *PortForwardRules;

	int prf_int, prt_int, proto;

	pfe = websGetVar(wp, T("portForwardEnabled"), T(""));
	ip_address = websGetVar(wp, T("ip_address"), T(""));
	prf = websGetVar(wp, T("fromPort"), T(""));
	prt = websGetVar(wp, T("toPort"), T(""));
	protocol = websGetVar(wp, T("protocol"), T(""));
	comment = websGetVar(wp, T("comment"), T(""));
	iface = websGetVar(wp, T("fwdIface"), T(""));

	if (!pfe && !strlen(pfe))
		return;

	if (!atoi(pfe))
	{
		nvram_set(RT2860_NVRAM, "PortForwardEnable", "0");
		//no chainge in rules
		goto end;
	}

	// user choose nothing but press "apply" only
	if (!strlen(ip_address) && !strlen(prf) && !strlen(prt) && !strlen(comment) && !strlen(iface))
	{
		nvram_set(RT2860_NVRAM, "PortForwardEnable", "1");
		//generate and save rules
		iptablesPortForwardBuildScript();
		// no change in rules
		goto end;
	}

	if (!ip_address && !strlen(ip_address))
		return;
	if (!isIpValid(ip_address))
		return;

	// we dont trust user input.....
	if (!prf && !strlen(prf))
		return;
	if (!(prf_int = atoi(prf)))
		return;
	if (prf_int > 65535)
		return;

	if(!prt)
		return;
	
	if (strlen(prt))
	{
		if( !(prt_int = atoi(prt)) )
			return;
		if(prt_int < prf_int)
			return;
		if(prt_int > 65535)
			return;
	}
	else
		prt_int = 0;

	if(! strcmp(protocol, "TCP"))
		proto = PROTO_TCP;
	else if( !strcmp(protocol, "UDP"))
		proto = PROTO_UDP;
	else if( !strcmp(protocol, "TCP&UDP"))
		proto = PROTO_TCP_UDP;
	else
		return;
	
	if ((strcmp(iface, "LAN")!=0) && (strcmp(iface, "WAN")!=0) && (strcmp(iface, "VPN")!=0))
		return;

	if(strlen(comment) > 32)
		return;
	/* i know you will try to break our box... ;) */
	if(strchr(comment, ';') || strchr(comment, ','))
		return;

	nvram_set(RT2860_NVRAM, "PortForwardEnable", "1");

	if ((PortForwardRules = nvram_bufget(RT2860_NVRAM, "PortForwardRules")) && strlen( PortForwardRules))
		snprintf(rule, sizeof(rule), "%s;%s,%s,%d,%d,%d,%s",  PortForwardRules, iface, ip_address, prf_int, prt_int, proto, comment);
	else
		snprintf(rule, sizeof(rule), "%s,%s,%d,%d,%d,%s", iface, ip_address, prf_int, prt_int, proto, comment);

	nvram_set(RT2860_NVRAM, "PortForwardRules", rule);
	nvram_commit(RT2860_NVRAM);

end:
	websHeader(wp);
	websWrite(wp, T("portForwardEnabled: %s<br>\n"), pfe);
	websWrite(wp, T("ip: %s<br>\n"), ip_address);
	websWrite(wp, T("fromPort: %s<br>\n"), prf);
	websWrite(wp, T("toPort: %s<br>\n"), prt);
	websWrite(wp, T("protocol: %s<br>\n"), protocol);
	websWrite(wp, T("comment: %s<br>\n"), comment);

	websFooter(wp);
	websDone(wp, 200);

	// generate and save rules
	iptablesPortForwardBuildScript();
	// Call rwfs to store data
	doSystem("fs save &");
	// call iptables
	firewall_rebuild();
}

static void BasicSettings(webs_t wp, char_t *path, char_t *query)
{
	char *default_policy, *firewall_enable;

	firewall_enable = websGetVar(wp, T("portFilterEnabled"), T(""));
	default_policy = websGetVar(wp, T("defaultFirewallPolicy"), T("0"));

	switch(atoi(firewall_enable)){
	case 0:
		nvram_set(RT2860_NVRAM, "IPPortFilterEnable", "0");
		break;
	case 1:
		nvram_set(RT2860_NVRAM, "IPPortFilterEnable", "1");
		break;
	}

	switch(atoi(default_policy)){
	case 1:
		nvram_set(RT2860_NVRAM, "DefaultFirewallPolicy", "1");
		break;
	case 0:
	default:
		nvram_set(RT2860_NVRAM, "DefaultFirewallPolicy", "0");
		break;
	}
	nvram_commit(RT2860_NVRAM);

	websHeader(wp);
	websWrite(wp, T("default_policy: %s<br>\n"), default_policy);
	websFooter(wp);
	websDone(wp, 200);

	if (firewall_enable && atoi(firewall_enable))
	{
		iptablesIPPortFilterBuildScript();
		// Call rwfs to store data
		doSystem("fs save &");
		// call iptables
		firewall_rebuild();
	}
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
		nvram_set(RT2860_NVRAM, "DMZEnable", "1");
		if(strlen(ip_address)){
			nvram_set(RT2860_NVRAM, "DMZIPAddress", ip_address);
		}
	}

	nvram_commit(RT2860_NVRAM);
	websHeader(wp);
	websWrite(wp, T("DMZEnabled: %s<br>\n"), dmzE);
	websWrite(wp, T("ip_address: %s<br>\n"), ip_address);
	websFooter(wp);
	websDone(wp, 200);
	firewall_rebuild();
}

static void websSysFirewall(webs_t wp, char_t *path, char_t *query)
{
	char *wpfE = websGetVar(wp, T("pingFrmWANFilterEnabled"), T(""));

	// someone use malform page.....
	if(!wpfE || !strlen(wpfE))
		return;

	nvram_commit(RT2860_NVRAM);
	websHeader(wp);
	websWrite(wp, T("WANPingFilter: %s<br>\n"), wpfE);
	websFooter(wp);
	websDone(wp, 200);

	iptablesIPPortFilterBuildScript();
	// Call rwfs to store data
	doSystem("fs save &");
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
	int i;
	int content_filter = 0;
	char entry[256];
	char *url_filter = nvram_bufget(RT2860_NVRAM, "websURLFilters");
	char *host_filter = nvram_bufget(RT2860_NVRAM, "websHostFilters");
	char *proxy = nvram_bufget(RT2860_NVRAM, "websFilterProxy");
	char *java = nvram_bufget(RT2860_NVRAM, "websFilterJava");
	char *activex = nvram_bufget(RT2860_NVRAM, "websFilterActivex");
	char *cookies = nvram_bufget(RT2860_NVRAM, "websFilterCookies");

	if(!url_filter || !host_filter || !proxy || !java || !activex || !cookies)
		return;

	// Content filter
	if(!strcmp(java, "1"))
		content_filter += BLK_JAVA;
	if(!strcmp(activex, "1"))
		content_filter += BLK_ACTIVE;
	if(!strcmp(cookies, "1"))
		content_filter += BLK_COOKIE;
	if(!strcmp(proxy, "1"))
		content_filter += BLK_PROXY;

	if (content_filter)
	{
		// Why only 3 ports are inspected?(This idea is from CyberTAN source code)
		// TODO: use layer7 to inspect HTTP
		doSystem("iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp --dport 80   -m webstr --content %d -j REJECT --reject-with tcp-reset", content_filter);
		doSystem("iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp --dport 3128 -m webstr --content %d -j REJECT --reject-with tcp-reset", content_filter);
		doSystem("iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp --dport 8080 -m webstr --content %d -j REJECT --reject-with tcp-reset", content_filter);
	}

	// URL filter
	i=0;
	while ((getNthValueSafe(i++, url_filter, ';', entry, sizeof(entry)) != -1) )
	{
		if (strlen(entry))
		{
			if(!strncasecmp(entry, "http://", strlen("http://")))
				strcpy(entry, entry + strlen("http://"));
			doSystem("iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp -m webstr --url  %s -j REJECT --reject-with tcp-reset", entry);
		}
	}

	// HOST(Keyword) filter
	i=0;
	while ( (getNthValueSafe(i++, host_filter, ';', entry, sizeof(entry)) != -1) )
	{
		if(strlen(entry))
			doSystem("iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp -m webstr --host %s -j REJECT --reject-with tcp-reset", entry);
	}

	return;
}


static void websURLFilterDelete(webs_t wp, char_t *path, char_t *query)
{
	int i, j, rule_count;
	char_t name_buf[16];
	char_t *value;
	int *deleArray;

	char *rules = nvram_bufget(RT2860_NVRAM, "websURLFilters");
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
	nvram_commit(RT2860_NVRAM);

	doSystem("iptables -t filter -F " WEB_FILTER_CHAIN);
	iptablesWebsFilterRun();

	websHeader(wp);
	websWrite(wp, T("Delete : success<br>\n") );
	websFooter(wp);
	websDone(wp, 200);

	return;
}

static void websHostFilterDelete(webs_t wp, char_t *path, char_t *query)
{
	int i, j, rule_count;
	char_t name_buf[16];
	char_t *value;
	int *deleArray;

	char *rules = nvram_bufget(RT2860_NVRAM, "websHostFilters");
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
	nvram_commit(RT2860_NVRAM);

	doSystem("iptables -t filter -F " WEB_FILTER_CHAIN);
	iptablesWebsFilterRun();

	websHeader(wp);
	websWrite(wp, T("Delete : success<br>\n") );
	websFooter(wp);
	websDone(wp, 200);

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

	nvram_bufset(RT2860_NVRAM, "websFilterProxy",   atoi(proxy)   ? "1" : "0" );
	nvram_bufset(RT2860_NVRAM, "websFilterJava",    atoi(java)    ? "1" : "0" );
	nvram_bufset(RT2860_NVRAM, "websFilterActivex", atoi(activex) ? "1" : "0" );
	nvram_bufset(RT2860_NVRAM, "websFilterCookies", atoi(cookies) ? "1" : "0" );
	nvram_commit(RT2860_NVRAM);

	doSystem("iptables -t filter -F " WEB_FILTER_CHAIN);
	iptablesWebsFilterRun();

	websHeader(wp);
	websWrite(wp, T("Proxy: %s<br>\n"),  atoi(proxy) ? "enable" : "disable");
	websWrite(wp, T("Java: %s<br>\n"),   atoi(java) ? "enable" : "disable");
	websWrite(wp, T("Activex: %s<br>\n"), atoi(activex) ? "enable" : "disable");
	websWrite(wp, T("Cookies: %s<br>\n"), atoi(cookies) ? "enable" : "disable");
	websFooter(wp);
	websDone(wp, 200);
}

static void websURLFilter(webs_t wp, char_t *path, char_t *query)
{
	char *urlfilters = nvram_bufget(RT2860_NVRAM, "websURLFilters");
	char *rule = websGetVar(wp, T("addURLFilter"), T(""));
	char *new_urlfilters;
	if(!rule)
		return;
	if(strchr(rule, ';'))
		return;

	if(!urlfilters || !strlen(urlfilters))
		nvram_bufset(RT2860_NVRAM, "websURLFilters", rule);
	else
	{
		if(! (new_urlfilters = (char *)malloc(sizeof(char) * (strlen(urlfilters)+strlen(rule)+1))))
			return;
		new_urlfilters[0] = '\0';
		strcat(new_urlfilters, urlfilters);
		strcat(new_urlfilters, ";");
		strcat(new_urlfilters, rule);
		nvram_bufset(RT2860_NVRAM, "websURLFilters", new_urlfilters);
		free(new_urlfilters);
	}
	nvram_commit(RT2860_NVRAM);

	doSystem("iptables -t filter -F " WEB_FILTER_CHAIN);
	iptablesWebsFilterRun();

	websHeader(wp);
	websWrite(wp, T("add URL filter: %s<br>\n"), rule);
	websFooter(wp);
	websDone(wp, 200);
}

static void websHostFilter(webs_t wp, char_t *path, char_t *query)
{
	char *hostfilters = nvram_bufget(RT2860_NVRAM, "websHostFilters");
	char *rule = websGetVar(wp, T("addHostFilter"), T(""));
	char *new_hostfilters;
	if(!rule)
		return;
	if(strchr(rule, ';'))
		return;

	if(!hostfilters || !strlen(hostfilters))
		nvram_bufset(RT2860_NVRAM, "websHostFilters", rule);
	else{
		if(! (new_hostfilters = (char *)malloc(sizeof(char) * (strlen(hostfilters)+strlen(rule)+1))))
			return;
		new_hostfilters[0] = '\0';
		strcat(new_hostfilters, hostfilters);
		strcat(new_hostfilters, ";");
		strcat(new_hostfilters, rule);
		nvram_bufset(RT2860_NVRAM, "websHostFilters", new_hostfilters);
		free(new_hostfilters);
	}
	nvram_commit(RT2860_NVRAM);

	doSystem("iptables -t filter -F " WEB_FILTER_CHAIN);
	iptablesWebsFilterRun();

	websHeader(wp);
	websWrite(wp, T("add Host filter: %s<br>\n"), rule);
	websFooter(wp);
	websDone(wp, 200);
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
	websAspDefine(T("getDefaultFirewallPolicyASP"), getDefaultFirewallPolicyASP);
	websFormDefine(T("BasicSettings"), BasicSettings);

	websAspDefine(T("getIPPortFilterEnableASP"), getIPPortFilterEnableASP);
	websAspDefine(T("showIPPortFilterRulesASP"), showIPPortFilterRulesASP);
	websAspDefine(T("getIPPortRuleNumsASP"), getIPPortRuleNumsASP);
	websFormDefine(T("ipportFilter"), ipportFilter);
	websFormDefine(T("ipportFilterDelete"), ipportFilterDelete);
	websFormDefine(T("getRulesPacketCount"), getRulesPacketCount);

	websFormDefine(T("DMZ"), DMZ);
	websAspDefine(T("getDMZEnableASP"), getDMZEnableASP);
	websAspDefine(T("showDMZIPAddressASP"), showDMZIPAddressASP);

	websAspDefine(T("getPortForwardEnableASP"), getPortForwardEnableASP);
	websAspDefine(T("showPortForwardRulesASP"), showPortForwardRulesASP);
	websAspDefine(T("getPortForwardRuleNumsASP"), getPortForwardRuleNumsASP);
	websFormDefine(T("portForward"), portForward);
	websFormDefine(T("portForwardDelete"), portForwardDelete);

	websFormDefine(T("websSysFirewall"), websSysFirewall);
	websFormDefine(T("webContentFilter"), webContentFilter);
	websFormDefine(T("websURLFilterDelete"), websURLFilterDelete);
	websFormDefine(T("websHostFilterDelete"), websHostFilterDelete);
	websFormDefine(T("websHostFilter"), websHostFilter);
	websFormDefine(T("websURLFilter"), websURLFilter);

	websAspDefine(T("getLayer7FiltersASP"), getLayer7FiltersASP);

	websAspDefine(T("checkIfUnderBridgeModeASP"), checkIfUnderBridgeModeASP);
}

void firewall_rebuild(void)
{
	//rebuild firewall scripts in etc
	iptablesPortForwardBuildScript();
	iptablesIPPortFilterBuildScript();
	//no backgroudn it!!!!         
	doSystem("service iptables restart");
	///-----Load L7 filters rules----////
	LoadLayer7FilterName();
	iptablesWebsFilterRun();
}
