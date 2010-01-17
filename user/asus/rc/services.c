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
 * Miscellaneous services
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: services.c,v 1.1.1.1 2007/01/25 12:52:21 jiahao_jhou Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <nvram/bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <rc.h>

#ifndef ASUS_EXT
int
start_dhcpd(void)
{
	if (nvram_match("router_disable", "1") || nvram_invmatch("lan_proto", "dhcp"))
		return 0;

	FILE *fp;
	char name[100];
	int dhcp_lease_time;


	/* Touch leases file */
	if (!(fp = fopen("/tmp/udhcpd.leases", "a"))) {
		perror("/tmp/udhcpd.leases");
		return errno;
	}
	fclose(fp);

	/* Write configuration file based on current information */
	if (!(fp = fopen("/tmp/udhcpd.conf", "w"))) {
		perror("/tmp/udhcpd.conf");
		return errno;
	}
	fprintf(fp, "pidfile /var/run/udhcpd.pid\n");
	fprintf(fp, "start %s\n", nvram_safe_get("dhcp_start"));
	fprintf(fp, "end %s\n", nvram_safe_get("dhcp_end"));
	fprintf(fp, "interface %s\n", nvram_safe_get("lan_ifname"));
	fprintf(fp, "remaining yes\n");
	fprintf(fp, "lease_file /tmp/udhcpd.leases\n");
	fprintf(fp, "option subnet %s\n", nvram_safe_get("lan_netmask"));
	fprintf(fp, "option router %s\n", nvram_safe_get("lan_ipaddr"));
	if((dhcp_lease_time = atoi(nvram_safe_get(dhcp_lease))) <= 3)
		fprintf(fp, "option lease 86400\n");
	else
		fprintf(fp, "option lease %d\n", dhcp_lease_time);
	fprintf(fp, "option dns %s\n", nvram_safe_get("lan_ipaddr"));
	//fprintf(fp, "option lease %s\n", nvram_safe_get("lan_lease"));
	snprintf(name, sizeof(name), "%s_wins", nvram_safe_get("dhcp_wins"));
	if (nvram_invmatch(name, ""))
		fprintf(fp, "option wins %s\n", nvram_get(name));
	snprintf(name, sizeof(name), "%s_domain", nvram_safe_get("dhcp_domain"));
	if (nvram_invmatch(name, ""))
		fprintf(fp, "option domain %s\n", nvram_get(name));
	fclose(fp);

        char *dhcpd_argv[] = {"udhcpd", "/tmp/udhcpd.conf", NULL};
        int upid;
        _eval(dhcpd_argv, NULL, 0, &upid);

	return 0;
}

int
stop_dhcpd(void)
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
	sprintf(sigusr1, "-%d", SIGUSR1);
	eval("killall", sigusr1, "udhcpd");
	ret = eval("killall", "udhcpd");

	dprintf("done\n");
	return ret;
}

#endif	// ASUS_EXT

int
chpass(char *user, char *pass)
{
	char cmdbuf[128];

	if(!user)
		user = "admin";
	if(!pass)
		pass = "admin";

	memset(cmdbuf, 0, sizeof(cmdbuf));
	sprintf(cmdbuf, "echo '%s::0:0:Adminstrator:/:/bin/sh' > /etc/passwd", user);
	system(cmdbuf);
	memset(cmdbuf, 0, sizeof(cmdbuf));
	sprintf(cmdbuf, "echo '%s:x:0:%s' > /etc/group", user, user);
	system(cmdbuf);
	memset(cmdbuf, 0, sizeof(cmdbuf));
	sprintf(cmdbuf, "chpasswd.sh %s %s", user, pass);
	system(cmdbuf);

	return 0;
}

int 
start_telnetd()
{
	char *telnetd_argv[] = {"telnetd", NULL};

	system("killall telnetd");

	chpass(nvram_safe_get("http_username"), nvram_safe_get("http_passwd"));
	system("hostname RTNL");

	//return s_eval2(telnetd_argv, NULL, 0, NULL);
	return system("telnetd");
}

int 
stop_telnetd()
{
	return system("killall telnetd");
}

int
start_httpd(void)
{
        int ret;
        pid_t pid;
	char *httpd_argv[] = {"httpd", "eth2.2", NULL};

        chdir("/www");
        //ret = eval("httpd", "eth2.2");
        //ret = s_eval2(httpd_argv, NULL, 0, NULL);
        ret = system("httpd eth2.2 &");

        chdir("/");

        logmessage(LOGNAME, "start httpd");

        return ret;
}

int
stop_httpd(void)
{
	int ret = eval("killall", "httpd");

	return ret;
}

