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
 * Router default NVRAM values
 *
 * Copyright 2001-2003, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: defaults.c,v 1.2 2007/06/21 04:55:53 jordan_hsiao Exp $
 */

//#include <epivers.h>
//#include <string.h>
#include <nvram/bcmnvram.h>
//#include <nvram/typedefs.h>
//#include <wlioctl.h>

//#define XSTR(s) STR(s)
//#define STR(s) #s

struct nvram_tuple router_defaults[] = {
	/* Restore defaults */
	{ "restore_defaults", "0", 0 },		/* Set to 0 to not restore defaults on boot */

#if 0
	{"ModuleName", "RT2860", 0},
	{"SystemName", "RalinkAP", 0},
	{"WebInit", "1", 0},
	{"HostName", "10.10.10.254", 0},
	{"Login", "admin", 0},
	{"Password", "admin", 0},
#ifdef CONFIG_LAN_WAN_SUPPORT
	{"OperationMode", "1", 0},              //0:Bridge, 1:Gateway, 2:Wireless-ISP
	{"Platform", "ICPLUS", 0},
	{"wanConnectionMode", "DHCP", 0},       //STATIC,DHCP,PPPOE,L2TP,PPTP
	{"wan_ipaddr", "192.168.1.1", 0},
	{"wan_netmask", "255.255.255.0", 0},
	{"wan_gateway", "192.168.1.254", 0},
	{"wan_primary_dns", "192.168.1.5", 0},
	{"wan_secondary_dns", "168.95.1.1", 0},
#else
	{"OperationMode", "0", 0},              //0:Bridge, 1:Gateway, 2:Wireless-ISP
	{"Platform", "MARVELL", 0},
	{"wanConnectionMode", "STATIC", 0},     //STATIC,DHCP,PPPOE,L2TP,PPTP
	{"wan_ipaddr", "10.10.10.254", 0},
	{"wan_netmask", "255.255.255.0", 0},
	{"wan_gateway", "10.10.10.253", 0},
	{"wan_primary_dns", "10.10.10.251", 0},
	{"wan_secondary_dns", "168.95.1.1", 0},
#endif
	{"wan_pppoe_user", "pppoe_user", 0},
	{"wan_pppoe_pass", "pppoe_passwd", 0},
	{"wan_l2tp_ip", "l2tp_server", 0},
	{"wan_l2tp_user", "l2tp_user", 0},
	{"wan_l2tp_pass", "l2tp_passwd", 0},
	{"wan_pptp_ip", "pptp_server", 0},
	{"wan_pptp_user", "pptp_user", 0},
	{"wan_pptp_pass", "pptp_passwd", 0},
	{"lan_ipaddr", "10.10.10.254", 0},
	{"lan_netmask", "255.255.255.0", 0},
	{"dhcpEnabled", "1", 0},
	{"dhcpStart", "10.10.10.100", 0},
	{"dhcpEnd", "10.10.10.200", 0},
	{"dhcpMask", "255.255.255.0", 0},
	{"dhcpPriDns", "10.10.10.251", 0},
	{"dhcpSecDns", "168.95.1.1", 0},
	{"dhcpGateway", "10.10.10.254", 0},
	{"dhcpLease", "86400", 0},
	{"stpEnabled", "0", 0},
	{"lltdEnabled", "0", 0},
	{"natEnabled", "1", 0},
	{"IPPortFilterEnable", "0", 0},
	{"IPPortFilterRules", "", 0},
	{"PortForwardEnable", "0", 0},
	{"PortForwardRules", "", 0},
	{"MacFilterEnable", "0", 0},
	{"MacFilterRules", "", 0},
	{"DefaultFirewallPolicy", "1", 0},
	{"DMZEnable", "0", 0},
	{"DMZIPAddress", "", 0},
	{"TZ", "", 0},
	{"NTPServerIP", "", 0},
	{"NTPSync", "", 0},
	{"DDNSProvider", "", 0},
	{"DDNS", "", 0},
	{"DDNSAccount", "", 0},
	{"DDNSPassword", "", 0},
#endif
	
