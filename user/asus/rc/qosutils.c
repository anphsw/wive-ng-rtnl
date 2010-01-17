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
/************************************************************************************************************

 * File Name	: qosutile.c
 * Description  : modify from WRT54GS-v4.70.6:voip_qos.c. Called by speedtest.
 * Auther       : Wendel Huang.
 * History	: 2006.05.22 modify to post on WL500gP.
		  2006.07.18 Post to WL700g, and remove execution message from release version.
		  2006.08.08 modify check_wan_link() to ckeck WL700g wan status.
		  2006.08.14 modify get_dns_list() to get WL700g's DNS setting.
 ************************************************************************************************************/

#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <linux/sockios.h>
#include <errno.h>
#include <flash_ioctl.h>
#include <fcntl.h>
#include "shutils.h"
#include "ra3052.h"

#include <nvram/bcmnvram.h>

#define RTL8366S_GETPORTLINKSTATUS                 0xf1

#include "qosutils.h"

#ifdef WL500GP	//For WL500gP, WL550gE

int get_ppp_pid(char *conntype)
{
        int pid = -1;
        char tmp[80], tmp1[80];
        snprintf(tmp, sizeof(tmp), "/var/run/%s.pid", conntype);
#ifdef QOS_DEBUG                                                                                                                        
	printf("	get_ppp_pid() ** tmp = %s\n", tmp);
#endif
        file_to_buf(tmp, tmp1, sizeof(tmp1));
        pid = atoi(tmp1);
#ifdef QOS_DEBUG
	printf("	get_ppp_pid() ** pid = %d\n", pid);
#endif
        return pid;
}

/* Find process name by pid from /proc directory */
char *find_name_by_proc(int pid)
{
        FILE *fp;
        char line[254];
        char filename[80];
        static char name[80];
                                                                                                 
        snprintf(filename, sizeof(filename), "/proc/%d/status", pid);
                                                                                                 
        if((fp = fopen(filename, "r"))){
                fgets(line, sizeof(line), fp);
                /* Buffer should contain a string like "Name:   binary_name" */
                sscanf(line, "%*s %s", name);
#ifdef QOS_DEBUG
		printf("	find_name_by_proc() ** name=%s\n", name);
#endif
                fclose(fp);
                return name;
        }
                                                                                                 
        return "";
}

int check_ppp_exist(){
        DIR *dir;
        struct dirent *dent;
        char task_file[64], cmdline[64];
        int pid, fd;

        if((dir = opendir("/proc")) == NULL){
                perror("open proc");
                return -1;
        }

        while((dent = readdir(dir)) != NULL){
                if((pid = atoi(dent->d_name)) > 1){
                        memset(task_file, 0, 64);
                        sprintf(task_file, "/proc/%d/cmdline", pid);
                        if((fd = open(task_file, O_RDONLY)) > 0){
                                memset(cmdline, 0, 64);
                                read(fd, cmdline, 64);
                                close(fd);

                                if(strstr(cmdline, "pppoecd")
                                                || strstr(cmdline, "pppd")
                                                ){
                                        closedir(dir);
                                        return 0;
                                }
                        }
                        else
                                printf("cannot open %s\n", task_file);
                }
        }
        closedir(dir);

        return -1;
}

int osl_ifflags(const char *ifname)
{
    int sockfd;
    struct ifreq ifreq;
    int flags = 0;
                                                                                                 
    if ((sockfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0) {
        perror("socket");
        return flags;
    }
                                                                                                 
    strncpy(ifreq.ifr_name, ifname, IFNAMSIZ);
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifreq) < 0) {
            flags = 0;
    } else {
            flags = ifreq.ifr_flags;
    }
    close(sockfd);
    return flags;
}

// Added by Joey for ethtool
#include <net/if.h>
#include "ethtool-util.h"
#undef SIOCETHTOOL
#define	SIOCETHTOOL	0x8946
#ifndef SIOCGETCPHYRD
#define	SIOCGETCPHYRD	0x89FE
#endif                                                                                                                          

