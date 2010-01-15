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
/*****************************************************************************************************************
 * Filr Name	: speedtest.c
 * Description  : modify from ping.c
 * related file : qosutils.h qosutils.c
 * Auther       : Wendel Huang
 * History	: 2006.05.22 	post to WL500gP
		  2006.07.18 	Post to WL700g, and remove execution message form release version
		  2006.08.01    Debug the error in MER mode
 *****************************************************************************************************************/
#include <stdio.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/signal.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <shutils.h>

#include <rc_event.h>

#include "qosutils.h"

#ifdef WL600g
#include "asusbcmnvram.h"
#include "dbapi.h"
#include "ifcwanapi.h"
#include "syscall.h"
#include "bcmadsl.h"	//for ADSL_CONNECTION_INFO
#else	//WL500gP, WL550gE
#include "nvram/bcmnvram.h"
#endif

#ifdef WL600g
extern ADSL_CONNECTION_INFO glbAdslInfo;// global ADSL info variable is declared in syscall.c
#endif

#include <sys/syscall.h>
_syscall2( int, track_flag, int *, flag, ulong *, ipaddr);

#define STRUCT_LEN(name)    sizeof(name)/sizeof(name[0])
#define MAX_BANDWIDTH	15359	// S = (1500-64)*8/1024 (kbits)
				// S *1000 / (thelta t) * 1.369 (weight) = B
#define DETECT_FILE "/tmp/detect_ip"
#define INIT_INTERVAL 200000

static const int DEFDATALEN = 56;
static const int MAXIPLEN = 60;
static const int MAXICMPLEN = 76;
static const int MAXWAIT = 10;
static const int ADSLMXLEN =1400;
static const int HEADOVERHEAD = 42;
unsigned long int triptime[18];		//14(frame header)+20(ip header)+8(icmp header)
static int request_time=1;
static int interval=INIT_INTERVAL;

#define	A(bit)		rcvd_tbl[(bit)>>3]	/* identify byte in array */
#define	B(bit)		(1 << ((bit) & 0x07))	/* identify bit in byte */
#define	SET(bit)	(A(bit) |= B(bit))
#define	CLR(bit)	(A(bit) &= (~B(bit)))
#define	TST(bit)	(A(bit) & B(bit))

enum { WAN_RATE_SUCCESS, WAN_LINK_FAIL };

/*struct icmp_map_t{
	unsigned difference;
	unsigned rate;
} icmp_map[] = {
	{3000, MAX_BANDWIDTH},
	{1638, 2048},
	{819, 1024},
	{600, 640},
	{410, 512},
	{204, 256},
	{102, 128},
	{51, 64},
	{27, 33},
};//difference = rate * 0.8
*/

struct wan_link_rate {
	unsigned ds;
	unsigned us;
};

/* common routines */
static int in_cksum(unsigned short *buf, int sz)
{
	int nleft = sz;
	int sum = 0;
	unsigned short *w = buf;
	unsigned short ans = 0;

	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		*(unsigned char *) (&ans) = *(unsigned char *) w;
		sum += ans;
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	ans = ~sum;
	return (ans);
}

static struct sockaddr_in pingaddr;

int datalen; /* intentionally uninitialized to work around gcc bug */

// brcm: changed default value of pingcount from 0 to 4.
static long ntransmitted=0, nreceived=0;
int pingcount=10;
static int myid;
static struct timeval tstart, tend;

struct hostent *hostent;

static char gateway_ip[6][30], get_sp[6][10];
static int detect();
static void sendtrack();

/**************************************************************************/

