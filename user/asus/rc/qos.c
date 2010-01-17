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
#ifdef QOS
#include<stdio.h>
#include<stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <dirent.h>

#ifdef ASUS_RC
#include<asusbcmnvram.h>
#else
#include <epivers.h>
#include<nvram/bcmnvram.h>
#endif


#include <shutils.h>
FILE *fp=NULL;

/***************************************************************************************/
//  The definitions of all partations
//    10:1 root class for upload
//    10:2 root class for download 
//    10:10 partation for tos(bit 4)	& short pkt 	& psh
//    10:20 partation for tos(bit 3)	& UD(high) 
//    10:30 partation for tos(bit 2)	& application	
//    10:40 partation for ftp on router& UD(middle)
//    10:50 partation for tos(bit 1)
//    10:60 default class
//    10:70 partation for UD(low)
//    10:80 partation only for vlan
/***************************************************************************************/

void tc_class_global(char *network, char *idx, char *minBW, char *maxBW, char *prio_level)
{
	char flowid[32];
        char filter_handle[32];
	char handle[32];

	char pbuf[128];

        sprintf(flowid, "%s%s", "10:", idx);
        sprintf(handle, "%s%s%s", "1", idx, ":");
        sprintf(filter_handle, "%s", idx);

        eval("tc","class","add","dev",network,"parent","10:1","classid",flowid,"htb","rate",\
		minBW,"ceil",maxBW, "prio", prio_level);

#if 0
/* tmp test */
        memset(pbuf, 0, sizeof(pbuf));
        sprintf(pbuf, "echo 'tc class add dev %s parent 10:1 classid %s htb rate %s ceil %s prio %s' >>/tmp/tc_rules", network, flowid, minBW, maxBW, prio_level);
        system(pbuf);
        printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif
        eval("tc","qdisc","add","dev",network,"parent",flowid,"handle",handle,"pfifo");

#if 0
/* tmp test */
        memset(pbuf, 0, sizeof(pbuf));
        sprintf(pbuf, "echo 'tc qdisc add dev %s parent %s handle %s pfifo'>>/tmp/tc_rules", network, flowid, handle);
        system(pbuf);
        printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif
        eval("tc","filter","add","dev",network,"parent","10:", "protocol", "ip", "prio", \
		prio_level,"handle", filter_handle, "fw", "classid", flowid);
#if 0
/* tmp test */
        memset(pbuf, 0, sizeof(pbuf));
        sprintf(pbuf, "echo 'tc filter add dev %s parent 10: protocol ip prio %s handle %s fw classid %s' >>/tmp/tc_rules", network, prio_level, filter_handle, flowid);
        system(pbuf);
        printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif

#ifdef QOS_DEBUG
        syslog(LOG_NOTICE, "tc class add dev %s parent 10:1 classid %s htb rate %s ceil %s prio %s\n",\
		network, flowid, minBW, maxBW, prio_level);
        syslog(LOG_NOTICE, "tc qdisc add dev %s parent %s handle %s pfifo\n", network, flowid, handle);
        syslog(LOG_NOTICE, "tc filter add dev %s parent 10: protocol ip prio %s handle %s fw classid %s\n",\
		network, prio_level, filter_handle, flowid);
	printf("tc class add dev %s parent 10:1 classid %s htb rate %s ceil %s prio %s\n", network,\
		flowid, minBW, maxBW, prio_level);
        printf("tc qdisc add dev %s parent %s handle %s pfifo\n", network, flowid, handle);
        printf("tc filter add dev %s parent 10: protocol ip prio %s handle %s fw classid %s\n", network,\
		prio_level, filter_handle, flowid);
#endif
}

/************************************************************************************/
/* This function is to add the filter match user specify. For LAN IP address, the   */
/* interface should be br0. For WAN IP address, the interface should be eth0/ppp0   */
/************************************************************************************/
char *Ch_conv(char *proto_name, int idx)
{
	char *proto;
	char qos_name_x[32];
	sprintf(qos_name_x, "%s%d", proto_name, idx);
	if (nvram_match(qos_name_x,""))
	{
		return NULL;
	}
	else 
	{
		proto=nvram_get(qos_name_x);
		return proto;
	}
}

