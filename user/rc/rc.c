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
 * $Id: rc.c,v 1.1.1.1 2007/01/25 12:52:21 jiahao_jhou Exp $
 */

#include <stdio.h>
#include <stdlib.h>
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
#include <sys/mount.h>
#include <sys/vfs.h>

#include <rc.h>
#include <rc_event.h>
#include <shutils.h>
#include <nvram/typedefs.h>
#include <nvram/bcmnvram.h>
#include <nvparse.h>
#include "rtl8366s.h"
#include <semaphore_mfp.h>
#include <ra3052.h>

static void restore_defaults(void);
static void sysinit(void);
static void rc_signal(int sig);

int remove_usb_mass(char *product);
void usbtpt(int argc, char *argv[]);
int start_telnetd();
int start_telnetd2();	// tmp test
int start_telnetd3();
void print_sw_mode();

extern struct nvram_tuple router_defaults[];

static int noconsole = 0;

static const char *const environment[] = {
	"HOME=/",
	"PATH=/sbin:/usr/sbin:/bin:/usr/bin",
	"SHELL=/bin/sh",
	"USER=root",
	NULL
};

//2008.10 magic{
#define csprintf(fmt, args...) do{\
	FILE *cp = fopen("/dev/console", "w");\
	if(cp) {\
		fprintf(cp, fmt, ## args);\
		fclose(cp);\
	}\
}while(0)
//2008.10 magic}

#ifdef GUEST_ACCOUNT
static void
virtual_radio_restore_defaults(void)
{
	char tmp[100], prefix[] = "wlXXXXXXXXXX_mssid_";
	int i,j;
	
	nvram_unset("unbridged_ifnames");
	nvram_unset("ure_disable");
	
	/* Delete dynamically generated variables */
	for (i = 0; i < MAX_NVPARSE; i++) {
		sprintf(prefix, "wl%d_", i);
		nvram_unset(strcat_r(prefix, "vifs", tmp));
		nvram_unset(strcat_r(prefix, "ssid", tmp));
		nvram_unset(strcat_r(prefix, "guest", tmp));
		nvram_unset(strcat_r(prefix, "ure", tmp));
		nvram_unset(strcat_r(prefix, "ipconfig_index", tmp));
		sprintf(prefix, "lan%d_", i);
		nvram_unset(strcat_r(prefix, "ifname", tmp));
		nvram_unset(strcat_r(prefix, "ifnames", tmp));
		nvram_unset(strcat_r(prefix, "gateway", tmp));
		nvram_unset(strcat_r(prefix, "proto", tmp));
		nvram_unset(strcat_r(prefix, "ipaddr", tmp));
		nvram_unset(strcat_r(prefix, "netmask", tmp));
		nvram_unset(strcat_r(prefix, "lease", tmp));
		sprintf(prefix, "dhcp%d_", i);
		nvram_unset(strcat_r(prefix, "start", tmp));
		nvram_unset(strcat_r(prefix, "end", tmp));
		
		/* clear virtual versions */
		for (j=0; j< 16;j++){
			sprintf(prefix, "wl%d.%d_", i,j);
			nvram_unset(strcat_r(prefix, "ssid", tmp));
			nvram_unset(strcat_r(prefix, "ipconfig_index", tmp));
			nvram_unset(strcat_r(prefix, "guest", tmp));		
			nvram_unset(strcat_r(prefix, "closed", tmp));
			nvram_unset(strcat_r(prefix, "wpa_psk", tmp));
			nvram_unset(strcat_r(prefix, "auth", tmp));
			nvram_unset(strcat_r(prefix, "wep", tmp));
			nvram_unset(strcat_r(prefix, "auth_mode", tmp));
			nvram_unset(strcat_r(prefix, "crypto", tmp));
			nvram_unset(strcat_r(prefix, "akm", tmp));
		}
	}
}
#endif

static void
restore_defaults(void)
{
	eval("insmod", "nvram_linux.o");


#ifdef CONFIG_SENTRY5
#include "rcs5.h"
#else
#define RC1_START() 
#define RC1_STOP()  
#define RC7_START()
#define RC7_STOP()
#define LINUX_OVERRIDES() 
#define EXTRA_RESTORE_DEFAULTS() 
#endif

	nvram_set("NVRAMMAGIC", "");

	struct nvram_tuple *t, *u;
	int restore_defaults, i;

	/* Restore defaults if told to or OS has changed */
	restore_defaults = !nvram_match("restore_defaults", "0")/* || nvram_invmatch("os_name", "linux")*/;

	if (restore_defaults){
		fprintf(stderr, "\n## Restoring defaults... ##\n");
		logmessage(LOGNAME, "Restoring defaults...");
	}

	/* Restore defaults */
	for (t = router_defaults; t->name; t++) {
		if (restore_defaults || !nvram_get(t->name)) {
			{
				nvram_set(t->name, t->value);
			}
		}
	}

	/* Commit values */
	if (restore_defaults) {
		/* default value of vlan */
		nvram_commit();		
		fprintf(stderr, "done\n");
	}

	klogctl(8, NULL, atoi(nvram_safe_get("console_loglevel")));
}

static void
set_wan0_vars(void)
{
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	
	// check if there are any connections configured
	for (unit = 0; unit < MAX_NVPARSE; unit ++) {
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		if (nvram_get(strcat_r(prefix, "unit", tmp)))
			break;
	}
	// automatically configure wan0_ if no connections found
	if (unit >= MAX_NVPARSE) {
		struct nvram_tuple *t;
		char *v;

		// Write through to wan0_ variable set
		snprintf(prefix, sizeof(prefix), "wan%d_", 0);
		for (t = router_defaults; t->name; t ++) {
			if (!strncmp(t->name, "wan_", 4)) {
				if (nvram_get(strcat_r(prefix, &t->name[4], tmp)))
					continue;
				v = nvram_get(t->name);
				nvram_set(tmp, v ? v : t->value);
			}
		}
		nvram_set(strcat_r(prefix, "unit", tmp), "0");
		nvram_set(strcat_r(prefix, "desc", tmp), "Default Connection");
		nvram_set(strcat_r(prefix, "primary", tmp), "1");
	}

	/* before usb hotplug and some events, we initial it */
	nvram_set("r_Setting", "0");
	nvram_set("usb_dev_state", "none");
	nvram_set("usb_mass_path", "none");
	nvram_set("usb_mnt_first_path", "");
	nvram_set("ftp_running", "0");
	nvram_set("swap_on", "0");
	nvram_set("apps_running", "0");
	nvram_set("apps_dlx", "0");
	nvram_set("apps_status_checked", "1");  // it means need to check
	nvram_set("usb_disc0_port", "0");
	nvram_set("usb_disc0_dev", "");
	nvram_set("usb_path1", "");
	nvram_set("upnp_running", "0");
	nvram_set("hotplug_usb_mem_cric", "0");
	nvram_set("wanup_mem_cric", "0");
	nvram_set("usb_mass_hotplug", "0");
	nvram_set("ots_running", "0");
	nvram_set("event_mem_out", "0");
	nvram_set("update_resolv", "free");
	nvram_set("mount_late", "0");
}

