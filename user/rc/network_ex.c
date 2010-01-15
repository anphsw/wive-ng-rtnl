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
 * $Id: network_ex.c,v 1.1.1.1 2007/01/25 12:52:21 jiahao_jhou Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>															
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <signal.h>

#include <nvram/bcmnvram.h>
#include <netconf.h>
#include <shutils.h>

typedef unsigned char   bool;   // 1204 ham

#include <wlutils.h>
#include <nvparse.h>
#include <rc.h>
#include <nvram/bcmutils.h>

#ifdef REMOVE
int start_pppoe(void)
{

	int timeout = 5;
	char pppunit[] = "XXXXXXXXXXXX";

	/* Add optional arguments */
	for (arg = pppoe_argv; *arg; arg++);
	if (nvram_invmatch(strcat_r(prefix, "pppoe_service", tmp), "")) {
		*arg++ = "-s";
				*arg++ = nvram_safe_get(strcat_r(prefix, "pppoe_service", tmp));
			}
			if (nvram_invmatch(strcat_r(prefix, "pppoe_ac", tmp), "")) {
				*arg++ = "-a";
				*arg++ = nvram_safe_get(strcat_r(prefix, "pppoe_ac", tmp));
			}
			if (nvram_match(strcat_r(prefix, "pppoe_demand", tmp), "1") || 
			    nvram_match(strcat_r(prefix, "pppoe_keepalive", tmp), "1"))
				*arg++ = "-k";
			snprintf(pppunit, sizeof(pppunit), "%d", unit);
			*arg++ = "-U";
			*arg++ = pppunit;

			/* launch pppoe client daemon */


			/* ppp interface name is referenced from this point on */
			wan_ifname = nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp));
			
			/* Pretend that the WAN interface is up */
			if (nvram_match(strcat_r(prefix, "pppoe_demand", tmp), "1")) {
				/* Wait for pppx to be created */
				while (ifconfig(wan_ifname, IFUP, NULL, NULL) && timeout--)
					sleep(1);

				/* Retrieve IP info */
				if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
					continue;
				strncpy(ifr.ifr_name, wan_ifname, IFNAMSIZ);

				/* Set temporary IP address */
				if (ioctl(s, SIOCGIFADDR, &ifr))
					perror(wan_ifname);
				nvram_set(strcat_r(prefix, "ipaddr", tmp), inet_ntoa(sin_addr(&ifr.ifr_addr)));
				nvram_set(strcat_r(prefix, "netmask", tmp), "255.255.255.255");

				/* Set temporary P-t-P address */
				if (ioctl(s, SIOCGIFDSTADDR, &ifr))
					perror(wan_ifname);
				nvram_set(strcat_r(prefix, "gateway", tmp), inet_ntoa(sin_addr(&ifr.ifr_dstaddr)));

				close(s);

				/* 
				* Preset routes so that traffic can be sent to proper pppx even before 
				* the link is brought up.
				*/
				preset_wan_routes(wan_ifname);
}
#endif