void tc_filter(char *ipaddr, char *port, int prio)
{	
	int user_mark;
	user_mark= prio*10;
		
	if (port == NULL)
	{
		fprintf(fp, "-A PREROUTING -s %s -j MARK --set-mark %d\n", ipaddr,user_mark);
	}
	else if (ipaddr == NULL)
	{
		fprintf(fp, "-A PREROUTING -p tcp --dport %s -j MARK --set-mark %d\n", port, user_mark);
		fprintf(fp, "-A PREROUTING -p udp --dport %s -j MARK --set-mark %d\n", port, user_mark);
	}
	else
	{
		fprintf(fp, "-A PREROUTING -s %s -p tcp --dport %s -j MARK --set-mark %d\n", ipaddr, port, user_mark);
		fprintf(fp, "-A PREROUTING -s %s -p udp --dport %s -j MARK --set-mark %d\n",ipaddr, port, user_mark);
	}
}

void start_qos(char *wan_ipaddr)	//modified by angela 2008.05
{
	int rulenum=0;
	int idx_class=0, idx_filter=0, idx_class_S=0; // control index
	int idx=0;
	char net_name[32], net_down_name[32];
	char qos_name[32];
	int ubw=0, service_ubw=0;
	char qos_ubw[32],qos_min_ubw[32], qos_data_ubw[32], qos_down_ubw[32],qos_down[32];
	char dfragment_size[32];
	int flag=0;
	char pbuf[128];
	int class_num;
#undef GPQOS

	printf("start qos\n");	// tmp test

	nvram_set("qos_XXX", "1");
	rulenum=atoi(nvram_safe_get("qos_rulenum_x"));
	
	//Open file for writing iptables rules
	if ((fp=fopen("/tmp/mangle_rules", "w"))==NULL) return;
	fprintf(fp, "*mangle\n");
	
	/* Get interface name */
	if (nvram_match("wan0_proto", "pppoe") || nvram_match("wan0_proto","pptp"))
		strcpy (net_name, nvram_safe_get("wan0_pppoe_ifname"));
	else	
		strcpy (net_name, nvram_safe_get("wan0_ifname"));
		
	/* Add new qdisc for wan classify */
	#if 0
	if(nvram_match("qos_pshack_prio", "1")||nvram_match("qos_tos_prio", "1"))
		eval("tc","qdisc","add", "dev", net_name, "root", "handle", "10:",\
			 "htb", "default", "10");
	else 
		eval("tc","qdisc","add", "dev", net_name, "root", "handle", "10:",\
			 "htb", "default", "50");
	#endif
	eval("tc","qdisc","add", "dev", net_name, "root", "handle", "10:",\
		 "htb", "default", "51");
#if 0
/* tmp test */
	memset(pbuf, 0, sizeof(pbuf));
	sprintf(pbuf, "echo 'tc qdisc add dev %s root handle 10: htb default 51' >>/tmp/tc_rules", net_name);
	system(pbuf);
	printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif

#if 0
	#ifdef GPQOS	
	printf("\n\n! GPQOS !\n\n");	// tmp test
	// Add root class 10:2 and BW = unlimited 
	eval("tc", "class", "add", "dev", net_name, "parent", "10:", "classid", "10:2",\
		 "htb", "rate", "100000kbit", "ceil", "100000kbit");

	// Add class 10:80 for local web access 
       	char lan_ipaddr[32];
        sprintf(lan_ipaddr, "%s%s", nvram_get("lan_ipaddr"), "/24");

	eval("tc", "class", "add", "dev", net_name, "parent", "10:2", "classid", "10:80",\
		 "htb", "rate", "90000kbit", "ceil", "90000kbit", "prio", "1");

	eval("tc", "qdisc", "add", "dev", net_name, "parent", "10:80", "handle", "180:", "pfifo");
		
	eval("tc", "filter", "add", "dev", net_name, "protocol", "ip", "parent", "10:", "prio", "1" ,\
		"u32", "match", "ip", "dst", lan_ipaddr, "flowid", "10:80");

        #endif
#endif
	/*Decide all the max/min=qos_ubw/qos_ubw bandwidth for each classes */		
	class_num = 2;	// 50,51
	if(strcmp(nvram_safe_get("qos_pshack_prio"), "1"))
		++class_num;
	if(strcmp(nvram_safe_get("qos_shortpkt_prio"), "1"))
		++class_num;
	if(strcmp(nvram_safe_get("qos_service_enable"), "1"))
		++class_num;
	printf("the setting class num is %d\n", class_num);	// tmp test

	ubw = atoi(nvram_safe_get("qos_ubw"));
	//ubw=0.85*ubw;
	if(ubw < 100)
		ubw = 200;
	sprintf(qos_ubw, "%d%s", ubw, "kbit");
	//sprintf(qos_min_ubw,"1bps");
	sprintf(qos_min_ubw,"%dkbit", (ubw/class_num));		// for r2q
	printf("qos rate will be set as %d\n", qos_min_ubw);	// tmp test

	/* Add class for global settings / user specify / server services */
	/* Add root class 10:1 and BW = qos_ubw */
	eval("tc", "class", "add", "dev", net_name, "parent", "10:", "classid", "10:1",\
		 "htb", "rate", qos_ubw, "ceil", qos_ubw);		
#if 0
/* tmp test */
        //memset(pbuf, 0, sizeof(pbuf));
        //sprintf(pbuf, "echo 'tc class add dev %s parent 10: classid 10:1 htb rate %s ceil %s' >>/tmp/tc_rules", net_name, qos_ubw, qos_ubw);
        //system(pbuf);
	//printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif
	// Add class for GAME or VOIP 
	if(nvram_match("qos_pshack_prio", "1")||nvram_match("qos_tos_prio", "1")) {
		tc_class_global(net_name, "10", qos_min_ubw, qos_ubw, "2");
		flag |= 0x01;
	}
	
	// Add class for Application 
	if(nvram_match("qos_shortpkt_prio", "1")) {
		tc_class_global(net_name, "20", qos_min_ubw, qos_ubw, "3");
#if 0
	//#ifdef GPQOS
		printf("\n\n! ## GPQOS ## ! \n\n");	// tmp test
		//for download of WL500gpv2
		eval("tc", "class", "add", "dev", net_name, "parent", "10:2", "classid", "10:70",\
		 	"htb", "rate", "90000kbit", "ceil", "90000kbit", "prio", "8" );

		eval("tc", "qdisc", "add", "dev", net_name, "parent", "10:70", "handle", "170:", "pfifo");

		eval("tc", "filter", "add", "dev", net_name, "parent", "10:", "protocol", "ip", "prio", "8", \
		 	"handle", "70", "fw", "classid", "10:70");
			
		eval("tc", "filter", "del", "dev", net_name, "protocol", "ip", "parent", "10:", "prio", "1" ,\
			"u32", "match", "ip", "dst", lan_ipaddr, "flowid", "10:80");

		 eval("tc", "filter", "add", "dev", net_name, "parent", "10:", "protocol", "ip", "prio", "1",\
			"handle", "80", "fw", "classid", "10:80");
		
		if(atoi(nvram_get("qos_ubw"))>640)		 
			sprintf(qos_down,"1Mbit");
		else
			sprintf(qos_down,"256kbit");

		eval("tc", "class", "add", "dev", net_name, "parent", "10:2", "classid", "10:90",\
		 	"htb", "rate", qos_down, "ceil", qos_down, "prio", "9" );
		eval("tc", "qdisc", "add", "dev", net_name, "parent", "10:90", "handle", "190:", "pfifo");

		eval("tc", "filter", "add", "dev", net_name, "parent", "10:", "protocol", "ip", "prio", "9", \
		 	"handle", "90", "fw", "classid", "10:90");
#endif
	//#else
		// for download of WL500W
		sprintf(net_down_name,"br0");
		sprintf(qos_down_ubw, "100000kbit");
		eval ("tc","qdisc","del", "dev", "br0","root","2>/dev/null");

#if 0
/* tmp test */
        memset(pbuf, 0, sizeof(pbuf));
        sprintf(pbuf, "echo 'tc qdisc del dev br0 root' >>/tmp/tc_rules");
        system(pbuf);
	printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif
		eval("tc","qdisc","add", "dev", net_down_name, "root", "handle", "10:",\
		 	"htb", "default", "91"); 

#if 0
/* tmp test */
        memset(pbuf, 0, sizeof(pbuf));
        sprintf(pbuf, "echo 'tc qdisc add dev %s root handle 10: htb default 91' >>/tmp/tc_rules", net_down_name);
        system(pbuf);
	printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif

		eval("tc",  "class", "add", "dev", net_down_name, "parent", "10:", "classid", "10:1",\
		 	"htb", "rate", qos_down_ubw, "ceil", qos_down_ubw);
#if 0
/* tmp test */
        memset(pbuf, 0, sizeof(pbuf));
        sprintf(pbuf, "echo 'tc class add dev %s parent 10: classid 10:1 htb rate %s ceil %s' >>/tmp/tc_rules", net_down_name, qos_down_ubw, qos_down_ubw);
        system(pbuf);
	printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif
		if(atoi(nvram_get("qos_ubw"))>640)		 
			sprintf(qos_down,"1Mbit");
		else
			sprintf(qos_down,"256kbit");

		tc_class_global(net_down_name, "70", qos_min_ubw, qos_down_ubw, "8");
		tc_class_global(net_down_name, "80", qos_min_ubw, qos_down_ubw, "1");
		tc_class_global(net_down_name, "90", qos_min_ubw, qos_down, "9");
		tc_class_global(net_down_name, "91", qos_min_ubw, qos_down, "9");
	//#endif
	
		//Add iptables rule of Applications
		fprintf(fp, "-A POSTROUTING -p tcp -m tcp --dport 80 -j MARK --set-mark 20\n");
		fprintf(fp, "-A POSTROUTING -p tcp -m tcp --dport 80 -j RETURN\n");
		fprintf(fp, "-A POSTROUTING -p tcp -m tcp --dport 8080 -j MARK --set-mark 20\n");
		fprintf(fp, "-A POSTROUTING -p tcp -m tcp --dport 8080 -j RETURN\n");
		fprintf(fp, "-A POSTROUTING -p tcp -m tcp --dport 443 -j MARK --set-mark 20\n");
		fprintf(fp, "-A POSTROUTING -p tcp -m tcp --dport 443 -j RETURN\n");
		fprintf(fp, "-A POSTROUTING -p tcp -m tcp --dport 25 -j MARK --set-mark 20\n");
		fprintf(fp, "-A POSTROUTING -p tcp -m tcp --dport 25 -j RETURN\n");
	#if 0
		fprintf(fp, "-A POSTROUTING -p udp --dport 5060 -j MARK --set-mark 20\n");
		fprintf(fp, "-A POSTROUTING -p udp --dport 5060 -j RETURN\n");
		fprintf(fp, "-A POSTROUTING -p udp --dport 6000 -j MARK --set-mark 20\n");
		fprintf(fp, "-A POSTROUTING -p udp --dport 6000 -j RETURN\n");
		fprintf(fp, "-A POSTROUTING -p tcp --dport 32000 -j MARK --set-mark 20\n");
		fprintf(fp, "-A POSTROUTING -p tcp --dport 32000 -j RETURN\n");
		fprintf(fp, "-A POSTROUTING -p udp --dport 32000 -j MARK --set-mark 20\n");
		fprintf(fp, "-A POSTROUTING -p udp --dport 32000 -j RETURN\n");
		fprintf(fp, "-A POSTROUTING -p tcp --dport 32010 -j MARK --set-mark 20\n");
		fprintf(fp, "-A POSTROUTING -p tcp --dport 32010 -j RETURN\n");
		fprintf(fp, "-A POSTROUTING -p udp -m mport --dport 16384:16583 -j MARK --set-mark 20\n");
		fprintf(fp, "-A POSTROUTING -p udp -m mport --dport 16384:16583 -j RETURN\n");
	#endif
	}
	
	// Add class for services such as ftp on router 10:30 priority 4
	if (nvram_match("qos_service_enable", "1")) {
		tc_class_global(net_name, "30", qos_min_ubw, qos_ubw, "4");

		//Add iptables rule of ftp
		fprintf(fp, "-A POSTROUTING -m helper --helper ftp -j MARK --set-mark 30\n"); 
		fprintf(fp, "-A POSTROUTING -m helper --helper ftp -j RETURN\n"); 
	}
			
	/* Add class for user specify, 10:20(high), 10:40(middle), 10:60(low)*/
	if (nvram_match("qos_userspec_app","1")) {
		for(idx_class=0; idx_class < rulenum; idx_class++) {
			/*tc_class(net_name, num_conv(idx_class),\
				 qos_userspec_ubw, qos_other_max, Ch_conv("qos_prio_x", idx_class));*/
			switch(atoi(Ch_conv("qos_prio_x", idx_class))) {
				case 1:
					if((flag & 0x01)==0) {						
						tc_class_global(net_name, "10", qos_min_ubw, qos_ubw, "2");
						flag |= 0x01;
					}
					break;
				case 4:
					if((flag & 0x02)==0) {
						tc_class_global(net_name, "40", qos_min_ubw, qos_ubw, "5");
						flag |= 0x02;
					}
					break;
				case 6:
					if((flag & 0x04)==0) {
						tc_class_global(net_name, "60", qos_min_ubw, qos_ubw, "7");
						flag |= 0x04;
					}
					break;
			}

			tc_filter(Ch_conv("qos_ip_x", idx_class), \
              			Ch_conv("qos_port_x", idx_class), atoi(Ch_conv("qos_prio_x", idx_class)));

		}
	}
		
	/*Add the default class 10:10 */		
	tc_class_global(net_name, "50", qos_min_ubw, qos_ubw, "6");
	//TFA parent ffff: protocol ip handle 50 fw police rate 100kbit mtu 12k burst 10k drop
					
//	eval("tc", "filter", "add", "dev", net_name, "parent", "ffff:", "protocol", "ip", \
	 	"handle", "50", "fw", "police", "rate", "10kbit", "mtu", "2k", "burst", "10k", "drop");


	eval("tc", "class", "add", "dev", net_name, "parent", "10:1", "classid", "10:51",\
		 "htb", "rate", qos_min_ubw, "ceil", qos_ubw, "prio", "6");