static int detect()
{
	FILE *fp=NULL;
	char cmd[80];
	char detect_ip[20]="18.52.86.150";
	char line[254];
	char get_ip[6][30];
	char *no_host="!H", *no_net="!N", *tr_str = "traceroute to";
	int j;

	printf("## detect ##\n");	// tmp test

	j = 0;
	while(j < 6)
	{
		memset(&get_ip[j], 0, sizeof(get_ip[j]));
		memset(&gateway_ip[j], 0, sizeof(gateway_ip[j]));
		memset(&get_sp[j], 0, sizeof(get_sp[j]));
		++j;
	}
	snprintf(cmd,sizeof(cmd),"traceroute -m 6 %s >%s",detect_ip,DETECT_FILE);
	remove(DETECT_FILE);
	int i;
	int ping=0;
	char *p;
	char *ms_p, *line_p, *tmp_p, tmp_buf[10]; 
	double ms_num, ms_n, ms_avg, sp;

	system(cmd);

	i = 0;
	if ((fp = fopen(DETECT_FILE, "r")) != NULL) 
	{
		while(i < 6)
		{
			/* get ip */
			if( fgets(line, sizeof(line), fp) != NULL ) 
			{
                                if(strstr(line, no_host) || strstr(line, no_net))       // add
                                {
                                        printf("detect: no host or no network\n");      // tmp test
					fclose(fp);
                                        return -1;
                                }

				if(strstr(line, tr_str))
					continue;

				p=index(line,'(');
				if(!p)
					continue;

				j=1;
				while((*p)!=')')
				{
					++p;
					get_ip[i][j-1]=*p;
					++j;
				}
				get_ip[i][j-2]='\0';
				
			}

			/* get speed */
                        ms_num = 0.0;
                        ms_n = 0;
                        line_p = line;
                        while((ms_p = strstr(line_p, "ms")) != NULL)
                        {
                                ++ms_n;
                                tmp_p = ms_p-2;
                                line_p = ms_p + 1;
                                j = 0;
                                memset(tmp_buf, 0, sizeof(tmp_buf));

                                for(; *tmp_p!= ' '; --tmp_p, ++j)
                                {
                                        tmp_buf[9-j] = *tmp_p;
                                }
                                j=0;
                                while((tmp_buf[j]=='\0') && (j < 10))
                                {
                                        tmp_buf[j] = '0';
                                        ++j;
                                }

                                ms_num += atof(tmp_buf);
                                //printf("get ms from [%s]: %f, (%s)\n", get_ip[i], ms_num, tmp_buf);     // tmp test
                        }
                        if(ms_num > 0.0)
                        {
                                memset(tmp_buf, 0, sizeof(tmp_buf));
                                ms_avg = ms_num / ms_n;
                                sp = (((38.00/1024.00)/(ms_avg/1000000)))/2;
                                sprintf(get_sp[i], "%.2f", sp);
                                //printf("get ms avg : %f(%f), sp=%s\n", ms_avg, ms_num, get_sp[i]);      // tmp test
                        }
                        ++i;
		}

		i=0;
		while(i<6)
		{
			sprintf(gateway_ip[i],"%s",get_ip[i]);
			++i;
		}	
	}
	else
		return 0;

	fclose(fp);
	printf("end detect\n");	// tmp test
	return 1;
}

/**************************************************************************
 * Function Name: qos_get_wan_rate
 * Description  : get the ADSL line rate
 * Parameters   : None
 * Returns      : Is successful when get the ADSL data rate? 
			0 : Success
			1 : Link fail
 **************************************************************************/
