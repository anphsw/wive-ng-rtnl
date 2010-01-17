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
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <asm/types.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <ctype.h>
#include <errno.h>
#include "linux/autoconf.h"

#ifdef USER_MANAGEMENT_SUPPORT
#include "um.h"
#endif
#include "nvram.h"
#include "utils.h"
#include "webs.h"
#include "internet.h"
#include "wireless.h"

#include "management.h"
#ifdef CONFIG_RT2860V2_AP_WSC
#include "wps.h"

extern void WPSRestart(void);
extern void formDefineWPS(void);
#endif

#define COMMAND_MAX	1024
static char system_command[COMMAND_MAX];


/*
 * goform/setSysAdm
 */
static void setSysAdm(webs_t wp, char_t *path, char_t *query)
{
	char_t *admuser, *admpass;
	char *old_user;

	old_user = nvram_bufget(RT2860_NVRAM, "Login");
	admuser = websGetVar(wp, T("admuser"), T(""));
	admpass = websGetVar(wp, T("admpass"), T(""));

	if (!strlen(admuser)) {
		error(E_L, E_LOG, T("setSysAdm: account empty, leave it unchanged"));
		return;
	}
	if (!strlen(admpass)) {
		error(E_L, E_LOG, T("setSysAdm: password empty, leave it unchanged"));
		return;
	}
	nvram_bufset(RT2860_NVRAM, "Login", admuser);
	nvram_bufset(RT2860_NVRAM, "Password", admpass);
	nvram_commit(RT2860_NVRAM);

#ifdef USER_MANAGEMENT_SUPPORT
	if (umGroupExists(T("adm")) == FALSE)
		umAddGroup(T("adm"), 0x07, AM_DIGEST, FALSE, FALSE);
	if (old_user != NULL && umUserExists(old_user))
		umDeleteUser(old_user);
	if (umUserExists(admuser))
		umDeleteUser(admuser);
	umAddUser(admuser, admpass, T("adm"), FALSE, FALSE);
#endif

	websHeader(wp);
	websWrite(wp, T("<h2>Adminstrator Settings</h2><br>\n"));
	websWrite(wp, T("adm user: %s<br>\n"), admuser);
	websWrite(wp, T("adm pass: %s<br>\n"), admpass);
	websFooter(wp);
	websDone(wp, 200);        
}

/*
 * goform/setSysLang
 */
static void setSysLang(webs_t wp, char_t *path, char_t *query)
{
	char_t *lang;

	lang = websGetVar(wp, T("langSelection"), T(""));
	nvram_bufset(RT2860_NVRAM, "Language", lang);
	nvram_commit(RT2860_NVRAM);

	websHeader(wp);
	websWrite(wp, T("<h2>Language Selection</h2><br>\n"));
	websWrite(wp, T("language: %s<br>\n"), lang);
	websFooter(wp);
	websDone(wp, 200);        
}

/*
 * goform/NTP
 */
static void NTP(webs_t wp, char_t *path, char_t *query)
{
	char *tz, *ntpServer, *ntpSync;

	tz = websGetVar(wp, T("time_zone"), T(""));
	ntpServer = websGetVar(wp, T("NTPServerIP"), T(""));
	ntpSync = websGetVar(wp, T("NTPSync"), T(""));

	if(!tz || !ntpServer || !ntpSync)
		return;

	if(!strlen(tz))
		return;

	if(checkSemicolon(tz))
		return;

	if(!strlen(ntpServer)){
		// user choose to make  NTP server disable
		nvram_bufset(RT2860_NVRAM, "NTPServerIP", "");
		nvram_bufset(RT2860_NVRAM, "NTPSync", "");
	}else{
		if(checkSemicolon(ntpServer))
			return;
		if(!strlen(ntpSync))
			return;
		if(atoi(ntpSync) > 300)
			return;
		nvram_bufset(RT2860_NVRAM, "NTPServerIP", ntpServer);
		nvram_bufset(RT2860_NVRAM, "NTPSync", ntpSync);
	}
	nvram_bufset(RT2860_NVRAM, "TZ", tz);
	nvram_commit(RT2860_NVRAM);

	doSystem("ntp.sh");

	websHeader(wp);
	websWrite(wp, T("<h2>NTP Settings</h2><br>\n"));
	websWrite(wp, T("NTPserver: %s<br>\n"), ntpServer);
	websWrite(wp, T("TZ: %s<br>\n"), tz);
	websWrite(wp, T("NTPSync: %s<br>\n"), ntpSync);
	websFooter(wp);
	websDone(wp, 200);        
}

/*
 * goform/NTPSyncWithHost
 */
static void NTPSyncWithHost(webs_t wp, char_t *path, char_t *query)
{
	if(!query || (!strlen(query)))
		return;
	if(strchr(query, ';'))
		return;

    doSystem("date -s %s", query);


	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\n\n"));
    websWrite(wp, T("n/a"));
    websDone(wp, 200);
}