#if 0
/* tmp test */
        memset(pbuf, 0, sizeof(pbuf));
        sprintf(pbuf, "echo 'tc class add dev %s parent 10:1 classid 10:51 htb rate %s ceil %s prio 6' >>/tmp/tc_rules", net_name, qos_min_ubw, qos_ubw);
        system(pbuf);
	printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif

	eval("tc", "qdisc", "add", "dev", net_name, "parent", "10:51", "handle", "151:", "pfifo");

#if 0
/* tmp test */
        memset(pbuf, 0, sizeof(pbuf));
        sprintf(pbuf, "echo 'tc qdisc add dev %s parent 10:51 handle 151: pfifo' >>/tmp/tc_rules", net_name);
        system(pbuf);
	printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif

//	eval("tc", "filter", "add", "dev", net_name, "parent", "10:", "protocol", "ip", "prio", "1",\
			"handle", "50", "fw", "classid", "10:50");

	eval("tc", "filter", "add", "dev", net_name, "parent", "10:", "protocol", "ip", \
	 	"handle", "51", "fw", "police", "rate", "10kbit", "mtu", "10k", "burst", "1k", "drop");
#if 0
/* tmp test */
        memset(pbuf, 0, sizeof(pbuf));
        sprintf(pbuf, "echo 'tc filter add dev %s parent 10: protocol ip handle 51 fw police rate 10kbit mtu 10k burst 1k drop' >>/tmp/tc_rules", net_name);
        system(pbuf);
	printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif		
	/*Enable Dynamic Fragmentation function*/
	//536 =  standard MTU(576) - IP header(20) - TCP header(20)
	//1200 = suggested trasmmit packet size
	if( nvram_match("qos_dfragment_enable", "1")) {
		//int mss_size = 1200;
		//float pct = atoi(nvram_get("qos_dfragment_size"));
		//pct = pct/100;
		//mss_size = 536 + (1200-536)*pct;
		
		//packet size % 4 Must equl 0 (no padding octets)
		//if(mss_size%4)
		//	mss_size = (mss_size/4 +1) *4;
		//sprintf(dfragment_size, "%d", mss_size);


		fprintf(fp, "-A POSTROUTING -p tcp --tcp-flags SYN,ACK SYN,ACK \
				-j TCPMSS --set-mss %s\n",nvram_get("qos_dfragment_size"));
	}
		
	/*write "COMMIT" to /tmp/mangle_rules to end after writing all rules*/
	fprintf(fp, "COMMIT\n\n");
	fclose(fp);
		
	/*recover all iptables rules*/
	eval("iptables-restore", "/tmp/mangle_rules");
	printf("start qos end\n");	// tmp test
}
#endif
