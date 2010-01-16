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
 * Copyright 2004, ASUSTek Inc.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND ASUS GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: services_ex.c,v 1.1.1.1 2007/01/25 12:52:21 jiahao_jhou Exp $
 */

#ifdef ASUS_EXT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <sys/mount.h>
#include <nvram/bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <rc.h>
#include <syslog.h>
#include "iboxcom.h"
#include "lp.h"
#include <sys/vfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>

#include <ra3052.h>
#include <rc_event.h>

#ifdef WCN
#include "ILibParsers.h"
#include <wlutils.h>
#endif

#ifdef DLM
#include <asm/page.h>
#include <sys/swap.h>
#include <sys/sysinfo.h>
#endif

#define logs(s) syslog(LOG_NOTICE, s)

char *usb_dev_file = "/proc/bus/usb/devices";

#define USB_CLS_PER_INTERFACE         0       /* for DeviceClass */
#define USB_CLS_AUDIO                 1
#define USB_CLS_COMM                  2
#define USB_CLS_HID                   3
#define USB_CLS_PHYSICAL              5
#define USB_CLS_STILL_IMAGE           6
#define USB_CLS_PRINTER               7
#define USB_CLS_MASS_STORAGE          8
#define USB_CLS_HUB                   9
#define USB_CLS_CDC_DATA              0x0a
#define USB_CLS_CSCID                 0x0b    /* chip+ smart card */
#define USB_CLS_CONTENT_SEC           0x0d    /* content security */
#define USB_CLS_VIDEO                 0x0e
#define USB_CLS_WIRELESS_CONTROLLER   0xe0
#define USB_CLS_MISC                  0xef
#define USB_CLS_APP_SPEC              0xfe
#define USB_CLS_VENDOR_SPEC           0xff
#define USB_CLS_3GDEV                 0x35

#define OP_MOUNT		1
#define OP_UMOUNT		2
#define OP_SETNVRAM		3

#ifdef DLM
/* CRC lookup table */
static unsigned long crcs[256]={ 0x00000000,0x77073096,0xEE0E612C,0x990951BA,
0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,0x0EDB8832,0x79DCB8A4,0xE0D5E91E,
0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,0x1DB71064,0x6AB020F2,
0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,0x136C9856,
0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,
0xA50AB56B,0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,
0xDCD60DCF,0xABD13D59,0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,
0x56B3C423,0xCFBA9599,0xB8BDA50F,0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,
0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,0x76DC4190,0x01DB7106,0x98D220BC,
0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,0x7807C9A2,0x0F00F934,
0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,0x6B6B51F4,
0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,
0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,
0xFBD44C65,0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,
0xA4D1C46D,0xD3D6F4FB,0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,
0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,
0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,0x5EDEF90E,0x29D9C998,0xB0D09822,
0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,0xEDB88320,0x9ABFB3B6,
0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,0xE3630B12,
0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,
0x6E6B06E7,0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,
0x17B7BE43,0x60B08ED5,0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,
0xA6BC5767,0x3FB506DD,0x48B2364B,0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,
0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,0xCB61B38C,0xBC66831A,0x256FD2A0,
0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,0xC5BA3BBE,0xB2BD0B28,
0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,0x9B64C2B0,
0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,
0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,
0x0BDBDF21,0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,
0x6FB077E1,0x18B74777,0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,
0xF862AE69,0x616BFFD3,0x166CCF45,0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,
0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,0xAED16A4A,0xD9D65ADC,0x40DF0B66,
0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,0xBDBDF21C,0xCABAC28A,
0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,0xB3667A2E,
0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D};
#endif

#ifdef USB_SUPPORT
//#define WEBCAM_SUPPORT 1
#define PRINTER_SUPPORT 1
#define MASSSTORAGE_SUPPORT 1
//#define AUDIO_SUPPORT 1

enum
{
        WEB_NONE = 0,
        WEB_PWCWEB,
        WEB_OVWEB,
        WEB_AUDIO,
        USB_PRINTER, //2008.05 James.
        MASS_STORAGE //2008.03.13 Yau add
#ifdef CDMA
        , USB_ACM       // HSDPA
#endif // CDMA
} WEBTYPE;

char *PWCLIST[] = {"471","69a","46d","55d","41e","4cc","d81", NULL};
char *OVLIST[] = {"5a9","813","b62", NULL};

#endif

char buf_g[512];

void init_apps();
void run_ftp();
void exec_apps();
int check_task(char *cmd);
void chk_partitions(int state);
void start_igmpproxy(char *wan_ifname);

int file_to_buf(char *path, char *buf, int len)
{
        FILE *fp;
                                                                                                               
        memset(buf, 0 , len);
                                                                                                               
        if ((fp = fopen(path, "r"))) {
                fgets(buf, len, fp);
                fclose(fp);
                return 1;
        }
                                                                                                               
        return 0;
}

int 
stop_infosvr()
{
	return system("killall infosvr");
}

int 
start_infosvr()
{
        char *infosvr_argv[] = {"/usr/sbin/infosvr", "br0", NULL};
        pid_t pid;

	return _eval(infosvr_argv, NULL, 0, &pid);
}

int
start_8021x()
{
	char *apd_argv[] = {"/bin/rt2860apd", NULL};
	pid_t pid;

	if (nvram_match("wl_auth_mode", "wpa") || 
	nvram_match("wl_auth_mode", "radius") || 
	nvram_match("wl_auth_mode", "wpa2")
	) 
		return _eval(apd_argv, NULL, 0, &pid);
	else
		return 0;
}

int
start_dhcpd(void)
{
	FILE *fp;
	char *dhcpd_argv[] = {"/usr/sbin/udhcpd", "/tmp/udhcpd.conf", NULL, NULL};
	char *slease = "/tmp/udhcpd-br0.sleases";
	char dhcpd_cmd[64];
	pid_t pid;

        //if (nvram_match("router_disable", "1") || nvram_invmatch("lan1_proto", "dhcp"))
        //        return 0;

	if (	nvram_match("sw_mode_ex", "3") ||
		(nvram_match("sw_mode_ex", "1") && nvram_invmatch("lan_proto", "dhcp")))
	{
		fprintf(stderr, "skip running udhcpd...\n");
		return 0;
	}
	else
		fprintf(stderr, "starting...\n");


	dprintf("%s %s %s %s\n",
		nvram_safe_get("lan_ifname"),
		nvram_safe_get("dhcp_start"),
		nvram_safe_get("dhcp_end"),
		//nvram_safe_get("lan_lease"));
		nvram_safe_get("dhcp_lease"));

	if (!(fp = fopen("/tmp/udhcpd-br0.leases", "a"))) {
		perror("/tmp/udhcpd-br0.leases");
		return errno;
	}
	fclose(fp);

	// Write configuration file based on current information
	if (!(fp = fopen("/tmp/udhcpd.conf", "w"))) {
		perror("/tmp/udhcpd.conf");
		return errno;
	}
	
	//fprintf(fp, "pidfile /var/run/udhcpd-br0.pid\n");
	fprintf(fp, "start %s\n", nvram_safe_get("dhcp_start"));
	fprintf(fp, "end %s\n", nvram_safe_get("dhcp_end"));
	//fprintf(fp, "interface %s\n", nvram_safe_get("lan_ifname"));
	fprintf(fp, "interface br0\n");
	fprintf(fp, "remaining yes\n");
	fprintf(fp, "lease_file /tmp/udhcpd-br0.leases\n");
	fprintf(fp, "option subnet %s\n", nvram_safe_get("lan_netmask"));
	
	if (nvram_invmatch("dhcp_gateway_x",""))
	    fprintf(fp, "option router %s\n", nvram_safe_get("dhcp_gateway_x"));	
	else	
	    fprintf(fp, "option router %s\n", nvram_safe_get("lan_ipaddr"));	
	
	if (nvram_invmatch("dhcp_dns1_x",""))		
		fprintf(fp, "option dns %s\n", nvram_safe_get("dhcp_dns1_x"));		
	fprintf(fp, "option dns %s\n", nvram_safe_get("lan_ipaddr"));
	//fprintf(fp, "option lease %s\n", nvram_safe_get("lan_lease"));
	fprintf(fp, "option lease %s\n", nvram_safe_get("dhcp_lease"));

	if (nvram_invmatch("dhcp_wins_x",""))		
		fprintf(fp, "option wins %s\n", nvram_safe_get("dhcp_wins_x"));		
	if (nvram_invmatch("lan_domain", ""))
		fprintf(fp, "option domain %s\n", nvram_safe_get("lan_domain"));
	fclose(fp);

	memset(dhcpd_cmd, 0, sizeof(dhcpd_cmd));

	if (nvram_match("dhcp_static_x","1"))
	{	
		write_static_leases(slease);
		dhcpd_argv[2] = slease;
		sprintf(dhcpd_cmd, "/usr/sbin/udhcpd /tmp/udhcpd.conf /tmp/udhcpd-br0.sleases");	
	}
	else
	{
		dhcpd_argv[2] = NULL;
		sprintf(dhcpd_cmd, "/usr/sbin/udhcpd /tmp/udhcpd.conf");	
	}

	system(dhcpd_cmd);

	//_eval(dhcpd_argv, NULL, 0, &pid);

	//dprintf("done\n");
	return 0;
}

int
stop_dhcpd(void)
{
	int ret;

	ret = eval("killall", "udhcpd");

	dprintf("done\n");
	return ret;
}

