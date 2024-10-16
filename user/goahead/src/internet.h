/*
 *	internet.h -- Internet Configuration Header
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: internet.h,v 1.8 2008-03-17 07:47:16 yy Exp $
 */

#define ETH_SIG	"eth"
#define BR_SIG	"br"
#if defined(CONFIG_RAETH_ROUTER) || defined(CONFIG_RT_3052_ESW)				/* internal ralink esw */
#define WAN_DEF "eth2.2"
#elif !defined(CONFIG_RAETH_GMAC2) && defined(CONFIG_MAC_TO_MAC_MODE)			/* vetisse or rtl8367m one phy */
#define WAN_DEF "eth2.2"
#elif defined(CONFIG_RAETH_GMAC2) && defined(CONFIG_MAC_TO_MAC_MODE)			/* vetisse or rtl8367m dual phy */
#define WAN_DEF "eth3"
#elif defined(CONFIG_CONFIG_GE1_RGMII_AN) || defined(CONFIG_CONFIG_GE1_RGMII_AN)	/* MARVELL & IC+ */
#define WAN_DEF "eth2"
#else
#define WAN_DEF "eth2.2"								/* this is stub support only switched devices */
#endif

#define VPN_SIG	"ppp"
#define VPN_DEF "ppp0"

#define _PATH_PROCNET_DEV		"/proc/net/dev"
#define PATH_PPP_ROUTES			"/etc/routes_ppp_replace"
#define PATH_LANWAN_ROUTES		"/etc/routes_replace"

#define _PATH_DHCP_ALIAS_FILE		"/etc/dhcpd_static.conf"
#define _PATH_IPT_ACCOUNTING_FILE	"/proc/net/ipt_account/mynetwork"
#define IPT_SHORT_ACCOUNT

#define _PATH_PFW			"/etc"
#define _PATH_PFW_FILE			_PATH_PFW "/portforward"
#define _PATH_PFW_FILE_VPN		_PATH_PFW "/portforward_vpn"
#define _PATH_PFW_FILE_ACCESS		_PATH_PFW "/portforward_access"
#define _PATH_MACIP_FILE		_PATH_PFW "/macipfilter"
#define _PATH_WEBS_FILE			_PATH_PFW "/websfilter"

#ifndef IF_NAMESIZE
#define IF_NAMESIZE IFNAMSIZ
#endif

void formDefineInternet(void);
void initInternet(void);
char* getLanIfName(void);
char* getWanIfName(void);
int getIfIp(char *ifname, char *if_addr);
int getIfMac(char *ifname, char *if_hw);