	{"CountryRegion", "5", 0},
	{"CountryRegionABand", "7", 0},
//	{"CountryCode", "", 0},
	{"BssidNum", "1", 0},
//	{"SSID", "RT2880_AP", 0},
//	{"WirelessMode", "9", 0},
//	{"FixedTxMode", "1", 0},
//	{"TxRate", "0", 0},
//	{"Channel", "6", 0},
//	{"BasicRate", "15", 0},
//	{"BeaconPeriod", "100", 0},
//	{"DtimPeriod", "1", 0},
	{"TxPower", "100", 0},
//	{"BGProtection", "0", 0},
	{"DisableOLBC", "0", 0},
//	{"TxAntenna", "", 0},
//	{"RxAntenna", "", 0},
	{"TxPreamble", "0", 0},
//	{"RTSThreshold", "2347", 0},
//	{"FragThreshold", "2346", 0},
	{"TxBurst", "1", 0},
	{"PktAggregate", "1", 0},
//	{"NoForwarding", "0", 0},
//	{"NoForwardingBTNBSSID", "0", 0},
//	{"HideSSID", "0", 0},
	{"ShortSlot", "1", 0},
//	{"AutoChannelSelect", "0", 0},
	{"WiFiTest", "0", 0},
	{"TGnWifiTest", "0", 0},
//	{"AccessPolicy0", "0", 0},
//	{"AccessControlList0", "", 0},
	{"AccessPolicy1", "0", 0},
	{"AccessControlList1", "", 0},
	{"AccessPolicy2", "0", 0},
	{"AccessControlList2", "", 0},
	{"AccessPolicy3", "0", 0},
	{"AccessControlList3", "", 0},

	{"HT_AutoBA", "1", 0},
	{"HT_HTC", "1", 0},
	{"HT_RDG", "1", 0},
	{"HT_LinkAdapt", "0", 0},
	{"HT_BW", "1", 0},
	{"HT_EXTCHA", "1", 0},
	{"HT_OpMode", "0", 0},
	{"HT_MpduDensity", "5", 0},
	{"HT_AMSDU", "0", 0},
	{"HT_GI", "1", 0},
	{"HT_BAWinSize", "64", 0},
	{"HT_MCS", "33", 0},
	{"HT_BADecline", "0", 0},
	{"HT_TxStream", "2", 0},
	{"HT_RxStream", "2", 0},
	{"HT_STBC", "1", 0},
	{"HT_PROTECT", "1", 0},
	{"HT_MIMOPSMode", "3", 0},
	{"HT_40MHZ_INTOLERANT", "0", 0},

	{"McastPhyMode", "0", 0},
	{"McastMcs", "0", 0},

	{"GreenAP", "1", 0},

/*
	{"WscConfMode", "0", 0},
	{"WscConfStatus", "2", 0},
	{"WscAKMP", "1", 0},
	{"WscConfigured", "0", 0},
	{"WscModeOption", "0", 0},
	{"WscActionIndex", "9", 0},
	{"WscPinCode", "", 0},
	{"WscRegResult", "1", 0},
	{"WscUseUPnP", "1", 0},
	{"WscUseUFD", "0", 0},
	{"WscSSID", "RalinkInitialAP", 0},
	{"WscKeyMGMT", "WPA-EAP", 0},
	{"WscConfigMethod", "138", 0},
	{"WscAuthType", "1", 0},
	{"WscEncrypType", "1", 0},
	{"WscNewKey", "scaptest", 0},
*/

//	{"WmmCapable", "1", 0},
	{"APAifsn", "3;7;1;1", 0},
	{"APCwmin", "4;4;3;2", 0},
	{"APCwmax", "6;10;4;3", 0},
	{"APTxop", "0;0;94;47", 0},
	{"APACM", "0;0;0;0", 0},
	{"BSSAifsn", "3;7;2;2", 0},
	{"BSSCwmin", "4;4;3;2", 0},
	{"BSSCwmax", "10;10;4;3", 0},
	{"BSSTxop", "0;0;94;47", 0},
	{"BSSACM", "0;0;0;0", 0},
//	{"AckPolicy", "0;0;0;0", 0},
	{"APSDCapable", "0", 0},
	{"DLSCapable", "0", 0},