/*
 * goform/DDNS
 */
static void DDNS(webs_t wp, char_t *path, char_t *query)
{
	char *ddns_provider, *ddns, *ddns_acc, *ddns_pass;
	char empty_char = '\0';

	ddns_provider = websGetVar(wp, T("DDNSProvider"), T("none"));
	ddns = websGetVar(wp, T("DDNS"), T(""));
	ddns_acc = websGetVar(wp, T("Account"), T(""));
	ddns_pass = websGetVar(wp, T("Password"), T(""));

	if(!ddns_provider || !ddns || !ddns_acc || !ddns_pass)
		return;

	if(!strcmp(T("none"), ddns_provider )){
		ddns = ddns_acc = ddns_pass = &empty_char;
	}else{
		if(!strlen(ddns) || !strlen(ddns_acc) || !strlen(ddns_pass))
			return;
	}

	if(checkSemicolon(ddns) || checkSemicolon(ddns_acc) || checkSemicolon(ddns_pass))
		return;

	nvram_bufset(RT2860_NVRAM, "DDNSProvider", ddns_provider);
	nvram_bufset(RT2860_NVRAM, "DDNS", ddns);
	nvram_bufset(RT2860_NVRAM, "DDNSAccount", ddns_acc);
	nvram_bufset(RT2860_NVRAM, "DDNSPassword", ddns_pass);
	nvram_commit(RT2860_NVRAM);

	doSystem("ddns.sh");

	websHeader(wp);
	websWrite(wp, T("<h2>DDNS Settings</h2><br>\n"));
	websWrite(wp, T("DDNSProvider: %s<br>\n"), ddns_provider);
	websWrite(wp, T("DDNS: %s<br>\n"), ddns);
	websWrite(wp, T("DDNSAccount: %s<br>\n"), ddns_acc);
	websWrite(wp, T("DDNSPassword: %s<br>\n"), ddns_pass);
	websFooter(wp);
	websDone(wp, 200);        
}

static void SystemCommand(webs_t wp, char_t *path, char_t *query)
{
	char *command;

	command = websGetVar(wp, T("command"), T(""));

	if(!command)
		return;

	if(!strlen(command))
		snprintf(system_command, COMMAND_MAX, "cat /dev/null > %s", SYSTEM_COMMAND_LOG);
	else
		snprintf(system_command, COMMAND_MAX, "%s 1>%s 2>&1", command, SYSTEM_COMMAND_LOG);
	
	if(strlen(system_command))
		doSystem(system_command);

	// FIXME/TODO, YYHuang 07/04/11
	// the path here should be obtained by goahead internal function.
	// (is it existed?)
	websRedirect(wp, "adm/system_command.asp");

	return;
}


int showSystemCommandASP(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char buf[1024];
	
	fp = fopen(SYSTEM_COMMAND_LOG, "r");
	if(!fp){
		websWrite(wp, T(""));
		return 0;
	}

	while(fgets(buf, 1024, fp)){
		websWrite(wp, T("%s"), buf);
	}
	fclose(fp);
	
	return 0;
}

static inline char *strip_space(char *str)
{
	while( *str == ' ')
		str++;
	return str;
}


char* getField(char *a_line, char *delim, int count)
{
	int i=0;
	char *tok;
	tok = strtok(a_line, delim);
	while(tok){
		if(i == count)
			break;
        i++;
		tok = strtok(NULL, delim);
    }
    if(tok)
		return tok;

	return NULL;
}

/*
 *   C version. (ASP version is below)
 */