static void
sysinit(void)
{
	time_t tm = 0;
	//printf("** sysinit\n");	// tmp test

	/* /proc */
	//mount("proc", "/proc", "proc", MS_MGC_VAL, NULL);

	/* /tmp */
	//mount("ramfs", "/tmp", "ramfs", MS_MGC_VAL, NULL);

	eval("mount", "-a");
	eval("dev_init.sh");

	/* /var */
	mkdir("/tmp/rc_notification", 0777);	// 2008.10 magic
	mkdir("/tmp/rc_action_incomplete", 0777);	// 2008.10 magic

	mkdir("/tmp/var", 0777);
	mkdir("/var/lock", 0777);
	mkdir("/var/log", 0777);
	mkdir("/var/run", 0777);
	mkdir("/var/tmp", 0777);

	/* for user space nvram utility */
	eval ("mknod", "/dev/nvram", "c", "228", "0");

	/* Setup console */
	if (console_init())
		noconsole = 1;

	chdir("/");
	setsid();
	{
		const char *const *e;
		/* Make sure environs is set to something sane */
		for (e = environment; *e; e++)
			putenv((char *) *e);
	}

	/* Set a sane date */
	stime(&tm);

	// extra settings
	symlink("/tmp", "/shares");
	system("echo 90 > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout");
	system("echo 4096 > /proc/sys/net/nf_conntrack_max");
	system("echo 900 > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established");
}

static void
insmod(void)
{
	char buf[PATH_MAX];
	struct utsname name;
	struct stat tmp_stat;

	eval("insmod", "-q", "rt2860v2_ap");
	eval("insmod", "-q", "lm");
	eval("insmod", "-q", "dwc_otg");
	//eval("insmod", "-q", "usblp");
/*
	if(strcmp(nvram_safe_get("wan_proto"), "3g") == 0)
	{
		printf("ins 3g modules\n");	// tmp test
        	system("insmod usbserial");
        	system("insmod hso");
	}
*/
#ifdef USB_SUPPORT
#ifdef LANGUAGE_TW
/*
	eval("insmod", "nls_cp950.o");
	eval("insmod", "nls_big5.o");
	eval("insmod", "nls_cp936.o");
	eval("insmod", "nls_gb2312.o");
	eval("insmod", "nls_utf8.o");
*/
#endif
#ifdef LANGUAGE_CN
/*
	eval("insmod", "nls_cp936.o");
	eval("insmod", "nls_gb2312.o");
	eval("insmod", "nls_cp950.o");
	eval("insmod", "nls_big5.o");
	eval("insmod", "nls_utf8.o");
*/
#endif
#ifdef LANGUAGE_KR
	eval("insmod", "nls_cp949.o");
	eval("insmod", "nls_euc-kr.o");
	eval("insmod", "nls_utf8.o");
#endif
#ifdef LANGUAGE_JP
	eval("insmod", "nls_cp932.o");
	eval("insmod", "nls_euc-jp.o");
	eval("insmod", "nls_sjis.o");
	eval("insmod", "nls_utf8.o");
#endif
#endif
}

/* States */
enum {
	RESTART,
	STOP,
	START,
	TIMER,
	IDLE,
	SERVICE,
	HOTPLUG,
	RECOVER,
};
static int state = START;
static int signalled = -1;


/* Signal handling */
static void
rc_signal(int sig)
{
	if (state == IDLE) {	
		if (sig == SIGHUP) {
			signalled = RESTART;
		}
		else if (sig == SIGUSR2) {
			signalled = START;
		}
		else if (sig == SIGINT) {
			signalled = STOP;
		}
		else if (sig == SIGALRM) {
			signalled = TIMER;
		}
		else if (sig == SIGUSR1) {
			signalled = SERVICE;
		}
		else if (sig == SIGTTIN) {
			signalled = HOTPLUG;
		}
	}
}

/* Timer procedure */
int
do_timer(void)
{
	int interval = atoi(nvram_safe_get("timer_interval"));
	time_t now;
	struct tm gm, local;
	struct timezone tz;

#ifdef ASUS_EXT
	/* Update kernel timezone */
	setenv("TZ", nvram_safe_get("time_zone_x"), 1);
	time(&now);
	gmtime_r(&now, &gm);
	localtime_r(&now, &local);
	tz.tz_minuteswest = (mktime(&gm) - mktime(&local)) / 60;
	settimeofday(NULL, &tz);
	return 0;
#endif
	if (interval == 0)
		return 0;

	/* Report stats */
	if (nvram_invmatch("stats_server", "")) {
		char *stats_argv[] = { "stats", nvram_get("stats_server"), NULL };
		_eval(stats_argv, NULL, 5, NULL);
	}

	/* Sync time */
	start_ntpc();

	/* Update kernel timezone */
	setenv("TZ", nvram_safe_get("time_zone_x"), 1);
	time(&now);
	gmtime_r(&now, &gm);
	localtime_r(&now, &local);
	tz.tz_minuteswest = (mktime(&gm) - mktime(&local)) / 60;
	settimeofday(NULL, &tz);
	alarm(interval);
	return 0;
}