int start_pppd(char *prefix)
{
	if(!((nvram_match("wan0_proto", "pppoe")) || (nvram_match("wan0_proto", "pptp")) || (nvram_match("wan0_proto", "l2tp"))))
		return -1;

        int ret;
        FILE *fp;
        char options[80];
        char *pppd_argv[] = { "/usr/sbin/pppd", "file", options, NULL};
        char tmp[100];
        mode_t mask;
	int pid;

	printf("[rc] start pppd\n");	// tmp test

        sprintf(options, "/tmp/ppp/options.wan%s",
                nvram_safe_get(strcat_r(prefix, "unit", tmp)));

        mask = umask(066);

        /* Generate options file */
        if (!(fp = fopen(options, "w"))) {
                perror(options);
                umask(mask);
                return -1;
        }

        umask(mask);

        /* do not authenticate peer and do not use eap */
        fprintf(fp, "noauth refuse-eap\n");
        fprintf(fp, "user '%s'\n",
                nvram_safe_get(strcat_r(prefix, "pppoe_username", tmp)));
        fprintf(fp, "password '%s'\n",
                nvram_safe_get(strcat_r(prefix, "pppoe_passwd", tmp)));

        if (nvram_match(strcat_r(prefix, "proto", tmp), "pptp"))
        {
                fprintf(fp, "connect true\n");
	// disable for tmp
                fprintf(fp, "sync pty '/usr/sbin/pptp %s --nolaunchpppd --nobuffer --sync'\n",
                        nvram_invmatch("wan_heartbeat_x", "") ?
                        nvram_safe_get("wan_heartbeat_x") :
                        nvram_safe_get(strcat_r(prefix, "pppoe_gateway", tmp)));

/*
                fprintf(fp, "pty '/usr/sbin/pptp %s --nolaunchpppd'\n",
                        nvram_invmatch("wan_heartbeat_x", "") ?
                        nvram_safe_get("wan_heartbeat_x") :
                        nvram_safe_get(strcat_r(prefix, "pppoe_gateway", tmp)));	// tmp test
*/
                fprintf(fp, "lock\n");
                /* see KB Q189595 -- historyless & mtu */
                fprintf(fp, "nomppe-stateful %s mtu 1400\n",	
                        nvram_safe_get(strcat_r(prefix, "pptp_options_x", tmp)));	// disable for tmp
                //fprintf(fp, "nomppe-stateful %s mtu 1600\n",
                //        nvram_safe_get(strcat_r(prefix, "pptp_options_x", tmp)));	// tmp test
                fprintf(fp, "holdoff 10\n");	// tmp test
        } else {
                fprintf(fp, "nomppe nomppc\n");
        }

        if (nvram_match(strcat_r(prefix, "proto", tmp), "pppoe"))
        {
                fprintf(fp, "plugin rp-pppoe.so");

                if (nvram_invmatch(strcat_r(prefix, "pppoe_service", tmp), "")) {
                        fprintf(fp, " rp_pppoe_service '%s'",
                                nvram_safe_get(strcat_r(prefix, "pppoe_service", tmp)));
                }

                if (nvram_invmatch(strcat_r(prefix, "pppoe_ac", tmp), "")) {
                        fprintf(fp, " rp_pppoe_ac '%s'",
                                nvram_safe_get(strcat_r(prefix, "pppoe_ac", tmp)));
                }

                fprintf(fp, " nic-%s\n", nvram_safe_get(strcat_r(prefix, "ifname", tmp)));

                fprintf(fp, "mru %s mtu %s\n",
                        nvram_safe_get(strcat_r(prefix, "pppoe_mru", tmp)),
                        nvram_safe_get(strcat_r(prefix, "pppoe_mtu", tmp)));
        }

        if (atoi(nvram_safe_get(strcat_r(prefix, "pppoe_idletime", tmp))) &&
             nvram_match(strcat_r(prefix, "pppoe_demand", tmp), "1"))
        {
                fprintf(fp, "idle %s ", nvram_safe_get(strcat_r(prefix, "pppoe_idletime", tmp)));
                if (nvram_invmatch(strcat_r(prefix, "pppoe_txonly_x", tmp), "0")) {
                        fprintf(fp, "tx_only ");
                }
                fprintf(fp, "demand\n");
        }

        fprintf(fp, "maxfail 0\n");

        if (nvram_invmatch(strcat_r(prefix, "dnsenable_x", tmp), "0"))
                fprintf(fp, "usepeerdns\n");

        if (nvram_invmatch(strcat_r(prefix, "proto", tmp), "l2tp"))
                fprintf(fp, "persist\n");

        fprintf(fp, "ipcp-accept-remote ipcp-accept-local noipdefault\n");
        fprintf(fp, "ktune\n");

        /* pppoe set these options automatically */
        /* looks like pptp also likes them */
        fprintf(fp, "default-asyncmap nopcomp noaccomp\n");

        /* pppoe disables "vj bsdcomp deflate" automagically */
        /* ccp should still be enabled - mppe/mppc requires this */
        fprintf(fp, "novj nobsdcomp nodeflate\n");

        /* echo failures */
        fprintf(fp, "lcp-echo-interval 6\n");	// disable for tmp
        fprintf(fp, "lcp-echo-failure 10\n");		// disable for tmp
        //fprintf(fp, "lcp-echo-interval 3\n");	// disable for tmp
        //fprintf(fp, "lcp-echo-failure 4\n");		// disable for tmp

        fprintf(fp, "unit %s\n",
                nvram_get(strcat_r(prefix, "unit", tmp)) ? : "0");

        /* user specific options */
        fprintf(fp, "%s\n",
                nvram_safe_get(strcat_r(prefix, "pppoe_options_x", tmp)));

        fclose(fp);

        if (nvram_match(strcat_r(prefix, "proto", tmp), "l2tp"))
        {
                if (!(fp = fopen("/tmp/l2tp.conf", "w"))) {
                        perror(options);
                        return -1;
                }

		printf("\n\nbuild l2tp.conf\n");	// tmp test
                fprintf(fp, "# automagically generated\n"
                        "global\n\n"
                        "load-handler \"sync-pppd.so\"\n"
                        "load-handler \"cmd.so\"\n\n"
                        "section sync-pppd\n\n"
                        "lac-pppd-opts \"file %s\"\n\n"
                        "section peer\n"
                        "peername %s\n"
			"hostname %s\n"
                        "lac-handler sync-pppd\n"
                        "persist yes\n"
                        "maxfail %s\n"
                        "holdoff %s\n"
                        "section cmd\n\n",
                        options,
                        nvram_invmatch("wan_heartbeat_x", "") ?
                                nvram_safe_get("wan_heartbeat_x") :
                                nvram_safe_get(strcat_r(prefix, "pppoe_gateway", tmp)),
                        nvram_invmatch(strcat_r(prefix, "hostname", tmp), "") ?	// ham 0509
                                nvram_safe_get(strcat_r(prefix, "hostname", tmp)) : "localhost",
                        nvram_invmatch(strcat_r(prefix, "pppoe_maxfail", tmp), "") ?
                                nvram_safe_get(strcat_r(prefix, "pppoe_maxfail", tmp)) : "32767",
                        nvram_invmatch(strcat_r(prefix, "pppoe_holdoff", tmp), "") ?
                                nvram_safe_get(strcat_r(prefix, "pppoe_holdoff", tmp)) : "30");

                fclose(fp);

                /* launch l2tp */
                eval("/usr/sbin/l2tpd");		// tmp disable

                sleep(1);

                /* start-session */
                ret = eval("/usr/sbin/l2tp-control", "start-session 0.0.0.0");	// tmp disable

                /* pppd sync nodetach noaccomp nobsdcomp nodeflate */
                /* nopcomp novj novjccomp file /tmp/ppp/options.l2tp */

        } else
                _eval(pppd_argv, NULL, 0, &pid);

        return 0;
}