int 
start_upnp()
{
	char cmdbuf[64];
	char *wan_proto;
        char *upnp_argv1[] = {"upnpd", "-f", "ppp0", "br0", NULL};
        char *upnp_argv2[] = {"upnpd", "-f", "eth2.2", "br0", NULL};
        //char *upnp_argv1[] = {"upnpd", "ppp0", "br0", NULL};
        //char *upnp_argv2[] = {"upnpd", "eth2.2", "br0", NULL};
        pid_t pid;

        if (!nvram_invmatch("upnp_ENABLE", "0") || nvram_match("router_disable", "1"))
                return 0;

	system("route add -net 239.0.0.0 netmask 255.0.0.0 dev br0");
	memset(cmdbuf, 0, sizeof(cmdbuf));
	sprintf(cmdbuf, "upnp_xml.sh %s", nvram_safe_get("lan_ipaddr_t"));
	system(cmdbuf);

	nvram_set("upnp_running", "1");
	//printf("##set upnp_running\n");	// tmp test
	wan_proto = nvram_safe_get("wan_proto");
        if (strcmp(wan_proto, "pppoe") == 0 || 
	    strcmp(wan_proto, "pptp") == 0 || 
	    strcmp(wan_proto, "l2tp") == 0)
	{
		system("upnpd -f ppp0 br0 &");
		//_eval(upnp_argv1, NULL, 0, &pid);
		//s_eval2(upnp_argv1, NULL, 0, NULL);
	}
	else
	{
		system("upnpd -f eth2.2 br0 &");
		//_eval(upnp_argv2, NULL, 0, &pid);
		//s_eval2(upnp_argv2, NULL, 0, NULL);
	}

	return 0;
}

int
stop_upnp(void)
{
	int ret = 0;

	nvram_set("upnp_running", "0");

	//if (nvram_invmatch("upnp_ENABLE", "0"))
	ret = eval("killall", "upnpd");

	//return 0;
	return ret;
}

int
start_nas(char *type)
{
	char cfgfile[64];
	char pidfile[64];
	if (!type || !*type)
		type = "lan";
	snprintf(cfgfile, sizeof(cfgfile), "/tmp/nas.%s.conf", type);
	snprintf(pidfile, sizeof(pidfile), "/tmp/nas.%s.pid", type);
	{
		char *argv[] = {"nas", cfgfile, pidfile, type, NULL};
		pid_t pid;

		_eval(argv, NULL, 0, &pid);
		dprintf("done\n");
	}
	return 0;
}

int
stop_nas(void)
{
	int ret = eval("killall", "nas");

	dprintf("done\n");
	return ret;
}

int
start_ntpc(void)
{
#ifdef ASUS_EXT
	char *ntp_argv[] = {"ntp", NULL};
	pid_t pid;

	//_eval(ntp_argv, NULL, 0, &pid);
	system("ntp &");
	//_eval(ntp_argv, NULL, 0, NULL);	// it will be blocked
#else
	char *servers = nvram_safe_get("ntp_server");
	
	if (strlen(servers)) {
		char *nas_argv[] = {"/usr/sbin/ntpclient", "-h", servers, "-i", "3", "-l", "-s", NULL};
		_eval(nas_argv, NULL, 0, NULL);
	}
	
#endif
	return 0;
}

int
stop_ntpc(void)
{
        //if(nvram_match("wan_nat_X", "0"))
        //        return 0;
#ifdef ASUS_EXT
	int ret = eval("killall", "ntp");
#else
	int ret = eval("killall", "ntpclient");
#endif

	return ret;
}

/* +++ Cherry Cho added in 2006/12/14. +++ */
int start_lltd(void)
{
        pid_t pid;
        char *lltd_argv[] = {"lld2d", "br0", NULL};
	char lltd_cmd[32];

	memset(lltd_cmd, 0, sizeof(lltd_cmd));
	sprintf(lltd_cmd, "lld2d br0");
	system(lltd_cmd);
	
	return 0;
}

/* Cherry Cho added in 2006/12/14. */
int stop_lltd(void)
{
	int ret;
	
	ret = eval("killall", "lld2d");

	return ret;
}

int 
start_lpd()
{
	unlink("/var/run/lpdparent.pid");
	return system("lpd &");
}

int 
stop_lpd()
{
	unlink("/var/run/lpdparent.pid");
	return system("killall lpd");
}

void 
enable_greenEthernet()
{
	eval("set8366s", "83", "3");
}

