/* vi: set sw=4 ts=4 sts=4 fdm=marker: */
/*
 *	internet.c -- Internet Settings
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: internet.c,v 1.167.2.8 2009-04-22 01:31:35 chhung Exp $
 */

#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<net/if.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<net/route.h>
#include	<string.h>
#include	<dirent.h>

#include	"utils.h"
#include	"internet.h"
#include	"procps.h"
#include 	"firewall.h"
#include	"management.h"
#include	"station.h"
#include	"wireless.h"
#include	"helpers.h"

#ifdef CONFIG_USER_802_1X
#include	"wps.h"
#endif

#define _PATH_PROCNET_DEV      "/proc/net/dev"
#define _PATH_VPN_RT           "/etc"
#define _PATH_VPN_RT_FILE      _PATH_VPN_RT "/routes_ppp"
#define _PATH_VPN_RT_SCRIPT    _PATH_VPN_RT "/routes_ppp_replace"

/*** VPN statuses ***/
typedef struct vpn_status_t
{
	const char_t *status;
	long          color;
} vpn_status_t;

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

#ifdef CONFIG_NET_SCHED
#include      "qos.h"
#endif

static int vpnShowVPNStatus(int eid, webs_t wp, int argc, char_t **argv);
static int vpnIfaceList(int eid, webs_t wp, int argc, char_t **argv);
static void formVPNSetup(webs_t wp, char_t *path, char_t *query);

static int getMeshBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getWDSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getWSCBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getSTABuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getMBSSIDBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getUSBBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getStorageBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getFtpBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getSmbBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getMediaBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getWebCamBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getPrinterSrvBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getIgmpProxyBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getVPNBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDnsmasqBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getGWBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getLltdBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getPppoeRelayBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getUpnpBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getRadvdBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDynamicRoutingBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getSWQoSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDATEBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getDDNSBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getSpotBuilt(int eid, webs_t wp, int argc, char_t **argv);

// BEGIN KABINET PROVIDER
static int getLANAUTHBuilt(int eid, webs_t wp, int argc, char_t **argv);
// END KABINET PROVIDER

static int getSysLogBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int getETHTOOLBuilt(int eid, webs_t wp, int argc, char_t **argv);

static int getDhcpCliList(int eid, webs_t wp, int argc, char_t **argv);
static int getDns(int eid, webs_t wp, int argc, char_t **argv);
static int getHostSupp(int eid, webs_t wp, int argc, char_t **argv);
static int getIfLiveWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getIfIsUpWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getLanIp(int eid, webs_t wp, int argc, char_t **argv);
static int getLanMac(int eid, webs_t wp, int argc, char_t **argv);
static int getLanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getLanNetmask(int eid, webs_t wp, int argc, char_t **argv);
static int getWanIp(int eid, webs_t wp, int argc, char_t **argv);
static int getWanMac(int eid, webs_t wp, int argc, char_t **argv);
static int getWanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv);
static int getWanNetmask(int eid, webs_t wp, int argc, char_t **argv);
static int getWanGateway(int eid, webs_t wp, int argc, char_t **argv);
static int getRoutingTable(int eid, webs_t wp, int argc, char_t **argv);
static void setLan(webs_t wp, char_t *path, char_t *query);
static void setWan(webs_t wp, char_t *path, char_t *query);
static void getMyMAC(webs_t wp, char_t *path, char_t *query);
static void addRouting(webs_t wp, char_t *path, char_t *query);
static void delRouting(webs_t wp, char_t *path, char_t *query);
static void dynamicRouting(webs_t wp, char_t *path, char_t *query);
inline void zebraRestart(void);
void ripdRestart(void);

#ifdef CONFIG_USER_CHILLISPOT
static int getSpotIp(int eid, webs_t wp, int argc, char_t **argv);
static int getSpotNetmask(int eid, webs_t wp, int argc, char_t **argv);
static void setHotspot(webs_t wp, char_t *path, char_t *query);
#endif

void formDefineInternet(void) {
	websAspDefine(T("getDhcpCliList"), getDhcpCliList);
	websAspDefine(T("getDns"), getDns);
	websAspDefine(T("getHostSupp"), getHostSupp);
	websAspDefine(T("getIfLiveWeb"), getIfLiveWeb);
	websAspDefine(T("getIfIsUpWeb"), getIfIsUpWeb);
	websAspDefine(T("getIgmpProxyBuilt"), getIgmpProxyBuilt);
	websAspDefine(T("getVPNBuilt"), getVPNBuilt);
	websAspDefine(T("getLanIp"), getLanIp);
	websAspDefine(T("getLanMac"), getLanMac);
	websAspDefine(T("getLanIfNameWeb"), getLanIfNameWeb);
	websAspDefine(T("getLanNetmask"), getLanNetmask);
	websAspDefine(T("getDnsmasqBuilt"), getDnsmasqBuilt);
	websAspDefine(T("getGWBuilt"), getGWBuilt);
	websAspDefine(T("getLltdBuilt"), getLltdBuilt);
	websAspDefine(T("getPppoeRelayBuilt"), getPppoeRelayBuilt);
	websAspDefine(T("getUpnpBuilt"), getUpnpBuilt);
	websAspDefine(T("getRadvdBuilt"), getRadvdBuilt);
	websAspDefine(T("getWanIp"), getWanIp);
	websAspDefine(T("getWanMac"), getWanMac);
	websAspDefine(T("getWanIfNameWeb"), getWanIfNameWeb);
	websAspDefine(T("getWanNetmask"), getWanNetmask);
	websAspDefine(T("getWanGateway"), getWanGateway);
	websAspDefine(T("getRoutingTable"), getRoutingTable);
	websAspDefine(T("getMeshBuilt"), getMeshBuilt);
	websAspDefine(T("getWDSBuilt"), getWDSBuilt);
	websAspDefine(T("getWSCBuilt"), getWSCBuilt);
	websAspDefine(T("getSTABuilt"), getSTABuilt);
	websAspDefine(T("getMBSSIDBuilt"), getMBSSIDBuilt);
	websAspDefine(T("getUSBBuilt"), getUSBBuilt);
	websAspDefine(T("getStorageBuilt"), getStorageBuilt);
	websAspDefine(T("getFtpBuilt"), getFtpBuilt);
	websAspDefine(T("getSmbBuilt"), getSmbBuilt);
	websAspDefine(T("getMediaBuilt"), getMediaBuilt);
	websAspDefine(T("getWebCamBuilt"), getWebCamBuilt);
	websAspDefine(T("getPrinterSrvBuilt"), getPrinterSrvBuilt);
	websFormDefine(T("setLan"), setLan);
	websFormDefine(T("setWan"), setWan);
	websFormDefine(T("getMyMAC"), getMyMAC);
	websFormDefine(T("addRouting"), addRouting);
	websFormDefine(T("delRouting"), delRouting);
	websFormDefine(T("dynamicRouting"), dynamicRouting);
	websAspDefine(T("getDynamicRoutingBuilt"), getDynamicRoutingBuilt);
	websAspDefine(T("getSWQoSBuilt"), getSWQoSBuilt);
	websAspDefine(T("getDATEBuilt"), getDATEBuilt);
	websAspDefine(T("getDDNSBuilt"), getDDNSBuilt);
	websAspDefine(T("getSpotBuilt"), getSpotBuilt);

	websAspDefine(T("getLANAUTHBuilt"), getLANAUTHBuilt);

	websAspDefine(T("getSysLogBuilt"), getSysLogBuilt);
	websAspDefine(T("getETHTOOLBuilt"), getETHTOOLBuilt);

	websAspDefine(T("vpnShowVPNStatus"), vpnShowVPNStatus);
	websAspDefine(T("vpnIfaceList"), vpnIfaceList);
	websFormDefine(T("formVPNSetup"), formVPNSetup);

#ifdef CONFIG_USER_CHILLISPOT
	websAspDefine(T("getSpotIp"), getSpotIp);
	websAspDefine(T("getSpotNetmask"), getSpotNetmask);
	websFormDefine(T("setHotspot"), setHotspot);
#endif
}

/*
 * arguments: ifname  - interface name
 * description: test the existence of interface through /proc/net/dev
 * return: -1 = fopen error, 1 = not found, 0 = found
 */
