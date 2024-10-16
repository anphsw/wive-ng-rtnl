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

#define NV_DEV "/dev/nvram"

#define BUFSZ			1024
#define ENV_BLK_SIZE		0x1000

#ifdef CONFIG_DUAL_IMAGE
#define ENV_UBOOT_SIZE 0x1000
#define FLASH_BLOCK_NUM	5
#define UBOOT_NVRAM	0
#define RT2860_NVRAM    1
#define RTINIC_NVRAM    2
#else
#define FLASH_BLOCK_NUM	4
#define RT2860_NVRAM    0
#define RTINIC_NVRAM    1
#endif

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

#define MAX_CACHE_ENTRY 1000
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
#define MAX_VALUE_LEN (ENV_BLK_SIZE * 5)
#define MAX_PERMITTED_VALUE_LEN  (MAX_VALUE_LEN * 2)
typedef struct nvram_ioctl_s {
	int index;
	int ret;
	char *name;
	char *value;
	int size;
} nvram_ioctl_t;


int nvram_init(int index);
void nvram_close(int index);

int nvram_set(int index, char *name, char *value);
int nvram_bufset(int index, char *name, char *value);
char *nvram_get(int index, char *name);
char *nvram_bufget(int index, char *name);

int nvram_commit(int index);
int nvram_clear(int index);

int getNvramNum(void);
unsigned int getNvramOffset(int index);
unsigned int getNvramBlockSize(int index);
char *getNvramName(int index);
unsigned int getNvramIndex(char *name);
void toggleNvramDebug(void);
int renew_nvram(int mode, char *fname);
int nvram_show(int mode);
#endif
