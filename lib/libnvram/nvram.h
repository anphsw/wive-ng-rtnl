#ifndef _NVRAM_H
#define _NVRAM_H 	1

#include <linux/autoconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFSZ			1024

#ifdef CONFIG_LIB_LIBNVRAM_SSTRDUP
#ifdef CONFIG_LIB_PTHREAD_FORCE
#include <pthread.h>
#endif

#define MAX_NV_VALUE_LEN	64
#define NV_BUFFERS_COUNT	128
#endif

#ifdef CONFIG_DUAL_IMAGE
#define UBOOT_NVRAM	0
#define RT2860_NVRAM    1
#define RTDEV_NVRAM    	2
#define CERT_NVRAM    	3
#define WAPI_NVRAM    	4
#define RTINIC_NVRAM    1
#define RT2561_NVRAM    2
#else
#define RT2860_NVRAM    0
#define RTDEV_NVRAM    	1
#define CERT_NVRAM    	2
#define WAPI_NVRAM    	3
#define RTINIC_NVRAM    1
#define RT2561_NVRAM    2
#endif

void nvram_init(int index);
void nvram_close(int index);

int nvram_set(int index, char *name, char *value);
int nvram_bufset(int index, char *name, char *value);
char *nvram_get(int index, char *name);
char *nvram_bufget(int index, char *name);

void nvram_buflist(int index);
int nvram_commit(int index);
int nvram_clear(int index);
int nvram_erase(int index);

int getNvramNum(void);
unsigned int getNvramOffset(int index);
unsigned int getNvramBlockSize(int index);
char *getNvramName(int index);
unsigned int getNvramIndex(char *name);
void toggleNvramDebug(void);
int renew_nvram(int mode, char *fname);
int nvram_show(int mode);
int nvram_load_default(void);
int gen_wifi_config(int mode);
#endif