int getIfLive(char *ifname)
{
	FILE *fp;
	char buf[256], *p;
	int i;

	if (NULL == (fp = fopen("/proc/net/dev", "r"))) {
		error(E_L, E_LOG, T("getIfLive: open /proc/net/dev error"));
		return -1;
	}

	fgets(buf, 256, fp);
	fgets(buf, 256, fp);
	while (NULL != fgets(buf, 256, fp)) {
		i = 0;
		while (isspace(buf[i++]))
			;
		p = buf + i - 1;
		while (':' != buf[i++])
			;
		buf[i-1] = '\0';
		if (!strcmp(p, ifname)) {
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	error(E_L, E_LOG, T("getIfLive: device %s not found"), ifname);
	return 1;
}

/*
 * arguments: ifname  - interface name
 *            if_addr - a 18-byte buffer to store mac address
 * description: fetch mac address according to given interface name
 */
int getIfMac(char *ifname, char *if_hw)
{
	struct ifreq ifr;
	char *ptr;
	int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error(E_L, E_LOG, T("getIfMac: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		//error(E_L, E_LOG, T("getIfMac: ioctl SIOCGIFHWADDR error for %s"), ifname);
		return -1;
	}

	ptr = (char *)&ifr.ifr_addr.sa_data;
	sprintf(if_hw, "%02X:%02X:%02X:%02X:%02X:%02X",
			(ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
			(ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

	close(skfd);
	return 0;
}

/*
 * arguments: ifname  - interface name
 *            if_addr - a 16-byte buffer to store ip address
 * description: fetch ip address, netmask associated to given interface name
 */
int getIfIp(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error(E_L, E_LOG, T("getIfIp: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}

/*
 * arguments: ifname - interface name
 * description: return 1 if interface is up
 *              return 0 if interface is down
 */
int getIfIsUp(char *ifname)
{
	struct ifreq ifr;
	int skfd;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd == -1) {
		perror("socket");
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
		perror("ioctl");
		close(skfd);
		return -1;
	}
	close(skfd);
	if (ifr.ifr_flags & IFF_UP)
		return 1;
	else
		return 0;
}

/*
 * arguments: ifname - interface name
 *            if_net - a 16-byte buffer to store subnet mask
 * description: fetch subnet mask associated to given interface name
 *              0 = bridge, 1 = gateway, 2 = wirelss isp
 */
int getIfNetmask(char *ifname, char *if_net)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		error(E_L, E_LOG, T("getIfNetmask: open socket error"));
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		return -1;
	}
	strcpy(if_net, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	close(skfd);
	return 0;
}

/*
 * description: return WAN interface name
 *              0 = bridge, 1 = gateway, 2 = wirelss isp
 */
char* getWanIfName(void)
{
	char *mode = nvram_get(RT2860_NVRAM, "OperationMode");
	static char *if_name = "br0";

	if (NULL == mode)
		return if_name;
	if (!strncmp(mode, "0", 2))
		if_name = "br0";
	else if (!strncmp(mode, "1", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
		if_name = "eth2.2";
#else /* MARVELL & CONFIG_ICPLUS_PHY */
		if_name = "eth2";
#endif
	}
	else if (!strncmp(mode, "2", 2))
		if_name = "ra0";
	else if (!strncmp(mode, "3", 2))
		if_name = "apcli0";
	return if_name;
}

char* getWanIfNamePPP(void)
{
    char *cm = nvram_get(RT2860_NVRAM, "wanConnectionMode");

    if (!strncmp(cm, "PPPOE", 6) || !strncmp(cm, "L2TP", 5) || !strncmp(cm, "PPTP", 5) 
#ifdef CONFIG_USER_3G
		|| !strncmp(cm, "3G", 3)
#endif
	){
        return "ppp0";
	}

    return getWanIfName();
}


/*
 * description: return LAN interface name
 */
char* getLanIfName(void)
{
	char *mode = nvram_get(RT2860_NVRAM, "OperationMode");
	static char *if_name = "br0";

	if (NULL == mode)
		return if_name;
	if (!strncmp(mode, "0", 2))
		if_name = "br0";
	else if (!strncmp(mode, "1", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
		if_name = "br0";
#elif defined  CONFIG_ICPLUS_PHY && CONFIG_RT2860V2_AP_MBSS
		char *num_s = nvram_get(RT2860_NVRAM, "BssidNum");
		if(atoi(num_s) > 1)	// multiple ssid
			if_name = "br0";
		else
			if_name = "ra0";
#else
		if_name = "ra0";
#endif
	}
	else if (!strncmp(mode, "2", 2)) {
		if_name = "eth2";
	}
	else if (!strncmp(mode, "3", 2)) {
		if_name = "br0";
	}
	return if_name;
}

/*
 * description: get the value "WAN" or "LAN" the interface is belong to.
 */
char *getLanWanNamebyIf(char *ifname)
{
	char *mode = nvram_get(RT2860_NVRAM, "OperationMode");

	if (NULL == mode)
		return "Unknown";

	if (!strcmp(mode, "0")){	// bridge mode
		if(!strcmp(ifname, "br0"))
			return "LAN";
		return ifname;
	}

	if (!strcmp(mode, "1")) {	// gateway mode
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
		if(!strcmp(ifname, "br0"))
			return "LAN";
		if(!strcmp(ifname, "eth2.2") || !strcmp(ifname, "ppp0"))
			return "WAN";
		return ifname;
#elif defined  CONFIG_ICPLUS_PHY && CONFIG_RT2860V2_AP_MBSS
		char *num_s = nvram_get(RT2860_NVRAM, "BssidNum");
		if(atoi(num_s) > 1 && !strcmp(ifname, "br0") )	// multiple ssid
			return "LAN";
		if(atoi(num_s) == 1 && !strcmp(ifname, "ra0"))
			return "LAN";
		if (!strcmp(ifname, "eth2") || !strcmp(ifname, "ppp0"))
			return "WAN";
		return ifname;
#else
		if(!strcmp(ifname, "ra0"))
			return "LAN";
		return ifname;
#endif
	}else if (!strncmp(mode, "2", 2)) {	// ethernet convertor
		if(!strcmp("eth2", ifname))
			return "LAN";
		if(!strcmp("ra0", ifname))
			return "WAN";
		return ifname;
	}else if (!strncmp(mode, "3", 2)) {	// apcli mode
		if(!strcmp("br0", ifname))
			return "LAN";
		if(!strcmp("apcli0", ifname))
			return "WAN";
		return ifname;
	}
	return ifname;
}

const parameter_fetch_t vpn_args[] =
{
	{ T("vpn_server"),             "vpnServer",            0 },
	{ T("vpn_range"),              "vpnRange",             0 },
	{ T("vpn_user"),               "vpnUser",              0 },
	{ T("vpn_pass"),               "vpnPassword",          0 },
	{ T("vpn_mtu"),                "vpnMTU",               0 },
	{ T("vpn_type"),               "vpnType",              0 },
	{ T("vpn_mppe"),               "vpnMPPE",              1 },
	{ T("vpn_dgw"),                "vpnDGW",               0 },
	{ T("vpn_peerdns"),            "vpnPeerDNS",           1 },
	{ T("vpn_debug"),              "vpnDebug",             1 },
	{ T("vpn_nat"),                "vpnNAT",               1 },
	{ T("vpn_lcp"),                "vpnEnableLCP",         1 },
	{ T("vpn_auth_type"),          "vpnAuthProtocol",      1 },
	{ T("vpn_pppoe_iface"),        "vpnInterface",         0 },
	{ T("vpn_pure_pppoe"),         "vpnPurePPPOE",         2 },
	{ NULL, 0, 0 } // Terminator
};

const parameter_fetch_t lanauth_args[] =
{
	{ T("vpn_type"),               "vpnType",              0 },
	{ T("vpn_pass"),               "vpnPassword",          0 },
	{ T("lanauth_access"),         "LANAUTH_LVL",          0 },
	
	{ NULL, 0, 0 } // Terminator
};

/*
 * VPN statuses
 */
const vpn_status_t vpn_statuses[] =
{

	{ "disabled",     0x808080        },
	{ "offline",      0xff0000        },
	{ "connecting",   0xff8000        },
	{ "online",       0x00ff00        }
};

#if defined CONFIG_USER_KABINET
/*
 * LANAUTH status
 */
const vpn_status_t lanauth_statuses[] =
{

	{ "disabled",           0x808080        },
	{ "not started",        0x808080        },
	{ "offline",            0xff0000        },
	{ "kabinet networks",   0x33bb33        },
	{ "full access",        0x00ff00        }
};
#endif


#if defined CONFIG_USER_KABINET
/* returns actual lanauth state+1 or 0 if lanauth process not found
*/
static int get_LANAUTHState()
{
	FILE *fp;
	char	result[96],*r;
	int i,state;

	fp = popen("ps|grep lanaut[h]", "r");
	fgets(result, sizeof(result), fp);
	pclose(fp);
	for (i=0, state=0, r=result; *r && i<sizeof(result) ;i++,r++)
	{
		switch(state)
		{
			case 0:
				if (*r == '-')
					state++;
				break;
			case 1:
				if (*r == 'A')
					state++;
				else
					state = 0;
				break;
			case 2:
				if (*r == ' ' || *r == '\t')
					state = 3;
				else
					state = 0;
				break;
			case 3:
				if (*r == '0')
					return 1;
				else if (*r == '1')
					return 2;
				else if (*r == '2')
					return 3;
				else
					state = 0;
			default:
				break;
		}
	}
	return 0;
}
#endif

/*
 * Show PPTP VPN status
 */
static int vpnShowVPNStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	int status = 0; // Status is 'disabled'
	const vpn_status_t *st_table = vpn_statuses;

	// Get value
	char *vpn_enabled = nvram_get(RT2860_NVRAM, "vpnEnabled");
	char *vpn_type = nvram_get(RT2860_NVRAM, "vpnType");
	if ((vpn_enabled==NULL) || (vpn_enabled[0]=='\0'))
		vpn_enabled = "off";
	
	// Do not perform other checks if VPN is turned off
	if (strcmp(vpn_enabled, "on")==0)
	{
#ifdef CONFIG_USER_KABINET
		if (strcmp(vpn_type, "6") == 0)
		{
			status = (get_LANAUTHState() + 1) % 5;
			st_table = lanauth_statuses;
		}
		else
		{
#endif
			// Status is at least 'offline' now
			status++;
			
			// Try to find pppd or xl2tpd
			int found = procps_count("pppd");
			if (found==0)
				found = procps_count("xl2tpd");
		
			if (found>0)
			{
				// Now status is at least 'connecting'
				status++;
			
				// Try to search for 'pppXX' device
				FILE * fd = fopen(_PATH_PROCNET_DEV, "r");
			
				if (fd != NULL)
				{
					int ppp_id;
					char_t line[256];
				
					// Read all ifaces and check match
					while (fgets(line, 255, fd)!=NULL)
					{
						// Filter only 'pppXX'
						if (sscanf(line, " ppp%d", &ppp_id)==1)
						{
							// Check if ppp interface has number at least 8
							if ((ppp_id >= 0) && (ppp_id <= 8))
							{
								status++; // Status is set to 'connected'
								break; // Do not search more
							}
						}
					}
					
					fclose(fd);
				}
				else
				{
					fprintf(stderr, "Warning: cannot open %s (%s).\n",
						_PATH_PROCNET_DEV, strerror(errno));
				}
			}
			else if (found<0)
			{
				fprintf(stderr, "Warning: cannot serach process 'pppd' or 'xl2tpd': %s\n",
						strerror(-found));
			}
#ifdef CONFIG_USER_KABINET
		}
#endif
	}
	
	// Output connection status
	const vpn_status_t *st = &st_table[status];
	websWrite(
		wp,
		T("<b>Status: <font color=\"#%06x\">%s</font></b>\n"),
		st->color, st->status
	);
	
	return 0;
}

const char *vpn_ifaces[] =
{
	"LAN",
	"WAN",
	NULL // Terminator
};

static int vpnIfaceList(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t iface[32];
	const char **line;
	
	// Fetch VPN interface
	char *rrs  = nvram_get(RT2860_NVRAM, "vpnInterface");
	if (rrs!=NULL)
		strcpy(iface, rrs);
	else
		iface[0] = '\0';
	if (strlen(iface)<=0)
		strcpy(iface, "LAN");
		
	// Read all ifaces and check match
	for (line = vpn_ifaces; *line != NULL; line++)
	{
		// Write iface to output if it was found
		websWrite(wp, T("<option value=\"%s\"%s>%s</option>\n"),
			*line,
			(strcmp(*line, iface)==0) ? " selected=\"selected\"" : "",
			*line
		);
	}
	
	return 0;
}

void formVPNSetup(webs_t wp, char_t *path, char_t *query)
{
	char_t  *vpn_enabled, *submitUrl;

	vpn_enabled = websGetVar(wp, T("vpn_enabled"), T(""));
	if (vpn_enabled[0] == '\0')
		vpn_enabled="off";

	if (nvram_set(RT2860_NVRAM, "vpnEnabled", (void *)vpn_enabled)!=0)
	{
		printf("Set vpnEnabled error!\n");
		return;
	}

	nvram_init(RT2860_NVRAM);

	// Now store VPN_ENABLED flag
	// Do not set other params if VPN is turned off
	if (!strcmp(vpn_enabled, "on"))
	{
		const parameter_fetch_t *fetch = vpn_args;

#ifdef CONFIG_USER_KABINET
		char_t *vpn_type = websGetVar(wp, T("vpn_type"), T("0"));
		if (strcmp(vpn_type, "6") == 0)
			fetch = lanauth_args;
#endif
		printf("vpn_enabled value : %s\n", vpn_enabled);

		setupParameters(wp, fetch, 0);
/*		while (fetch->web_param != NULL)
		{
			// Get variable
			char_t *str = websGetVar(wp, (char_t *)fetch->web_param, T(""));
			if (fetch->is_switch) // Check if need update a switch
			{
				if (str[0]=='\0')
					str = "off";
			}
			
			if (nvram_bufset(RT2860_NVRAM, (char_t *)fetch->nvram_param, (void *)str)!=0) //!!!
				printf("Set %s nvram error!", fetch->nvram_param);
			
			printf("%s value : %s\n", fetch->nvram_param, str);
			fetch++;
		}*/
		
		// Check if routing table is enabled
		char *vpn_rt_enabled = websGetVar(wp, T("vpn_routing_enabled"), T("off"));
		if (vpn_rt_enabled[0] == '\0')
			vpn_rt_enabled="off";
	}

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	//kill helpers firt sigterm second sigkill
	printf("Kill helpers\n");
	system("/bin/killall -q S70vpnhelper");
	system("/bin/killall -q vpnhelper");
	system("/bin/killall -q -9 S70vpnhelper");
	system("/bin/killall -q -9 vpnhelper");
	printf("Calling vpn helper...\n");
	system("service vpnhelper restart &");

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (submitUrl[0])
		websRedirect(wp, submitUrl);
	else
		websDone(wp, 200);
}


/*
 * description: write DHCP client list
 */
static int getDhcpCliList(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	struct dyn_lease lease;

	int i;
	struct in_addr addr;
	int64_t written_at, curr, expired_abs;
	
	//if DHCP is disabled - just exit
	char* dhcpEnabled = nvram_get(RT2860_NVRAM, "dhcpEnabled");
	if (!strncmp(dhcpEnabled, "0", 2))
		return 0;

	doSystem("killall -q -USR1 udhcpd");

	fp = fopen("/var/udhcpd.leases", "r");
	if (NULL == fp)
		return websWrite(wp, T(""));

	/* Read header of dhcpleases */
	if (fread(&written_at, 1, sizeof(written_at), fp) != sizeof(written_at))
		return 0;
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
		websWrite(wp, T("<td>%02X"), lease.lease_mac[0]);
		for (i = 1; i < 6; i++)
			websWrite(wp, T(":%02X"), lease.lease_mac[i]);
		// IP
		addr.s_addr = lease.lease_nip;
		websWrite(wp, T("</td><td>%s</td><td>"), inet_ntoa(addr));

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
		websWrite(wp, "</tr>\n");
	}

	fclose(fp);
	return 0;
}

/*
 * arguments: type - 1 = write Primary DNS
 *                   2 = write Secondary DNS
 * description: write DNS ip address accordingly
 */
static int getDns(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char buf[80] = {0}, ns_str[11], dns[16] = {0};
	int type, idx = 0, req = 0;

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type)
			req = 1;
		else if (2 == type)
			req = 2;
		else
			return websWrite(wp, T(""));
	}

	fp = fopen("/etc/resolv.conf", "r");
	if (NULL == fp)
		return websWrite(wp, T(""));
	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		if (sscanf(buf, "%s %s", ns_str, dns) != 2)
			continue;
		if (strcasecmp(ns_str, "nameserver") != 0)
			continue;
		
		idx++;
		if (idx == req) {
			websWrite(wp, T("%s"), dns);
			break;
		}
	}
	fclose(fp);
	return 0;
}

/*
 * arguments: 
 * description: return 1 if hostname is supported
 */
static int getHostSupp(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef GA_HOSTNAME_SUPPORT
	ejSetResult(eid, "1");
#else
	ejSetResult(eid, "0");
#endif
	return 0;
}

/*
 * arguments: name - interface name (ex. eth0, rax ..etc)
 * description: write the existence of given interface,
 *              0 = ifc dosen't exist, 1 = ifc exists
 */
static int getIfLiveWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *name;
	char exist[2] = "0";

	if (ejArgs(argc, argv, T("%s"), &name) < 1) {
		websError(wp, 400, T("Insufficient args\n"));
		return -1;
	}
	exist[0] = (getIfLive(name) == 0)? '1' : '0';
	return websWrite(wp, T("%s"), exist);
}

/*
 * arguments: name - interface name (ex. eth0, rax ..etc)
 * description: write the existence of given interface,
 *              0 = ifc is down, 1 = ifc is up
 */
static int getIfIsUpWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *name;
	char up[2] = "1";

	if (ejArgs(argc, argv, T("%s"), &name) < 1) {
		websError(wp, 400, T("Insufficient args\n"));
		return -1;
	}
	up[0] = (getIfIsUp(name) == 1)? '1' : '0';
	return websWrite(wp, T("%s"), up);
}

static int getIgmpProxyBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_IGMP_PROXY
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getVPNBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_NF_CONNTRACK_PPTP || defined CONFIG_NF_CONNTRACK_PPTP_MODULE || \
    defined CONFIG_IP_NF_PPTP        || defined CONFIG_IP_NF_PPTP_MODULE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getMeshBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RT2860V2_AP_MESH || defined CONFIG_RT2860V2_STA_MESH
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWDSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RT2860V2_AP_WDS
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWSCBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RT2860V2_AP_WSC
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSTABuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RT2860V2_STA_WSC
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getMBSSIDBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_RT2860V2_AP_MBSS
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUSBBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USB
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getStorageBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USB_STORAGE && defined CONFIG_USER_STORAGE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getFtpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_STUPID_FTPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSmbBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_SAMBA
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getMediaBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USB && defined CONFIG_USER_USHARE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWebCamBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USB && defined CONFIG_USER_UVC_STREAM
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getPrinterSrvBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USB && defined CONFIG_USER_P910ND
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDynamicRoutingBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_ZEBRA
    return websWrite(wp, T("1"));
#else
    return websWrite(wp, T("0"));
#endif
}

static int getSWQoSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_NET_SCHED
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDATEBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_DATE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDDNSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_INADYN
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getLANAUTHBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_KABINET
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSysLogBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_SYSLOGD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getETHTOOLBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined CONFIG_USER_ETHTOOL
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write LAN ip address accordingly
 */
static int getLanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (-1 == getIfIp(getLanIfName(), if_addr)) {
		//websError(wp, 500, T("getLanIp: calling getIfIp error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_addr);
}

/*
 * description: write LAN MAC address accordingly
 */
static int getLanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (-1 == getIfMac(getLanIfName(), if_mac)) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * arguments: type - 0 = return LAN interface name (default)
 *                   1 = write LAN interface name
 * description: return or write LAN interface name accordingly
 */
static int getLanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char *name = getLanIfName();

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type) {
			return websWrite(wp, T("%s"), name);
		}
	}
	ejSetResult(eid, name);
	return 0;
}

