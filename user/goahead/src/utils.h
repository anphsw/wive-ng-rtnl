/*
 *	utils.h -- System Utilitie Header
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: utils.h,v 1.25 2008-03-17 09:28:40 yy Exp $
 */

#include        "config/autoconf.h"			//user config
#include	<linux/autoconf.h>			//kernel config
#include        "user/busybox/include/autoconf.h"	//busybox config
#include        "sdk_version.h"				//version config

#include        "linux/ralink_gpio.h"			//gpio config

#include	<linux/reboot.h>
#include	<sys/reboot.h>

#include 	"webs.h"
#include	"uemf.h"

#include        "nvram.h"
#include        "stapriv.h"

#ifdef CONFIG_USER_WSC
#include	"wps.h"
#endif

#ifdef CONFIG_RALINKAPP_SWQOS
#include	"qos.h"
#endif

#define LED_ON				1
#define LED_OFF				0

#define WPS_LED_RESET			1
#define WPS_LED_PROGRESS		2
#define WPS_LED_ERROR			3
#define WPS_LED_SESSION_OVERLAP		4
#define WPS_LED_SUCCESS			5

#if defined(CONFIG_RALINK_RT3052)
#define PROCREG_GMAC	"/proc/rt3052/gmac"
#elif defined(CONFIG_RALINK_RT3352)
#define PROCREG_GMAC	"/proc/rt3352/gmac"
#elif defined (CONFIG_RALINK_RT5350)
#define PROCREG_GMAC	"/proc/rt5350/gmac"
#elif defined(CONFIG_RALINK_RT3883)
#define PROCREG_GMAC	"/proc/rt3883/gmac"
#elif defined (CONFIG_RALINK_RT6855)
#define PROCREG_GMAC	"/proc/rt6855/gmac"
#elif defined (CONFIG_RALINK_RT63365)
#define PROCREG_GMAC	"/proc/rt63365/gmac"
#elif defined (CONFIG_RALINK_MT7620)
#define PROCREG_GMAC	"/proc/mt7620/gmac"
#else
#define PROCREG_GMAC	"/proc/rt3052/gmac"
#endif

void reboot_now(void);
void formDefineUtilities(void);
int checkSemicolon(char *str);
int doSystem(char_t *fmt, ...);
char *getNthValue(int index, char *values);
char *setNthValue(int index, char *old_values, char *new_value);
int deleteNthValueMulti(int index[], int count, char *value, char delimit);
int getNthValueSafe(int index, char *value, char delimit, char *result, int len);
int setTimer(int mili, void ((*sigroutine)(int)));
void stopTimer(void);
int ledAlways(int gpio, int on);
int ledWps(int gpio, int mode);
unsigned int Sleep(unsigned int secs);
char *racat(char *s, int i);
void arplookup(char *ip, char *arp);
void websLongWrite(webs_t wp, char *longstr);
char *strip_space(char *str);
int netmask_aton(const char *ip);
void STFs(int nvram, int index, char *flash_key, char *value);
#ifdef CONFIG_USER_802_1X
void restart8021XDaemon(int nvram);
void updateFlash8021x(int nvram);
#endif