	{"IEEE80211H", "0", 0},
	{"CSPeriod", "6", 0},
	{"RDRegion", "0", 0},
	{"WirelessEvent", "0", 0},

//	{"AuthMode", "OPEN", 0},
//	{"EncrypType", "NONE", 0},
//	{"DefaultKeyID", "1", 0},
//	{"Key1Str", "", 0},
//	{"Key2Str", "", 0},
//	{"Key3Str", "", 0},
//	{"Key4Str", "", 0},
//	{"Key1Type", "0", 0},	
//	{"Key2Type", "0", 0},
//	{"Key3Type", "0", 0},
//	{"Key4Type", "0", 0},
//	{"WPAPSK", "12345678", 0},
	{"RekeyMethod", "TIME", 0},
//	{"RekeyInterval", "86400", 0},
	{"PMKCachePeriod", "10", 0},
	{"PreAuth", "0", 0},

//	{"WdsEnable", "0", 0},
	{"WdsPhyMode", "0"},		//0:CCK, 1:OFDM, 2:HTMIX, 3:GREENFIELD
//	{"WdsEncrypType", "NONE", 0},	//NONE, WEP, TKIP, AES
//	{"WdsList", "", 0},
//	{"WdsKey", "", 0},

//	{"IEEE8021X", "0", 0},
//	{"RADIUS_Server", "", 0},
//	{"RADIUS_Port", "1812", 0},
//	{"RADIUS_Key", "", 0},
	{"RADIUS_Acct_Server", "", 0},
	{"RADIUS_Acct_Port", "1813", 0},
	{"RADIUS_Acct_Key", "", 0},
	{"EAPifname", "br0", 0},
	{"PreAuthifname", "br0", 0},
	{"session_timeout_interval", "0", 0},
	{"idle_timeout_interval", "0", 0},

	{"IgmpSnEnable", "1", 0},

	/* OS parameters */
	{ "os_name", "", 0 },			/* OS name string */
//	{ "os_version", EPI_VERSION_STR, 0 },	/* OS revision */
//	{ "os_date", __DATE__, 0 },		/* OS date */

	/* Miscellaneous parameters */
	{ "timer_interval", "3600", 0 },	/* Timer interval in seconds */
	{ "ntp_server", "192.5.41.40 192.5.41.41 133.100.9.2", 0 },		/* NTP server */
	{ "time_zone", "PST8PDT", 0 },		/* Time zone (GNU TZ format) */
	{ "log_level", "0", 0 },		/* Bitmask 0:off 1:denied 2:accepted */
	{ "upnp_enable", "0", 0 },		/* Start UPnP */
	{ "ezc_enable", "1", 0 },		/* Enable EZConfig updates */
//	{ "ezc_version", EZC_VERSION_STR, 0 },	/* EZConfig version */
	{ "is_default", "1", 0 },		/* is it default setting: 1:yes 0:no */
	{ "os_server", "", 0 },			/* URL for getting upgrades */
	{ "stats_server", "", 0 },		/* URL for posting stats */
	{ "console_loglevel", "1", 0 },		/* Kernel panics only */

	/* Big switches */
	{ "router_disable", "0", 0 },		/* lan_proto=static lan_stp=0 wan_proto=disabled */
	{ "ure_disable", "1", 0 },		/* sets APSTA for radio and puts wirelesss interfaces in correct lan */
	{ "fw_disable", "0", 0 },		/* Disable firewall (allow new connections from the WAN) */
	//{ "fw_disable", "1", 0 },		/* Disable firewall (allow new connections from the WAN) */	// win7 logo
	{ "log_ipaddr", "", 0 },		/* syslog recipient */

	/* LAN H/W parameters */
	{ "lan_ifname", "br0", 0 },		/* LAN interface name */
	{ "lan_ifnames", "br0", 0 },		/* Enslaved LAN interfaces */
	{ "lan_hwnames", "", 0 },		/* LAN driver names (e.g. et0) */
	{ "lan_hwaddr", "", 0 },		/* LAN interface MAC address */