/*
 * description: write LAN subnet mask accordingly
 */
static int getLanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];

	if (-1 == getIfNetmask(getLanIfName(), if_net)) {
		//websError(wp, 500, T("getLanNetmask: calling getIfNetmask error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_net);
}

static int getGWBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("1"));
}

static int getDnsmasqBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_DNSMASQ
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getLltdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_LLTD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getPppoeRelayBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_PPPPOE_RELAY
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUpnpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_MINIUPNPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSpotBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_CHILLISPOT
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRadvdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_RADVD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write WAN ip address accordingly
 */
static int getWanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (-1 == getIfIp(getWanIfNamePPP(), if_addr)) {
		//websError(wp, 500, T("getWanIp: calling getIfIp error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_addr);
}

/*
 * description: write WAN MAC address accordingly
 */
static int getWanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (-1 == getIfMac(getWanIfName(), if_mac)) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * arguments: type - 0 = return WAN interface name (default)
 *                   1 = write WAN interface name
 * description: return or write WAN interface name accordingly
 */
static int getWanIfNameWeb(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char *name = getWanIfName();

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type) {
			return websWrite(wp, T("%s"), name);
		}
	}
	ejSetResult(eid, name);
	return 0;
}

/*
 * description: write WAN subnet mask accordingly
 */