int
start_dns(void)
{
	FILE *fp;
	//char *dproxy_argv[] = {"dproxy", "-d", "-c", "/tmp/dproxy.conf", NULL};
	char *dproxy_argv[] = {"dproxy", "-c", "/tmp/dproxy.conf", NULL};
	pid_t pid;
	char word[256], *next;
	int ret, active;
	char *dns_list;

	printf("start dns\n");	// tmp test
	if (nvram_match("router_disable", "1"))
		return 0;

	/* Create resolv.conf with empty nameserver list */
	if (!(fp = fopen("/tmp/resolv.conf", "r")))
	{
		if (!(fp = fopen("/tmp/resolv.conf", "w"))) 
		{
			perror("/tmp/resolv.conf");
			return errno;
		}
		else fclose(fp);
	}
	else fclose(fp);

	if (!(fp = fopen("/tmp/dproxy.conf", "w"))) {
		perror("/tmp/dproxy.conf");
		return errno;
	}


//	fprintf(fp, "name_server=140.113.1.1\n");
	fprintf(fp, "ppp_detect=no\n");
	fprintf(fp, "purge_time=1200\n");
	fprintf(fp, "deny_file=/tmp/dproxy.deny\n");
	fprintf(fp, "cache_file=/tmp/dproxy.cache\n");
	fprintf(fp, "hosts_file=/tmp/hosts\n");
	fprintf(fp, "dhcp_lease_file=\n");
	fprintf(fp, "ppp_dev=/var/run/ppp0.pid\n");
	fclose(fp);

	// if user want to set dns server by himself
	if (nvram_invmatch("wan_dnsenable_x", "1") || nvram_match("wan0_proto", "static"))
	{
		printf("set dns by user\n");	// tmp test
		/* Write resolv.conf with upstream nameservers */
		if (!(fp = fopen("/tmp/resolv.conf", "w"))) {
			perror("/tmp/resolv.conf");
			return errno;
		}
	
		if (nvram_invmatch("wan_dns1_x",""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("wan_dns1_x"));		
		if (nvram_invmatch("wan_dns2_x",""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("wan_dns2_x"));
		fclose(fp);
		/* also /etc/resolv.conf */
                if (!(fp = fopen("/etc/resolv.conf", "w"))) {
                        perror("/etc/resolv.conf");
                        return errno;
                }

                if (nvram_invmatch("wan_dns1_x",""))
                        fprintf(fp, "nameserver %s\n", nvram_safe_get("wan_dns1_x"));
                if (nvram_invmatch("wan_dns2_x",""))
                        fprintf(fp, "nameserver %s\n", nvram_safe_get("wan_dns2_x"));
                fclose(fp);
	}

	dns_list = ((nvram_safe_get("wan0_dns") ? : nvram_safe_get("wanx_dns")));
	if(strlen(dns_list) > 0)	// chk, should not has this case
	{
		printf("\n## auto dns list is ready\n");	// tmp test
        	if (!(fp = fopen("/tmp/resolv.conf", "w+"))) {
                	perror("/tmp/resolv.conf");
                	return errno;
        	}

        	foreach(word, (nvram_safe_get("wan0_dns") ? :
                	nvram_safe_get("wanx_dns")), next)
        	{
                	fprintf(fp, "nameserver %s\n", word);
        	}
        	fclose(fp);

        	/* write also /etc/resolv.conf */
        	if (!(fp = fopen("/etc/resolv.conf", "w+"))) {
                	perror("/etc/resolv.conf");
                	return errno;
        	}

        	foreach(word, (nvram_safe_get("wan0_dns") ? :
                	nvram_safe_get("wanx_dns")), next)
        	{
                	fprintf(fp, "nameserver %s\n", word);
        	}
        	fclose(fp);
	}

	active = timecheck_item(nvram_safe_get("url_date_x"), nvram_safe_get("url_time_x"));

	if (nvram_match("url_enable_x", "1") && active) 
	{
		int i;
		
		if (!(fp = fopen("/tmp/dproxy.deny", "w"))) {
			perror("/tmp/dproxy.deny");
			return errno;
		}

		for(i=0; i<atoi(nvram_safe_get("url_num_x")); i++) {
			sprintf(word, "url_keyword_x%d", i);
			fprintf(fp, "%s\n", nvram_safe_get(word));
		}
	
		fclose(fp);	
	}
	else{
		unlink("/tmp/dproxy.deny");
	}

	if (!(fp = fopen("/tmp/hosts", "w"))) {
		perror("/tmp/hosts");
		return errno;
	}

	fprintf(fp, "127.0.0.1 localhost.localdomain localhost\n");
	fprintf(fp, "%s	my.router\n", nvram_safe_get("lan_ipaddr"));
	fprintf(fp, "%s	my.%s\n", nvram_safe_get("lan_ipaddr"), nvram_safe_get("productid"));
#ifdef WL330GE
	//fprintf(fp, "%s	my.WL330gE\n", nvram_safe_get("lan_ipaddr"));
	//fprintf(fp, "%s	my.330gE\n", nvram_safe_get("lan_ipaddr"));
#endif
	if (nvram_invmatch("lan_hostname", ""))
	{
		fprintf(fp, "%s %s.%s %s\n", nvram_safe_get("lan_ipaddr"),
					nvram_safe_get("lan_hostname"),
					nvram_safe_get("lan_domain"),
					nvram_safe_get("lan_hostname"));
	}	
	fclose(fp);	
		
	//_eval(dproxy_argv, NULL, 0, &pid);
	//_eval(dproxy_argv, NULL, 0, NULL);
	return system("dproxy -c /tmp/dproxy.conf &");

	//return ret;
}	

int
stop_dns(void)
{
	int ret = eval("killall", "dproxy");
	unlink("/tmp/dproxy.deny");		
	return ret;
}

int 
restart_dns()
{
	return system("killall -SIGHUP dproxy");
}

int
ddns_updated_main(int argc, char *argv[])
{
	FILE *fp;
	char buf[64], *ip;

	if (!(fp=fopen("/tmp/ddns.cache", "r"))) return 0;
	
	fgets(buf, sizeof(buf), fp);
	fclose(fp);

	if (!(ip=strchr(buf, ','))) return 0;
	
	nvram_set("ddns_cache", buf);
	nvram_set("ddns_ipaddr", ip+1);
	nvram_set("ddns_status", "1");
	nvram_commit();

	logmessage("ddns", "ddns update ok");

	dprintf("done\n");

	return 0;
}
	

int 
start_ddns(void)
{
	FILE *fp;
	char *wan_ip, *ddns_cache;
	char server[32];
	char user[32];
	char passwd[32];
	char host[64];
	char service[32];
	char usrstr[64];
	char wan_ifname[16];
	int  wild=nvram_match("ddns_wildcard_x", "1");


	if (nvram_match("router_disable", "1")) return -1;
	
	if (nvram_invmatch("ddns_enable_x", "1")) return -1;
	
	if ((wan_ip = nvram_safe_get("wan_ipaddr_t"))==NULL || nvram_match("wan_ipaddr_t", "")) return -1;

	if (nvram_match("ddns_ipaddr", wan_ip))
	{
		logmessage("ddns", "IP address has not changed since the last update");
		return -1;
	}

	if (inet_addr(wan_ip)==inet_addr(nvram_safe_get("ddns_ipaddr")))
	{
		logmessage("ddns", "IP address has not changed since the last update");
		return -1;
	}

	// TODO : Check /tmp/ddns.cache to see current IP in DDNS
	// update when,
	// 	1. if ipaddr!= ipaddr in cache
	// 	
        // update
	// * nvram ddns_cache, the same with /tmp/ddns.cache


	if ((fp=fopen("/tmp/ddns.cache", "r"))==NULL && 
	     (ddns_cache=nvram_safe_get("ddns_cache"))!=NULL)
	{
		if ((fp = fopen("/tmp/ddns.cache", "w+"))!=NULL)
		{
			fprintf(fp, "%s", ddns_cache);
			fclose(fp);
		}
	}

	strcpy(server, nvram_safe_get("ddns_server_x"));
	strcpy(user, nvram_safe_get("ddns_username_x"));
	strcpy(passwd, nvram_safe_get("ddns_passwd_x"));
	strcpy(host, nvram_safe_get("ddns_hostname_x"));
	strcpy(service, "");
			
	if (strcmp(server, "WWW.DYNDNS.ORG")==0)
		strcpy(service, "dyndns");			
	else if (strcmp(server, "WWW.DYNDNS.ORG(CUSTOM)")==0)
		strcpy(service, "dyndns");			
	else if (strcmp(server, "WWW.DYNDNS.ORG(STATIC)")==0)
		strcpy(service, "dyndns");			
	else if (strcmp(server, "WWW.TZO.COM")==0)
		strcpy(service, "tzo");			
	else if (strcmp(server, "WWW.ZONEEDIT.COM")==0)
		strcpy(service, "zoneedit");
	else if (strcmp(server, "WWW.JUSTLINUX.COM")==0)
		strcpy(service, "justlinux");
	else if (strcmp(server, "WWW.EASYDNS.COM")==0)
		strcpy(service, "easydns");
#ifdef ASUS_DDNS //2007.03.20 Yau add
        else if (strcmp(server, "WWW.ASUS.COM")==0)
                strcpy(service, "dyndns");
#endif
	else strcpy(service, "dyndns");
			
	sprintf(usrstr, "%s:%s", user, passwd);
	
#ifdef CDMA // HSDPA
        if(strcmp(nvram_safe_get("hsdpa_product"), "") != 0)
                strcpy(wan_ifname, "ppp0");
        else
#endif
        if (nvram_match("wan_proto", "pppoe") || nvram_match("wan_proto", "pptp") ||
               nvram_match("wan_proto", "l2tp"))	// oleg patch

	{
		strcpy(wan_ifname, nvram_safe_get("wan0_pppoe_ifname"));
	}
	else
	{
		strcpy(wan_ifname, nvram_safe_get("wan0_ifname"));
	}	

	dprintf("wan_ifname: %s\n\n\n\n", wan_ifname);

#ifdef ASUS_DDNS //2007.03.20 Yau add
        if (strcmp(server, "WWW.ASUS.COM")==0)
        {
                char *ddns_argv[] = {"ez-ipupdate",
                "-h", host,
                "-s", "ns1.asuscomm.com",
                "-S", service,
                "-i", wan_ifname,
                "-A", "1",
                NULL};
                pid_t pid;

                dprintf("ddns update %s %s\n", server, service);
                eval("killall", "ez-ipupdate");
                _eval(ddns_argv, NULL, 0, &pid);
        }
        else
#endif //End of Yau add

	if (strlen(service)>0)
	{
		char *ddns_argv[] = {"ez-ipupdate", 
		"-S", service,
	        "-i", wan_ifname,
 		"-u", usrstr,
		"-h", host,
		"-e", "/sbin/ddns_updated",
		"-b", "/tmp/ddns.cache",
		wild ? "-w" : NULL,
		NULL};	
		pid_t pid;

		dprintf("ddns update %s %s\n", server, service);
		nvram_unset("ddns_cache");
		nvram_unset("ddns_ipaddr");
		nvram_unset("ddns_status");

		nvram_set("ddns_updated", "1");
		eval("killall", "ez-ipupdate");
		_eval(ddns_argv, NULL, 0, &pid);
	}
	return 0;
}

int 
stop_ddns(void)
{
	int ret = eval("killall", "ez-ipupdate");

	return ret;
}

int
stop_syslogd()
{
	return system("killall syslogd");
}

int 
stop_klogd()
{
	return system("killall klogd");
}

int 
start_syslogd()
{
	//if (nvram_match("router_disable", "1"))
	//	return 0;

	pid_t pid;

        if (nvram_invmatch("log_ipaddr", ""))
        {
                char *syslogd_argv[] = {"syslogd", "-l", "1", "-O", "/var/log/messages", "-R", nvram_safe_get("log_ipaddr"), "-L", NULL};

                _eval(syslogd_argv, NULL, 0, &pid);
        }
        else
        {
                char *syslogd_argv[] = {"syslogd", "-l", "1", "-O", "/var/log/messages", NULL};

                _eval(syslogd_argv, NULL, 0, &pid);
        }
}

int
start_klogd()
{
	//if (nvram_match("router_disable", "1"))
	//	return 0;

	pid_t pid;

        if (nvram_invmatch("log_ipaddr", ""))
        {
#ifdef KERNEL_DBG
                char *klogd_argv[] = {"/sbin/klogd", "-d", NULL};
#else
                char *klogd_argv[] = {"/sbin/klogd", NULL};
#endif
                _eval(klogd_argv, NULL, 0, &pid);       // 1003 disable
        }
        else
        {
#ifdef KERNEL_DBG
                char *klogd_argv[] = {"/sbin/klogd", "-d", NULL};
#else
                char *klogd_argv[] = {"/sbin/klogd", NULL};
#endif
                _eval(klogd_argv, NULL, 0, &pid);       // 1003 disable
        }
}

int 
start_logger(void)
{		
	start_syslogd();
	start_klogd();

	return 0;
}

int 
start_misc(void)
{ 
	char *infosvr_argv[] = {"infosvr", "br0", NULL};
	char *watchdog_argv[] = {"watchdog", NULL};
	pid_t pid;

	_eval(infosvr_argv, NULL, 0, &pid);
	_eval(watchdog_argv, NULL, 0, &pid);

	return 0;
}

int
stop_misc(void)
{
	int ret1 = eval("killall", "infosvr");
	ret1 = eval("killall", "watchdog");
	ret1 = eval("killall", "ntpclient");   // add for WL-330gE
	ret1 = eval("killall", "ntp");

	//dprintf("done\n");
	return(ret1);
}

int
stop_misc_no_watchdog(void)
{
	int ret1 = eval("killall", "infosvr");
	eval("killall", "ntp");
	eval("killall", "ntpclient");	// add for WL-330gE
	eval("killall", "udhcpc");	// add for WL-330gE

	stop_wsc();
	stop_lltd();	// 1017 add

	dprintf("done\n");
	return(ret1);
}


#ifndef USB_SUPPORT
/*
int start_usb(void)
{
	return 0;
}

int stop_usb(void)
{
	return 0;
}

int hotplug_usb(void)
{
	return 0;
}
*/
#else

void
start_u2ec()
{
#ifdef U2EC
        char *u2ec_argv[] = {"u2ec", NULL};
        pid_t pid_u2ec;
        _eval(u2ec_argv, ">/dev/null", 0, &pid_u2ec);
        start_usdsvr_broadcast();
        start_usdsvr_unicast();
#endif
}

int
stop_u2ec()
{
        return eval("killall", "usdsvr_broadcast");
        return eval("killall", "usdsvr_unicast");
	return eval("killall", "u2ec");
}

int 
start_usb(void)
{
	start_u2ec();

#ifdef PRINTER_SUPPORT
	mkdir("/var/state", 0777);
	mkdir("/var/state/parport", 0777);
	mkdir("/var/state/parport/svr_statue", 0777);
#endif	
	umask(0000);			// added by Jiahao for WL500gP
	mkdir("/tmp/harddisk", 0777);

#ifdef CDMA
	eval("modprobe", "cdc-acm");
#endif
}

int
stop_usb(void)
{
#ifdef U2EC
	eval("killall", "u2ec");
#endif
	remove_usb_mass();

#ifdef CDMA // HSDPA
        eval("rmmod", "cdc-acm");
#endif // CDMA // HSDPA

}

#ifdef DLM
void write_ftpd_conf()
{
	FILE *fp;
	char user[64], user1[64], password[64], path[64];
	char tmpstr[64];
	char rright[384], wright[384], maxuser[16];
	int snum, unum, i, j;
	//char *test_path=(char *)malloc(64);
	char test_path[64];
	char *tmp1=NULL;
	char root_path[64];

	/* write /etc/vsftpd.conf */
	fp=fopen("/tmp/vsftpd.conf", "w");
	if (fp==NULL) return;

        if(nvram_match("st_ftp_mode", "2"))
                fprintf(fp, "anonymous_enable=NO\n");
        else{
                fprintf(fp, "anonymous_enable=YES\n");
                fprintf(fp, "anon_upload_enable=YES\n");
                fprintf(fp, "anon_mkdir_write_enable=YES\n");
                fprintf(fp, "anon_other_write_enable=YES\n");
        }

	fprintf(fp, "nopriv_user=root\n");
	fprintf(fp, "write_enable=YES\n");
	fprintf(fp, "local_enable=YES\n");
	fprintf(fp, "chroot_local_user=YES\n");
	fprintf(fp, "local_umask=000\n");
	fprintf(fp, "dirmessage_enable=NO\n");
	fprintf(fp, "xferlog_enable=NO\n");
	fprintf(fp, "syslog_enable=NO\n");
	fprintf(fp, "connect_from_port_20=YES\n");
	fprintf(fp, "listen=YES\n");
	fprintf(fp, "pasv_enable=YES\n");
	fprintf(fp, "ssl_enable=NO\n");
	fprintf(fp, "tcp_wrappers=NO\n");
	strcpy(maxuser, nvram_safe_get("st_max_user"));
	if((atoi(maxuser)) > 0)
		fprintf(fp, "max_clients=%s\n", maxuser);
	else
		fprintf(fp, "max_clients=%s\n", "10");
	fprintf(fp, "ftp_username=anonymous\n");
	fprintf(fp, "ftpd_banner=Welcome to ASUS %s FTP service.\n", nvram_safe_get("productid"));
#ifdef LANGUAGE_TW
	fprintf(fp, "enable_iconv=YES\n");
	fprintf(fp, "remote_charset=cp950\n");
#endif
#ifdef LANGUAGE_CN
	fprintf(fp, "enable_iconv=YES\n");
	//fprintf(fp, "remote_charset=cp936\n");
	fprintf(fp, "remote_charset=gb2312\n");
#endif
#ifdef LANGUAGE_KR
	fprintf(fp, "enable_iconv=YES\n");
	fprintf(fp, "remote_charset=cp949\n");
#endif
#ifdef LANGUAGE_JP
	fprintf(fp, "enable_iconv=YES\n");
	fprintf(fp, "remote_charset=cp932\n");
#endif
	fclose(fp);

	//get_first_partition(test_path);
	memset(test_path, 0, sizeof(test_path));
	strcpy(test_path, "/tmp/harddisk/part0");
	nvram_set("first_partition", test_path);
	//free(test_path);
}

int
start_ftpd()	// added by Jiahao for WL500gP
{
	int ret=0;

	printf("\n[rc] start ftpd\n");	// tmp test
	if (nvram_match("st_ftp_modex", "0")) return 0;

	eval("killall", "-SIGKILL", "vsftpd");

	write_ftpd_conf();

	if (nvram_match("st_ftp_modex", "1"))
		printf("ftp mode: login to first partition\n");
	else if (nvram_match("st_ftp_modex", "2"))
		printf("ftp mode: login to first matched shared node\n");

	ret=system("vsftpd&");

	if(!ret)
	{
		logmessage("FTP server", "daemon is started");
		return 0;
	}
	else
		return 1;
}

int
test_user(char *target, char *pattern)	// added by Jiahao for WL500gP
{
	char s[384];
	char p[32];
	char *start;
	char *pp;
	strcpy(s, target);
	strcpy(p, pattern);
	start = s;
	while (pp=strchr(start, ';'))
	{
		*pp='\0';
		if(! strcmp(start, p))
			return 1;
		start=pp+1;
	}
	return 0;
}
#endif

#ifdef USBTPT
int
usbtpt(int argc, char *argv[])
{
        struct timeval tv1, tv2;
        struct timezone tz1, tz2;
        float diff = 0.0, rate = 0.0;
        int fd, Mbsize = 0, len;
        int i, counts;

        if(argc < 4)
                return 0;

        if(strcmp("argv[1]", "-h") == 0)
        {
                printf("usage: utpt [file] [size(Mb)] [buflen(Kb)]\n");
                return 0;
        }

        len = atoi(argv[3])*1024;
        char buf[len];

        Mbsize = atoi(argv[2]) * 1024 * 1024;
        counts = Mbsize / len;

        memset(buf, 'a', sizeof(buf));
        unlink(argv[1]);

        printf("write size is %d, buf len is %d, counts is %d\n", Mbsize, len, counts);

        gettimeofday(&tv1, &tz1);

        if((fd = open(argv[1], O_CREAT|O_WRONLY)) < 0)
        {
                perror("open");
                exit(-1);
        }
        for(i=0; i < counts; ++i)
                write(fd, buf, sizeof(buf));

        close(fd);

        gettimeofday(&tv2, &tz2);


        diff = (float)((float)(tv2.tv_sec - tv1.tv_sec) + ((float)(tv2.tv_usec - tv1.tv_usec))/1000000);
        rate = (float)((float)Mbsize/diff)/(float)(1024*1024);
        printf("tv1 = (%d, %d), tv2 = (%d, %d), diff is %.6f, rate is %.3f Mbps\n", tv1.tv_sec, tv1.tv_usec, tv2.tv_sec, tv2.tv_usec, diff, rate);
        char tmpstr[40];
        sprintf(tmpstr, "chmod 666 %s", argv[1]);
        system(tmpstr);
        return 0;
}
#endif

/* remove usb mass storage */
int
remove_usb_mass(char *product)
{
	int ret;
	
	ret = swap_check();

#ifdef REMOVE
	if (product!=NULL)
	   logmessage("USB storage", product);
	else
    	   logmessage("USB storage", "NULL");
#endif

	system("killall snarf");
	system("killall giftd");
	system("killall rtorrent");
	system("killall dmathined");
	system("killall vsftpd");
	system("killall usbtest");
/*
	system("swapoff /tmp/harddisk/part0/.swap");
	system("rm -f /tmp/harddisk/part0/.swap");
	unlink("/tmp/harddisk/part0");
	system("rm -Rf /media/*");
*/
	/* force stop if necessary */
	system("killall -SIGKILL snarf");
	system("killall -SIGKILL giftd");
	system("killall -SIGKILL rtorrent");
	system("killall -SIGKILL dmathined");
	system("killall -SIGKILL vsftpd");

	chk_partitions(USB_PLUG_OFF);
	system("rm -f /tmp/harddisk/part0/.swap");
	unlink("/tmp/harddisk/part0");
	system("rm -Rf /media/*");

	nvram_set("usb_mass_hotplug", "0");
	printf("You can plugoff usb now\n");
	return 0;
}

int 
remove_usb_3g()
{
	printf("## remove usb 3g dev\n");	// tmp test
	system("killall pppd");
	system("rmmod usbserial");
	system("rmmod hso");
	nvram_set("wan0_ipaddr", "");
	return 0;
}

int
remove_storage_main(void)
{
	remove_usb_mass(NULL);
	return 0;
}

#define MOUNT_VAL_FAIL 	0
#define MOUNT_VAL_RONLY	1
#define MOUNT_VAL_RW 	2

#ifdef DLM
int calc_crc32( const char *fname, unsigned long *crc ) {
    FILE *in;           /* input file */
    unsigned char buf[BUFSIZ]; /* pointer to the input buffer */
    size_t i, j;        /* buffer positions*/
    int k;              /* generic integer */
    unsigned long tmpcrc=0xFFFFFFFF;

    /* open file */
    if((in = fopen(fname, "rb")) == NULL) return -1;

    /* loop through the file and calculate CRC */
    while( (i=fread(buf, 1, BUFSIZ, in)) != 0 ){
        for(j=0; j<i; j++){
            k=(tmpcrc ^ buf[j]) & 0x000000FFL;
            tmpcrc=((tmpcrc >> 8) & 0x00FFFFFFL) ^ crcs[k];
        }
    }
    fclose(in);
    *crc=~tmpcrc; /* postconditioning */
    return 0;
}

int
ckeck_apps_completeness(const char *dirname)
{
	if(nvram_match("dm_dbg", "1"))
		return 1;

	FILE *fp=NULL;
	FILE *fp2=NULL;
	char line[256];
	char line2[256];
	//char listfilename[64]="/apps/list";
	//char crcfilename[64]="/apps/crc";
	char listfilename[32]="/tmp/harddisk/part0/.apps/list";
	char crcfilename[32]="/tmp/harddisk/part0/.apps/crc";
	char appsdir[64];
	char tmp[256];
	char crc_str[9];
	unsigned long crc;
	unsigned long crc_org;
	
	sprintf(appsdir, "%s%s", dirname, "/.apps");

	//printf("check apps completeness...ing\n");	// tmp test
	if ((fp=fopen(listfilename, "r"))==NULL)
	{
		printf("Cannot find %s.\n", listfilename);
		printf("Make sure it's available.\n");
		return 0;
	}
	
	if ((fp2=fopen(crcfilename, "r"))==NULL)
	{
		printf("Cannot find %s.\n", crcfilename);
		printf("Make sure it's available.\n");
		fclose(fp);
		return 0;
	}

	while (!feof(fp))
	{
		if (fgets(line,sizeof(line),fp)==NULL) break;	/* end-of-file */
		if (strlen(line)<3) continue;			/* line empty */
		if((strncmp(line, "./crc", 5) == 0) || (strncmp(line, "./list", 6) == 0))
			continue;
		
		sprintf(tmp, "%s%s", appsdir, line+1);		/* file path */
		tmp[strlen(tmp)-1]='\0';

		//printf("check crc [%s]\n", tmp);	// tmp test
		if(calc_crc32(tmp, &crc) != 0)
		{
			printf("Error reading file %s.\n", tmp);
			sprintf(tmp, "rm -rf %s", appsdir);
			system(tmp);	
			fclose(fp);
			fclose(fp2);
			return 0;
		}
		
		if (fgets(line2,sizeof(line2),fp2)==NULL)
		{
			printf("fgets err\n");	// tmp test
			sprintf(tmp, "rm -rf %s", appsdir);
			system(tmp);
			fclose(fp);
			fclose(fp2);
			return 0;
		}
		sprintf(crc_str, "%08lX", crc);
		//printf("CRC32 now: %s\n", crc_str);

		line2[strlen(line2)-1]='\0';
		//printf("CRC32 org: %s\n", line2);
		
		if(strcmp(crc_str, line2)!=0)
		{
			printf("compare crc err\n");	// tmp test
			sprintf(tmp, "rm -rf %s", appsdir);
			system(tmp);
			fclose(fp);
			fclose(fp2);
			return 0;
		}
	}

	fclose(fp);
	fclose(fp2);
	return 1;
}

/*
int
ckeck_apps_completeness_pre(const char *dirname)
{
	struct stat fbuf;
	int ret;
	char apps_dir[64];

	memset(apps_dir, 0, sizeof(apps_dir));
	sprintf(apps_dir, nvram_safe_get);
	if((ret = lstat("/media/AiDisk_a1/.apps", &fbuf)) < 0)
		return 0;
	else 
		return 1;

}
*/
#endif

int
stop_usblp()
{
	return system("rmmod usblp");
}

int 
start_usblp()
{
	return system("insmod usblp.ko");
}

void 
squeeze_mem()
{
	FILE *fp;

	stop_u2ec();
	stop_telnetd();
	stop_ntpc();
	stop_ots();
	stop_upnp();
	stop_lpd();
	stop_dns();
	stop_watchdog();
#ifndef W7_LOGO
	stop_pspfix();
#endif
	stop_infosvr();
	stop_logger();
	stop_usblp();

        if ((fp=fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_max", "w+")))
        {
                fputs("4096", fp);
                fclose(fp);
	}
}

void
recover_squeeze_mem()
{
	FILE *fp;

	start_usblp();
	start_dns();
	start_ntpc();
	start_u2ec();
	start_ots();
	start_lpd();

	stop_upnp();
	start_upnp();
	start_watchdog();
	start_infosvr();
	start_logger();

        if(nvram_invmatch("wsc_config_state", "1") && nvram_match("sw_mode_ex", "1"))
        {
#ifndef W7_LOGO
		start_pspfix();
#endif
	}

        if ((fp=fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_max", "w+")))
        {
                fputs("15360", fp);
                fclose(fp);
        }
}

int
write_file(char *swap_path, int buf_size, int runs, int index)
{
	FILE *fp;
        char write_buf[buf_size];
	int i;

        memset(write_buf, 0, buf_size);

        fp=fopen(swap_path, "a");
        if(fp!=NULL)
        {
                printf("--(cric) start to write swap file[%d]:(runs:%d)--\n", index, runs);        // tmp test
                for(i=0; i< runs; ++i)
                {
         		fwrite(write_buf, buf_size, 1, fp);
                }
        }
        else
        {
                perror("fopen /media/AiDisk_xx/.swap");
		return -1;
        }

        fclose(fp);
        printf("--(cric) end to write swap file[%d]--\n", index);  // tmp test
	return 0;
}

int swap_write_count = 0;

int 
create_swap_file(char *swap_path)
{
        unsigned int total_swap_size = 1024*1024*64;
        unsigned int unit_size = 1024*2;
	unsigned int wr_num = 8;
        unsigned int truncate_run = total_swap_size / unit_size / wr_num;
	char test_path[128];
	int result = 0, i;
	time_t start_time, elapsed;
	int swap_timeout = 50;	// seconds

	memset(test_path, 0, sizeof(test_path));

	swap_write_count = 0;
	start_time = time(NULL);
	for(i=0; i < wr_num; ++i)
	{
		sleep(1);
		if((result = write_file(swap_path, unit_size, truncate_run, i)) < 0)
			break;
		elapsed = time(NULL) - start_time;
		printf(" # elapsed %d seconds\n", elapsed);	// tmp test
		if(elapsed >= swap_timeout)
		{
			// stop create & stop service
			printf("flash disk slow, stop creating swap\n");	// tmp test
			logmessage("USB storage", "flash disk slow, stop creating swap");
			break;
		}
	}
	swap_write_count = i;
	printf("swap write count is %d\n", swap_write_count);	// tmp test

	if(result == 0)
	{
                sprintf(test_path, "mkswap %s", swap_path);
                system(test_path);

		memset(test_path, 0, sizeof(test_path));
                sprintf(test_path, "swapon %s", swap_path);
                system(test_path);
         
	        logmessage("USB storage", "64MB swap file is added");
                nvram_set("swap_on", "1");
	}	
	else
                nvram_set("swap_on", "0");

	return result;
}

/* insert usb mass storage */
int
hotplug_usb_mass(char *product)
{	
	char tmp[128];
	int n=0, m=0, p=0;
	struct dirent *dp, *dp_disc, **dpopen;
	char tmpstr[128], test_path[128];
	int i, j;
	int chk_freeDisk;
	int apps_comp=0;
	int apps_status=0;
	int apps_disk_free=0;
	int new_disc[2];
	new_disc[0]=0;
	new_disc[1]=0;
# ifdef DLM
	int buflen=0;
	struct sysinfo info;
# endif
	char *usb_mass_first_path = nvram_safe_get("usb_mnt_first_path");
	if(!usb_mass_first_path)
		usb_mass_first_path = "fail_path";
	printf("\n[rc] get usb mass first path = %s\n", usb_mass_first_path);	// tmp test

	LED_CONTROL(LED_POWER, LED_OFF);

	nvram_set("usb_mass_hotplug", "1");

	squeeze_mem();

	memset(tmp, 0, sizeof(tmp));
#ifdef DLM
	nvram_set("usb_storage_busy", "1");
	nvram_set("apps_status_checked", "1");	// it means need to check
#endif
	nvram_set("usb_disc_mount_path", usb_mass_first_path);
	nvram_set("usb_disc_fs_path", usb_mass_first_path);
	nvram_set("usb_disc_path", "/tmp/harddisk/part0");
	printf("\n### [hotplug] link part0 to the first usb path (%s)\n", usb_mass_first_path);	// tmp test
	
	for(i=0; i<3; ++i)
	{
		if(symlink(usb_mass_first_path, "/tmp/harddisk/part0") == 0)
			break;
		else
		{
			printf("link to part0 fail, retrying...(%d)\n", i);	// tmp test
			sleep(1);
		}
	}
	
	nvram_set("usb_disc0_path0", "/tmp/harddisk/part0");
	nvram_set("usb_disc0_port", "1");
	nvram_set("apps_running", "0");
	nvram_set("usb_disc0_dev", usb_mass_first_path);
	//nvram_set("apps_dlx", "1");
	nvram_set("apps_dlx", nvram_safe_get("apps_dl"));
	
#ifdef DLM
	nvram_set("eject_from_web", "0");
		
	memset(test_path, 0, sizeof(test_path));
	strcpy(test_path, "/tmp/harddisk/part0");
	memset(tmp, 0, sizeof(tmp));
	strcpy(tmp, test_path);

	system("rm -Rf /tmp/harddisk/part0/share/.apps");
	if((apps_comp=ckeck_apps_completeness(tmp))==1)
		nvram_set("apps_comp", "1");
	else
	{
		printf("verify apps fail\n");	// tmp test
		system("rm -Rf /tmp/harddisk/part0/.apps");
		nvram_set("apps_comp", "0");
	}

	printf("\n\n**apps_completeness=%d\n\n", apps_comp);	// tmp test

	sprintf(tmpstr, "%s/.swap", test_path);
	unlink("/tmp/harddisk/part0/.swap");

	/* chk disk total > 1G */
	chk_freeDisk = check_disk_free_GE_1G(test_path);
			
	printf("chk freeDisk result= %d\n", chk_freeDisk);	// tmp test
	if ((chk_freeDisk == 2) || (chk_freeDisk == 3))
	{
		if(create_swap_file(tmpstr) < 0)
			return -1;
	}
	else if (chk_freeDisk == 1)
		logmessage("USB storage", "The swap file is not added for free space is less than 128 Mb");
	//else if (chk_freeDisk == 3)	/* accept to create swap */
	//	logmessage("USB storage", "The swap file is not added for partition size is less than 1 G");
	else if (chk_freeDisk == 0)
		logmessage("USB storage", "The swap file is not added for unknown reasons");

	/* make necessary dir */
	init_apps();

	recover_squeeze_mem();

	/* run ftp latter*/
	eval("/sbin/test_of_var_files_in_mount_path", usb_mass_first_path);
	run_ftp();

	/* chk disk free */
	apps_disk_free=check_disk_free_apps(tmp, apps_comp);
	if (apps_disk_free==1)
		nvram_set("apps_disk_free", "1");
	else
		nvram_set("apps_disk_free", "0");

	/* before exec apps, we check all tasks first */
	check_all_tasks();

	printf("\nchk apps ready: apps_comp=%d, apps_dl=%s, apps_running=%s, chk_freeDisk=%d, swap_write_count=%d\n", apps_comp, nvram_safe_get("apps_dl"), nvram_safe_get("apps_running"), chk_freeDisk, swap_write_count);	// tmp test
	printf("it supposed to be 1, 1, 0, 2, 6");	// tmp test
	/* if apps is ready and not running, then run it */
	if((apps_comp==1) &&
	   nvram_match("apps_dl", "1") &&
	   nvram_match("apps_running", "0")
	  )
	{
		if((chk_freeDisk == 2) && (swap_write_count >= 6))
		{
			if (apps_comp==1 && apps_disk_free==1)
			{
				nvram_set("apps_dms_usb_port_x", "1");
				exec_apps();
				nvram_set("apps_running", "1");
			}
		}
		else
		{
			fprintf(stderr, "skip running DM: not enough disk space or slow disk\n");
			logmessage("Download Master", "not enough space or slow disk, daemon is not started");
		}
	}
	else
	{
		fprintf(stderr, "skip running DM: apps not ready or disabled\n");
		logmessage("Download Master", "apps not ready or disabled, daemon is not started");
	}

	nvram_set("usb_storage_busy", "0");
#endif

	LED_CONTROL(LED_POWER, LED_ON);
	return 0;
}

void
mnt_op(int disk_num, int part_num, int op_mode)
{
	char dev_path[32], mnt_path[32], cmd[128];

	memset(mnt_path, 0, sizeof(mnt_path));
	memset(dev_path, 0, sizeof(dev_path));
	memset(cmd, 0, sizeof(cmd));

	switch(op_mode){
	case OP_MOUNT:	// only needs for disk w/ no partition table
		if(part_num < 2)
			sprintf(dev_path, "sd%c", disk_num+97);
		else
			sprintf(dev_path, "sd%c%d", disk_num+97, part_num);
		sprintf(mnt_path, "AiDisk_%c%d", disk_num+97, part_num);

		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "/sbin/automount.sh %s %s", dev_path, mnt_path);
		system(cmd);
		printf("\n##[mnt_op]: MOUNT: %s(%d,%d)\n", cmd, disk_num, part_num);	// tmp test
		break;
	case OP_UMOUNT:
		sprintf(mnt_path, "/media/AiDisk_%c%d", disk_num+97, part_num);

		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "swapoff %s/.swap", mnt_path);
		system(cmd);

		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "umount2 %s", mnt_path);
		system(cmd);
		printf("[mnt_op]: UMOUNT: %s\n", cmd);	// tmp test
		break;
	case OP_SETNVRAM:
		sprintf(mnt_path, "/media/AiDisk_%c%d", disk_num+97, part_num);

		printf("[mnt_op]: set usb_mnt_first_path as %s\n", mnt_path);	// tmp test
		nvram_set("usb_mnt_first_path", mnt_path);
		break;
	default:
		break;
	}
}

void 
chk_partitions(int state)
{
	FILE *fp_p = fopen("/proc/partitions", "r");
	FILE *fp_m = fopen("/proc/mounts", "r");
	char buf[120];
	char par_n[26][20], mnt_n[26][20];
	char pc, pcn[3], *tp;
	int  pd;
	int  i, j, k;
	int  try_counts = 0;
	int  set_nv = 0;
	int  new_mount = -1;

	//printf("chk_partition tables\n");	// tmp test
	while((!fp_m) || (!fp_p))
	{
		if(try_counts > 10)
		{
			printf("cannot open mounts/partitions\n");
			return;
		}

		if(!fp_m)
			fp_m = fopen("/proc/mounts", "r");
		if(!fp_p)
			fp_p = fopen("/proc/partitions", "r");

		++try_counts;
		printf("open mounts/part fail, try [%d]\n", try_counts);
		sleep(1);
	}

	memset(par_n, 0, sizeof(par_n));
	memset(buf, 0, sizeof(buf));
	while(fgets(buf, sizeof(buf), fp_p))	// chk partitions
	{
		if(strstr(buf, "mtdblock"))
			continue;
		if((tp = strstr(buf, "sd")) != NULL)
		{
			pc = *(tp+2);
			memset(pcn, 0, sizeof(pcn));
			if((*(tp+3)) && ((*(tp+3)) != '\n') && ((*(tp+3)) != '\r') && ((*(tp+3)) != '\0'))
				pcn[0] = *(tp+3);
			if((*(tp+4)) && ((*(tp+4)) != '\n') && ((*(tp+4)) != '\r') && ((*(tp+4)) != '\0'))
				pcn[1] = *(tp+4);
			pd = atoi(pcn);
			if(pc >= 97)	// 'a' is 97
			{
				par_n[pc-97][pd] = 'h';
				//printf("**set part[%d][%d] **\n", pc-97, pd);	// tmp test
			}
		}
		memset(buf, 0, sizeof(buf));
	}

	memset(mnt_n, 0, sizeof(mnt_n));
	memset(buf, 0, sizeof(buf));
        while(fgets(buf, sizeof(buf), fp_m))    // chk mounts
        {
                if(!strstr(buf, "/media/AiDisk_"))
                        continue;
                if((tp = strstr(buf, "/media/AiDisk_")) != NULL)
                {
                        pc = *(tp+14);
                        memset(pcn, 0, sizeof(pcn));
                        if((*(tp+15)) && ((*(tp+15)) != ' '))
                                pcn[0] = *(tp+15);
                        if((*(tp+16)) && ((*(tp+16)) != ' '))
                                pcn[1] = *(tp+16);
                        pd = atoi(pcn);
                        if(pc >= 97)    // 'a' is 97, chk if starts from 'a'... and write flag 'h'(has)
			{
                                mnt_n[pc-97][pd] = 'h';
				//printf("**set mnt[%d][%d] **\n", pc-97, pd);	// tmp test
			}
                }
		memset(buf, 0, sizeof(buf));
        }

	fclose(fp_p);
	fclose(fp_m);

#if 0
	/* dump mnt_n/par_n */
        printf("\n\n############partitions##############\n\n");
        for(i=0, j=0, k=0; i<2; ++k)
        //for(i=25, j=0, k=0; i<26; ++k)
        {
                printf("(%d,%d)=[%c] ", i, j, par_n[i][j]);
                ++j;
                if(j==20)
                {
                        j = 0;
                        ++i;
                        continue;
                }
                if(k == 5)
                {
                        printf("\n");
                        k = 0;
                }
        }
        printf("\n\n############mounts##############\n\n");
        for(i=0, j=0, k=0; i<2; ++k)
        //for(i=25, j=0, k=0; i<26; ++k)
        {
                printf("(%d,%d)=[%c] ", i, j, mnt_n[i][j]);
                ++j;
                if(j==20)
                {
                        ++i;
                        j = 0;
                        continue;
                }
                if(k == 5)
                {
                        printf("\n");
                        k = 0;
                }
        }
#endif
	switch(state) {
	case USB_PLUG_ON:
		for(i=0; i<26; ++i)	// check disk w/ no partition table and mount it
		{
			if((par_n[i][0] == 'h') && (par_n[i][1] != 'h') && (mnt_n[i][1] != 'h'))
			{
					new_mount = i;
					mnt_op(i, 1, OP_MOUNT);
			}
			for(j=2; j<20; ++j)	// no need to chk sdx0/1
			{
				if((par_n[i][j] == 'h') && (mnt_n[i][j] != 'h'))
					mnt_op(i, j, OP_MOUNT);
			}
		}
		set_nv = 0;
		for(i=0; i<26; ++i)	// set nvram: usb_mnt_first_path
		{
			if(set_nv)
				break;

			if(((par_n[i][1] == 'h') || (par_n[i][0] == 'h')) && ((mnt_n[i][1] == 'h') || (mnt_n[i][0] == 'h')))  // consider sdx1 first 
			{
				mnt_op(i, 1, OP_SETNVRAM);
				break;
			} 
			else if((i == new_mount) && ((par_n[i][0] == 'h') && (par_n[i][1] != 'h') && (mnt_n[i][1] != 'h')))
			{
				//printf("new mount set= %d\n", i);	// tmp test
				mnt_op(i, 1, OP_SETNVRAM);
				break;
			}
			else
			{
				for(j=2; j<20; ++j)
				{
					if((par_n[i][j] == 'h') && (mnt_n[i][j] == 'h'))
					{
						mnt_op(i, j, OP_SETNVRAM);
						set_nv = 1;
						break;
					}
				}
			}
		}
		break;
	case USB_PLUG_OFF:
                for(i=0; i<26; ++i)     // set nvram: usb_mnt_first_path
                {
			for(j=1; j<20; ++j)
			{
                        	if(mnt_n[i][j] == 'h')
                                	mnt_op(i, j, OP_UMOUNT);
			}
                }
		break;
	}
}

void
set_dev_class(char *dev, int *num)
{
	printf("set dev class:[%c][%c]\n", dev[0], dev[1]);	// tmp test
        if((dev[0] == '0') && (dev[1] =='0'))
                *num = USB_CLS_PER_INTERFACE;
        else if((dev[0] == '0') && (dev[1] =='1'))
	{
                *num = USB_CLS_AUDIO;
		nvram_set("usb_path1", "audio");
	}
        else if((dev[0] == '0') && (dev[1] =='2'))
	{
                *num = USB_CLS_COMM;
		nvram_set("usb_path1", "comm");
	}
        else if((dev[0] == '0') && (dev[1] =='3'))
	{
                *num = USB_CLS_HID;
		nvram_set("usb_path1", "hid");
	}
        else if((dev[0] == '0') && (dev[1] =='5'))
	{
                *num = USB_CLS_PHYSICAL;
		nvram_set("usb_path1", "physical");
	}
        else if((dev[0] == '0') && (dev[1] =='6'))
	{
                *num = USB_CLS_STILL_IMAGE;
		nvram_set("usb_path1", "image");
	}
        else if((dev[0] == '0') && (dev[1] =='7'))
	{
                *num = USB_CLS_PRINTER;
		nvram_set("usb_path1", "printer");
	}
        else if((dev[0] == '0') && (dev[1] =='8'))
	{
                *num = USB_CLS_MASS_STORAGE;
		nvram_set("usb_path1", "storage");
	}
        else if((dev[0] == '0') && (dev[1] =='9'))
	{
                *num = USB_CLS_HUB;
		nvram_set("usb_path1", "hub");
	}
        else if((dev[0] == '0') && (dev[1] =='a'))
	{
                *num = USB_CLS_CDC_DATA;
		nvram_set("usb_path1", "cdc_data");
	}
        else if((dev[0] == '0') && (dev[1] =='b'))
	{
                *num = USB_CLS_CSCID;
		nvram_set("usb_path1", "cscid");
	}
        else if((dev[0] == '0') && (dev[1] =='d'))
                *num = USB_CLS_CONTENT_SEC;
        else if((dev[0] == '0') && (dev[1] =='e'))
	{
                *num = USB_CLS_VIDEO;
		nvram_set("usb_path1", "video");
	}
        else if((dev[0] == 'e') && (dev[1] =='0'))
	{
                *num = USB_CLS_WIRELESS_CONTROLLER;
		nvram_set("usb_path1", "w_controller");
	}
        else if((dev[0] == 'e') && (dev[1] =='f'))
	{
                *num = USB_CLS_MISC;
		nvram_set("usb_path1", "misc");
	}
        else if((dev[0] == 'f') && (dev[1] =='e'))
	{
                *num = USB_CLS_APP_SPEC;
		nvram_set("usb_path1", "app_spec");
	}
        else if((dev[0] == 'f') && (dev[1] =='f'))
	{
                *num = USB_CLS_VENDOR_SPEC;
		nvram_set("usb_path1", "vendor_spec");
	}
	else
	{
                *num = USB_CLS_VENDOR_SPEC;
		nvram_set("usb_path1", "vendor_spec");
	}
	//printf("get usb_path is %s\n", nvram_safe_get("usb_path1"));	// tmp test
}

int
get_dev_info(int *dev_class, char *product_id)
{
        int fd, i, bus_num = 0;
        char *skey_1 = "I:*", *skey_2 = "Cls=", *skey_3 = "P:", *skey_4 = "Vendor=", *skey_5 = "ProdID=", *skey_6 = "Rev=", *skey_7="Cls=07(print)", *skey_8 = "S:  Manufacturer=", *skey_9 = "S:  Product=", *skey_10 = "S:  SerialNumber=", *bp, *tmp_p;
        char buf[2048], b_class[10][3], vendor[5], prodid[5], rev[6], tmp_name[100];
	char *product_name;
	int g_f1=0, g_f2=0;

        if((fd=open(usb_dev_file, O_RDONLY)) <= 0)
        {
                printf("open usb devices fail\n");
                return 0;
        }

        memset(buf, 0, sizeof(buf));
        if(read(fd, buf, sizeof(buf)) <= 0)
        {
                printf("read usb devices fail\n");
                return 0;
        }

        bp = buf;
        while((tmp_p = strstr(bp, skey_3)) != NULL)	/* chk each product */
        {
                bp = tmp_p + 1;
                ++bus_num;

		if(bus_num == 1)
			continue;

                memset(vendor, 0, sizeof(vendor));
                memset(prodid, 0, sizeof(prodid));
                memset(rev, 0, sizeof(rev));
		/* get vendor */
                tmp_p = strstr(bp, skey_4);
		if(tmp_p)
		{
                	vendor[0] = *(tmp_p + 7);
                	vendor[1] = *(tmp_p + 8);
                	vendor[2] = *(tmp_p + 9);
                	vendor[3] = *(tmp_p + 10);
		} 
		if((strcmp(vendor, "12d1") == 0) ||
		(strcmp(vendor, "805") == 0) ||
		(strcmp(vendor, "1a8d") == 0)
		)
			g_f1 = 1;	/* the flag 1 to chkif 3g dev */
		/* get productid */
                tmp_p = strstr(bp, skey_5);
		if(tmp_p)
		{
                	prodid[0] = *(tmp_p + 7);
                	prodid[1] = *(tmp_p + 8);
                	prodid[2] = *(tmp_p + 9);
                	prodid[3] = *(tmp_p + 10);
		}
		/* get rev */
                tmp_p = strstr(bp, skey_6);
		if(tmp_p)
		{
                	rev[0] = *(tmp_p + 4);
               		rev[1] = *(tmp_p + 5);
                	rev[2] = *(tmp_p + 6);
                	rev[3] = *(tmp_p + 7);
                	rev[4] = *(tmp_p + 8);
		}

                sprintf(product_id, "%s/%s/%s", vendor, prodid, rev);
		//printf("set product_id as %s/%s/%s\n", vendor, prodid, rev);	// tmp test
		/* get manufact */
		tmp_p = strstr(bp, skey_8);
		memset(tmp_name, 0, sizeof(tmp_name));
		if(tmp_p)
		{
			tmp_p+=strlen(skey_8);
			for(i=0; (*(tmp_p+i)!='\n')&&(i<100); ++i)
				tmp_name[i] = *(tmp_p+i);
			//printf("get Manufacturer=%s\n", tmp_name);	// tmp test
			nvram_set("usb_Manufacturer", tmp_name);
		}
		else
			nvram_set("usb_Manufacturer", "unknown");
		/* get product name */
		tmp_p = strstr(bp, skey_9);
		memset(tmp_name, 0, sizeof(tmp_name));
		if(tmp_p)
		{
			tmp_p+=strlen(skey_9);
                	for(i=0; (*(tmp_p+i)!='\n')&&(i<100); ++i)
                        	tmp_name[i] = *(tmp_p+i);
                	//printf("get productname=%s\n", tmp_name);      // tmp test
                	nvram_set("usb_Product", tmp_name);
		}
		else
                	nvram_set("usb_Product", "unknown");
		product_name = nvram_safe_get("usb_Product");
		/* get serialnum */
		tmp_p = strstr(bp, skey_10);
		memset(tmp_name, 0, sizeof(tmp_name));
		if(tmp_p)
		{
			tmp_p+=strlen(skey_10);
                	for(i=0; (*(tmp_p+i)!='\n')&&(i<100); ++i)
                        	tmp_name[i] = *(tmp_p+i);
                	//printf("get serialnum=%s\n", tmp_name);      // tmp test
                	nvram_set("usb_SerialNumber", tmp_name);
		}
		else
                	nvram_set("usb_SerialNumber", "unknown");

		printf("get class:(%d)\n", sizeof(b_class));	// tmp test
                /* get class */
                memset(b_class, 0, sizeof(b_class));
		for(i=0; i<10; ++i)	/* chk at most 10 interface */
		{
                	tmp_p = strstr(bp, skey_2);
			if(!tmp_p)
				break;

			bp = tmp_p + 1;
                	if(tmp_p)
                	{
                        	b_class[i][0] = *(tmp_p + 4);
                        	b_class[i][1] = *(tmp_p + 5);
                	}
                	else
                	{
                        	b_class[i][0] = 'f';
                        	b_class[i][1] = 'f';
                	}
			if(b_class[i][0]=='f')
				g_f2 = 1;	/* the flag2 to chk if 3g device */
		}
		printf("%s has %d if(s)\n", product_name, i);	// tmp test
		/* select one target */
		//if(g_f1 && g_f2)
		if(g_f1 && (strcmp(nvram_safe_get("wan_proto"), "3g")==0) && (strlen(nvram_safe_get("Dev3G")) > 0))
		{
			*dev_class = USB_CLS_3GDEV;
		}
                /* set device class */
		else
                	set_dev_class(b_class[0], dev_class);
        }
	printf("get productid is %s, busnum=%d\n", product_id, bus_num);	// tmp test

        close(fd);

	if(bus_num <= 1)
		return 0;

        return 1;
}

#ifdef CDMA // HSDPA {
int hsdpa_detect(void)
{
        int f;
        int ret=-1;
        extern int errno;

        if((f=open("/dev/ttyACM0", O_WRONLY))!=-1)
        {
		printf("--- hsdpa_detect: Detect as ACM! ---\n");
                close(f);
                ret=0;
        }
        else if((f=open("/dev/ttyUSB0", O_WRONLY))!=-1)
        {
		printf("--- hsdpa_detect: Detect as USB! ---\n");
                close(f);
                ret=1;
        }
        else
        {
		printf("--- hsdpa_detect: Detect as not HSDPA! ---\n");
        }
        return ret;
}

int hotplug_hsdpa(char *product)
{
        char tmp[100];
        char vid[32];
        char pid[32];
        char *ptr1, *ptr2;
        int combo;

        strcpy(tmp, product);
        ptr1=strchr(tmp, '/');
        *ptr1=0;
        sprintf(vid, "vendor=0x%s", tmp);
        ptr2=strchr(ptr1+1, '/');
        *ptr2=0;
        sprintf(pid, "product=0x%s", ptr1+1);

        if (nvram_match("hsdpa_enable", "0")) return 0;

        if (nvram_match("hsdpa_product", ""))
        {
                eval("rmmod", "usbserial");

		printf("mknod ttyACM nodes.\n");
                eval("mknod", "/dev/ttyACM0", "c", "166", "0");
                eval("mknod", "/dev/ttyACM1", "c", "166", "1");

		printf("mknod ttyUSB nodes.\n");	// tmp test
                eval("mknod", "/dev/ttyUSB0", "c", "188", "0");
                eval("mknod", "/dev/ttyUSB1", "c", "188", "1");
                eval("mknod", "/dev/ttyUSB2", "c", "188", "2");
                eval("mknod", "/dev/ttyUSB3", "c", "188", "3");

                eval("modprobe", "usbserial", vid, pid);

                if(!strncmp(product, "1a8d/", 5)){
                        logmessage("Add USB Modem", product);

                        nvram_set("hsdpa_product", product);
                        nvram_set("hsdpa_combo", "1");

                        nvram_set("got_HSDPA", "1");
                        nvram_set("hsdpa_ttyUSB_num", "1");

                        nvram_set("cdma_down", "1");

                        return 1;
                }
                else if(!strcmp(product, "805/480f/0")){
                        logmessage("Add USB Modem", product);

                        nvram_set("hsdpa_product", product);
                        nvram_set("hsdpa_combo", "1");

                        nvram_set("got_HSDPA", "1");
                        nvram_set("hsdpa_ttyUSB_num", "3");

                        nvram_set("cdma_down", "1");

                        return 1;
                }
                else if(!strcmp(product, "12d1/1001/0")
                                        || !strcmp(product, "12d1/1003/0")
                                        ){
                        logmessage("Add USB Modem", product);

                        nvram_set("hsdpa_product", product);
                        nvram_set("hsdpa_combo", "1");

                        nvram_set("got_HSDPA", "1");
                        nvram_set("hsdpa_ttyUSB_num", "0");

                        nvram_set("cdma_down", "1");

                        return 1;
                }
                else if((combo = hsdpa_detect()) != -1)
                {
                        logmessage("Add USB Modem", product);

                        nvram_set("hsdpa_product", product);
                        if(combo){
                                nvram_set("hsdpa_combo", "1");

                                /* nvram_set("got_HSDPA", "0"); */
                                nvram_set("hsdpa_ttyUSB_num", "");
                        }
                        else
                                nvram_set("hsdpa_combo", "0");

                        /* start cdma */
                        nvram_set("cdma_down", "1");

                        return 1;
                }
                else{
                        eval("rmmod", "usbserial");

                        nvram_set("hsdpa_combo", "-1");
                }
        }

        /* skip other interface on the same device */
        if(nvram_match("hsdpa_product", product)){
	printf("--- hotplug_hsdpa: Found other interface of the HSDPA card! ---\n");
                return 1;
        }

        return 0;
}

int unplug_hsdpa(char *product)
{
        char tmp[100];
        char vid[32];
        char pid[32];
        char *ptr1, *ptr2;

        strcpy(tmp, product);
        ptr1=strchr(tmp, '/');
        *ptr1=0;
        sprintf(vid, "vendor=0x%s", tmp);
        ptr2=strchr(ptr1+1, '/');
        *ptr2=0;
        sprintf(pid, "product=0x%s", ptr1+1);

        if (nvram_match("hsdpa_enable", "0")) return 0;

        if (nvram_match("hsdpa_product", product))
        {
                logmessage("Remove USB Modem", product);

                eval("rmmod", "usbserial");
                nvram_set("hsdpa_product", "");
                nvram_set("hsdpa_combo", "-1");

                /* stop cdma */
                nvram_set("cdma_down", "99");
                nvram_set("got_HSDPA", "0");
                
                return 1;
        }
        
        return 0;
}       
#endif // CDMA // HSDPA }

int 
hotplug_usb()
{
        unsigned int usb_dev_class_num, bus_plugged = 0;
        char productID[20];
	char *product, *usb_path;
	char usbpath_nvram[16];
	char temp_usbpath_device[16];
#ifdef U2EC
        int u2ec_fifo;
#endif

	if (nvram_match("asus_mfg", "1"))
	{
                nvram_set("usb_device", "1");
		return 0;
	}

	product = getenv("PRODUCT");
	usb_path = getenv("USBDEVICE_PATH");

        if(usb_path){	// tmp add
                nvram_set("usb_path", usb_path);

                memset(usbpath_nvram, 0, 16);
                sprintf(usbpath_nvram, "usb_path%s", usb_path);

                memset(temp_usbpath_device, 0, 16);
                strcpy(temp_usbpath_device, nvram_safe_get(usbpath_nvram));
        }

        memset(productID, 0, sizeof(productID));
        bus_plugged = get_dev_info(&usb_dev_class_num, productID);

	printf("bus plugged is %d\n", bus_plugged);	// tmp test
	if(bus_plugged)
		printf("hotplug_usb: bus plugged is %d, pID is %s, class_num is %d\n", bus_plugged, productID, usb_dev_class_num);  // tmp test
	else
		printf("bus plugged failed\n");	// tmp test

	if(strlen(productID) > 0)
		nvram_set("usb_vidpid", productID);
	else
		nvram_set("usb_vidpid", "");

	/* to apart 'wanup func' and 'usb storage func'*/
	if(usb_dev_class_num == USB_CLS_MASS_STORAGE)
	{
		while(strcmp(nvram_safe_get("wanup_mem_cric"), "0") != 0)
			sleep(1);
		nvram_set("hotplug_usb_mem_cric", "1");
	}

	if(bus_plugged)
	{
		printf("bus plugged\n");	// tmp test
/*
        	if(nvram_match("usb_mass_hotplug", "1"))
		{
			nvram_set("hotplug_usb_mem_cric", "0");
        		return 0;
		}	
*/
                nvram_set("usb_device", "1");
		nvram_set("usb_storage_device", productID);
		logmessage("USB storage", "\"%s\" was plugged in.", productID);
#ifdef U2EC
		printf("open u2ec fifo\n");	// tmp test
                u2ec_fifo = open("/tmp/u2ec_fifo", O_WRONLY|O_NONBLOCK);
                write(u2ec_fifo, "a", 1);
                close(u2ec_fifo);
#endif
		if(usb_dev_class_num == USB_CLS_MASS_STORAGE)
		{
			chk_partitions(USB_PLUG_ON);
			if((strlen(nvram_safe_get("usb_mnt_first_path")) <= 0) && (strcmp(nvram_safe_get("wan_proto"), "3g")==0) && (strlen(nvram_safe_get("Dev3G")) > 0))
				usb_dev_class_num = USB_CLS_3GDEV;
		}
		
		printf("service_ex: start excuting usb_hotplug process, type: %d\n", usb_dev_class_num);	// tmp test
		if(usb_dev_class_num == USB_CLS_MASS_STORAGE)
		{
			printf("we got storage dev\n");	// tmp test
			hotplug_usb_mass(productID);
		}
		else if(usb_dev_class_num == USB_CLS_3GDEV)
		{
			printf("we got 3g dev\n");	// tmp test
			//if(check_pppd() > 0)
			start_3g();
			//else
			//	printf("3g: pppd already invoked\n");
		}
		else
		{
			printf("other usb dev class\n");	// tmp test
#ifdef CDMA // HSDPA {
                        if(hotplug_hsdpa(product))
			{
                                nvram_set("usb_hsdpa_device", product);
                                if(usb_path){
                                        nvram_set(usbpath_nvram, "HSDPA");
                                        nvram_set("usb_path_hsdpa", usb_path);
                                }
                        }
#endif // HSDPA }
		}
	}
	else
	{
		printf("bus plugoff\n");	// tmp test
		chk_partitions(USB_PLUG_OFF);

                nvram_set("usb_device", "0");
		nvram_set("usb_storage_device", "");
		nvram_set("usb_mass_hotplug", "0");
		logmessage("USB device", "device removed");
#ifdef U2EC
                u2ec_fifo = open("/tmp/u2ec_fifo", O_WRONLY|O_NONBLOCK);
                write(u2ec_fifo, "r", 1);
                close(u2ec_fifo);
#endif
        	nvram_set("ftp_running", "0");
        	nvram_set("swap_on", "0");
        	nvram_set("apps_running", "0");
        	nvram_set("apps_dlx", "0");
        	nvram_set("apps_status_checked", "1");  // it means need to check
        	nvram_set("usb_disc0_port", "0");
        	nvram_set("usb_disc0_dev", "");
        	nvram_set("usb_path1", "");

		//if(usb_dev_class_num == USB_CLS_MASS_STORAGE)
		remove_usb_mass(NULL);

		if(usb_dev_class_num == USB_CLS_3GDEV)
			remove_usb_3g();

#ifdef CDMA // HSDPA {
                if(!strcmp(temp_usbpath_device, "HSDPA"))
		{
                        if(unplug_hsdpa(product))
			{
                                nvram_set("usb_hsdpa_device", "");
                                nvram_set("usb_path_hsdpa", "");
                        }
                }
#endif // HSDPA }

	}

	nvram_set("hotplug_usb_mem_cric", "0");
        return 0;
}

#endif	

/* stop necessary services for firmware upgrade */	
/* stopservice: for firmware upgarde */
/* stopservice 1: for button setup   */
int
stop_service_main(int type)
{
	if (type==1)
	{
		stop_usb();
		stop_upnp();
		stop_dns();
		stop_httpd();
		eval("killall", "udhcpc");
		//eval("killall", "infosvr");
	}
	else
	{
		if(type==99) stop_misc_no_watchdog();
		else stop_misc();
		stop_logger();
		stop_usb();

		stop_upnp();
		stop_dhcpd();
		stop_dns();
	}

	return 0;
}

int update_resolvconf(void);
extern int chk_flag;
int service_handle(void)
{
	char *service;
	char tmp[100], *str;
	int pid;
	char *ping_argv[] = { "ping", "140.113.1.1", "-c", "3", NULL};
	FILE *fp;
	char pid_buf[32];

	service = nvram_safe_get("rc_service");

	if(!service)
		kill(1, SIGHUP);

	if(strstr(service,"wan_disconnect")!=NULL)
	{
		logmessage("wan", "disconnected manually");

#ifdef CDMA // HSDPA test
                if(nvram_invmatch("hsdpa_product", "") && nvram_invmatch("hsdpa_combo", "-1")){
			printf("--- HSDPA test: disconnect! ---\n");	// tmp test
                        nvram_set("cdma_down", "3");
                }
                else
#endif // CDMA*/

		if (nvram_match("wan0_proto", "dhcp") ||
			nvram_match("wan0_proto", "bigpond"))
		{		
			snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", 0);
			if ((str = file2str(tmp))) {
				pid = atoi(str);
				free(str);			
				kill(pid, SIGUSR2);
			}
		}
		else if(
		nvram_match("wan0_proto", "pptp")
		//nvram_match("wan0_proto", "pppoe") ||
		//nvram_match("wan0_proto", "l2tp")
		)
		{
			printf("stop wan ppp manually\n");	// tmp test
			stop_wan_ppp();
		}
		else 
		{			
			printf("services stop wan2 \n");	// tmp test
			stop_wan2();
			update_wan_status(0);
			//sleep(2);

			if (nvram_match("wan0_proto", "static"))
			{
				system("ifconfig eth2.2 0.0.0.0");
			}

		}
	}
	else if (strstr(service,"wan_connect")!=NULL)
	{
		logmessage("wan", "connected manually");
		//setup_ethernet(nvram_safe_get("wan_ifname"));

#ifdef CDMA // HSDPA test
                if(nvram_invmatch("hsdpa_product", "") && nvram_invmatch("hsdpa_combo", "-1")){
printf("--- HSDPA test: connect! ---\n");
                        nvram_set("cdma_down", "1");
                }
                else
#endif // CDMA*/
		if (nvram_match("wan0_proto", "dhcp") ||
			nvram_match("wan0_proto", "bigpond"))
		{
			snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", 0);
			if ((str = file2str(tmp))) {
				pid = atoi(str);
				free(str);
				kill(pid, SIGUSR1);
			}
		}
                else if(
                nvram_match("wan0_proto", "pptp")
                //nvram_match("wan0_proto", "pppoe") ||
                //nvram_match("wan0_proto", "l2tp")
                )
                {
			printf("start wan ppp manually\n");	// tmp test
                        start_wan_ppp();
                }
		else 
		{
#ifndef REMOVE
			// pppoe or ppptp, check if /tmp/ppp exist
			if (nvram_invmatch("wan0_proto", "static") && (fp=fopen("/tmp/ppp/ip-up", "r"))!=NULL)
			{
				fclose(fp);
				_eval(ping_argv, NULL, 0, &pid);
			}
			else
			{
				stop_wan();
				unlink("/tmp/ppp/link.ppp0");
				unlink("/tmp/ppp/options.wan0");

				sleep(1);

				start_wan();

				sleep(2);

				_eval(ping_argv, NULL, 0, &pid);
			}
#endif

# if 0
#ifdef REMOVE
			stop_wan();
			sleep(2);
			start_wan();
			/* trigger connect */
			eval("ntpclient", "-h", "test", "-c", "1");
#endif
#endif
		}
	}
#ifdef ASUS_DDNS //2007.03.26 Yau add for asus ddns
        else if(strstr(service,"ddns_hostname_check")!=NULL)
        {
                char host[64];
                char wan_ifname[16];

                strcpy(host, nvram_safe_get("ddns_hostname_x"));
#ifdef CDMA // HSDPA
                if(strcmp(nvram_safe_get("hsdpa_product"), "") != 0)
                        strcpy(wan_ifname, "ppp0");
                else
#endif
                if (nvram_match("wan_proto", "pppoe") || nvram_match("wan_proto", "pptp") || nvram_match("wan_proto", "l2tp")) //2008.10 magic add l2tp
                {
                        strcpy(wan_ifname, nvram_safe_get("wan0_pppoe_ifname"));
                }
                else
                {
                        strcpy(wan_ifname, nvram_safe_get("wan0_ifname"));
                }
                                                   
                char *ddns_argv[] = {"ez-ipupdate",
                                     "-h", host,
                                     "-s", "ns1.asuscomm.com",
                                     "-S", "dyndns",
                                     "-i", wan_ifname,
                                     "-A", "1",
                                     NULL};
                pid_t pid;

		//Execute ez-ipupdate then die.
                eval("killall", "ez-ipupdate");
                _eval(ddns_argv, NULL, 0, &pid);

        }
#endif
	nvram_unset("rc_service");
	return 0;
}

#ifdef AUDIO_SUPPORT
int hotplug_usb_audio(char *product)
{					
	char *wave_argv[]={"waveserver", NULL};
	pid_t pid;

	if (strlen(product)==0) return;
	_eval(wave_argv, ">/dev/null", 0, NULL);
}

int remove_usb_audio(char *product)
{
	eval("killall", "waveserver");
}

int
start_audio(void)
{
	char *wave_argv[] = {"waveservermain", NULL};
	pid_t pid;

	_eval(wave_argv, NULL, 0, &pid);
	return 0;
}

int
stop_audio(void)
{
	int ret = eval("killall", "waveserver");
}
#endif
#endif

#ifdef GUEST_ACCOUNT
int
start_dhcpd_guest(void)
{
	FILE *fp;
	char *dhcpd_argv[] = {"udhcpd", "/tmp/udhcpd1.conf", NULL, NULL};
	char *slease = "/tmp/udhcpd-br1.sleases";
	pid_t pid;

	if (nvram_match("router_disable", "1") || nvram_invmatch("wl_guest_ENABLE", "1") || nvram_invmatch("lan1_proto", "dhcp") || nvram_match("mssid_ENABLE", "1"))
		return 0;

	dprintf("%s %s %s %s\n",
		nvram_safe_get("lan1_ifname"),
		nvram_safe_get("dhcp1_start"),
		nvram_safe_get("dhcp1_end"),
		nvram_safe_get("lan1_lease"));

	if (!(fp = fopen("/tmp/udhcpd-br1.leases", "a"))) {
		perror("/tmp/udhcpd-br1.leases");
		return errno;
	}
	fclose(fp);

	/* Write configuration file based on current information */
	if (!(fp = fopen("/tmp/udhcpd1.conf", "w"))) {
		perror("/tmp/udhcpd1.conf");
		return errno;
	}
	
	fprintf(fp, "pidfile /var/run/udhcpd-br1.pid\n");
	fprintf(fp, "start %s\n", nvram_safe_get("dhcp1_start"));
	fprintf(fp, "end %s\n", nvram_safe_get("dhcp1_end"));
	fprintf(fp, "interface %s\n", nvram_safe_get("lan1_ifname"));
	fprintf(fp, "remaining yes\n");
	fprintf(fp, "lease_file /tmp/udhcpd-br1.leases\n");
	fprintf(fp, "option subnet %s\n", nvram_safe_get("lan1_netmask"));
	fprintf(fp, "option router %s\n", nvram_safe_get("lan1_ipaddr"));	
	
	if (nvram_invmatch("dhcp_dns1_x",""))		
		fprintf(fp, "option dns %s\n", nvram_safe_get("dhcp_dns1_x"));		
	fprintf(fp, "option dns %s\n", nvram_safe_get("lan1_ipaddr"));
	fprintf(fp, "option lease %s\n", nvram_safe_get("lan1_lease"));

	if (nvram_invmatch("dhcp_wins_x",""))		
		fprintf(fp, "option wins %s\n", nvram_safe_get("dhcp_wins_x"));		
	if (nvram_invmatch("lan_domain", ""))
		fprintf(fp, "option domain %s\n", nvram_safe_get("lan_domain"));
	fclose(fp);

	dhcpd_argv[2] = NULL;
	_eval(dhcpd_argv, NULL, 0, &pid);


	dprintf("done\n");
	return 0;
}

int
stop_dhcpd_guest(void)
{
	char sigusr1[] = "-XX";
	int ret;

/*
* Process udhcpd handles two signals - SIGTERM and SIGUSR1
*
*  - SIGUSR1 saves all leases in /tmp/udhcpd.leases
*  - SIGTERM causes the process to be killed
*
* The SIGUSR1+SIGTERM behavior is what we like so that all current client
* leases will be honorred when the dhcpd restarts and all clients can extend
* their leases and continue their current IP addresses. Otherwise clients
* would get NAK'd when they try to extend/rebind their leases and they 
* would have to release current IP and to request a new one which causes 
* a no-IP gap in between.
*/
	ret = eval("killall", "udhcpd");

	return ret;
}
#endif

int
check_disk_free(char *res, char *diskpath)	// added by Jiahao for WL500gP
{
	char *sizebuf, *freebuf, *databuf;
	struct statfs fsbuf;

	if(statfs(diskpath, &fsbuf))
	{
		perror("*** check_disk_free: statfs fail!");
		return 2;
	}

	sizebuf = malloc(32);
	freebuf = malloc(32);
	databuf = malloc(64);

	sprintf(sizebuf, "%.1f", (double)((double)((double)fsbuf.f_blocks * fsbuf.f_bsize)/(1024*1024)));
	sprintf(freebuf, "%.1f", (double)((double)((double)fsbuf.f_bfree * fsbuf.f_bsize)/(1024*1024)));
	sprintf(res, "size: %s, free: %s", sizebuf, freebuf);

	if(sizebuf) 
		free(sizebuf);
	if(freebuf) 
		free(freebuf);
	if(databuf) 
		free(databuf);
	if(fsbuf.f_bfree == 0)
		return 0;
	else
		return 1;
}

#ifdef DLM
int
check_disk_free_GE_1G(char *diskpath)	// added by Jiahao for WL500gP
{
	struct statfs fsbuf;
	double free_size, block_size;

	if(statfs(diskpath, &fsbuf))
	{
		perror("*** check_disk_free_GE_1G: statfs fail!");
		return 0;
	}

	free_size = (double)((double)((double)fsbuf.f_bfree * fsbuf.f_bsize)/(1024*1024));
	block_size = (double)((double)((double)fsbuf.f_blocks * fsbuf.f_bsize)/(1024*1024));
	printf("\nchk disk: free=%fMB, blocksize=%fMB\n", free_size, block_size);	// tmp test

	if ( free_size < (double)128 )
		return 1;
	else if ( block_size > (double)1024 )
		return 2;
	else
		return 3;
}

int
check_disk_free_apps(char *diskpath, int ac_flag)			// added by Jiahao for WL500gP
{
	struct statfs fsbuf;
	double free_size;

	//printf("chk disk free apps\n");	// tmp test
	if(statfs(diskpath, &fsbuf))
	{
		perror("*** check_disk_free_apps: statfs fail!");
		return 0;
	}

	free_size = (double)((double)((double)fsbuf.f_bfree * fsbuf.f_bsize)/(1024*1024));
	//printf("free_size = %f\n", free_size);	// tmp test

	if (ac_flag==1)
	{
		if (nvram_match("apps_dlx", "0"))
			return 1;

		if ( free_size < (double)1 )
			return 0;
		else
			return 1;
	}
	else
	{
		if ( free_size < (double)5 )
			return 0;
		else
			return 1;
	}
}
#endif

#ifdef DLM
void set_apps_ver()
{
	nvram_set("apps_ver", "1.00");
}

void exec_apps()
{	
	FILE *fp;
	
	char pool[32];
	char share[32];
	char rundl[8];
	char rundms[8];
	char name[32];
	char mymac[32];
	int cap=0;
	int ret=0;

	char EXBIN[32]="/.apps/bin";
	char test_path[128];

	char tmpstr[256];
	char tmpstr2[256];
	char share_port_from[5];
	char share_port_to[5];

	//printf("\n## exec apps\n");	// tmp test
	memset(test_path, 0, sizeof(test_path));
	strcpy(test_path, "/tmp/harddisk/part0");
	strcpy(tmpstr, EXBIN);
	sprintf(EXBIN, "%s%s", test_path, tmpstr);
	
	eval("killall", "dms");
	eval("killall", "snarf");
	eval("killall", "rtorrent");
	eval("killall", "giftd");
	eval("killall", "dmathined");

	nvram_set("apps_installed", "0");
	nvram_set("apps_dl_x", "0");

	strcpy(pool, nvram_safe_get("apps_pool"));
	strcpy(share, nvram_safe_get("apps_share"));
	strcpy(rundl, nvram_safe_get("apps_dlx"));
	strcpy(rundms, nvram_safe_get("apps_dmsx"));
	strcpy(name, nvram_safe_get("computer_name"));
	strcpy(mymac, nvram_safe_get("et0macaddr"));
	
	strcpy(share_port_from, nvram_safe_get("apps_dl_share_port_from"));
	strcpy(share_port_to, nvram_safe_get("apps_dl_share_port_to"));
	
	fp=fopen("/tmp/.prange", "w");
	if(fp)
	{
		fprintf(fp,"%s:%s", share_port_from, share_port_to);
		fclose(fp);
	}
	if (nvram_match("apps_dlx", "1") && nvram_match("swap_on", "1"))
	{
		sprintf(tmpstr2, "%s/dmex", EXBIN);
		ret=system(tmpstr2);	
		if(!ret)
		{
			nvram_set("apps_dl_x", "1");
			logmessage("Download Master", "daemon is started");
		}
	}
	nvram_set("apps_installed", "1");
	nvram_set("apps_status_checked", "0");
}

void init_apps()
{
	char pool[32];
	char share[32];
	char origver[32];
	char target_dir[32];
	char pool_dir[32];
	char EXLIB[128]="/.apps/lib";
	char EXBIN[128]="/.apps/bin";
	char EXUSR[128]="/.apps/usr";
	char EXETC[32];
	char NEWVER[8];
	char tmpstr[256];
	char tmpstr2[256];
	char tmpstr3[256];
	char tmpstr4[256];

	char test_path[128];
	char *p;

	memset(test_path, 0, sizeof(test_path));
	strcpy(test_path, "/tmp/harddisk/part0");
	//nvram_set("apps_pool", nvram_safe_get("usb_mnt_first_path"));
	nvram_set("apps_pool", "harddisk/part0");
	
	strcpy(tmpstr, EXLIB);
	sprintf(EXLIB, "%s%s", test_path, tmpstr);
	strcpy(tmpstr, EXBIN);
	sprintf(EXBIN, "%s%s", test_path, tmpstr);
	strcpy(tmpstr, EXUSR);
	sprintf(EXUSR, "%s%s", test_path, tmpstr);

	strcpy(pool, nvram_safe_get("apps_pool"));
	strcpy(share, nvram_safe_get("apps_share"));
	strcpy(origver, nvram_safe_get("apps_ver"));
	sprintf(target_dir, "/shares/%s/%s", pool, share);
	sprintf(pool_dir, "/shares/%s", pool);
	sprintf(EXETC, "%s/.etc", pool_dir);

	eval("rm", "-rf", "/shares/lib");
	eval("rm", "-rf", "/shares/bin");
	eval("rm", "-rf", "/shares/usr");
	eval("rm", "-rf", "/shares/etc");
	eval("rm", "-rf", EXETC);

	mkdir_if_none(pool_dir);
	mkdir_if_none(target_dir);

	eval("ln", "-s", EXLIB, "/shares/lib");
	eval("ln", "-s", EXBIN, "/shares/bin");
	eval("ln", "-s", EXUSR, "/shares/usr");
	
	set_apps_ver();
	sprintf(NEWVER, nvram_safe_get("apps_ver"));

	sprintf(tmpstr, "%s/Music", target_dir);
	if (mkdir_if_none(tmpstr)==0)
		printf("Music DIR exist\n");

	sprintf(tmpstr, "%s/Video", target_dir);
	if (mkdir_if_none(tmpstr)==0)
		printf("Video DIR exist\n");

	sprintf(tmpstr, "%s/Photo", target_dir);
	if (mkdir_if_none(tmpstr)==0)
		printf("PHOTO DIR exist\n");

	sprintf(tmpstr, "%s/Download", target_dir);
	if (mkdir_if_none(tmpstr)==0)
		printf("DOWNLOAD DIR exist\n");

	sprintf(tmpstr, "%s/Download/config", target_dir);
	mkdir_if_none(tmpstr);
	sprintf(tmpstr, "%s/Download/Complete", target_dir);
	mkdir_if_none(tmpstr);
	sprintf(tmpstr, "%s/Download/InComplete", target_dir);
	mkdir_if_none(tmpstr);
	sprintf(tmpstr, "%s/Download/.logs", target_dir);
	mkdir_if_none(tmpstr);	

	eval("rm", "-rf", EXETC);
	mkdir_if_none(EXETC);
	eval("ln", "-s", EXETC, "/shares/etc");
	
	sprintf(tmpstr, "%s/asus_gift.conf", EXETC);
	if (check_if_file_exist(tmpstr)==1)
		printf("FILE asus_gift exist\n");
	else
	{
		sprintf(tmpstr, "%s/gift-nasoc/bin/asus_gift.conf", EXUSR);
		sprintf(tmpstr2, "cp %s %s/. -f", tmpstr, EXETC);
		system(tmpstr2);
	}
	
	sprintf(tmpstr, "%s/.giFT", EXETC);
	if (mkdir_if_none(tmpstr)==0)
		printf(".giFT DIR exist\n");
	else
	{
		sprintf(tmpstr2, "%s/.", tmpstr);
		sprintf(tmpstr4, "cp /shares/usr/.giFT/* %s -f", tmpstr2);
		system(tmpstr4);
		
		sprintf(tmpstr2, "%s/FastTrack", tmpstr);
		mkdir_if_none(tmpstr2);

		sprintf(tmpstr3, "%s/.", tmpstr2);
		sprintf(tmpstr4, "cp /shares/usr/.giFT/FastTrack/* %s -f", tmpstr3);
		system(tmpstr4);

		sprintf(tmpstr2, "%s/OpenFT", tmpstr);
		mkdir_if_none(tmpstr2);

		sprintf(tmpstr3, "%s/.", tmpstr2);
		sprintf(tmpstr4, "cp /shares/usr/.giFT/OpenFT/* %s -f", tmpstr3);
		system(tmpstr4);
		
		sprintf(tmpstr2, "%s/Gnutella", tmpstr);
		mkdir_if_none(tmpstr2);

		sprintf(tmpstr3, "%s/.", tmpstr2);
		sprintf(tmpstr4, "cp /shares/usr/.giFT/Gnutella/* %s -f", tmpstr3);
		system(tmpstr4);
		
		sprintf(tmpstr2, "%s/completed", tmpstr);
		mkdir_if_none(tmpstr2);
		sprintf(tmpstr2, "%s/completed/corrupted", tmpstr);
		mkdir_if_none(tmpstr2);
		sprintf(tmpstr2, "%s/ui", tmpstr);
		mkdir_if_none(tmpstr2);

		sprintf(tmpstr3, "%s/.", tmpstr2);
		sprintf(tmpstr4, "cp /shares/usr/.giFT/ui/* %s -f", tmpstr3);
		system(tmpstr4);
		
		sprintf(tmpstr, "%s/giFT", EXETC);
		mkdir_if_none(tmpstr);

		sprintf(tmpstr2, "%s/.", tmpstr);
		sprintf(tmpstr4, "cp /shares/usr/gift-nasoc/share/giFT/* %s -f", tmpstr2);
		system(tmpstr4);
		
		sprintf(tmpstr2, "%s/FastTrack", tmpstr);
		mkdir_if_none(tmpstr2);
		sprintf(tmpstr2, "%s/OpenFT", tmpstr);
		mkdir_if_none(tmpstr2);		
		sprintf(tmpstr2, "%s/Gnutella", tmpstr);
		mkdir_if_none(tmpstr2);
		sprintf(tmpstr2, "%s/completed", tmpstr);
		mkdir_if_none(tmpstr2);
		sprintf(tmpstr2, "%s/completed/corrupted", tmpstr);
		mkdir_if_none(tmpstr2);
		sprintf(tmpstr2, "%s/ui", tmpstr);
		mkdir_if_none(tmpstr2);

		sprintf(tmpstr3, "%s/.", tmpstr2);
		sprintf(tmpstr4, "cp /shares/usr/gift-nasoc/share/giFT/ui/* %s -f", tmpstr3);
		system(tmpstr4);
	}
	
	eval("rm" ,"-rf" ,"/shares/DMSRoot");
	mkdir_if_none("/shares/DMSRoot");
	
	sprintf(tmpstr, "%s/Download/Complete", target_dir);
	eval("ln", "-s", tmpstr, "/shares/DMSRoot/Download");
	sprintf(tmpstr, "%s/Video", target_dir);
	eval("ln", "-s", tmpstr, "/shares/DMSRoot/Video");
	sprintf(tmpstr, "%s/Music", target_dir);
	eval("ln", "-s", tmpstr, "/shares/DMSRoot/Music");
	sprintf(tmpstr, "%s/Photo", target_dir);
	eval("ln", "-s", tmpstr, "/shares/DMSRoot/Photo");
	
	eval("rm" ,"-rf" ,"/shares/dmathined");
	mkdir_if_none("/shares/dmathined");
	mkdir_if_none("/shares/dmathined/Download");

	sprintf(tmpstr, "%s/Download/config", target_dir);
	eval("ln", "-s", tmpstr, "/shares/dmathined/Download/config");
	sprintf(tmpstr, "%s/Download/Complete", target_dir);
	eval("ln", "-s", tmpstr, "/shares/dmathined/Download/Complete");
	sprintf(tmpstr, "%s/Download/InComplete", target_dir);
	eval("ln", "-s", tmpstr, "/shares/dmathined/Download/InComplete");
	sprintf(tmpstr, "%s/Download/.logs", target_dir);
	eval("ln", "-s", tmpstr, "/shares/dmathined/Download/.logs");	

	system("chmod 777 /tmp/harddisk/part0/.apps/bin/*");
	system("chmod 777 /tmp/harddisk/part0/.apps/lib/*");
	system("chmod 777 /tmp/harddisk/part0/.apps/usr/gift-nasoc/lib/*");
	system("chmod 777 /tmp/harddisk/part0/.apps/usr/gift-nasoc/bin/*");
	system("chmod 777 /tmp/harddisk/part0/share");
	system("chmod 777 /tmp/harddisk/part0/share/Download");
	system("chmod 777 /tmp/harddisk/part0/share/Download/InComplete");
	system("chmod 777 /tmp/harddisk/part0/share/Download/Complete");
}

/*
 * st_ftp_modex: 0:no-ftp, 1:anonymous, 2:account 
 */

void
run_ftp()
{
	printf("run ftp\n");	// tmp test
	//if (nvram_invmatch("st_ftp_modex", "0"))
	//{
	fprintf(stderr, "starting vsftpd\n");
	if (start_ftpd()==0)
	{
		nvram_set("ftp_running", "1");
	}
	//}
}

void stop_ftp(){
        if(nvram_match("ftp_running", "0"))
                return ;

        eval("killall", "-SIGKILL", "vsftpd");
        unlink("/tmp/vsftpd.conf");

        logmessage("FTP Server", "daemon is stoped");

        nvram_set("ftp_running", "0");
}

int
run_apps()
{
	int apps_comp=0;
	int apps_comp_pre=0;
	int apps_status=0;
	int apps_disk_free=0;
	int ret=0, i;
	char tmp[32];
	char tmpstr[64];
	char tmpstr2[64];
	FILE *fp;
	char *buf=NULL;
	int buflen=0;
	struct sysinfo info;

	printf("\nrun apps\n");	// tmp test
	system("rm -Rf /tmp/harddisk/part0/.apps");
	if(rename("/tmp/harddisk/part0/share/.apps", "/tmp/harddisk/part0/.apps") < 0)
	{
		perror("mv apps fail\n");
		//return -1;
	}

        if((apps_comp=ckeck_apps_completeness("/tmp/harddisk/part0"))==1)
                nvram_set("apps_comp", "1");
        else
        {
                printf("invalid apps\n");   // tmp test
                //system("rm -Rf /media/AiDisk_a1/.apps");	// tmp disable
                nvram_set("apps_comp", "0");
		return -1;
        }

	nvram_set("usb_storage_busy", "1");
	init_apps();
	exec_apps();

	return 0;
}

int
swap_check()
{
	struct sysinfo info;

	system(&info);
	if(info.totalswap>0)
		return 1;
	else
		return 0;
}
#endif

int					// added by Jiahao for WL500gP
check_if_dir_exist(char *dir)
{
	DIR *dp;
	if(!(dp=opendir(dir)))
		return 0;
	closedir(dp);
	return 1;
}

int					// added by Jiahao for WL500gP
check_if_file_exist(char *filename)
{
	FILE *fp;
	fp=fopen(filename, "r");
	if(fp)
	{
		fclose(fp);
		return 1;
	}
	else
		return 0;
}

int
mkdir_if_none(char *dir)		// added by Jiahao for WL500gP
{
	DIR *dp;
	if(!(dp=opendir(dir)))
	{
		umask(0000);
		mkdir(dir, 0777);
		return 1;
	}
	closedir(dp);
	return 0;
}

//2008.10 magic{
int start_networkmap(void)
{
	char *networkmap_argv[] = {"networkmap", NULL};
	pid_t pid;
	
	_eval(networkmap_argv, NULL, 0, &pid);
	//_eval(networkmap_argv, NULL, 0, NULL);
	
	return 0;
}

//2008.10 magic}

int stop_networkmap()
{
        return eval("killall", "networkmap");
}

int 
restart_task()
{
	if(!(task_mask & (1 << TASK_HTTPD)))
		start_httpd();
	if(!(task_mask & (1 << TASK_UDHCPD)))
		start_dhcpd();
	if(!(task_mask & (1 << TASK_LLD2D)))
		start_lltd();
	if(!(task_mask & (1 << TASK_WANDUCK)))
		start_wanduck();
	if(!(task_mask & (1 << TASK_UDHCPC)))
		start_udhcpc();
	if(!(task_mask & (1 << TASK_NETWORKMAP)))
		start_networkmap();
	if(!(task_mask & (1 << TASK_DPROXY)))
		start_dns();
	if(!(task_mask & (1 << TASK_NTP)))
		start_ntpc();
	if(!(task_mask & (1 << TASK_U2EC)))
		start_u2ec();
	if(!(task_mask & (1 << TASK_OTS)))
		start_ots();
	if(!(task_mask & (1 << TASK_LPD)))
		start_lpd();
	if(!(task_mask & (1 << TASK_UPNPD)))
		start_upnp();
	if(!(task_mask & (1 << TASK_WATCHDOG)))
		start_watchdog();
	if(!(task_mask & (1 << TASK_INFOSVR)))
		start_infosvr();
	if(!(task_mask & (1 << TASK_SYSLOGD)))
		start_syslogd();
	if(!(task_mask & (1 << TASK_KLOGD)))
		start_klogd();
	//if(!(task_mask & (1 << TASK_PPPD)))	// illegal if wan down
	//	start_pppd();
	if(!(task_mask & (1 << TASK_PPPOE_RELAY)))
		start_pppoe_relay(nvram_safe_get("wan_ifname"));
	if(!(task_mask & (1 << TASK_IGMP)))
		start_igmpproxy("eth2.2");
}

int
check_task(char *cmd)
{
        if(strstr(cmd, "httpd")){
                task_mask |= (1 << TASK_HTTPD);
                return 0;
        } else if(strstr(cmd, "udhcpd")){
                task_mask |= (1 << TASK_UDHCPD);
                return 0;
        } else if(strstr(cmd, "lld2d")){
                task_mask |= (1 << TASK_LLD2D);
                return 0;
        } else if(strstr(cmd, "wanduck")){
                task_mask |= (1 << TASK_WANDUCK);
                return 0;
        } else if(strstr(cmd, "udhcpc")){
                task_mask |= (1 << TASK_UDHCPC);
                return 0;
        } else if(strstr(cmd, "networkmap")){
                task_mask |= (1 << TASK_NETWORKMAP);
                return 0;
        } else if(strstr(cmd, "dproxy")){
                task_mask |= (1 << TASK_DPROXY);
                return 0;
        } else if(strstr(cmd, "ntp")){
                task_mask |= (1 << TASK_NTP);
                return 0;
        //} else if(strstr(cmd, "u2ec")){
        //        task_mask |= (1 << TASK_U2EC);
        //        return 0;
        } else if(strstr(cmd, "ots")){
                task_mask |= (1 << TASK_OTS);
                return 0;
        } else if(strstr(cmd, "lpd")){
                task_mask |= (1 << TASK_LPD);
                return 0;
        } else if(strstr(cmd, "upnpd")){
                task_mask |= (1 << TASK_UPNPD);
                return 0;
        } else if(strstr(cmd, "watchdog")){
                task_mask |= (1 << TASK_WATCHDOG);
                return 0;
        } else if(strstr(cmd, "infosvr")){
                task_mask |= (1 << TASK_INFOSVR);
                return 0;
        } else if(strstr(cmd, "syslogd")){
                task_mask |= (1 << TASK_SYSLOGD);
                return 0;
        } else if(strstr(cmd, "klogd")){
                task_mask |= (1 << TASK_KLOGD);
                return 0;
        } else if(strstr(cmd, "pppd")){
                task_mask |= (1 << TASK_PPPD);
                return 0;
        } else if(strstr(cmd, "pppoe-relay")){
                task_mask |= (1 << TASK_PPPOE_RELAY);
                return 0;
        } else if(strstr(cmd, "igmpproxy")){
                task_mask |= (1 << TASK_IGMP);
                return 0;
        }
        return -1;
}

int
check_all_tasks()
{
        DIR  *dir;
        struct dirent *dent;
        char task_file[50], cmdline[64];
        int pid, fd;

        if(!(dir=opendir("/proc")))
        {
                perror("open proc");
                return -1;
        }

	task_mask = 0;
        while(dent = readdir(dir))
        {
                if((pid=atoi(dent->d_name)) > 1)
                {
                        memset(task_file, 0, sizeof(task_file));
                        sprintf(task_file, "/proc/%d/cmdline", pid);
                        if((fd=open(task_file, O_RDONLY)) > 0)
                        {
                                memset(cmdline, 0, sizeof(cmdline));
                                read(fd, cmdline, sizeof(cmdline));
                                check_task(cmdline);
				close(fd);
                        } else
				printf("cannot open %s\n", task_file);
                }
        }

        printf("** task mask is %lu\n", task_mask); // tmp test
	restart_task();

        closedir(dir);
        return 0;
}

int
check_pppd()
{
        DIR  *dir;
        struct dirent *dent;
        char task_file[50], cmdline[64];
        int pid, fd;
	int ret = 0;

        if(!(dir=opendir("/proc")))
        {
                perror("open proc");
                return -1;
        }

        task_mask = 0;
        while(dent = readdir(dir))
        {
                if((pid=atoi(dent->d_name)) > 1)
                {
                        memset(task_file, 0, sizeof(task_file));
                        sprintf(task_file, "/proc/%d/cmdline", pid);
                        if((fd=open(task_file, O_RDONLY)) > 0)
                        {
                                memset(cmdline, 0, sizeof(cmdline));
                                read(fd, cmdline, sizeof(cmdline));
				if(strstr(cmdline, "pppd"))
				{
					ret = 1;
                                	close(fd);
					break;
				}
                                close(fd);
                        } else
                                printf("cannot open %s\n", task_file);
                }
        }

        closedir(dir);
        return ret;
}