void 
set_WL0_vars(void)
{
	if (nvram_match("wl_mode_x", "1"))
	{
		nvram_set("wl_mode", "wds");
		nvram_set("WL_mode", "wds");
	}
	else
	{
		nvram_set("wl_mode", "ap");
		nvram_set("WL_mode", "ap");
	}

	nvram_set("WL_ap_isolate",      nvram_safe_get("wl_ap_isolate"));
	nvram_set("WL_auth_mode",       nvram_safe_get("wl_auth_mode"));
	nvram_set("WL_bcn",		nvram_safe_get("wl_bcn"));
	nvram_set("WL_channel",		nvram_safe_get("wl_channel"));
	nvram_set("WL_closed",		nvram_safe_get("wl_closed"));
	nvram_set("WL_crypto",		nvram_safe_get("wl_crypto"));
	nvram_set("WL_dtim",		nvram_safe_get("wl_dtim"));
	nvram_set("WL_frag",		nvram_safe_get("wl_frag"));
	nvram_set("WL_gmode",		nvram_safe_get("wl_gmode"));
	nvram_set("WL_gmode_protection",nvram_safe_get("wl_gmode_protection"));
	nvram_set("WL_key1",		nvram_safe_get("wl_key1"));
	nvram_set("WL_key2",		nvram_safe_get("wl_key2"));
	nvram_set("WL_key3",		nvram_safe_get("wl_key3"));
	nvram_set("WL_key4",		nvram_safe_get("wl_key4"));
	nvram_set("WL_key",		nvram_safe_get("wl_key"));
	nvram_set("WL_lazywds",		nvram_safe_get("wl_lazywds"));
	nvram_set("WL_macapply_x",      nvram_safe_get("wl_macapply_x"));
	nvram_set("WL_maclist",		nvram_safe_get("wl_maclist"));
	nvram_set("WL_maclist_x",       nvram_safe_get("wl_maclist_x"));
	nvram_set("WL_macmode",		nvram_safe_get("wl_macmode"));
	nvram_set("WL_macnum_x",	nvram_safe_get("wl_macnum_x"));
	nvram_set("WL_mode_ex",		nvram_safe_get("wl_mode_ex"));
	nvram_set("WL_mode_x",		nvram_safe_get("wl_mode_x"));
	nvram_set("WL_phrase_x",	nvram_safe_get("wl_phrase_x"));
	nvram_set("WL_radio_date_x",    nvram_safe_get("wl_radio_date_x"));
	nvram_set("WL_radio_time_x",    nvram_safe_get("wl_radio_time_x"));
	nvram_set("WL_radio_x",		nvram_safe_get("wl_radio_x"));
	nvram_set("WL_radius_ipaddr",   nvram_safe_get("wl_radius_ipaddr"));
	nvram_set("WL_radius_key",      nvram_safe_get("wl_radius_key"));
	nvram_set("WL_radius_port",     nvram_safe_get("wl_radius_port"));
	nvram_set("WL_rateset",		nvram_safe_get("wl_rateset"));
	nvram_set("WL_rts",		nvram_safe_get("wl_rts"));
	nvram_set("WL_ssid",		nvram_safe_get("wl_ssid"));
	nvram_set("WL_wdsapply_x",      nvram_safe_get("wl_wdsapply_x"));
	nvram_set("WL_wdslist_x",       nvram_safe_get("wl_wdslist_x"));
	nvram_set("WL_wdsnum_x",	nvram_safe_get("wl_wdsnum_x"));
	nvram_set("WL_wep_x",		nvram_safe_get("wl_wep_x"));
	nvram_set("WL_wme",		nvram_safe_get("wl_wme"));
	nvram_set("WL_wme_no_ack",	nvram_safe_get("wl_wme_no_ack"));
	nvram_set("WL_wpa_gtk_rekey",   nvram_safe_get("wl_wpa_gtk_rekey"));
	nvram_set("WL_wpa_mode",	nvram_safe_get("wl_wpa_mode"));
	nvram_set("WL_wpa_psk",		nvram_safe_get("wl_wpa_psk"));
	nvram_set("WL_APSDCapable",	nvram_safe_get("wl_APSDCapable"));
	nvram_set("WL_DLSCapable",	nvram_safe_get("wl_DLSCapable"));
}

int 
stop_watchdog()
{
	return system("killall watchdog");
}

int 
start_watchdog()
{
	char *watchdog_argv[] = {"watchdog", NULL};
	int whpid;

	return _eval(watchdog_argv, NULL, 0, &whpid);
}

int 
start_apcli_monitor()
{
	char *apcli_monitor_argv[] = {"apcli_monitor", NULL};
	int ampid;

	return _eval(apcli_monitor_argv, NULL, 0, &ampid);
}

int 
start_ping_keep_alive()
{
	char *ping_keep_alive_argv[] = {"ping_keep_alive", NULL};
	int pid;

	return _eval(ping_keep_alive_argv, NULL, 0, &pid);
}

#ifdef U2EC
int
start_usdsvr_broadcast()
{
	char *usdsvr_broadcast_argv[] = {"usdsvr_broadcast", NULL};
	int ubpid;

	return _eval(usdsvr_broadcast_argv, NULL, 0, &ubpid);
}

int
start_usdsvr_unicast()
{
	char *usdsvr_unicast_argv[] = {"usdsvr_unicast", NULL};
	int uupid;

	return _eval(usdsvr_unicast_argv, NULL, 0, &uupid);
}
#endif

#ifndef W7_LOGO
int 
stop_pspfix()
{
	return system("killall pspfix");
}

int 
start_pspfix()		// psp fix
{
	char *pspfix_argv[] = {"pspfix", NULL};
	int whpid;

	return _eval(pspfix_argv, NULL, 0, &whpid);
}
#endif

// oleg patch ~
static void
early_defaults(void)
{
       int stbport;

       if (nvram_match("wan_route_x", "IP_Bridged")) {
	       if (nvram_match("boardtype", "0x48E") && nvram_match("boardnum", "45"))	
	       {
		       nvram_set("vlan0ports", "0 1 2 3 4 5*");
		       nvram_set("vlan1ports", "5u");
	       }
       } else
       { /* router mode, use vlans */
	       /* Adjust switch config to bridge STB LAN port with WAN port */
	       stbport = atoi(nvram_safe_get("wan_stb_x"));

	       /* Check existing config for validity */
	       if (stbport < 0 || stbport > 5)
		       stbport = 0;

	       /* predefined config for WL520gu, WL520gc -- check boardtype for others */
	       /* there is no easy way to do LANx to real port number mapping, so we use array */
	       if (nvram_match("boardtype", "0x48E") && nvram_match("boardnum", "45"))
	       {
		       /* why don't you use different boardnum??? */
		       if (nvram_match("productid","WL500gpv2"))
		       {
			       /* todo: adjust port mapping */
			       nvram_set("vlan0ports", "0 1 2 3 5*");
			       nvram_set("vlan1ports", "4 5u");
		       } else {
			       static char *vlan0ports[] = { "1 2 3 4 5*",
				       "2 3 4 5*", "1 3 4 5*", "1 2 4 5*", "1 2 3 5*", "1 2 5*" };
			       static char *vlan1ports[] = { "0 5u",
				       "1 0 5u", "2 0 5u", "3 0 5u", "4 0 5u", "3 4 0 5u" };


			       nvram_set("vlan0ports", vlan0ports[stbport]);
			       nvram_set("vlan1ports", vlan1ports[stbport]);
		       }
	       }
       }
}

// ~ oleg patch