	/* LAN TCP/IP parameters */
	{ "lan_dhcp", "0", 0 },			/* DHCP client [static|dhcp] */
	{ "lan_ipaddr", "192.168.1.1", 0 },	/* LAN IP address */
	//{ "lan_ipaddr", "192.168.0.1", 0 },	/* LAN IP address */		// for win7 logo test
	{ "lan_netmask", "255.255.255.0", 0 },	/* LAN netmask */
	{ "lan_gateway", "192.168.1.1", 0 },	/* LAN gateway */
	//{ "lan_gateway", "192.168.0.1", 0 },	/* LAN gateway */		// for win7 logo test
	{ "lan_proto", "dhcp", 0 },		/* DHCP server [static|dhcp] */
	{ "lan_wins", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "lan_domain", "", 0 },		/* LAN domain name */
	{ "lan_lease", "86400", 0 },		/* LAN lease time in seconds */
	{ "lan_stp", "1", 0 },			/* LAN spanning tree protocol */
	{ "lan_route", "", 0 },			/* Static routes (ipaddr:netmask:gateway:metric:ifname ...) */

//#ifdef __CONFIG_NAT__
	/* WAN H/W parameters */
	{ "wan_ifname", "eth2.2", 0 },		/* WAN interface name */
	{ "wan_ifnames", "eth2.2", 0 },		/* WAN interface names */
	{ "wan_hwname", "", 0 },		/* WAN driver name (e.g. et1) */
	{ "wan_hwaddr", "", 0 },		/* WAN interface MAC address */
	{ "cur_hwaddr", "", 0 },		/* eth MAC address */

	/* WAN TCP/IP parameters */
	{ "wan_proto", "dhcp", 0 },		/* [static|dhcp|pppoe|disabled] */
	//{ "wan_proto", "static", 0 },		/* [static|dhcp|pppoe|disabled] */	// win7 logo
	{ "wan_ipaddr", "0.0.0.0", 0 },	/* WAN IP address */
	{ "wan_netmask", "0.0.0.0", 0 },	/* WAN netmask */
	{ "wan_gateway", "0.0.0.0", 0 },	/* WAN gateway */
	//{ "wan_ipaddr", "17.1.1.1", 0 },	/* WAN IP address */	// win7 logo
	//{ "wan_netmask", "255.255.255.0", 0 },	/* WAN netmask */	// win7 logo
	//{ "wan_gateway", "17.1.1.1", 0 },	/* WAN gateway */	// win7 logo
	{ "wan_dns", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "wan_wins", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "wan_hostname", "", 0 },		/* WAN hostname */
	{ "wan_domain", "", 0 },		/* WAN domain name */
	{ "wan_lease", "86400", 0 },		/* WAN lease time in seconds */

	/* PPPoE parameters */
	{ "wan_pppoe_ifname", "ppp0", 0 },		/* PPPoE enslaved interface */
	{ "wan_pppoe_username", "", 0 },	/* PPP username */
	{ "wan_pppoe_passwd", "", 0 },		/* PPP password */
	//{ "wan_pppoe_idletime", "60", 0 },	/* Dial on demand max idle time (seconds) */
	{ "wan_pppoe_idletime", "0", 0 },	// oleg patch
	{ "wan_pppoe_keepalive", "0", 0 },	/* Restore link automatically */
	{ "wan_pppoe_demand", "0", 0 },		/* Dial on demand */
	{ "wan_pppoe_mru", "1492", 0 },		/* Negotiate MRU to this value */
	{ "wan_pppoe_mtu", "1492", 0 },		/* Negotiate MTU to the smaller of this value or the peer MRU */
	{ "wan_pppoe_service", "", 0 },		/* PPPoE service name */
	{ "wan_pppoe_ac", "", 0 },		/* PPPoE access concentrator name */

	/* Misc WAN parameters */
	{ "wan_desc", "", 0 },			/* WAN connection description */
	{ "wan_route", "", 0 },			/* Static routes (ipaddr:netmask:gateway:metric:ifname ...) */
	{ "wan_primary", "0", 0 },		/* Primary wan connection */
	{ "wan_unit", "0", 0 },			/* Last configured connection */

	/* Filters */
	{ "filter_maclist", "", 0 },		/* xx:xx:xx:xx:xx:xx ... */
	{ "filter_macmode", "deny", 0 },	/* "allow" only, "deny" only, or "disabled" (allow all) */
	{ "filter_client0", "", 0 },		/* [lan_ipaddr0-lan_ipaddr1|*]:lan_port0-lan_port1,
						 * proto,enable,day_start-day_end,sec_start-sec_end,desc */

	/* Port forwards */
	{ "dmz_ipaddr", "", 0 },		/* x.x.x.x (equivalent to 0-60999>dmz_ipaddr: 0-60999) */
	{ "forward_port0", "", 0 },		/* wan_port0-wan_port1>lan_ipaddr: lan_port0-lan_port1[:,]proto[:,]enable[:,]desc */
	{ "autofw_port0", "", 0 },		/* out_proto:out_port,in_proto:in_port0-in_port1>to_port0-to_port1,enable,desc */