int qos_get_wan_rate(void)
{
	static struct wan_link_rate wanrate;
	char us[256], ds[256] ;
	char * gw_ip=NULL;
	struct timeval deltaval;
	double delta_us = 0;
	wanrate.us = wanrate.ds = 0;
	int step=3;
	char *ip = nvram_safe_get("wan_dns_t");
	int max_request_time;
	double sp, sp_avg, sp_valid, tmp_add;
	char ubw_buf[10];
	int i;

	printf("\nQos get wan rate\n");	// tmp test

	if( !check_wan_link(0) )
	{
		printf("check wan link fail\n");	// tmp test
		return WAN_LINK_FAIL;
	}

	gw_ip = nvram_safe_get("lan_ipaddr");

	if((strchr(ip, ' ')) ||(!strcmp(ip, "")))
	{
		struct dns_lists *dns_list = get_dns_list(1);

		for(i=0 ; i<dns_list->num_servers ; i++){
			ip = dns_list->dns_server[i];
			if(    (!strchr(ip, ' ')) 
					&& (strcmp(ip, ""))
					&& (strcmp(ip, gw_ip))) //WL600g auto assign LAN IP as DNS Server 
				break;
		}
		free(dns_list);
	}

	if( (strchr(ip, ' ')) || (!strcmp(ip, "")) || (!strcmp(ip, gw_ip)) ) {
		nvram_set("qos_ubw_status", "fail");
		nvram_set("qos_ubw_reason", "DNS server fail");
		nvram_set("qos_ubw", "0kbit");
		return WAN_LINK_FAIL;
	}
	else 
	{
		if(detect() == 1)
		{
			tmp_add = 0.0;
			sp_valid = 0.0;
			for(i=1; i<6; ++i)	/* discard first gateway */
			{
				if(strlen(gateway_ip[i]) > 0)
				{
					//printf("test [%s][%s]\n", gateway_ip[i], get_sp[i]);	// tmp test
					if((sp=atof(get_sp[i])) > 0.0)
					{
						tmp_add += sp;
						++sp_valid;
						//printf("tmp_add is %f, valid is %f\n", tmp_add, sp_valid);	// tmp test
					}
				}
			}
			sp_avg = tmp_add/sp_valid;
			memset(ubw_buf, 0, sizeof(ubw_buf));
			sprintf(ubw_buf, "%.2f", sp_avg);

			nvram_set("qos_ubw_status", "success");
			nvram_set("qos_ubw", ubw_buf);
			nvram_set("qos_ubw_tmp", ubw_buf);
			nvram_set("qos_ubw_reason", "");

			printf("Sp avg is %s (%f)(%f)\n", ubw_buf, sp_valid, tmp_add);	// tmp test

			//return WAN_RATE_SUCCESS;
			return 0;
		} 
		else
		{
			nvram_set("qos_ubw_status", "fail");
			nvram_set("qos_ubw_reason", "detect fail");
			nvram_set("qos_ubw", "0");
			return WAN_LINK_FAIL;
		}
	}
}

#define isdigit(c) (c >= '0' && c <= '9') 
static long atoid(const char *s)
{
         int i=0,j;
	 long k=0;
	 for(j=1; j<5; j++) {
		 i=0;
	         while (isdigit(*s)) {
        	         i = i*10 + *(s++) - '0';
		}
		 k = k*256 +i;
		 if(j == 4)
			 return k;
		 s++;
	}
         return k;
}

int flag = 0;
ulong ipaddr = 0;

void
track_set(char *c_track)
{
        int k_track = atoi(c_track);
        ipaddr = atoid(nvram_safe_get("wan_ipaddr_t"));

        if(!track_flag(&k_track, &ipaddr))
	{
                printf("track ok:%d\n", k_track);
		event_code = k_track;
	}
        else
                printf("track fail\n");

}