// 2008.08 magic {
static void handle_notifications(void){
	DIR *directory = opendir("/tmp/rc_notification");
	
	printf("handle_notifications() start\n");
	
	state = IDLE;
	
	if(directory == NULL)
		return;
	
	while(TRUE){
		struct dirent *entry;
		char *full_name;
		FILE *test_fp;

		entry = readdir(directory);
		if (entry == NULL)
			break;
		if (strcmp(entry->d_name, ".") == 0)
			continue;
		if (strcmp(entry->d_name, "..") == 0)
			continue;

		/* Remove the marker file. */
		full_name = (char *)(malloc(strlen(entry->d_name) + 100));
		if (full_name == NULL)
		{
			fprintf(stderr,
					"Error: Failed trying to allocate %lu bytes of memory for "
					"the full name of an rc notification marker file.\n",
					(unsigned long)(strlen(entry->d_name) + 100));
			break;
		}
		sprintf(full_name, "/tmp/rc_notification/%s", entry->d_name);
		remove(full_name);
		
		//printf("Flag : %s\n", entry->d_name);

		/* Take the appropriate action. */
		if (strcmp(entry->d_name, "restart_reboot") == 0)
		{
			fprintf(stderr, "rc rebooting the system.\n");
			sleep(1);	// wait httpd sends the page to the browser.
			eval("reboot");
			return;
		}
		else if (strcmp(entry->d_name, "restart_networking") == 0)
		{
			fprintf(stderr, "rc restarting networking.\n");
			
#ifdef WEB_REDIRECT
			printf("--- SERVICE: Wait to kill wanduck ---\n");
			stop_wanduck();
			
			signalled = RESTART;
			//eval("reboot");
#endif
			return;
		}
#ifdef CDMA // HSDPA
                else if (strcmp(entry->d_name, "restart_hsdpa") == 0)
                {
                        fprintf(stderr, "rc restarting HSDPA.\n");	// tmp test

                        if(strcmp(nvram_safe_get("hsdpa_product"), "") != 0)
                                eval("reboot");
                        else
                                printf("--- Needn't reboot! ---\n");

                        return;
                }
#endif // HSDPA
		else if (strcmp(entry->d_name, "restart_cifs") == 0)
		{
			printf("rc restarting ftp\n");	// tmp test
			//nvram_set("usb_storage_busy", "1");	// 2007.12 James.
			//run_ftpsamba();
			stop_ftp();
			sleep(1);
			run_ftp();
			//nvram_set("usb_storage_busy", "0");	// 2007.12 James.
		}
		/*else if (strcmp(entry->d_name, "restart_ftp") == 0)
		{
			fprintf(stderr, "rc restarting FTP.\n");
			//run_ftp();
		}*/
		else if (strcmp(entry->d_name, "restart_ddns") == 0)
		{
			fprintf(stderr, "rc restarting DDNS.\n");
			stop_ddns();
			
			if(nvram_match("ddns_enable_x", "1")){
				start_ddns();
				
				if(nvram_match("ddns_server_x", "WWW.ASUS.COM")
						&& strstr(nvram_safe_get("ddns_hostname_x"), ".asuscomm.com") != NULL){
					// because the computer_name is followed by DDNS's hostname.
					/*if(nvram_match("samba_running", "1")){
						stop_samba();
						run_samba();
					}
					
					if(nvram_match("ftp_running", "1")){
						stop_ftp();
						run_ftp();
					}
					
					if(nvram_match("dms_running", "1")){
						stop_dms();
						run_dms();
					}*/
				}
			}
		}
		else if (strcmp(entry->d_name, "restart_httpd") == 0)
		{
			fprintf(stderr, "rc restarting HTTPD.\n");
			stop_httpd();
			nvram_unset("login_ip");
			nvram_unset("login_timestamp");
			start_httpd();
		}
		else if (strcmp(entry->d_name, "restart_dns") == 0)
		{
			fprintf(stderr, "rc restarting DNS.\n");
			//stop_dns();
			//start_dns();
			restart_dns();
		}
		else if (strcmp(entry->d_name, "restart_dhcpd") == 0)
		{
			fprintf(stderr, "rc restarting DHCPD.\n");
			stop_dhcpd();
			start_dhcpd();
		}
		else if (strcmp(entry->d_name, "restart_upnp") == 0)
		{
			//fprintf(stderr, "rc restart UPNP.\n");	// tmp test
			stop_upnp();
			if(nvram_match("upnp_enable", "1")){
				fprintf(stderr, "rc restarting UPNP.\n");
				start_upnp();
			}
			
			// when apps_running isn't set, it means no disk. when apps_running is set 1 or 0, it means there are disks.
			/*fprintf(stderr, "rc stoping Media Server.\n");
			stop_dms();
			if(nvram_match("apps_dms", "1")){
				fprintf(stderr, "rc restarting Media Server.\n");
				run_dms();
			}*/
		}
#ifdef QOS
		else if (strcmp(entry->d_name, "restart_qos") == 0)
		{
			printf("rc restarting QOS.\n");
			
			//if(nvram_match("wan_status_t", "Connected"))
			//{
				qos_get_wan_rate();
				Speedtest_Init();
			//}
		}
#endif
		else if (strcmp(entry->d_name, "restart_syslog") == 0)
		{
			fprintf(stderr, "rc restarting syslogd.\n");
#ifdef ASUS_EXT  
	stop_logger();
	start_logger();
#endif

		}
		else if (strcmp(entry->d_name, "restart_firewall") == 0)
		{
			char wan_ifname[16];
			char *wan_proto = nvram_safe_get("wan_proto");
			
			fprintf(stderr, "rc restarting firewall.\n");
			/*if(!nvram_match("wan_status_t", "Connected"))
				continue;*/
			
			memset(wan_ifname, 0, 16);
			strncpy(wan_ifname, nvram_safe_get("wan_ifname_t"), 16);
			if(strlen(wan_ifname) == 0){
				if(!strcmp(wan_proto, "pppoe")
						|| !strcmp(wan_proto, "pptp")
						|| !strcmp(wan_proto, "l2tp")
#ifdef CDMA // HSDPA
                                                || (strcmp(nvram_safe_get("hsdpa_product"), "") != 0)
#endif
					)
					strcpy(wan_ifname, "ppp0");
				else
					strcpy(wan_ifname, "eth2.2");
			}
			
			start_firewall();
			
#ifdef NOIPTABLES
			start_firewall2(wan_ifname);
#else
			fprintf(stderr, "rc restarting IPTABLES firewall.\n");
			start_firewall_ex(wan_ifname, nvram_safe_get("wan0_ipaddr"), "br0", nvram_safe_get("lan_ipaddr"));
#endif
			
#ifndef ASUS_EXT
			/* Start connection dependent firewall */
			start_firewall2(wan_ifname);
#endif
		}
		else if (strcmp(entry->d_name, "restart_ntpc") == 0)
		{
			fprintf(stderr, "rc restarting ntpc.\n");
			stop_ntpc();
			start_ntpc();
		}
		else if (strcmp(entry->d_name, "rebuild_cifs_config_and_password") ==
				 0)
		{
			fprintf(stderr, "rc rebuilding CIFS config and password databases.\n");
//			regen_passwd_files(); /* Must be called before regen_cifs_config_file(). */
			//regen_cifs_config_file();
		}
		else if (strcmp(entry->d_name, "ddns_update") == 0)
		{
			fprintf(stderr, "rc updating ez-ipupdate for ddns changes.\n");
			//update_ddns_changes();
		}
// 2008.01 James. {
		else if(strcmp(entry->d_name, "restart_time") == 0)
		{
			fprintf(stderr, "rc restarting time.\n");
			
			do_timer();
			
#ifdef ASUS_EXT  
			stop_logger();
			start_logger();
#endif
			
			stop_ntpc();
			start_ntpc();
		}
#ifdef WSC
		else if(!strcmp(entry->d_name, "restart_wps"))
		{
			fprintf(stderr, "rc restart_wps\n");
			/*char *wsc_mode = nvram_safe_get("wsc_mode");
			char *old_wsc_mode = nvram_safe_get("old_wsc_mode");
			
			if((nvram_match("wsc_config_state", "0") && nvram_match("wsc_proc_status", "0")) || strcmp(wsc_mode, old_wsc_mode) != 0){
				fprintf(stderr, "rc restarting WPS.\n");
				
				stop_nas();// Cherry Cho added in 2008/1/24.
				stop_wsc();
				start_wsc();
				start_nas("lan");// Cherry Cho added in 2008/1/24.
				
				if(strcmp(wsc_mode, old_wsc_mode) != 0){
					nvram_set("old_wsc_mode", wsc_mode);
					nvram_commit();
				}
			}
			else
				fprintf(stderr, "Don't need restarting WPS.\n");*/
			;	// do nothing
		}
#endif
		else if(!strcmp(entry->d_name, "restart_apcli"))
		{
			if (nvram_match("apcli_workaround", "0"))
			{
				nvram_set("apcli_workaround", "2");

				fprintf(stderr, "rc restarting apcli_monitor.\n");

				eval("brctl", "addif", "br0", "apcli0");
				kill_pidfile_s("/var/run/apcli_monitor.pid", SIGTSTP);
			}
		}
// 2008.01 James. }
		else
		{
			fprintf(stderr,
					"WARNING: rc notified of unrecognized event `%s'.\n",
					entry->d_name);
		}

		/*
		 * If there hasn't been another request for the same event made since
		 * we started, we can safely remove the ``action incomplete'' marker.
		 * Otherwise, we leave the marker because we'll go through here again
		 * for this even and mark it complete only after we've completed it
		 * without getting another request for the same event while handling
		 * it.
		 */
		test_fp = fopen(full_name, "r");
		if (test_fp != NULL)
		{
			fclose(test_fp);
		}
		else
		{
			/* Remove the marker file. */
			sprintf(full_name, "/tmp/rc_action_incomplete/%s", entry->d_name);
			remove(full_name);
		}

		free(full_name);
	} 
	
	printf("handle_notifications() end, state : %d\n", state);
	closedir(directory);
}