static int getWanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];
	char *cm = nvram_get(RT2860_NVRAM, "wanConnectionMode");

	if (!strncmp(cm, "PPPOE", 6) || !strncmp(cm, "L2TP", 5) || !strncmp(cm, "PPTP", 5) 
#ifdef CONFIG_USER_3G
			|| !strncmp(cm, "3G", 3)
#endif
	){ //fetch ip from ppp0
		if (-1 == getIfNetmask("ppp0", if_net)) {
			return websWrite(wp, T(""));
		}
	}
	else if (-1 == getIfNetmask(getWanIfName(), if_net)) {
		//websError(wp, 500, T("getWanNetmask: calling getIfNetmask error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_net);
}

/*
 * description: write WAN default gateway accordingly
 */
static int getWanGateway(int eid, webs_t wp, int argc, char_t **argv)
{
	char   buff[256];
	int    nl = 0 ;
	struct in_addr dest;
	struct in_addr gw;
	int    flgs, ref, use, metric;
	unsigned long int d,g,m;
	int    find_default_flag = 0;

	char sgw[16];

	FILE *fp = fopen("/proc/net/route", "r");

	while (fgets(buff, sizeof(buff), fp) != NULL) {
		if (nl) {
			int ifl = 0;
			while (buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0')
				ifl++;
			buff[ifl]=0;    /* interface */
			if (sscanf(buff+ifl+1, "%lx%lx%X%d%d%d%lx",
						&d, &g, &flgs, &ref, &use, &metric, &m)!=7) {
				fclose(fp);
				return websWrite(wp, T("format error"));
			}

			if (flgs&RTF_UP) {
				dest.s_addr = d;
				gw.s_addr   = g;
				strcpy(sgw, (gw.s_addr==0 ? "" : inet_ntoa(gw)));

				if (dest.s_addr == 0) {
					find_default_flag = 1;
					break;
				}
			}
		}
		nl++;
	}
	fclose(fp);

	if (find_default_flag == 1)
		return websWrite(wp, T("%s"), sgw);
	else
		return websWrite(wp, T(""));
}


#define DD printf("%d\n", __LINE__);fflush(stdout);

/*
 *
 */
int getIndexOfRoutingRule(char *dest, char *netmask, char *interface)
{
	int index=0;
	char *rrs, one_rule[256];
	char dest_f[32], netmask_f[32], interface_f[32];

	rrs = nvram_get(RT2860_NVRAM, "RoutingRules");
	if(!rrs || !strlen(rrs))
		return -1;

	while( getNthValueSafe(index, rrs, ';', one_rule, 256) != -1 ){
		if((getNthValueSafe(0, one_rule, ',', dest_f, sizeof(dest_f)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(1, one_rule, ',', netmask_f, sizeof(netmask_f)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(4, one_rule, ',', interface_f, sizeof(interface_f)) == -1)){
			index++;
			continue;
		}
		//printf("@@@@@ %s %s %s\n", dest_f, netmask_f, interface_f);
		//printf("----- %s %s %s\n", dest, netmask, interface);
		if( (!strcmp(dest, dest_f)) && (!strcmp(netmask, netmask_f)) && (!strcmp(interface, interface_f))){
			return index;
		}
		index++;
	}

	return -1;
}

static void removeRoutingRule(char *dest, char *netmask, char *ifname)
{
	char cmd[1024];
	strcpy(cmd, "route del ");
	
	// host or net?
	if(!strcmp(netmask, "255.255.255.255") )
		strcat(cmd, "-host ");
	else
		strcat(cmd, "-net ");

	// destination
	strcat(cmd, dest);
	strcat(cmd, " ");

	// netmask
	if(strcmp(netmask, "255.255.255.255"))
		sprintf(cmd, "%s netmask %s", cmd, netmask);

	//interface
	sprintf(cmd, "%s dev %s ", cmd, ifname);
	doSystem(cmd);
}

void staticRoutingInit(void)
{
	int index=0;
	char one_rule[256];
	char *rrs;
	struct in_addr dest_s, gw_s, netmask_s;
	char dest[32], netmask[32], gw[32], interface[32], true_interface[32], custom_interface[32], comment[32];
	int	flgs, ref, use, metric, nl=0;
	unsigned long int d,g,m;
	int isGatewayMode = (!strcmp("1", nvram_get(RT2860_NVRAM, "OperationMode"))) ? 1 : 0 ;

	// delete old user rules
	FILE *fp = fopen("/proc/net/route", "r");
	if(!fp)
		return;

	while (fgets(one_rule, sizeof(one_rule), fp) != NULL) {
		if (nl) {
			if (sscanf(one_rule, "%s%lx%lx%X%d%d%d%lx",
					interface, &d, &g, &flgs, &ref, &use, &metric, &m) != 8) {
				printf("format error\n");
				fclose(fp);
				return;
			}
			dest_s.s_addr = d;
			gw_s.s_addr = g;
			netmask_s.s_addr = m;

			strncpy(dest, inet_ntoa(dest_s), sizeof(dest));
			strncpy(gw, inet_ntoa(gw_s), sizeof(gw));
			strncpy(netmask, inet_ntoa(netmask_s), sizeof(netmask));

			// check if internal routing rules
			if( (index=getIndexOfRoutingRule(dest, netmask, interface)) != -1){
				removeRoutingRule(dest, netmask, interface);
			}
		}
		nl++;
	}
	fclose(fp);

	index = 0;
	rrs = nvram_get(RT2860_NVRAM, "RoutingRules");
	if(!rrs|| !strlen(rrs))
		return;

	while( getNthValueSafe(index, rrs, ';', one_rule, 256) != -1 ){
		char cmd[1024];

		if((getNthValueSafe(0, one_rule, ',', dest, sizeof(dest)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(1, one_rule, ',', netmask, sizeof(netmask)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(2, one_rule, ',', gw, sizeof(gw)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(3, one_rule, ',', interface, sizeof(interface)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(4, one_rule, ',', true_interface, sizeof(true_interface)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(5, one_rule, ',', custom_interface, sizeof(custom_interface)) == -1)){
			index++;
			continue;
		}
		if((getNthValueSafe(6, one_rule, ',', comment, sizeof(comment)) == -1)){
			index++;
			continue;
		}

		strcpy(cmd, "route add ");
		
		// host or net?
		if(!strcmp(netmask, "255.255.255.255") )
			strcat(cmd, "-host ");
		else
			strcat(cmd, "-net ");

		// destination
		strcat(cmd, dest);
		strcat(cmd, " ");

		// netmask
		if(strcmp(netmask, "255.255.255.255") )
			sprintf(cmd, "%s netmask %s", cmd, netmask);

		// gateway
		if(strcmp(gw, "0.0.0.0"))
			sprintf(cmd, "%s gw %s", cmd, gw);

		sprintf(cmd, "%s dev %s ", cmd, true_interface);

		strcat(cmd, "2>&1 ");

		if(strcmp(interface, "WAN") || (!strcmp(interface, "WAN") && isGatewayMode)  ){
			doSystem(cmd);
		}else{
			printf("Skip WAN routing rule in the non-Gateway mode: %s\n", cmd);
		}

		index++;
	}
	return;
}

void dynamicRoutingInit(void)
{
	zebraRestart();
	ripdRestart();
}

void RoutingInit(void)
{
	staticRoutingInit();
	dynamicRoutingInit();
}

static inline int getNums(char *value, char delimit)
{
    char *pos = value;
    int count=1;
    if(!pos || !strlen(pos))
        return 0;
    while( (pos = strchr(pos, delimit))){
        pos = pos+1;
        count++;
    }
    return count;
}

/*
 * description: get routing table
 */
static int getRoutingTable(int eid, webs_t wp, int argc, char_t **argv)
{
	char   result[4096] = {0};
	char   buff[512];
	int    nl = 0, index;
	char   ifname[32], interface[128];
	struct in_addr dest, gw, netmask;
	char   dest_str[32], gw_str[32], netmask_str[32], comment[32];
	int    flgs, ref, use, metric;
	int	   *running_rules = NULL;
	unsigned long int d,g,m;
	char *rrs;
	int  rule_count;
	FILE *fp = fopen("/proc/net/route", "r");
	if(!fp)
		return -1;

	rrs = nvram_get(RT2860_NVRAM, "RoutingRules");
	rule_count = getNums(rrs, ';');

	if(rule_count){
		running_rules = calloc(1, sizeof(int) * rule_count);
		if(!running_rules)
			return -1;
	}
		
	strncat(result, "\"", sizeof(result));
	while (fgets(buff, sizeof(buff), fp) != NULL) {
		if (nl) {
			if (sscanf(buff, "%s%lx%lx%X%d%d%d%lx",
					ifname, &d, &g, &flgs, &ref, &use, &metric, &m) != 8) {
				printf("format error\n");
				fclose(fp);
				return websWrite(wp, T(""));
			}
			dest.s_addr = d;
			gw.s_addr = g;
			netmask.s_addr = m;

			if(! (flgs & 0x1) )	// skip not usable
				continue;

			strncpy(dest_str, inet_ntoa(dest), sizeof(dest_str));
			strncpy(gw_str, inet_ntoa(gw), sizeof(gw_str));
			strncpy(netmask_str, inet_ntoa(netmask), sizeof(netmask_str));

			if(nl > 1)
				strncat(result, ";", sizeof(result));
			strncat(result, ifname, sizeof(result));		strncat(result, ",", sizeof(result));
			strncat(result, dest_str, sizeof(result));		strncat(result, ",", sizeof(result));
			strncat(result, gw_str, sizeof(result));			strncat(result, ",", sizeof(result));
			strncat(result, netmask_str, sizeof(result) );	strncat(result, ",", sizeof(result));
			snprintf(result, sizeof(result), "%s%d,%d,%d,%d,", result, flgs, ref, use, metric);

			// check if internal routing rules
			strcpy(comment, " ");
			if( (index=getIndexOfRoutingRule(dest_str, netmask_str, ifname)) != -1){
				char one_rule[256];

				if(index < rule_count)
					running_rules[index] = 1;
				else
					printf("fatal error in %s\n", __FUNCTION__);

				snprintf(result, sizeof(result), "%s%d,", result, index);
				if(rrs && strlen(rrs)){
					if( getNthValueSafe(index, rrs, ';', one_rule, sizeof(one_rule)) != -1){

						if( getNthValueSafe(3, one_rule, ',', interface, sizeof(interface)) != -1){
							strncat(result, interface, sizeof(result));
							strncat(result, ",", sizeof(result));
						}
						if( getNthValueSafe(6, one_rule, ',', comment, sizeof(comment)) != -1){
							// do nothing;
						}
					}
				}
			}else{
				strncat(result, "-1,", sizeof(result));
				strncat(result, getLanWanNamebyIf(ifname), sizeof(result));
				strncat(result, ",", sizeof(result));
			}
			strncat(result, "0,", sizeof(result));	// used rule
			strncat(result, comment, sizeof(result));
		}
		nl++;
	}

	for(index=0; index < rule_count; index++){
		char one_rule[256];

		if(running_rules[index])
			continue;

		if(getNthValueSafe(index, rrs, ';', one_rule, sizeof(one_rule)) == -1)
			continue;

		if(getNthValueSafe(0, one_rule, ',', dest_str, sizeof(dest_str)) == -1)
			continue;

		if(getNthValueSafe(1, one_rule, ',', netmask_str, sizeof(netmask_str)) == -1)
			continue;

		if(getNthValueSafe(2, one_rule, ',', gw_str, sizeof(gw_str)) == -1)
			continue;

		if(getNthValueSafe(3, one_rule, ',', interface, sizeof(interface)) == -1)
			continue;

		if(getNthValueSafe(4, one_rule, ',', ifname, sizeof(ifname)) == -1)
			continue;

		if(getNthValueSafe(6, one_rule, ',', comment, sizeof(comment)) == -1)
			continue;

		if(strlen(result))
			strncat(result, ";", sizeof(result));

		snprintf(result, sizeof(result), "%s%s,%s,%s,%s,0,0,0,0,%d,%s,1,%s", result, ifname, dest_str, gw_str, netmask_str, index, interface, comment);
	}

	strcat(result, "\"");
	websLongWrite(wp, result);
	fclose(fp);
	if(running_rules)
		free(running_rules);
	//printf("%s\n", result);
	return 0;
}

static void addRouting(webs_t wp, char_t *path, char_t *query)
{
	char_t *dest, *hostnet, *netmask, *gateway, *interface, *true_interface, *custom_interface, *comment;
	char cmd[256] = {0};
	char result[256] = {0};

	FILE *fp;

	dest = websGetVar(wp, T("dest"), T(""));
	hostnet = websGetVar(wp, T("hostnet"), T(""));
	netmask = websGetVar(wp, T("netmask"), T(""));	
	gateway = websGetVar(wp, T("gateway"), T(""));
	interface = websGetVar(wp, T("interface"), T(""));
	custom_interface = websGetVar(wp, T("custom_interface"), T(""));
	comment = websGetVar(wp, T("comment"), T(""));

	if( !dest)
		return;

	strcat(cmd, "route add ");
	
	// host or net?
	if(!gstrcmp(hostnet, "net"))
		strcat(cmd, "-net ");
	else
		strcat(cmd, "-host ");

	// destination
	strcat(cmd, dest);
	strcat(cmd, " ");

	// netmask
	if(gstrlen(netmask))
		sprintf(cmd, "%s netmask %s", cmd, netmask);
	else
		netmask = "255.255.255.255";

	//gateway
	if(gstrlen(gateway))
		sprintf(cmd, "%s gw %s", cmd, gateway);
	else
		gateway = "0.0.0.0";

	//interface
	if(gstrlen(interface)){
		if (!gstrcmp(interface, "WAN")){
			true_interface = getWanIfName();
		}else if (!gstrcmp(interface, "Custom")){
			if(!gstrlen(custom_interface))
				return;
			true_interface = custom_interface;
		}else	// LAN & unknown
			true_interface = getLanIfName();
	}else{
		interface = "LAN";
		true_interface = getLanIfName();
	}
	sprintf(cmd, "%s dev %s ", cmd, true_interface);

	strcat(cmd, "2>&1 ");

	printf("%s\n", cmd);
	fp = popen(cmd, "r");
	fgets(result, sizeof(result), fp);
	pclose(fp);

	if (!strlen(result))
	{
		// success, write down to the flash
		char tmp[1024];
		char *rrs = nvram_get(RT2860_NVRAM, "RoutingRules");
		if (!rrs || !strlen(rrs))
			memset(tmp, 0, sizeof(tmp));
		else
			strncpy(tmp, rrs, sizeof(tmp));

		if(strlen(tmp))
			strcat(tmp, ";");
		sprintf(tmp, "%s%s,%s,%s,%s,%s,%s,%s", tmp, dest, netmask, gateway, interface, true_interface, custom_interface, comment);
		nvram_set(RT2860_NVRAM, "RoutingRules", tmp);
	}
	else
	{
		websHeader(wp);
		websWrite(wp, T("<h1>Add routing failed:<br> %s<h1>"), result);
		websFooter(wp);
		websDone(wp, 200);
		return;
	}

	//debug print
	websHeader(wp);
	websWrite(wp, T("<h3>Add routing table:</h3><br>\n"));
	if (strlen(result))
		websWrite(wp, T("Success"));
	else
		websWrite(wp, T("%s"), result);

	websWrite(wp, T("Destination: %s<br>\n"), dest);
	websWrite(wp, T("Host/Net: %s<br>\n"), hostnet);
	websWrite(wp, T("Netmask: %s<br>\n"), netmask);
	websWrite(wp, T("Gateway: %s<br>\n"), gateway);
	websWrite(wp, T("Interface: %s<br>\n"), interface);
	websWrite(wp, T("True Interface: %s<br>\n"), true_interface);
	if(strlen(custom_interface))
		websWrite(wp, T("Custom_interface %s<br>\n"), custom_interface);
	websWrite(wp, T("Comment: %s<br>\n"), comment);
	websFooter(wp);
	websDone(wp, 200);
}

static void delRouting(webs_t wp, char_t *path, char_t *query)
{
	int index, rule_count;
	char_t *value, dest[256], netmask[256], true_interface[256];
	char name_buf[16] = {0};
	char *rrs;
	int *deleArray, j=0;
	
	rrs = nvram_get(RT2860_NVRAM, "RoutingRules");
	if(!rrs || !strlen(rrs))
		return;

	rule_count = getNums(rrs, ';');
	if(!rule_count)
		return;

	if(!(deleArray = malloc(sizeof(int) * rule_count) ) )
		return;
		
	websHeader(wp);

	for (index=0; index< rule_count; index++)
	{
		snprintf(name_buf, sizeof(name_buf), "DR%d", index);
		value = websGetVar(wp, name_buf, NULL);

		if (value)
		{
			deleArray[j++] = index;
			if(strlen(value) > 256)
				continue;
			sscanf(value, "%s%s%s", dest, netmask, true_interface);
			removeRoutingRule(dest, netmask, true_interface);
			websWrite(wp, T("Delete entry: %s,%s,%s<br>\n"), dest, netmask, true_interface);
		}
	}

	if (j>0)
	{
		deleteNthValueMulti(deleArray, j, rrs, ';');
		nvram_set(RT2860_NVRAM, "RoutingRules", rrs);
	}

	websFooter(wp);
	websDone(wp, 200);

	free(deleArray);
}

void ripdRestart(void)
{
	char lan_ip[16], wan_ip[16], lan_mask[16], wan_mask[16];

	char *opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	char *password = nvram_get(RT2860_NVRAM, "Password");
	char *RIPEnable = nvram_get(RT2860_NVRAM, "RIPEnable");

	if (!opmode || !strlen(opmode)) // unknown
		goto out;

	if (!strcmp(opmode, "0"))	// bridge
		goto out;

	if (!RIPEnable || !strlen(RIPEnable) || !strcmp(RIPEnable,"0")) //ripd disable
		goto out;

	if(!password || !strlen(password)) //if password not set
		password = "Admin";

	doSystem("echo \"hostname linux.router1\" > /etc/ripd.conf ");
	doSystem("echo \"password %s\" >> /etc/ripd.conf ", password);
	doSystem("echo \"router rip\" >> /etc/ripd.conf ");

	// deal with WAN
	if(getIfIp(getWanIfName(), wan_ip) != -1){
		if(getIfNetmask(getWanIfName(), wan_mask) != -1){
			doSystem("echo \"network %s/%d\" >> /etc/ripd.conf", wan_ip, netmask_aton(wan_mask));
			doSystem("echo \"network %s\" >> /etc/ripd.conf", getWanIfName());
		}else
			printf("ripdRestart(): The WAN IP is still undeterminated...\n");
	}else
		printf("ripdRestart(): The WAN IP is still undeterminated...\n");

	// deal with LAN
	if(getIfIp(getLanIfName(), lan_ip) != -1){
		if(getIfNetmask(getLanIfName(), lan_mask) != -1){
			doSystem("echo \"network %s/%d\" >> /etc/ripd.conf", lan_ip, netmask_aton(lan_mask));
			doSystem("echo \"network %s\" >> /etc/ripd.conf", getLanIfName());
		}
	}
	doSystem("echo \"version 2\" >> /etc/ripd.conf");
	doSystem("echo \"log syslog\" >> /etc/ripd.conf");
out:
	doSystem("service ripd restart &");
}

inline void zebraRestart(void)
{
	char *opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	char *password = nvram_get(RT2860_NVRAM, "Password");
	char *RIPEnable = nvram_get(RT2860_NVRAM, "RIPEnable");

	if(!opmode||!strlen(opmode))   //unknown
		goto out;

	if(!strcmp(opmode, "0"))	// bridge
		goto out;

	if(!RIPEnable || !strlen(RIPEnable) || !strcmp(RIPEnable,"0")) // zebra disabled
		goto out;

	if(!password || !strlen(password))
		password = "Admin";

	doSystem("echo \"hostname linux.router1\" > /etc/zebra.conf ");
	doSystem("echo \"password %s\" >> /etc/zebra.conf ", password);
	doSystem("echo \"enable password Admin\" >> /etc/zebra.conf ");
	doSystem("echo \"log syslog\" >> /etc/zebra.conf ");
out:
	doSystem("service zebra restart &");
}

static void dynamicRouting(webs_t wp, char_t *path, char_t *query)
{
	char_t *rip;
	char *RIPEnable;
	rip = websGetVar(wp, T("RIPSelect"), T(""));
	if(!rip || !strlen(rip))
		return;

	RIPEnable = nvram_get(RT2860_NVRAM, "RIPEnable");
	if(!RIPEnable || !strlen(RIPEnable))
		RIPEnable = "0";

	if(!gstrcmp(rip, "0") && !strcmp(RIPEnable, "0"))
	{
		// nothing changed
	}
	else if(!gstrcmp(rip, "1") && !strcmp(RIPEnable, "1"))
	{
		// nothing changed
	}
	else if(!gstrcmp(rip, "0") && !strcmp(RIPEnable, "1"))
	{
		nvram_set(RT2860_NVRAM, "RIPEnable", rip);
		
		doSystem("service ripd stop &");
		doSystem("service zebra stop &");
	}
	else if(!gstrcmp(rip, "1") && !strcmp(RIPEnable, "0"))
	{
		nvram_set(RT2860_NVRAM, "RIPEnable", rip);
		
		zebraRestart();
		ripdRestart();
	}
	else
		return;

	//debug print
	websHeader(wp);
	websWrite(wp, T("<h3>Dynamic Routing:</h3><br>\n"));
	websWrite(wp, T("RIPEnable %s<br>\n"), rip);
	websFooter(wp);
	websDone(wp, 200);
}

/*
 * description: setup internet according to nvram configurations
 *              (assume that nvram_init has already been called)
 *              return value: 0 = successful, -1 = failed
 */
void initInternet(void)
{
	char *auth_mode = nvram_get(RT2860_NVRAM, "AuthMode");
#if defined (CONFIG_RT2860V2_STA) || defined (CONFIG_RT2860V2_STA_MODULE)
	char *opmode;
#endif
	firewall_rebuild_etc();
	doSystem("internet.sh");

	//First Security LED init
	if (!strcmp(auth_mode, "Disable") || !strcmp(auth_mode, "OPEN"))
		ledAlways(13, LED_OFF); //turn off security LED (gpio 13)
	else
		ledAlways(13, LED_ON); //turn on security LED (gpio 13)

//automatically connect to AP according to the active profile
#if defined (CONFIG_RT2860V2_STA) || defined (CONFIG_RT2860V2_STA_MODULE)
	opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	if (!strcmp(opmode, "2") || (!strcmp(opmode, "0") && !strcmp("1",  nvram_get(RT2860_NVRAM, "ethConver")))) {
		if (initStaProfile() != -1)
			    initStaConnection();
	}
#endif
#ifdef CONFIG_USER_802_1X 
	restart8021XDaemon(RT2860_NVRAM);	// in wireless.c
#endif
#ifdef CONFIG_RT2860V2_AP_ANTENNA_DIVERSITY
	AntennaDiversityInit();
#endif
#if defined (CONFIG_RT2860V2_AP_WSC) || defined (CONFIG_RT2860V2_STA_WSC)
	WPSRestart();
#endif

//Routing and QoS in STA mode need set after connect to STA
	RoutingInit();
#ifdef CONFIG_NET_SCHED
	QoSInit();
#endif
}

static void getMyMAC(webs_t wp, char_t *path, char_t *query)
{
	char myMAC[32];

	arplookup(wp->ipaddr, myMAC);
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\n"));
	websWrite(wp, WEBS_CACHE_CONTROL_STRING);
	websWrite(wp, T("\n"));
	websWrite(wp, T("%s"), myMAC);
	websDone(wp, 200);
}

/* goform/setLan */
static void setLan(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ip, *nm;
	char_t	*gw = NULL, *pd = NULL, *sd = NULL;
	char_t *lan2enabled, *lan2_ip, *lan2_nm;
#ifdef GA_HOSTNAME_SUPPORT
	char_t	*host;
#endif
	char	*opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	char	*wan_ip = nvram_get(RT2860_NVRAM, "wan_ipaddr");
	char	*ctype = nvram_get(RT2860_NVRAM, "connectionType");

	ip = websGetVar(wp, T("lanIp"), T(""));
	nm = websGetVar(wp, T("lanNetmask"), T(""));
	lan2enabled = websGetVar(wp, T("lan2enabled"), T(""));
	lan2_ip = websGetVar(wp, T("lan2Ip"), T(""));
	lan2_nm = websGetVar(wp, T("lan2Netmask"), T(""));
#ifdef GA_HOSTNAME_SUPPORT
	host = websGetVar(wp, T("hostname"), T("0"));
#endif
	/*
	 * check static ip address:
	 * lan and wan ip should not be the same except in bridge mode
	 */
	if (strncmp(ctype, "STATIC", 7))
	{
		if (strcmp(opmode, "0") && !strncmp(ip, wan_ip, 15)) {
			websError(wp, 200, "IP address is identical to WAN");
			return;
		}
		if (!strcmp(lan2enabled, "1"))
		{
			if (strcmp(opmode, "0") && !strncmp(lan2_ip, wan_ip, 15)) {
				websError(wp, 200, "LAN2 IP address is identical to WAN");
				return;
			}
			else if (strcmp(opmode, "0") && !strncmp(lan2_ip, ip, 15)) {
				websError(wp, 200, "LAN2 IP address is identical to LAN1");
				return;
			}
		}
	}
	
	nvram_init(RT2860_NVRAM);
	
	// configure gateway and dns (WAN) at bridge mode
	if (!strncmp(opmode, "0", 2))
	{
		gw = websGetVar(wp, T("lanGateway"), T(""));
		pd = websGetVar(wp, T("lanPriDns"), T(""));
		sd = websGetVar(wp, T("lanSecDns"), T(""));
		nvram_bufset(RT2860_NVRAM, "wan_gateway", gw);
		nvram_bufset(RT2860_NVRAM, "wan_primary_dns", pd);
		nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", sd);
	}

	nvram_bufset(RT2860_NVRAM, "lan_ipaddr", ip);
	nvram_bufset(RT2860_NVRAM, "lan_netmask", nm);
	nvram_bufset(RT2860_NVRAM, "Lan2Enabled", lan2enabled);
	nvram_bufset(RT2860_NVRAM, "lan2_ipaddr", lan2_ip);
	nvram_bufset(RT2860_NVRAM, "lan2_netmask", lan2_nm);
#ifdef GA_HOSTNAME_SUPPORT
	nvram_bufset(RT2860_NVRAM, "HostName", host);
#endif
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	//debug print
	websHeader(wp);
	websWrite(wp, T("<h3>LAN Interface Setup</h3><br>\n"));
#ifdef GA_HOSTNAME_SUPPORT
	websWrite(wp, T("Hostname: %s<br>\n"), host);
#endif
	websWrite(wp, T("IP: %s<br>\n"), ip);
	websWrite(wp, T("Netmask: %s<br>\n"), nm);
	websWrite(wp, T("LAN2 Enabled: %s<br>\n"), lan2enabled);
	websWrite(wp, T("LAN2 IP: %s<br>\n"), lan2_ip);
	websWrite(wp, T("LAN2 Netmask: %s<br>\n"), lan2_nm);
	if (!strncmp(opmode, "0", 2))
	{
		websWrite(wp, T("Gateway: %s<br>\n"), gw);
		websWrite(wp, T("PriDns: %s<br>\n"), pd);
		websWrite(wp, T("SecDns: %s<br>\n"), sd);
	}
	websFooter(wp);
	websDone(wp, 200);

	doSystem("internet.sh lanonly");
}

/* goform/setWan */
static void setWan(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ctype, *req_ip;
	char_t	*ip, *nm, *gw;
	char_t	*eth, *user, *pass;
	char_t	*nat_enable;
	char_t	*vpn_srv, *vpn_mode;
	char_t	*l2tp_srv, *l2tp_mode;
#ifdef CONFIG_USER_3G
	char_t	*usb3g_dev;
#endif
	char	*opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	char	*lan_ip = nvram_get(RT2860_NVRAM, "lan_ipaddr");
	char	*lan2enabled = nvram_get(RT2860_NVRAM, "Lan2Enabled");

	ctype = ip = nm = gw = eth = user = pass =
	vpn_srv = vpn_mode = l2tp_srv = l2tp_mode = NULL;

	ctype = websGetVar(wp, T("connectionType"), T("0")); 
	req_ip = websGetVar(wp, T("dhcpReqIP"), T("")); 
	
	if (!strncmp(ctype, "STATIC", 7) || !strcmp(opmode, "0"))
	{
		//always treat bridge mode having static wan connection
		ip = websGetVar(wp, T("staticIp"), T(""));
		nm = websGetVar(wp, T("staticNetmask"), T("0"));
		gw = websGetVar(wp, T("staticGateway"), T(""));

		nvram_set(RT2860_NVRAM, "wanConnectionMode", ctype);
		if (-1 == inet_addr(ip))
		{
			websError(wp, 200, "invalid IP Address");
			return;
		}
		/*
		 * lan and wan ip should not be the same except in bridge mode
		 */
		if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan_ip, 15))
		{
			websError(wp, 200, "IP address is identical to LAN");
			return;
		}
		if (!strcmp(lan2enabled, "1"))
		{
			char *lan2_ip = nvram_get(RT2860_NVRAM, "lan2_ipaddr");
			if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan2_ip, 15))
			{
				websError(wp, 200, "IP address is identical to LAN2");
				return;
			}
		}

		if (-1 == inet_addr(nm))
		{
			websError(wp, 200, "invalid Subnet Mask");
			return;
		}

		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "wan_ipaddr", ip);
		nvram_bufset(RT2860_NVRAM, "wan_netmask", nm);
		/*
		 * in Bridge Mode, lan and wan are bridged together and associated with
		 * the same ip address
		 */
		if (NULL != opmode && !strcmp(opmode, "0"))
		{
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", ip);
			nvram_bufset(RT2860_NVRAM, "lan_netmask", nm);
		}
		nvram_bufset(RT2860_NVRAM, "wan_gateway", gw);
		
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
		
		// Reset /etc/resolv.conf
		FILE *fd = fopen("/etc/resolv.conf", "w");
		if (fd != NULL)
			fclose(fd);
	}
	else if (strncmp(ctype, "DHCP", 5) == 0)
	{
		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
		nvram_bufset(RT2860_NVRAM, "dhcpRequestIP", req_ip);
		printf("dhcpRequestIP = %s\n", req_ip);
		printf("wanConnectionMode = %s\n", ctype);
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
	else
	{
		websHeader(wp);
		websWrite(wp, T("<h2>Unknown Connection Type: %s</h2><br>\n"), ctype);
		websFooter(wp);
		websDone(wp, 200);
		return;
	}
	
	// Primary/Seconfary DNS set
	char_t *st_en = websGetVar(wp, T("wStaticDnsEnable"), T("off"));
	char_t *pd = websGetVar(wp, T("staticPriDns"), T(""));
	char_t *sd = websGetVar(wp, T("staticSecDns"), T(""));
	
	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "wan_static_dns", st_en);
	
	if (strcmp(st_en, "on") == 0)
	{
		nvram_bufset(RT2860_NVRAM, "wan_primary_dns", pd);
		nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", sd);
	}

	// NAT
	if (strcmp(opmode, "0") != 0)
	{
		nat_enable = websGetVar(wp, T("natEnabled"), T("off"));
		nat_enable = (strcmp(nat_enable, "on") == 0) ? "1" : "0";
		nvram_bufset(RT2860_NVRAM, "natEnabled", nat_enable);
	}
	
	// MTU for WAN
	char *wan_mtu = websGetVar(wp, T("wan_mtu"), T("0"));
	nvram_bufset(RT2860_NVRAM, "wan_manual_mtu", wan_mtu);
	
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	char *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (submitUrl == NULL)
	{
		// debug print
		websHeader(wp);
		websWrite(wp, T("<h2>Mode: %s</h2><br>\n"), ctype);
		if (!strncmp(ctype, "STATIC", 7))
		{
			websWrite(wp, T("IP Address: %s<br>\n"), ip);
			websWrite(wp, T("Subnet Mask: %s<br>\n"), nm);
			websWrite(wp, T("Default Gateway: %s<br>\n"), gw);
		}
		else if (!strncmp(ctype, "DHCP", 5))
		{
		}

		websFooter(wp);
		websDone(wp, 200);
	}
	else
		websRedirect(wp, submitUrl);

	/* Prevent deadloop at WAN apply change if VPN started */
	doSystem("(ip route flush cache &&  service vpnhelper stop) > /dev/null 2>&1");
	initInternet();
}

#ifdef CONFIG_USER_CHILLISPOT
// ChilliSpot variables
const parameter_fetch_t chilli_vars[] =
{
	{ T("sPriDns"),			"chilli_dns1",			0 },
	{ T("sSecDns"),			"chilli_dns2",			0 },
	{ T("sDomain"),			"chilli_domain",		0 },
	{ T("sLease"),			"chilli_lease",			0 },
	{ T("sRadServer1"),		"chilli_radiusserver1",		0 },
	{ T("sRadServer2"),		"chilli_radiusserver2",		0 },
	{ T("sRadSecret"),		"chilli_radiussecret",		0 },
	{ T("sNasId"),			"chilli_radiusnasid",		0 },
	{ T("sRadLocationId"),		"chilli_radiuslocationid",	0 },
	{ T("sRadLocationName"),	"chilli_radiuslocationname",	0 },
	{ T("sRadCoaPort"),		"chilli_coaport",		0 },
	{ T("sRadCoaNoIpCheck"),	"chilli_coanoipcheck",		1 },
	{ T("sUamServer"),		"chilli_uamserver",		0 },
	{ T("sUamHomepage"),		"chilli_uamhomepage",		0 },
	{ T("sUamSecret"),		"chilli_uamsecret",		0 },
	{ T("sUamAllowed"),		"chilli_uamallowed",		0 },
	{ T("sUamAnyDns"),		"chilli_uamanydns",		1 },
	{ T("sMacAllowed"),		"chilli_macallowed",		0 },
	{ NULL, 0, 0 } // Terminator
};


/*
 * description: write hotspot network ip address
 */
static int getSpotIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *nvdata = nvram_get(RT2860_NVRAM, "chilli_net");
	
	char* slashPos = strchr(nvdata, '/');
	if (slashPos == NULL) {
		return websWrite(wp, T("172.16.0.0"));
	}
	slashPos[0] = '\0';
	return websWrite(wp, T("%s"), nvdata);
}

/*
 * description: write hotspot network netmask
 */
static int getSpotNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *nvdata = nvram_get(RT2860_NVRAM, "chilli_net");
	unsigned int imask;
	struct in_addr mask;
	
	char* slashPos = strchr(nvdata, '/');
	if (slashPos == NULL) {
		return websWrite(wp, T("255.255.255.0"));
	}
	imask = atoi(slashPos+1);
	if (!imask)
		imask = 24;
	mask.s_addr = ntohl(0 - (1 << (32 - imask)));
	
	return websWrite(wp, T("%s"), inet_ntoa(mask));
}

/* goform/setHotspot */
static void setHotspot(webs_t wp, char_t *path, char_t *query)
{
	char	*opmode = nvram_get(RT2860_NVRAM, "OperationMode");

	fprintf(stderr, "goform/setHotspot: opmode %s", opmode);
	websHeader(wp);
	//Re-check operation mode before any changes
	if (!strcmp(opmode, "4"))
	{
		nvram_init(RT2860_NVRAM);
		char_t *ip = websGetVar(wp, T("sIp"), T(""));
		char_t *amask = websGetVar(wp, T("sNetmask"), T(""));
		
		struct in_addr iip;
		struct in_addr imask;
		iip.s_addr = inet_addr(ip);
		imask.s_addr = inet_addr(amask);
		int h_mask=ntohl(imask.s_addr);
		int i;
		for (i = 30; i > 0; i--) {
			if (h_mask >= 0 - (1 << (32 - i)))
				break;
		}
		if (!i) i = 24;
		iip.s_addr &= ntohl(0 - (1 << (32 - i)));
		
		char_t subnet[20];
		sprintf(subnet, "%s/%d", inet_ntoa(iip), i);
		if (nvram_bufset(RT2860_NVRAM, "chilli_net", (void *)subnet)!=0) //!!!
			printf("Set chilli_net nvram error!");
		
		setupParameters(wp, chilli_vars, 0);
		
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
		websWrite(wp, T("<h3>Hotspot configuration done.</h3><br>\n"));
		websWrite(wp, T("Wait till device will be reconfigured...<br>\n"), ip);
		websFooter(wp);
		websDone(wp, 200);
		doSystem("internet.sh lanonly");
	} else {
		websWrite(wp, T("<h3>Error, try again</h3><br>\n"));
		websFooter(wp);
		websDone(wp, 200);
	}
	return;
}
#endif //CONFIG_USER_CHILLISPOT