void start_pppoe_relay(char *wan_if)
{
	if (nvram_match("wan_pppoe_relay_x", "1"))
	{
		char *pppoerelay_argv[] = {"/usr/sbin/pppoe-relay", "-C", "br0", "-S", wan_if, "-F", NULL};
		int ret;
		pid_t pid;

		ret = _eval(pppoerelay_argv, NULL, 0, &pid);
	}
}

#ifdef CDMA
int write_cdma_conf(){
        FILE *fp;
        char username[32], passwd[32];
        char ttyUSB_num_str[2];
        int ttyUSB_num = -1;

        /* Get user name and password*/
        memset(username, 0, 32);
        strcpy(username, nvram_safe_get("hsdpa_username"));
        memset(passwd, 0, 32);
        strcpy(passwd, nvram_safe_get("hsdpa_passwd"));

        /* Setting pppd config file */
        if(!(fp = fopen("/tmp/ppp/peers/cdma", "w"))){
                perror("/tmp/ppp/peers/cdma");
                return -1;
        }

        if(nvram_match("hsdpa_combo", "1")){
                memset(ttyUSB_num_str, 0, 2);
                strcpy(ttyUSB_num_str, nvram_safe_get("hsdpa_ttyUSB_num"));
                ttyUSB_num = atoi(ttyUSB_num_str);

                if(!strcmp(nvram_safe_get("got_HSDPA"), "1")){
                        ;
                }
                else if(strlen(ttyUSB_num_str) > 0){
                        ++ttyUSB_num;
                        ttyUSB_num %= 4;
                        ttyUSB_num_str[0] = '0'+ttyUSB_num;
                        nvram_set("hsdpa_ttyUSB_num", ttyUSB_num_str);
                }
                else{
                        ttyUSB_num = 0;
                        nvram_set("hsdpa_ttyUSB_num", "0");
                }
printf("--- ttyUSB_num=%d. ---\n", ttyUSB_num);
                fprintf(fp, " /dev/ttyUSB%d\n", ttyUSB_num);
        }
        else
                fprintf(fp, " /dev/ttyACM0\n");
        fprintf(fp, " 460800\n");
        fprintf(fp, " modem\n");
        fprintf(fp, " noauth\n");
        fprintf(fp, " defaultroute\n");
        fprintf(fp, " noipdefault\n");
        if(nvram_match("wan_dnsenable_x", "1"))
                fprintf(fp, " usepeerdns\n");
/*      fprintf(fp, " -detach\n");
 *              fprintf(fp, " usehostname\n"); */
        fprintf(fp, " debug\n");
/*      fprintf(fp, " local\n"); */
        fprintf(fp, " user %s\n", username);
        fprintf(fp, " password %s\n", passwd);
/*      fprintf(fp, " persist\n");
 *              fprintf(fp, "show-password\n"); */
        fprintf(fp, " crtscts\n");
        fprintf(fp, " noccp\n");
        fprintf(fp, " novj\n");
        fprintf(fp, " nodeflate\n");
        fprintf(fp, " nobsdcomp\n");
        fprintf(fp, " connect-delay 5000\n");
/*      fprintf(fp, " connect '/usr/sbin/chat -vf /tmp/ppp/peers/cdma_chat'\n"); */
        fprintf(fp, " connect '/usr/sbin/chat -svf /tmp/ppp/peers/cdma_chat'\n");
        fprintf(fp, " mru %s\n", nvram_safe_get("wan_hsdpa_mru"));
        fprintf(fp, " mtu %s\n", nvram_safe_get("wan_hsdpa_mtu"));
/*      fprintf(fp, " disconnect '/usr/sbin/chat -vf /tmp/ppp/peers/cdma_disconnect'\n"); */
        fprintf(fp, "lcp-echo-interval 10\n");
        fprintf(fp, "lcp-echo-failure 6\n");

        fclose(fp);
        /* Create auth file */
        if(!(fp = fopen("/tmp/ppp/chap-secrets", "w"))){
                perror("/tmp/ppp/chap-secrets");
                return -1;
        }

        fprintf(fp, "\"%s\" * \"%s\" *\n", username, passwd);

        fclose(fp);

        /* Writing chat file */
        if(!(fp = fopen("/tmp/ppp/peers/cdma_chat", "w"))){
                perror("/tmp/ppp/peers/cdma_chat");
                return -1;
        }

/*      fprintf(fp, "'' 'AT'\n");
 *              fprintf(fp, "'OK' 'ATE0V1&F&D2&C1&C2S0=0'\n");
 *                      fprintf(fp, "'OK' 'ATE0V1'\n");
 *                              fprintf(fp, "'OK' 'ATS7=60'\n");
 *                                      fprintf(fp, "'OK' 'ATDT#777'\n");
 *                                              fprintf(fp, "ABORT BUSY\n");
 *                                                      fprintf(fp, "ABORT ERROR\n");
 *                                                              fprintf(fp, "ABORT VOICE\n"); */
        fprintf(fp, "ABORT 'NO CARRIER'\n");
        fprintf(fp, "ABORT 'NO DAILTONE'\n");
/*      fprintf(fp, "ABORT 'NO DAIL TONE'\n");
 *              fprintf(fp, "ABORT 'NO ANSWER'\n");
 *                      fprintf(fp, "REPORT CONNECT\n");
 *                              fprintf(fp, "TIMEOUT 10\n");
 *                                      fprintf(fp, "OK ATZ\n"); */
        fprintf(fp, "\"\" AT\n");
        fprintf(fp, "OK AT+CFUN=1\n");
/*      if(nvram_match("enable_apn", "1"))      */
        if(strcmp(nvram_safe_get("private_apn"), ""))
                fprintf(fp, "OK \'AT+CGDCONT=1, \"ip\",\"%s\"\'\n", nvram_safe_get("private_apn"));
/*      fprintf(fp, "OK \'AT+CGDCONT=1, \"ip\",\"internet\"\'\n");      */
        /*if(nvram_match("pin_code_enable", "1"))
 *                 fprintf(fp, "OK AT+CPIN=\"%s\"\n", nvram_safe_get("pin_code"));// */
/*      if(nvram_invmatch("wan_pppoe_passwd", ""))
 *                      fprintf(fp, "OK AT+CPIN=\"%s\"\n", passwd);
 *                              fprintf(fp, "\\d \\d \\d \\d \\d \n");
 *                                      fprintf(fp, "OK ATQ0V1E1S0=0&C1&D2+FCLASS=0\n");        */
        fprintf(fp, "OK ATQ0V1E1S0=0&C1&D2\n");
/*      fprintf(fp, "OK \"AT+IPR=115200\"\n");
 *              fprintf(fp, "OK \"ATE1\"\n");
 *                      fprintf(fp, "TIMEOUT 60\n");    */
        fprintf(fp, "\"\" ATDT%s\n", nvram_safe_get("hsdpa_dial_number"));
        fprintf(fp, "CONNECT \"\"\n");
/*      fprintf("\n"); */

        fclose(fp);

        return 0;
}

int start_cdma(void) // HSDPA
{
        int ret;
        char *cdma_argv[] = {"pppd", "call", "cdma", NULL};

        write_cdma_conf();

        eval("killall", "pppd");

        /* Call CMDA connection */
	printf("--- Wait to start HSDPA... ---\n");	// tmp test
        sleep(2);
	printf("--- Start HSDPA... ---\n");
        _eval(cdma_argv, NULL, 0, NULL);

        return 0;
}

int
stop_cdma(void)
{
        int ret;
        ret = eval("killall", "pppd");
        ret += eval("killall", "chat");
        dprintf("done\n");

        return ret;
}
#endif