int if_mounted_s()
{
        FILE *fp_m = fopen("/proc/mounts", "r");
        char buf[120];
        int ret = 0;

        memset(buf, 0, sizeof(buf));
        while(fgets(buf, sizeof(buf), fp_m))
        {
                if(strstr(buf, "/media/AiDisk_"))
                {
                        ret = 1;
                        break;
                }
                memset(buf, 0, sizeof(buf));
        }

        fclose(fp_m);
        return ret;
}

int if_mounted()
{
        FILE *fp_m = fopen("/proc/mounts", "r");
        FILE *fp_p = fopen("/proc/partitions", "r");
        char buf[120];
        int mounted_num = 0, blocks_cnt, valid_partnum = 0;
        #define MAXT 6
        char *p, *tokens[MAXT], *last;
        int i, ret = 0;

        printf("chk if_mounted\n");     // tmp test
        memset(buf, 0, sizeof(buf));
        while(fgets(buf, sizeof(buf), fp_m))
        {
                if(strstr(buf, "/media/AiDisk_"))
                {
                        ++mounted_num;
                }
                memset(buf, 0, sizeof(buf));
        }
        printf("mounted_num = %d\n", mounted_num);      // tmp test

        memset(buf, 0, sizeof(buf));
        while(fgets(buf, sizeof(buf), fp_p))
        {
                for (i=0, (p = strtok_r(buf, " ", &last)); p; (p = strtok_r(NULL, " ", &last)), ++i)
                {
                   if (i < MAXT - 1)
                           tokens[i] = p;
                }
                tokens[i] = NULL;

                if(((p = strstr(tokens[3], "sd"))!=NULL) && !((*(p+3)=='1')&&((*(p+4)==' ')||(*(p+4)=='\0')||(*(p+4)=='\t')||(*(p+4)=='\n'))))  // ignore chk sdx1
                {
                        blocks_cnt = atoi(tokens[2]);
                        //printf("chk %s blocks num: %d\n", tokens[3], blocks_cnt);       // tmp test
                        if(blocks_cnt > 5)	// 5 bytes is just a chk number
                                ++valid_partnum;
                }
                memset(buf, 0, sizeof(buf));
        }
        printf("valid partnum = %d\n", valid_partnum);  // tmp test

        if((mounted_num == valid_partnum) && (mounted_num > 0))
                ret = 1;
        else
                ret = 0;

        fclose(fp_m);
        fclose(fp_p);
        return ret;
}

// 2008.08 magic }

void init_spinlock()
{
	spinlock_init(SPINLOCK_SiteSurvey);
}

void reapchild()	// 0527 add	// dead code
{
	signal(SIGCHLD, reapchild);
	wait(NULL);
}

int get_dev_info(int *dev_class, char *product_id);

