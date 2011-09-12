#ifndef _NVRAM_H
#define _NVRAM_H 	1

#include <linux/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* use nutex only  if kernel
    nvram support disabled */
#ifndef CONFIG_KERNEL_NVRAM
#ifdef CONFIG_LIB_PTHREAD_FORCE
#define NVRAM_LIB_PTHREAD_FORCE
#endif
#else
#undef NVRAM_LIB_PTHREAD_FORCE
#endif

#ifdef CONFIG_LIB_LIBNVRAM_SSTRDUP
#define NVRAM_LIB_LIBNVRAM_SSTRDUP
#endif

#ifdef NVRAM_LIB_LIBNVRAM_SSTRDUP
#ifdef NVRAM_LIB_PTHREAD_FORCE
#include <pthread.h>
#endif
#endif

#define BUFSZ			1024
#define MAX_NV_VALUE_LEN	64
#define NV_BUFFERS_COUNT	128

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

#define NV_DEV "/dev/nvram"
#define RALINK_NVRAM_IOCTL_GET		0x01
#define RALINK_NVRAM_IOCTL_GETALL	0x02
#define RALINK_NVRAM_IOCTL_SET		0x03
#define RALINK_NVRAM_IOCTL_COMMIT	0x04
#define RALINK_NVRAM_IOCTL_CLEAR	0x05

#define FREE(x) do { if (x != NULL) {free(x); x=NULL;} } while(0)

typedef struct environment_s {
	unsigned long crc;		//CRC32 over data bytes
	char *data;
} env_t;

typedef struct cache_environment_s {
	char *name;
	char *value;
} cache_t;

#define MAX_CACHE_ENTRY 500
typedef struct block_s {
	char *name;
	env_t env;			//env block
	cache_t	cache[MAX_CACHE_ENTRY];	//env cache entry by entry
	unsigned long flash_offset;
	unsigned long flash_max_len;	//ENV_BLK_SIZE

	char valid;
	char dirty;
} block_t;

#define MAX_NAME_LEN 128
#define MAX_VALUE_LEN 1024
typedef struct nvram_ioctl_s {
	int index;
	int ret;
	char *name;
	char *value;
} nvram_ioctl_t;


#ifdef CONFIG_DUAL_IMAGE
#define FLASH_BLOCK_NUM	5
#else
#define FLASH_BLOCK_NUM	4
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