//#define EVBOARD		1	// tmp add for ev-board
/* RT-N14: phy0,1,2,3,4 = LLLLW */	/* EV board: WLLLL */
int is_phyconnected()
{
	int val = 0, idx = 1, ret;

	//printf("chk is_phyconn\n");	// tmp test

        if(switch_init() < 0)
                return 0;

	if(ra3052_reg_read(0x80, &val) != 0)
		return 0;

	//printf("get val: %x\n", val);	// tmp test
/*
#ifdef EVBOARD
	//printf("ev: ");	// tmp test

	ret = ((val & (idx << 25))) >> 25;

#else	// rt-n12
*/
	ret = ((val & (idx << 29))) >> 29;
//#endif
	switch_fini();

	//printf("ret is %d\n", ret);	// tmp test
        return ret;
}

/**************************************************************************
 * Function Name: check_wan_link()
 * Description  : Check Wan interface's connection status on WL500gp 
 * Parameters   : int num -> wan interface index
 * Returns      : wan status
			0 : WAN link fail
			1 : WAN link UP
 **************************************************************************/
int check_wan_link(int num)
{
        int wan_link = 0;
	char conntype[10];
        char wan_if[2][20]={"wan_ifname","wan_ifnames_t"};
        DIR *ppp_dir;   // 2008.01 James.
        struct dirent *entry;   // 2008.01 James.
	int got_ppp_link;

	//Check WAN Cable connect
	if( is_phyconnected(nvram_safe_get("wan_ifname"))==0 
#ifdef CDMA // HSDPA {
                        && !nvram_match("hsdpa_product", "")
#endif // HSDPA }
	) {
	//if ( 	strcmp( nvram_safe_get("wan_status_t"), "Disconnected")==0 
	//	&& strcmp( nvram_safe_get("wan_reason_t"), "Cable is not attached")==0 ) {
		wan_link=0;
 		nvram_set("qos_ubw_status", "fail");
		nvram_set("qos_ubw_reason", "Cable is not attached");
		return wan_link;
	}

        if(nvram_match("wan_proto", "pptp")
        /*|| nvram_match("wan_proto", "PPTP")*/
        || nvram_match("wan_proto", "l2tp")
        || nvram_match("wan_proto", "pppoe")
        /*|| nvram_match("wan_proto", "PPPoE")
        || nvram_match("wan_proto", "BigPong")
        || nvram_match("wan_proto", "heartbeat")*/
        ){
                FILE *fp;
                char filename[80];
		//char conntype[10];
		char buf1[256];
                char *name;
		char* pos;
		char* cpos;
                
                if(num == 0) {
			system("ls /tmp/ppp > /tmp/ppp/filelist");

			if ((fp = fopen("/tmp/ppp/filelist", "r"))) {
			    while( fgets(buf1,256,fp) ) 
				if ( (pos=strstr(buf1, "link")) ) {

				    sprintf(filename, "/tmp/ppp/%s", buf1); 
				    cpos = strchr(filename, '\n');
				    *cpos = '\0';
#ifdef QOS_DEBUG
				    printf("	check_wan_link() ** filename = %s\n", filename);
#endif

				    strcpy(conntype, pos+5);
				    cpos = strchr(conntype, '\n');
				    *cpos = '\0';
#ifdef QOS_DEBUG
				    printf("	check_wan_link() ** conntype = %s\n", conntype);
#endif
				    break;
				}
			}
			else { 
 			    nvram_set("qos_ubw_status", "fail");
			    return wan_link;
			}

			fclose(fp); 
			unlink("/tmp/ppp/filelist");
		}

                if ((fp = fopen(filename, "r"))){
                        int pid = -1;
                        fclose(fp);

                        if(nvram_match("wan_proto", "heartbeat")){
                                char buf[20];
                                file_to_buf(filename, buf, sizeof(buf));
                                pid = atoi(buf);
                        }
                        else
                                pid = get_ppp_pid(conntype);
                                                                                                 
                        name = find_name_by_proc(pid);
                        if(!strncmp(name, "pppoecd", 7) ||      // for PPPoE
                           !strncmp(name, "pppd", 4) ||         // for PPTP
                           !strncmp(name, "bpalogin", 8)) {      // for HeartBeat
                                wan_link = 1;     //connect
 				nvram_set("qos_ubw_status", "initialing");
			}
                        else{
#ifdef QOS_DEBUG
                                printf("The %s had been died, remove %s\n", nvram_safe_get("wan_proto"), filename);
#endif
                                wan_link = 0;   
				// For some reason, the pppoed had been died, by link file still exist.
                                unlink(filename);
                        }
                }
        }
#ifdef CDMA // HSDPA
        else if(!nvram_match("hsdpa_product", "")){
                if((ppp_dir = opendir("/tmp/ppp")) == NULL){
                        wan_link = 0;
                        nvram_set("qos_ubw_status", "fail");
                        nvram_set("qos_ubw_reason", "PPP Error");
                }
                else{
                        got_ppp_link = 0;
                        while((entry = readdir(ppp_dir)) != NULL){
                                if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                                        continue;

                                if(strstr(entry->d_name, "link") != NULL){
                                        got_ppp_link = 1;

                                        break;
                                }
                        }
                        closedir(ppp_dir);

                        if(got_ppp_link == 0){
                                wan_link = 0;
                                nvram_set("qos_ubw_status", "fail");
                                nvram_set("qos_ubw_reason", "PPP Error");
                        }
                        else if(check_ppp_exist() == -1){
                                wan_link = 0;
                                nvram_set("qos_ubw_status", "fail");
                                nvram_set("qos_ubw_reason", "PPP Error");
                        }
                        else{
                                wan_link = 1;
                                nvram_set("qos_ubw_status", "initialing");
                        }
                }
        }
#endif // CDMA
        else{
                if(nvram_invmatch("wan_ipaddr", "0.0.0.0")){
                        wan_link = 1;
 			nvram_set("qos_ubw_status", "initialing");
		}
	}

                                                                                                 
//#ifdef QOS_DEBUG
	printf("	check_wan_link() ** wan_link = %d\n", wan_link);	// tmp test
//#endif
        return wan_link;
}