int Speedtest_Init(void) {
	char net_name[32];
	FILE *fp=NULL;
	char pbuf[128];	// tmp test

	nvram_set("qos_userspec_app", "0");
	nvram_set("qos_global_enable", "0");
	nvram_set("qos_userdef_enable", "0");

	printf("\nSpeedtest init\n");	// tmp test

        if ( nvram_invmatch("qos_rulenum_x", "0"))
                nvram_set("qos_userspec_app", "1");
	
	//add by Angela 2008.05
	if(nvram_match("qos_tos_prio", "1")||nvram_match("qos_pshack_prio", "1")
	  || nvram_match("qos_service_enable", "1")|| nvram_match("qos_shortpkt_prio", "1"))
		nvram_set("qos_global_enable", "1");
	
	if(nvram_match("qos_userspec_app", "1") || nvram_match("qos_dfragment_enable", "1"))
		nvram_set("qos_userdef_enable", "1");

	/* Get interface name */
	if (nvram_match("wan0_proto", "pppoe") || nvram_match("wan0_proto","pptp"))
		strcpy (net_name, nvram_safe_get("wan0_pppoe_ifname"));
	else	
		strcpy (net_name, nvram_safe_get("wan0_ifname"));

	/* Reset all qdisc first */
	eval ("tc","qdisc","del", "dev", net_name, "root", "htb");
#if 0
/* tmp test */
        memset(pbuf, 0, sizeof(pbuf));
        sprintf(pbuf, "echo 'tc qdisc del dev %s root htb' >>/tmp/tc_rules", net_name);
        system(pbuf);
        printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif
	eval ("tc","qdisc","del", "dev", "br0", "root", "htb");

#if 0
/* tmp test */
        memset(pbuf, 0, sizeof(pbuf));
        sprintf(pbuf, "echo 'tc qdisc del dev br0 root htb' >>/tmp/tc_rules");
        system(pbuf);
        printf("[exe tc] %s\n", pbuf);
/*~tmp */
#endif
	/* Clean iptables*/
	/*eval("iptables", "-F", "-t", "mangle");*/
	if ((fp=fopen("/tmp/mangle_rules", "w"))==NULL) 
	{
		nvram_set("qos_file"," /tmp/mangle_rules file doesnot exist.");
		if((fp = fopen("/tmp/mangle_rules", "w+")) == NULL)
			return ;
	}
	fprintf(fp, "*mangle\n");
	fprintf(fp, "-F\n");
	fprintf(fp, "COMMIT\n\n");
	fclose(fp);
	eval("iptables-restore", "/tmp/mangle_rules");

	if(nvram_invmatch("qos_manual_ubw","0") && nvram_invmatch("qos_manual_ubw",""))
	{
		nvram_set("qos_ubw",nvram_safe_get("qos_manual_ubw"));
	}
	//else 
	//	nvram_set("qos_ubw",nvram_get("qos_ubw_tmp"));

//2008.10 magic{
	if(nvram_match("qos_global_enable", "1") || nvram_match("qos_userdef_enable", "1"))
	{
		nvram_set("qos_enable", "1");
		nvram_commit();
		track_set("1");

		if(nvram_invmatch("qos_ubw", "0"))
		{
			flag = 1;
			ipaddr = atoid(nvram_safe_get("wan_ipaddr_t"));
			start_qos();
		}
	}
	else
	{
		nvram_set("qos_enable", "0");
		nvram_commit();
		track_set("0");

		flag = 0;
	}
//2008.10 magic}
	if(!track_flag(&flag, &ipaddr))
		nvram_set("qos_sys", "right set");
	else 
		nvram_set("qos_syss", "error set");
	return 0;       
}

#if 0
#ifdef MYGCC
int main(int argc, char** argv)
{
	int i = 0;
	char *thisarg;

	printf("	%-20s	%-20s\n", "index", "value");

	while( i<argc ) {
	    printf("	%-20d	%-20s\n", i, argv[i]);
	    i++;
	}

	argc--;
	argv++;
	while( argc>0 && **argv=='-' ) {
	    	thisarg = *argv;
		thisarg++;
		switch (*thisarg) {
		case 'c':
			if (--argc <= 0)
			        exit(1);
			argv++;
			pingcount = atoi(*argv);
			break;
		case 's':
			if (--argc <= 0)
			        exit(1);
			argv++;
			datalen = atoi(*argv);
			break;
		default:
			exit(1);
		}
		argc--;
		argv++;
	}

	if( argc<0 ) 	exit(1);
	
        struct timeval deltaval;

        //deltatime(*argv, &deltaval);
	
	printf("\n	start time ** tv_sec=%u : tv_usec=%u\n",tstart.tv_sec, tstart.tv_usec);
	printf("	end time   ** tv_sec=%u : tv_usec=%u\n", tend.tv_sec, tend.tv_usec);
        printf("        	      delta_sec=%u : delta_usec=%u\n", deltaval.tv_sec*1000000, deltaval.tv_usec);
        
	double delta_us = 0;
	delta_us = (int)deltaval.tv_sec * 1000000 + deltaval.tv_usec;
        printf("        delta interval = %f us\n", delta_us);
                                                                                                 
        double bw = ( (( (datalen+42)*pingcount*8 )/delta_us) )*1000000/1024;
        printf("\n	bandwidth = %.0f kbps\n", bw);
	
	return 0;
}
#endif 
#endif 