	/* DHCP server parameters */
	{ "dhcp_start", "192.168.1.2", 0 },	/* First assignable DHCP address */
	//{ "dhcp_start", "192.168.0.2", 0 },	/* First assignable DHCP address */	// for win7 logo test
	{ "dhcp_end", "192.168.1.254", 0 },	/* Last assignable DHCP address */	
	//{ "dhcp_end", "192.168.0.254", 0 },	/* Last assignable DHCP address */	// for win7 logo test
	{ "dhcp_domain", "wan", 0 },		/* Use WAN domain name first if available (wan|lan) */
	{ "dhcp_wins", "wan", 0 },		/* Use WAN WINS first if available (wan|lan) */
//#endif	/* __CONFIG_NAT__ */

	/* Web server parameters */
	{ "http_username", "", 0 },		/* Username */
	{ "http_passwd", "admin", 0 },		/* Password */
	{ "http_wanport", "", 0 },		/* WAN port to listen on */
	{ "http_lanport", "80", 0 },		/* LAN port to listen on */

	/* Wireless parameters */
#if 0	
	{ "wl_ifname", "", 0 },			/* Interface name */
	{ "wl_hwaddr", "", 0 },			/* MAC address */
	{ "wl_phytype", "b", 0 },		/* Current wireless band ("a" (5 GHz), "b" (2.4 GHz), or "g" (2.4 GHz)) */
	{ "wl_phytypes", "", 0 },		/* List of supported wireless bands (e.g. "ga") */
	{ "wl_corerev", "", 0 },		/* Current core revision */
	{ "wl_radioids", "", 0 },		/* List of radio IDs */
	{ "wl_ssid", "Broadcom", 0 },		/* Service set ID (network name) */
	{ "wl_antdiv", "-1", 0 },		/* Antenna Diversity (-1|0|1|3) */
	{ "wl_infra", "1", 0 },			/* Network Type (BSS/IBSS) */
	{ "wl_nbw", "20", 0},			/* N-BW */
	{ "wl_nctrlsb", "none", 0},		/* N-CTRL SB */
	{ "wl_nband", "2", 0},			/* N-BAND */
	{ "wl_nmcsidx", "-1", 0},		/* MCS Index for N - rate */
	{ "wl_nmode", "-1", 0},			/* N-mode */
	{ "wl_nreqd", "0", 0},			/* Require 802.11n support */
	{ "wl_vlan_prio_mode", "off", 0},	/* VLAN Priority support */
	{ "wl_leddc", "0x640000", 0},		/* 100% duty cycle for LED on router */
	{ "wl_afterburner", "off", 0 },		/* AfterBurner */
	{ "wl_frameburst", "off", 0 },		/* BRCM Frambursting mode (off|on) */
	{ "wl_maxassoc", "128", 0},		/* Max associations driver could support */
	{ "wl_unit", "0", 0 },			/* Last configured interface */
	{ "wl_sta_retry_time", "5", 0 },	/* Seconds between association attempts */	
#endif

	{ "wl_country_code", "", 0 },		/* Country Code (default obtained from driver) */
	{ "wl_ssid", "EVE", 0},
	{ "wl_gmode", "2", 0 },			/* 54g mode */
	{ "wl_channel", "0", 0 },		/* Channel number */
	{ "wl_rateset", "default", 0 },		/* "default" or "all" or "12" */
	{ "wl_bcn", "100", 0 },			/* Beacon interval */
	{ "wl_dtim", "1", 0 },			/* DTIM period */
	{ "wl_gmode_protection", "auto", 0 },	/* 802.11g RTS/CTS protection (off|auto) */
	{ "wl_rts", "2347", 0 },		/* RTS threshold */
	{ "wl_frag", "2346", 0 },		/* Fragmentation threshold */
	{ "wl_ap_isolate", "0", 0 },            /* AP isolate mode */
	{ "wl_closed", "0", 0 },		/* Closed (hidden) network */
	{ "wl_macmode", "disabled", 0 },	/* "allow" only, "deny" only, or "disabled"(allow all) */
	{ "wl_maclist", "", 0 },		/* xx:xx:xx:xx:xx:xx ... */
	{ "wl_wme", "1", 0 },			/* WME mode (off|on) */
	{ "wl_wme_no_ack", "off", 0},           /* WME No-Acknowledgment mode */
	{ "wl_auth_mode", "open", 0 },		/* Network authentication mode Open System */
	{ "wl_key", "1", 0 },			/* Current WEP key */
	{ "wl_key1", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key2", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key3", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key4", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key_type", "0", 0 } ,		/* WEP key format (HEX/ASCII)*/
	{ "wl_mrate", "0", 0 },			/* Mcast Rate (bps, 0 for auto) */