/**************************************************************************
 * Function Name: get_dns_list()
 * Description  : get DNS server ip from user configuration or retrieved from ISP
 * Parameters   : int no -> indicate using static or dynamaic dns preferentially
                        no = 0 : Static DNS
                        no = 1 : dynamic DNS
 * Returns      : struct dns_lists *  -> contain DNS IP infomation
 **************************************************************************/
struct dns_lists *get_dns_list(int no)
{
        char list[254], prelist[254];
        char *next, word[254];
        struct dns_lists *dns_list = NULL;
        int i, match = 0;
        char got_dns[2][15]={"wan_dns1_x","wan_dns2_x"};
                                                                                                 
        dns_list = (struct dns_lists *)malloc(sizeof(struct dns_lists));
        memset(dns_list, 0, sizeof(struct dns_lists));
        dns_list->num_servers = 0;

        if(no == 0){
                snprintf(prelist, sizeof(prelist), "%s %s",nvram_safe_get(got_dns[0]), nvram_safe_get(got_dns[1]));
                snprintf(list, sizeof(list), "%s %s",prelist ,nvram_safe_get("wan0_dns"));
	}
	else {
                snprintf(prelist, sizeof(prelist), "%s",nvram_safe_get("wan0_dns"));
                snprintf(list, sizeof(list), "%s %s %s",prelist ,nvram_safe_get(got_dns[0]), nvram_safe_get(got_dns[1]));
	}
                                                                                                 
        foreach(word, list, next) {
                if(strcmp(word, "0.0.0.0") && strcmp(word, "")){
                        match = 0;
                       for(i=0 ; i<dns_list->num_servers ; i++){       // Skip same DNS
                                if(!strcmp(dns_list->dns_server[i], word))      match = 1;
                        }
                        if(!match){
                                snprintf(dns_list->dns_server[dns_list->num_servers], sizeof(dns_list->dns_server[dns_list->num_servers]), "%s", word);
                                dns_list->num_servers ++ ;
                        }
                }
                if(dns_list->num_servers == 4)      break;      // We only need 3 counts DNS entry
        }
        return dns_list;

}

/**************************************************************************
 * Function Name: get_gw_ip()
 * Description  : get gateway IP
 * Parameters   : None
 * Returns      : char*  -> Gateway IP String
 **************************************************************************/
/*
char *get_gw_ip(void) {
	char *addr=NULL;

	addr = (char *)malloc(16);
	
	strcpy(addr, nvram_safe_get("lan_ipaddr"));	

	return addr;
}
*/
#endif