static long long getIfStatistic(char *interface, int type)
{
	int found_flag = 0;
	int skip_line = 2;
	char buf[1024], *field, *semiColon = NULL;
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if(!fp){
		printf("no proc?\n");
		return -1;
	}

	while(fgets(buf, 1024, fp)){
		char *ifname;
		if(skip_line != 0){
			skip_line--;
			continue;
		}
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		ifname = buf;
		ifname = strip_space(ifname);

		if(!strcmp(ifname, interface)){
			found_flag = 1;
			break;
		}
	}
	fclose(fp);

	semiColon++;

	switch(type){
	case TXBYTE:
		if(  (field = getField(semiColon, " ", 8))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case TXPACKET:
		if(  (field = getField(semiColon, " ", 9))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case RXBYTE:
		if(  (field = getField(semiColon, " ", 0))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case RXPACKET:
		if(  (field = getField(semiColon, " ", 1))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	}
	return -1;
}

/*
 *     getIfStatistic()   ASP version
 */
int getIfStatisticASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int found_flag = 0;
	int skip_line = 2;
	char *interface, *type, *field, *semiColon = NULL;
	char buf[1024], result[32];
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if(!fp){
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

    if(ejArgs(argc, argv, T("%s %s"), &interface, &type) != 2){
		websWrite(wp, T("Wrong argument.\n"));
        return -1;
    }

	while(fgets(buf, 1024, fp)){
		char *ifname;
		if(skip_line != 0){
			skip_line--;
			continue;
		}
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		ifname = buf;
		ifname = strip_space(ifname);

		if(!strcmp(ifname, interface)){
			found_flag = 1;
			break;
		}
	}
	fclose(fp);

	semiColon++;

	if(!strcmp(type, T("TXBYTE")  )){
		if(  (field = getField(semiColon, " ", 8))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
	}else if(!strcmp(type, T("TXPACKET")  )){
		if(  (field = getField(semiColon, " ", 9))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else if(!strcmp(type, T("RXBYTE")  )){
		if(  (field = getField(semiColon, " ", 0))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else if(!strcmp(type, T("RXPACKET")  )){
		if(  (field = getField(semiColon, " ", 1))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else{
		websWrite(wp, T("unknown type.") );
		return -1;
	}
	return -1;
}

int getWANRxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), RXBYTE);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getWANRxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), RXPACKET);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getWANTxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), TXBYTE);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getWANTxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), TXPACKET);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANRxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), RXBYTE);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANRxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), RXPACKET);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANTxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), TXBYTE);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANTxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), TXPACKET);
	snprintf(buf, 32, "%lld", data);	
	websWrite(wp, T("%s"),buf);
	return 0;
}

/*
 * This ASP function is for javascript usage, ex:
 *
 * <script type="text/javascript">
 *   var a = new Array();
 *   a = [<% getAllNICStatisticASP(); %>];         //ex: a = ["lo","10","1000", "20", "2000","eth2"];
 *   document.write(a)
 * </script>
 *
 * Javascript could get info with  getAllNICStatisticASP().
 *
 * We dont produce table-related tag in this ASP function .It's
 * more extensive since ASP just handle data and Javascript present them,
 * although the data form is only for Javascript now.
 *
 * TODO: a lot, there are many ASP functions binding with table-relted tag...
 */
int getAllNICStatisticASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char result[1024];
	char buf[1024];
	int rc = 0, pos = 0, skip_line = 2;
	int first_time_flag = 1;
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if(!fp){
		printf("no proc?\n");
		return -1;
	}

	while(fgets(buf, 1024, fp)){
		char *ifname, *semiColon;
		if(skip_line != 0){
			skip_line--;
			continue;
		}
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';

		ifname = buf;
		ifname = strip_space(ifname);
		if(first_time_flag){
			pos = snprintf(result+rc, 1024-rc, "\"%s\"", ifname);
			rc += pos;
			first_time_flag = 0;
		}else{
			pos = snprintf(result+rc, 1024-rc, ",\"%s\"", ifname);
			rc += pos;
		}

		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", getIfStatistic(ifname, RXPACKET));
		rc += pos;
		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", getIfStatistic(ifname, RXBYTE));
		rc += pos;
		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", getIfStatistic(ifname, TXPACKET));
		rc += pos;
		pos = snprintf(result+rc, 1024-rc, ",\"%lld\"", getIfStatistic(ifname, TXBYTE));
		rc += pos;
	}
	fclose(fp);

	websWrite(wp, T("%s"), result);
    return 0;
}


int getMemTotalASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[1024], *semiColon, *key, *value;
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");
	if(!fp){
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	while(fgets(buf, 1024, fp)){
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		key = buf;
		value = semiColon + 1;
		if(!strcmp(key, "MemTotal")){
			value = strip_space(value);
			websWrite(wp, T("%s"), value);
			fclose(fp);
			return 0;
		}
	}
	websWrite(wp, T(""));
	fclose(fp);
	
	return -1;
}

int getCurrentTimeASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[64];
	FILE *fp = popen("date", "r");
	if(!fp){
		websWrite(wp, T("none"));
		return 0;
	}
	fgets(buf, 64, fp);
	pclose(fp);

	websWrite(wp, T("%s"), buf);
	return 0;
}

int getMemLeftASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[1024], *semiColon, *key, *value;
	FILE *fp = fopen(PROC_MEM_STATISTIC, "r");
	if(!fp){
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

	while(fgets(buf, 1024, fp)){
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		key = buf;
		value = semiColon + 1;
		if(!strcmp(key, "MemFree")){
			value = strip_space(value);
			websWrite(wp, T("%s"), value);
			fclose(fp);
			return 0;
		}
	}
	websWrite(wp, T(""));
	fclose(fp);
	return -1;
}