	/* WPA parameters */
	{ "wl_crypto", "tkip", 0 },		/* WPA data encryption */
	{ "wl_wpa_psk", "12345678", 0 },	/* WPA pre-shared key */
	{ "wl_wpa_gtk_rekey", "0", 0 },		/* GTK rotation interval */

	{ "wl_radius_ipaddr", "", 0 },		/* RADIUS server IP address */
	{ "wl_radius_port", "1812", 0 },	/* RADIUS server UDP port */
	{ "wl_radius_key", "", 0 },		/* RADIUS shared secret */

	{ "wl_lazywds", "0", 0 },		/* Enable "lazy" WDS mode (0|1) */
        { "ap_scanning", "0", 0 },
        { "updating", "0", 0 },
        { "ap_selecting", "0", 0 },
        { "httpd_check_ddns", "0", 0 },
        { "uploading", "0", 0 },
        { "pppd_way", "2", 0 },
        { "btn_rst", "0", 0 },          	// ate test     
        { "btn_ez", "0", 0 },           	// ate test

        { "wan_stb_x", "0", 0 },           	// oleg patch
        { "wan_pppoe_options_x", "", 0 },	// oleg patch
        { "wan_pptp_options_x", "", 0},		// oleg patch
        { "fw_dos_x", "0", 0 },			// oleg patch
        { "dr_enable_x", "1", 0 },		// oleg patch
        { "mr_enable_x", "0", 0 },		// oleg patch
        { "preferred_lang", "EN", 0 },		// 3.5g 
#if 0
	{ "wl_mode", "ap", 0 },			/* AP mode (ap|sta|wds) */
	{ "wl_auth", "0", 0 },			/* Shared key authentication optional (0) or required (1) */
	{ "wl_net_reauth", "36000", 0 },	/* Network Re-auth/PMK caching duration */
	{ "wl_akm", "", 0 },			/* WPA akm list */
	{ "wl_reg_mode", "off", 0 },		/* Regulatory: 802.11H(h)/802.11D(d)/off(off) */
	{ "wl_dfs_preism", "60", 0 },		/* 802.11H pre network CAC time */
	{ "wl_dfs_postism", "60", 0 },		/* 802.11H In Service Monitoring CAC time */
	{ "wl_wep", "disabled", 0 },		/* WEP data encryption (enabled|disabled) */
	{ "wl_rate", "0", 0 },			/* Rate (bps, 0 for auto) */
	{ "wl_plcphdr", "long", 0 },		/* 802.11b PLCP preamble type */
	{ "wl_radio", "1", 0 },			/* Enable (1) or disable (0) radio */

	/* WME parameters (cwmin cwmax aifsn txop_b txop_ag adm_control oldest_first) */

	/* EDCA parameters for STA */
	{ "wl_wme_sta_be", "15 1023 3 0 0 off off", 0 },	/* WME STA AC_BE parameters */
	{ "wl_wme_sta_bk", "15 1023 7 0 0 off off", 0 },	/* WME STA AC_BK parameters */
	{ "wl_wme_sta_vi", "7 15 2 6016 3008 off off", 0 },	/* WME STA AC_VI parameters */
	{ "wl_wme_sta_vo", "3 7 2 3264 1504 off off", 0 },	/* WME STA AC_VO parameters */

	/* EDCA parameters for AP */
	{ "wl_wme_ap_be", "15 63 3 0 0 off off", 0 },		/* WME AP AC_BE parameters */
	{ "wl_wme_ap_bk", "15 1023 7 0 0 off off", 0 },		/* WME AP AC_BK parameters */
	{ "wl_wme_ap_vi", "7 15 1 6016 3008 off off", 0 },	/* WME AP AC_VI parameters */
	{ "wl_wme_ap_vo", "3 7 1 3264 1504 off off", 0 },	/* WME AP AC_VO parameters */

	{ "wl_wme_apsd", "on", 0},		/* WME APSD mode */
#endif

	#include "./flash.default"
	{ 0, 0, 0 }
};