int
start_services(void)
{
	printf("[rc] start services\n");	// tmp test

//#if 0	// disable for tmp
	start_8021x();
	//start_telnetd();	// put it later
	start_httpd();
	start_dhcpd();
	//start_dns();		// put it earily
	start_infosvr();
#ifdef ASUS_EXT
	start_logger();
#endif
	start_lpd();
	start_upnp();		// no need run earily

#ifdef ASUS_EXT
	start_usb();
#endif
//#endif

#ifdef GUEST_ACCOUNT
#ifdef RANGE_EXTENDER
	if (nvram_match("wl_mode_EX", "re"))
	{
	}
	else
#endif
	if (nvram_match("wl_guest_ENABLE", "1") && nvram_match("wl_mode_EX", "ap") && nvram_match("wan_nat_X", "1"))
	{
		sleep(5);
		start_dhcpd_guest();
		start_guest_nas(); 
	}
#endif

//# if 0	// disable for tmp
	start_lltd(); /* Cherry Cho added in 2006/12/14. */

#ifdef W7_LOGO
#else
//#if 0	// disable for tmp, for win7 logo test
#ifdef WEB_REDIRECT
        if(!nvram_match("wanduck_down", "1")
                        /*&& nvram_match("wan_nat_x", "1")*/
			&& (!nvram_match("sw_mode", "3"))	// besides ap mode
		){
                printf("--- START: Wait to start wanduck ---\n");	// tmp test
                redirect_setting();
                start_wanduck();
                sleep(1);
        }
#endif
#endif

//#endif

        if (nvram_match("lan_stp", "1") && !is_ap_mode())
        {
                fprintf(stderr, "resume stp forwarding delay and hello time\n");
                eval("brctl", "setfd", "br0", "15");
                eval("brctl", "sethello", "br0", "2");
        }

	return 0;
}

int
stop_logger(void)
{
        int ret1 = eval("killall", "klogd");
        int ret2 = eval("killall", "syslogd");

        return (ret1|ret2);
}

int
stop_services(void)
{
#ifdef ASUS_EXT
#endif
	stop_upnp();
#ifdef ASUS_EXT
	stop_logger();
#endif

#ifdef GUEST_ACCOUNT
#ifdef RANGE_EXTENDER
	if(nvram_match("wl_mode_EX", "re"))
	{
		//do nothing
	}
	else
#endif
	stop_dhcpd_guest();
#endif
	stop_dhcpd();
	stop_dns();
	stop_httpd();

	stop_lltd();/* Cherry Cho added in 2006/12/14. */	
	stop_lpd();
	return 0;
}

#ifdef GUEST_ACCOUNT
/* Start NAS for the guest interfaces */
int
start_guest_nas(void)
{
	char *unbridged_interfaces;
	char *next;
	char name[IFNAMSIZ],lan[IFNAMSIZ];
	int index;
		
	unbridged_interfaces = nvram_get("unbridged_ifnames");
	
	if (unbridged_interfaces)
		foreach(name,unbridged_interfaces,next){
			index = get_ipconfig_index(name);
			if (index < 0) 
				continue;
			snprintf(lan,sizeof(lan),"lan%d",index);
			start_nas(lan);
		}

	return 0;
}
#endif
// 2008.10 magic {
#ifdef WEB_REDIRECT
int start_wanduck(void){
	
        if(nvram_match("wanduck_down", "1") || (nvram_match("sw_mode", "3")))
		return -1;
                
        char *argv[] = {"/usr/sbin/wanduck", NULL};
        int ret = 0;
        pid_t pid;
        FILE *fp = fopen("/var/run/wanduck.pid", "r");
	char *wan_proto_type = nvram_safe_get("wan0_proto");
	int i;

        if(fp != NULL){
                fclose(fp);
                return 0;
        }

	if(wan_proto_type && (strcmp(wan_proto_type, "pptp") || strcmp(wan_proto_type, "l2tp"))) // delay run
	{
		printf("\ndelay run wanduck\n");	// tmp test
		sleep(2);		
        	ret = _eval(argv, NULL, 0, &pid);
	}

        return ret;
}

int stop_wanduck(void){
	//kill_pidfile_s("/var/run/wanduckmain.pid", SIGUSR2);
	
	kill_pidfile_s("/var/run/wanduck.pid", SIGTERM);

	return 0;
}
/*
int start_wanduck_main(void){
	char *argv[] = {"/sbin/wanduckmain", NULL};
	int ret = 0;
	pid_t pid;
	FILE *fp = fopen("/var/run/wanduckmain.pid", "r");
	
	if(fp != NULL){
		fclose(fp);
		return 0;
	}
	
	ret = _eval(argv, NULL, 0, &pid);
	
	return ret;
}

int stop_wanduck_main(void){
	char *argv[] = {"killall", "wanduckmain", NULL};
	int ret;
	pid_t pid;
	FILE *fp = fopen("/var/run/wanduckmain.pid", "r");
	
	if(fp == NULL)
		return 0;
	fclose(fp);
	
	ret = _eval(argv, NULL, 0, &pid);
	
	return ret;
}
*/
#endif
// 2008.10 magic }