static void LoadDefaultSettings(webs_t wp, char_t *path, char_t *query)
{
	doSystem("ralink_init clear 2860");
#if defined INIC_SUPPORT || defined INICv2_SUPPORT
	doSystem("ralink_init clear inic");
#endif
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	doSystem("ralink_init clear 2561");
#endif

#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
	doSystem("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_vlan");
	#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	doSystem("ralink_init renew inic /etc_ro/Wireless/RT2860AP/RT2860_default_vlan");
	#endif
#elif defined(CONFIG_ICPLUS_PHY)
	doSystem("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_oneport");
	#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	doSystem("ralink_init renew inic /etc_ro/Wireless/RT2860AP/RT2860_default_oneport");
	#endif
#else
	doSystem("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_novlan");
	#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	doSystem("ralink_init renew inic /etc_ro/Wireless/RT2860AP/RT2860_default_novlan");
	#endif
#endif
#if defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
	doSystem("ralink_init renew 2561 /etc_ro/Wireless/RT61AP/RT2561_default");
#endif
	system("reboot");
}


/*
 * callee must free memory.
 */
/*
static char *getLog(char *filename)
{
	FILE *fp;
	struct stat filestat;
	char *log;

	if(stat(filename, &filestat) == -1)
		return NULL;

//	printf("%d\n", filestat.st_size);
	log = (char *)malloc(sizeof(char) * (filestat.st_size + 1) );
	if(!log)
		return NULL;

	if(!(fp = fopen(filename, "r"))){
		return NULL;
	}

	if( fread(log, 1, filestat.st_size, fp) != filestat.st_size){
		printf("read not enough\n");
		free(log);
		return NULL;
	}

	log[filestat.st_size] = '\0';

	fclose(fp);
	return log;
}
*/

static void clearlog(webs_t wp, char_t *path, char_t *query)
{
	doSystem("killall -q klogd");
	doSystem("killall -q syslogd");
	doSystem("syslogd -C8 1>/dev/null 2>&1");
	doSystem("klogd 1>/dev/null 2>&1");

	websRedirect(wp, "adm/syslog.asp");
}


#define LOG_MAX (16384)
static void syslog(webs_t wp, char_t *path, char_t *query)
{
	FILE *fp = NULL;
	char *log;

	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\n\n"));

	fp = popen("logread", "r");
	if(!fp){
		websWrite(wp, "-1");
		goto error;
	}

	log = malloc(LOG_MAX * sizeof(char));
	if(!log){
		websWrite(wp, "-1");
		goto error;
	}
	memset(log, 0, LOG_MAX);
	fread(log, 1, LOG_MAX, fp);
	websLongWrite(wp, log);

	free(log);
error:
	if(fp)
		pclose(fp);
	websDone(wp, 200);
}

void management_init(void)
{
	doSystem("ntp.sh");
	doSystem("ddns.sh");
#ifdef CONFIG_RT2860V2_AP_WSC
	WPSRestart();
#endif
	doSystem("killall -q klogd");
	doSystem("killall -q syslogd");
	doSystem("syslogd -C8 1>/dev/null 2>&1");
	doSystem("klogd 1>/dev/null 2>&1");
}

void management_fini(void)
{
	doSystem("killall -q klogd");
	doSystem("killall -q syslogd");
}

void formDefineManagement(void)
{
	websFormDefine(T("setSysAdm"), setSysAdm);
	websFormDefine(T("setSysLang"), setSysLang);
	websFormDefine(T("NTP"), NTP);
	websFormDefine(T("NTPSyncWithHost"), NTPSyncWithHost);
	websAspDefine(T("getCurrentTimeASP"), getCurrentTimeASP);
	websFormDefine(T("DDNS"), DDNS);

	websAspDefine(T("getMemLeftASP"), getMemLeftASP);
	websAspDefine(T("getMemTotalASP"), getMemTotalASP);

	websAspDefine(T("getWANRxByteASP"), getWANRxByteASP);
	websAspDefine(T("getWANTxByteASP"), getWANTxByteASP);
	websAspDefine(T("getLANRxByteASP"), getLANRxByteASP);
	websAspDefine(T("getLANTxByteASP"), getLANTxByteASP);
	websAspDefine(T("getWANRxPacketASP"), getWANRxPacketASP);
	websAspDefine(T("getWANTxPacketASP"), getWANTxPacketASP);
	websAspDefine(T("getLANRxPacketASP"), getLANRxPacketASP);
	websAspDefine(T("getLANTxPacketASP"), getLANTxPacketASP);

	websAspDefine(T("getAllNICStatisticASP"), getAllNICStatisticASP);

	websAspDefine(T("showSystemCommandASP"), showSystemCommandASP);
	websFormDefine(T("SystemCommand"), SystemCommand);

	websFormDefine(T("LoadDefaultSettings"), LoadDefaultSettings);

	websFormDefine(T("syslog"), syslog);
	websFormDefine(T("clearlog"), clearlog);
#ifdef CONFIG_RT2860V2_AP_WSC
	formDefineWPS();
#endif
}
