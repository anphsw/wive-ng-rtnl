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
 *	inic.h -- Wireless Configuration Header 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: firewall.h,v 1.4 2008-02-25 06:53:02 yy Exp $
 */


#define PROTO_UNKNOWN	0
#define PROTO_TCP		1
#define PROTO_UDP		2
#define PROTO_TCP_UDP	3
#define PROTO_ICMP		4
#define PROTO_NONE		5


#define RULE_MODE_DISABLE	0
#define RULE_MODE_DROP		1
#define RULE_MODE_ACCEPT	2

#define ACTION_DROP		0
#define ACTION_ACCEPT	1

#define WEB_FILTER_CHAIN	"web_filter"
#define IPPORT_FILTER_CHAIN	"macipport_filter"

#define DMZ_CHAIN			"DMZ"
#define PORT_FORWARD_CHAIN	"port_forward"


#define MAGIC_NUM			"IPCHAINS"			

#define HTML_NO_FIREWALL_UNDER_BRIDGE_MODE	\
"<img src=\"/graphics/warning.gif\"><font color=#ff0000>&nbsp; &nbsp;Warning: The current operation mode is \"Bridge mode\" and these settings may not be functional.</font>"

void iptablesFilterClear(void);
void iptablesFilterRun(char *rule);
void iptablesWebsFilterRun(void);
void formDefineFirewall(void);
void firewall_init(void);