unsigned int mem_out_count = 0;
/* Main loop */
static void
main_loop(void)
{
	sigset_t sigset;
	pid_t shell_pid = 0;
	char *usb_cur_state;
	int i;
	char chkbuf[12];
	FILE *fp;
	char *wan_proto_type;
	int bus_plugged, dev_class;
	char product_id[20];

#ifdef STB
	/* Convert vital config before loading modules */
	//early_defaults();
#endif

	/* Basic initialization */
	sysinit();

	/* Setup signal handlers */
	signal_init();
	signal(SIGHUP, rc_signal);
	signal(SIGUSR1, rc_signal);
	signal(SIGUSR2, rc_signal);
	signal(SIGINT, rc_signal);
	signal(SIGALRM, rc_signal);
	signal(SIGTTIN, rc_signal);	// usb storage
	//signal(SIGCHLD, reapchild);	// dead code
	sigemptyset(&sigset);

	/* Restore defaults if necessary */
	restore_defaults();

	getsyspara();
	insmod();

#ifdef ASUS_EXT
	init_switch_mode();
	convert_asus_values(0);
	nvram_set("wan_nat_X", nvram_safe_get("wan_nat_x"));
	nvram_set("upnp_ENABLE", nvram_safe_get("upnp_enable"));
	nvram_set("wan_route_X", nvram_safe_get("wan_route_x"));
	nvram_set("lan_proto_X", nvram_safe_get("lan_proto_x"));
	nvram_set("productid", "RT-N13U");
#endif

#ifdef W7_LOGO
	nvram_set("wan_proto", "static");
	nvram_set("wan0_proto", "static");
	nvram_set("wan_ipaddr", "17.1.1.1");
	nvram_set("wan0_ipaddr", "17.1.1.1");
	nvram_set("wanx_ipaddr", "17.1.1.1");
	nvram_set("wan_ipaddr_t", "17.1.1.1");
	nvram_set("wan_gateway", "17.1.1.1");
	nvram_set("wan0_gateway", "17.1.1.1");
	nvram_set("wanx_gateway", "17.1.1.1");
	nvram_set("wan_gateway_t", "17.1.1.1");
	nvram_set("wan_netmask", "255.0.0.0");
	nvram_set("wan0_netmask", "255.0.0.0");
	nvram_set("wanx_netmask", "255.0.0.0");
	nvram_set("wan_netmask_t", "255.0.0.0");
#endif
	set_WL0_vars();	
	gen_ralink_config();

	/* Setup wan0 variables if necessary */
	set_wan0_vars();

	nvram_set("rmem", "0");	// tmp test

	/* Loop forever */
	for (;;) {
		switch (state) {
		case RECOVER:
			check_all_tasks();

			state = IDLE;
			break;
		case SERVICE:
			printf("rc SERVICE\n");	// tmp test

			track_set("500");
			//if(event_code == EVT_MEM_OUT)	// mem out
			if((event_code == EVT_MEM_OUT) || nvram_match("rmem", "1"))	// tmp test
			{
				nvram_set("rmem", "0");	// tmp test
				/* reduce conntrack */
				if ((fp=fopen("/proc/sys/net/ipv4/netfilter/ip_conntrack_max", "w+")))
				{
					fputs("4096", fp);
					fclose(fp);
				}
				++mem_out_count;
				memset(chkbuf, 0, sizeof(chkbuf));
				sprintf(chkbuf, "%d", mem_out_count);
				nvram_set("event_mem_out", chkbuf);
				logmessage(LOGNAME, "Out of memory!");
				state = RECOVER;
				break;
			}

			if(nvram_get("rc_service") != NULL){	// for original process
				service_handle();
				state = IDLE;
			}
			else{	// for new process
				handle_notifications();
			}
			
			nvram_set("success_start_service", "1");	// 2008.05 James. For judging if the system is ready.
			break;
		case HOTPLUG:
			printf("\n## rc recv HOTPLUG\n");	// tmp test
			usb_cur_state = nvram_safe_get("usb_dev_state");

			track_set("501");

			printf("usb cur state is %s\n", usb_cur_state);	// tmp test
			switch(event_code) {
			case USB_PLUG_ON:
				if(strcmp(usb_cur_state, "on") == 0)	// ignore extra SIGTTIN 
				{
					printf("ignore SIGTTIN (on)\n");	// tmp test
					break;
				}
				memset(product_id, 0, sizeof(product_id));
				bus_plugged = get_dev_info(&dev_class, product_id);
				printf("bus_plggued = %d, dev_class = %d\n", bus_plugged, dev_class);	// tmp test

				if(dev_class == 0x35)	// USB_CLS_3GDEV
				{
					nvram_set("usb_path1", "HSDPA");
				}
				else
				{
					for(i=0; i<15; ++i)	// check if mounted
					{
						if(!if_mounted())
							sleep(1);
						else
							break;
					}
					if(i == 15)
					{
						printf("scsi mount fail\n");	// tmp test
						nvram_set("mount_late", "1");
					}
				}

				nvram_set("usb_dev_state", "on");
				hotplug_usb();
				break;
			case USB_PLUG_OFF:
				if(strcmp(usb_cur_state, "off") == 0)	// ignore extra SIGTTIN 
				{
					printf("ignore SIGTTIN (on)\n");	// tmp test
					break;
				}
				for(i=0; i<15; ++i)	// check if mounted
				{
					if(if_mounted_s())
						sleep(1);
					else
						break;
				}
				if(i == 15)
					printf("scsi umount fail\n");	// tmp test

				nvram_set("usb_dev_state", "off");
				nvram_set("usb_mass_path", "none");
				nvram_set("usb_mnt_first_path", "");
				nvram_set("mount_late", "0");
				hotplug_usb();
				break;
			case USB_PRT_PLUG_ON:
				if(strcmp(usb_cur_state, "on") == 0)    // ignore extra SIGTTIN 
				{
					printf("ignore SIGTTIN (prt_on)\n");	// tmp test
					break;
				}
				nvram_set("usb_dev_state", "on");
				hotplug_usb();
				break;
			case USB_PRT_PLUG_OFF:
				if(strcmp(usb_cur_state, "off") == 0)   // ignore extra SIGTTIN 
				{
					printf("ignore SIGTTIN (prt_off)\n");	// tmp test
					break;
				}
				nvram_set("usb_dev_state", "off");
				nvram_set("usb_mnt_first_path", "");
				hotplug_usb();
				break;
			case USB_SERIAL_PLUG_ON:
                                if(strcmp(usb_cur_state, "on") == 0)    // ignore extra SIGTTIN 
                                {
                                        printf("ignore SIGTTIN (serial_on)\n");    // tmp test
                                        break;
                                }
                                nvram_set("usb_dev_state", "on");
				nvram_set("usb_path1", "HSDPA");
                                //hotplug_usb();
                        	//if(check_pppd() == 0)
                                	start_3g();
                        	//else
                                //	printf("3g: pppd already invoked\n");

				break;
			case USB_SERIAL_PLUG_OFF:
                                if(strcmp(usb_cur_state, "off") == 0)   // ignore extra SIGTTIN 
                                {
                                        printf("ignore SIGTTIN (serial_off)\n");   // tmp test
                                        break;
                                }
                                nvram_set("usb_dev_state", "off");
                                nvram_set("usb_mnt_first_path", "");
				printf("usb serial plug off\n");	// tmp test
				nvram_set("usb_path1", "");
				stop_3g();
                                //hotplug_usb();
				break;
			default:
				printf("SIGTTIN: do nothing\n");	// tmp test
				break;
			}

			state = IDLE;
			break;
		case RESTART:
			/* Fall through */
		case STOP:	// when will it occur?
#ifdef ASUS_EXT
			stop_misc();
#endif
			stop_services();

			stop_wan();

#ifdef WSC
			stop_wsc();	/* Cherry Cho added in 2007/4/27. */
#endif

			stop_lan();
			if (state == STOP) {
				state = IDLE;
				break;
			}
			/* Fall through */
		case START:
			wan_proto_type = nvram_safe_get("wan0_proto");

			printf("\nchk ver:0710[rc] START\n");	// tmp test
			if(strcmp(nvram_safe_get("wl_ssid"), nvram_safe_get("wl_ssid2"))){
				char buff[100];
				memset(buff, 0, 100);
				char_to_ascii(buff, nvram_safe_get("wl_ssid"));
				nvram_set("wl_ssid2", buff);
				nvram_commit();
				//csprintf("nvram set wl_ssid2 = %s\n",nvram_safe_get("wl_ssid2"));
			} 

			config_loopback();
			vconfig();

        		if(wan_proto_type && (strcmp(wan_proto_type, "pptp") == 0)) // delay run
				sleep(2);	// tmp test

			start_lan();
			default_filter_setting();	// change place
			start_wan();
			//start_dns();			// change place

        		if(wan_proto_type && (strcmp(wan_proto_type, "pptp") == 0)) // delay run
				sleep(5);			// tmp test

			start_dns();
			//default_filter_setting();
			init_spinlock();
			start_services();
			// start watchdog when boot
			start_watchdog();
			start_telnetd();
#ifdef WSC
			if(nvram_invmatch("wsc_config_state", "1") && nvram_match("sw_mode_ex", "1"))// psp fix
			{
				start_wsc_pin_enrollee();
#ifndef W7_LOGO
				start_pspfix();								// psp fix
#endif
			}
			else										// psp fix
			{
				nvram_set("wps_enable", "0");
				nvram_set("wps_start_flag", "0");	// 0901 w7 update
			}
#endif
			//printf("test(5) get usb_path1=%s\n", nvram_safe_get("usb_path1"));	// tmp test
			if (nvram_match("sw_mode_ex", "2"))
			{
				start_apcli_monitor();
				start_ping_keep_alive();
			}

// 2008.10 magic {		
#ifdef ASUS_EXT
		
			start_networkmap();	
#ifdef WEB_REDIRECT
/* no need to call here
			if(!nvram_match("wanduck_down", "1")
					&& nvram_match("wan_nat_x", "1")){
				start_wanduck();
			}
*/
#endif
			nvram_set("success_start_service", "1");	// For judging if the system is ready.

#endif
	
// 2008.10 magic }
			/* Fall through */
		case TIMER:
			do_timer();
			/* Fall through */
		case IDLE:
			state = IDLE;
			/* Wait for user input or state change */
			while (signalled == -1) {
				if (!noconsole && (!shell_pid || kill(shell_pid, 0) != 0))
				{
					//for(i=0; i<10; ++i)	// tmp test
					//	sleep(1);
					shell_pid = run_shell(0, 1);
				}
				else
				{
					sigsuspend(&sigset);
				}
			}
			state = signalled;
			signalled = -1;
			break;
		default:
			return;
		}
	}
}
/*
int 
disktest()
{
	struct statfs fsbuf;
	double free_size, block_size;
	long f_type;

	if(statfs("/media/AiDisk_a1", &fsbuf))
	{
		perror("*** check_disk_free_GE_1G: statfs fail!");
		return 0;
	}

	f_type = fsbuf.f_type;
	free_size = (double)((double)((double)fsbuf.f_bfree * fsbuf.f_bsize)/(1024*1024));
	block_size = (double)((double)((double)fsbuf.f_blocks * fsbuf.f_bsize)/(1024*1024));
	printf("chk disk: type=%x, free=%fMB, blocksize=%fMB\n", f_type, free_size, block_size); // tmp test

}
*/
// 2008.10 magic replace all "strstr" to "!strcmp"
int
main(int argc, char **argv)
{
	char *base = strrchr(argv[0], '/');
	int i=0;
	
	base = base ? base + 1 : argv[0];

#if 0
// tmp test
	printf("\n\n########## CALL RC: ########## : ");
	for(i=0; i<argc; ++i)
	{
		printf("[%s] ", argv[i]);
	}
	printf("\n");	// tmp test
//
#endif

	/* init */
	if (!strcmp(base, "init")) {
		main_loop();
		return 0;
	}

	logmessage(LOGNAME, "%s starts", base);
	/* Set TZ for all rc programs */
//	setenv("TZ", nvram_safe_get("time_zone_x"), 1);

	/* erase [device] */
	if (!strcmp(base, "erase")) {
		if (argv[1])
			return mtd_erase(argv[1]);
		else {
			fprintf(stderr, "usage: erase [device]\n");
			return EINVAL;
		}
	}
	else if (!strcmp(base, "nvram_restore")) {
		restore_defaults();
		return 0;
	}
	else if (!strcmp(base, "wphy")) {
		return is_phyconnected();
	}
#ifndef W7_LOGO
	else if (!strcmp(base, "pspfix"))	// psp fix
	{
		pspfix();
		return 0;
	}
#endif
	/* invoke watchdog */
	else if (!strcmp(base, "watchdog")) {
		return(watchdog_main());
	}
	/* stats [ url ] */
	//else if (strstr(base, "stats")) {	// disable for tmp
	//	return http_stats(argv[1] ? : nvram_safe_get("stats_server"));
	//}
#ifndef FLASH2M
	/* write [path] [device] */
	else if (!strcmp(base, "write")) {
		if (argc >= 3)
			return mtd_write(argv[1], argv[2]);
		else {
			fprintf(stderr, "usage: write [path] [device]\n");
			return EINVAL;
		}
	}
#endif
	/* udhcpc [ deconfig bound renew ] */
	else if (!strcmp(base, "udhcpc"))
		return udhcpc_main(argc, argv);
#ifdef ASUS_EXT
	/* hotplug [event] */
	else if (strstr(base, "hotplug_usb_mass"))      // added by Jiahao for WL500gP
	{
		return hotplug_usb_mass("");
	}
#ifdef DLM
	else if (strstr(base, "run_apps"))
		return run_apps();
	else if(!strcmp(base, "run_ftp")){
		nvram_set("usb_storage_busy", "1");
		run_ftp();
		nvram_set("usb_storage_busy", "0");

		return 0;
	}
	else if(!strcmp(base, "stop_ftp")){
		nvram_set("usb_storage_busy", "1");
		stop_ftp();
		nvram_set("usb_storage_busy", "0");

		return 0;
	}
#endif
	/* ddns update ok */
	else if (!strcmp(base, "stopservice")) {
		if (argc >= 2)
			return(stop_service_main(atoi(argv[1])));
		else return(stop_service_main(0));
	}
	/* ddns update ok */
	else if (!strcmp(base, "ddns_updated")) 
	{
		return ddns_updated_main();
	}
	/* ddns update ok */
	else if (!strcmp(base, "start_ddns")) 
	{
		return start_ddns();
	}
	/* run ntp client */
	else if (!strcmp(base, "ntp")) {
		return (ntp_main());
	}
	else if (!strcmp(base, "gpio")) {
		return(gpio_main(/*atoi(argv[1])*/));
	}
	else if (!strcmp(base, "radioctrl")) {
		if (argc >= 1)
			return(radio_main(atoi(argv[1])));
		else return EINVAL;
	}
#ifdef BTN_SETUP
	/* invoke ots(one touch setup) */
	else if (!strcmp(base, "ots")) {	// no need. use WPS.
		return(ots_main());
	}
#endif
	/* write srom */
	else if (!strcmp(base, "wsrom")) 
	{
		do_timer();
		if (argc >= 4) 
			return wsrom_main(argv[1], atoi(argv[2]), atoi(argv[3]));
		else {
			fprintf(stderr, "usage: wsrom [dev] [position] [value in 2 bytes]\n");
			return EINVAL;
		}
	}
	/* read srom */
	else if (!strcmp(base, "rsrom")) 
	{
		if (argc >= 3)
		{	 
			rsrom_main(argv[1], atoi(argv[2]), 1);
			return 0;
		}
		else {
			fprintf(stderr, "usage: rsrom [dev] [position]\n");
			return EINVAL;
		}
	}
	/* write mac */
	else if (!strcmp(base, "wmac")) 
	{
		if (argc >= 3) 
			return write_mac(argv[1], argv[2]);
		else {
			fprintf(stderr, "usage: wmac [dev] [mac]\n");
			return EINVAL;
		}
	}
#ifndef FLASH2M
	/* udhcpc_ex [ deconfig bound renew ], for lan only */
	else if (!strcmp(base, "landhcpc"))
		return udhcpc_ex_main(argc, argv);
#endif
	/* rc [stop|start|restart ] */
	else if (!strcmp(base, "rc")) {
		if (argv[1]) {
			if (strncmp(argv[1], "start", 5) == 0)
				return kill(1, SIGUSR2);
			else if (strncmp(argv[1], "stop", 4) == 0)
				return kill(1, SIGINT);
			else if (strncmp(argv[1], "restart", 7) == 0)
				return kill(1, SIGHUP);
		} else {
			fprintf(stderr, "usage: rc [start|stop|restart]\n");
			return EINVAL;
		}
	}
#endif	// ASUS_EXT
	// Jiahao add
	else if (!strcmp(base, "getMAC")) {
		return getMAC();
	}
	else if (!strcmp(base, "setMAC")) {
		if (argc == 2)
			return setMAC(argv[1]);
		else
			return EINVAL;
	}
	//2008.10 magic{
	else if (!strcmp(base, "FWRITE")) {
		if (argc == 3)
			return FWRITE(argv[1], argv[2]);
		else
		return EINVAL;
	}
	//2008.10 magic}
	else if (!strcmp(base, "getCountryCode")) {
		return getCountryCode();
	}
	else if (!strcmp(base, "setCountryCode")) {
		if (argc == 2)
			return setCountryCode(argv[1]);
		else
			return EINVAL;
	}
	else if (!strcmp(base, "gen_ralink_config")) {
		return gen_ralink_config();
	}
	else if (!strcmp(base, "getPIN")) {
		return getPIN();
	}
	else if (!strcmp(base, "setPIN")) {
		if (argc == 2)
			return setPIN(argv[1]);
		else
			return EINVAL;
	}
	else if (!strcmp(base, "getSSID")) {
		return getSSID();
	}
	else if (!strcmp(base, "getChannel")) {
		return getChannel();
	}
	else if (!strcmp(base, "getSiteSurvey")) {
		return getSiteSurvey();
	}
	else if (!strcmp(base, "getWPSAP")) {
		return getWPSAP();
	}
	else if (!strcmp(base, "getBSSID")) {
		return getBSSID();
	}
	else if (!strcmp(base, "getBootV")) {
		return getBootVer();
	}
	else if (!strcmp(base, "setBootV")) {
		return setBootVer();
	}
	else if (!strcmp(base, "setDisassociate")) {
		return setDisassociate();
	}
	else if (!strcmp(base, "getCurrentAddress")) {
		return getCurrentAddress();
	}
	else if (!strcmp(base, "getStaConnectionSSID")) {
		return getStaConnectionSSID();
	}
	else if (!strcmp(base, "sta_wps_pbc")) {
		return sta_wps_pbc();
	}
	else if (!strcmp(base, "sta_wps_stop")) {
		return sta_wps_stop();
	}
	else if (!strcmp(base, "getApCliInfo")) {
		return getApCliInfo();
	}
	else if (!strcmp(base, "apcli_connect")) {
		apcli_connect(1);
		return 0;
	}
	else if (!strcmp(base, "apcli_monitor")) {
		return apcli_monitor();
	}
	else if (!strcmp(base, "ping_keep_alive")) {
		return ping_keep_alive();
	}
	else if (!strcmp(base, "usdsvr_broadcast")) {
		return usdsvr_broadcast();
	}
	else if (!strcmp(base, "usdsvr_unicast")) {
		return usdsvr_unicast();
	}
	else if (!strcmp(base, "dhcpc_apply_delayed")) {
		return dhcpc_apply_delayed();
	}
	else if (!strcmp(base, "asuscfe")) {
		if (argc == 2)
			return asuscfe(argv[1]);
		else
			return EINVAL;
	}
	else if (!strcmp(base, "ateshow")) {
		return ateshow();
	}
	else if (!strcmp(base, "atehelp")) {
		return atehelp();
	}
	else if (!strcmp(base, "wps_pin")) {
		if (argc == 2)
			return wps_pin(atoi(argv[1]));
		else if (argc == 1)
			return wps_pin(0);
		else
			return EINVAL;
	}
	else if (!strcmp(base, "wps_pbc")) {
		return wps_pbc();
	}
	else if (!strcmp(base, "wps_oob")) {
		wps_oob();
		return 0;
	}
	else if (!strcmp(base, "wps_start")) {
		return start_wsc();
	}
	else if (!strcmp(base, "wps_stop")) {
		return stop_wsc();
	}
	else if (!strcmp(base, "startWan")) {
		start_wan();
		return 0;
	}
	/* ppp */
#ifdef CDMA // HSDPA
        else if(!strcmp(base, "write_cdma_conf")){
                return write_cdma_conf();
        }
#endif // CDMA // HSDPA
	else if (!strcmp(base, "ip-up"))
		return ipup_main(argc, argv);
	else if (!strcmp(base, "ip-down"))
		return ipdown_main(argc, argv);
	else if (!strcmp(base, "wan-up"))
		return ipup_main(argc, argv);
	else if (!strcmp(base, "wan-down"))
		return ipdown_main(argc, argv);
	/* restore default */
	else if (!strcmp(base, "restore"))	// no need
	{
		if (argc==2)
		{
			int step = atoi(argv[1]);
			if (step>=1)
			{
				nvram_set("vlan_enable", "1");
				restore_defaults();
			}
			/* Setup wan0 variables if necessary */
			if (step>=2)
				set_wan0_vars();
			if (step>=3)
				RC1_START();
			if (step>=4)
				start_lan();
		}
		return 0;
	}
#ifdef ASUS_EXT
	/* wlan update */	
	else if (!strcmp(base, "wlan_update")) 
	{
		wlan_update();
		return 0;
	}
// 2008.10 magic {
#ifdef QOS
	else if(!strcmp(base, "speedtest"))
	{
		qos_get_wan_rate();
		
		return 0;
	}
#endif
	else if(!strcmp(base, "restart_dns"))
	{
		//stop_dns();
		//start_dns();
		restart_dns();
		
		return 0;
	}
	else if(!strcmp(base, "convert_asus_values"))
	{
		convert_asus_values(1);
		
		return 0;
	}
	else if(!strcmp(base, "umount2"))
	{
		umount2(argv[1], 0x00000002);	// MNT_DETACH
		return 0;
	}
	else if(!strcmp(base, "ejusb"))
	{
		//remove_usb_mass(NULL);
		stop_usb();
		return 0;
	}
	else if(!strcmp(base, "start_telnetd"))
	{
		start_telnetd();
		return 0;
	}
	else if(!strcmp(base, "start_ots"))
	{
		start_ots();
		return 0;
	}
	else if(!strcmp(base, "start_ntp"))
	{
		start_ntpc();
		return 0;
	}
	else if(!strcmp(base, "get_sw"))
	{
		printf("sw mode is %s", nvram_safe_get("sw_mode"));
		return 0;
	}
	else if(!strcmp(base, "tracktest"))
	{
		track_set(argv[1]);
		return 0;
	}
	else if(!strcmp(base, "chkalltask"))
	{
		check_all_tasks();
		return 0;
	}
	else if(!strcmp(base, "run_upnp"))
	{
		start_upnp();
		return 0;
	}
/*
	else if(!strcmp(base, "disktest"))
	{
		disktest();
		return 0;
	}
*/
#ifdef USBTPT
	else if(!strcmp(base, "utpt"))
	{
		usbtpt(argc, argv);
		return 0;
	}
#endif
/*
#ifdef WEB_REDIRECT
	else if(!strcmp(base, "wanduckmain")){
		return wanduck_main();
	}
#endif
*/
// 2008.10 magic }
#if 0
	else if (strstr(base, "early_convert"))	// no need
	{
		early_convert_asus_values();
		return 0;
	}
#endif
#endif
	return EINVAL;
}
