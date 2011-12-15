#include <linux/config.h>
#include "nvram.h"
#include "nvram_env.h"
#include "flash_api.h"

//#define DEBUG

#ifdef DEBUG
static char libnvram_debug = 1;
#else
static char libnvram_debug = 0;
#endif
#define LIBNV_PRINT(x, ...) do { if (libnvram_debug) printf("%s %d: " x, __FILE__, __LINE__, ## __VA_ARGS__); } while(0)
#define LIBNV_ERROR(x, ...) do { fprintf(stderr,"%s %d: ERROR! " x, __FILE__, __LINE__, ## __VA_ARGS__); } while(0)

static block_t fb[FLASH_BLOCK_NUM] =
{
#ifdef CONFIG_DUAL_IMAGE
	{
		.name = "uboot",
		.flash_offset =  0x0,
		.flash_max_len = ENV_UBOOT_SIZE,
		.valid = 0
	},
#endif
	{
		.name = "2860",
		.flash_offset =  0x2000,
		.flash_max_len = ENV_BLK_SIZE*4,
		.valid = 0
	},
	{
		.name = "rtdev",
		.flash_offset = 0x6000,
		.flash_max_len = ENV_BLK_SIZE*2,
		.valid = 0
	},
	{
		.name = "cert",
		.flash_offset = 0x8000,
		.flash_max_len = ENV_BLK_SIZE*2,
		.valid = 0
	},
	{
		.name = "wapi",
		.flash_offset = 0xa000,
		.flash_max_len = ENV_BLK_SIZE*5,
		.valid = 0
	}
};


//x is the value returned if the check failed
#define LIBNV_CHECK_INDEX(x) do { \
	if (index < 0 || index >= FLASH_BLOCK_NUM) { \
		LIBNV_PRINT("index(%d) is out of range\n", index); \
		return x; \
	} \
} while (0)

#define LIBNV_CHECK_VALID(x) do { \
	if (!fb[index].valid) { \
		LIBNV_PRINT("fb[%d] invalid, init again\n", index); \
		nvram_init(index); \
		if (!fb[index].valid) { \
		    LIBNV_PRINT("fb[%d] invalid, reinit not correct. return.\n", index); \
		    return x; \
		} \
	} \
} while (0)

#ifdef NVRAM_LIB_LIBNVRAM_SSTRDUP
static int bufitem = 0;
static char buf[NV_BUFFERS_COUNT][MAX_NV_VALUE_LEN];
#ifdef NVRAM_LIB_PTHREAD_FORCE
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
#endif

char* sstrdup(char* str)
{
	char* res;
	//lock till we'll have pointer to potentially free buffer
#ifdef NVRAM_LIB_PTHREAD_FORCE
	pthread_mutex_lock( &mutex1 );
#endif
	bufitem++;
	if (bufitem >= NV_BUFFERS_COUNT)
	    bufitem = 0;
	res = buf[bufitem];
#ifdef NVRAM_LIB_PTHREAD_FORCE
	pthread_mutex_unlock( &mutex1 );
#endif
	//work with that buffer
	strlcpy(res, str, MAX_NV_VALUE_LEN);
	return res;
}
#endif

/*
 * 1. read env from flash
 * 2. parse entries
 * 3. save the entries to cache
 */
int nvram_init(int index)
{
	unsigned long from;
	int i, len;
	char *p, *q;
#ifdef CONFIG_KERNEL_NVRAM
	int fd;
	nvram_ioctl_t nvr;

	/*
	 * read data from flash
	 * skip crc checking which is done by Kernel NVRAM module 
	 */
	from = fb[index].flash_offset + sizeof(fb[index].env.crc);
	len = fb[index].flash_max_len - sizeof(fb[index].env.crc);

	fb[index].env.data = (char *)malloc(len);
	if ( !fb[index].env.data )
	{
		perror("malloc");
		return -1;
	}

	nvr.index = index;
	nvr.value = fb[index].env.data;

	fd = open(NV_DEV, O_RDONLY);
	if ( fd < 0 )
	{
		perror(NV_DEV);
		free(fb[index].env.data);
		return -1;
	}
	if ( ioctl(fd, RALINK_NVRAM_IOCTL_GETALL, &nvr) < 0 )
	{
		perror("ioctl");
    		if(fd)
		    close(fd);
		free(fb[index].env.data);
		return -1;
	}
        if(fd)
	    close(fd);
#else
	//LIBNV_PRINT("--> nvram_init %d\n", index);
	LIBNV_CHECK_INDEX(0);

	//if reinit filed return...
	if (fb[index].valid)
		return 0;

	//read crc from flash
	from = fb[index].flash_offset;
	len = sizeof(fb[index].env.crc);
	flash_read((char *)&fb[index].env.crc, from, len);

	//read data from flash
	from = from + len;
	len = fb[index].flash_max_len - len;
	fb[index].env.data = (char *)malloc(len);
	if ( fb[index].env.data == NULL )
	{
		LIBNV_ERROR("nvram_init(%d): not enough memory!", index);
		return -1;
	}

	flash_read(fb[index].env.data, from, len);

	//check crc
	//printf("crc shall be %08lx\n", crc32(0, (unsigned char *)fb[index].env.data, len));
	if ( crc32(0, (unsigned char *)fb[index].env.data, len) != fb[index].env.crc )
	{
		LIBNV_PRINT("Bad CRC %x, ignore values in flash.\n", fb[index].env.crc);
		FREE(fb[index].env.data);
		//empty cache
		fb[index].valid = 1;
		fb[index].dirty = 0;
		return -1;
	}
#endif

	//parse env to cache
	p = fb[index].env.data;
	for ( i = 0; i < MAX_CACHE_ENTRY; i++ )
	{
		/* Store var name */
		if ( NULL == (q = strchr(p, '=')) )
		{
			LIBNV_PRINT("parsed failed - cannot find '='\n");
			break;
		}
		*q = '\0'; //strip '='
		fb[index].cache[i].name = strdup(p);
		//printf("  %d '%s'->", i, p);

		/* Store var value */
		p = q + 1; //value
		if ( NULL == (q = strchr(p, '\0')) )
		{
			LIBNV_PRINT("parsed failed - cannot find '\\0'\n");
			FREE(fb[index].cache[i].name);
			break;
		}
		fb[index].cache[i].value = strdup(p);
		//printf("'%s'\n", p);

		p = q + 1; //next entry
		if (p - fb[index].env.data + 1 >= len) //end of block
			break;
		if (*p == '\0') //end of env
			break;
	}
	if (i == MAX_CACHE_ENTRY)
		LIBNV_PRINT("run out of env cache, please increase MAX_CACHE_ENTRY\n");

	fb[index].valid = 1;
	fb[index].dirty = 0;

	return 0;
}

void nvram_close(int index)
{
	int i;

	//LIBNV_PRINT("--> nvram_close %d\n", index);
	LIBNV_CHECK_INDEX();

	if (!fb[index].valid)
		return;

	if (fb[index].dirty)
		nvram_commit(index);

	//free env
	FREE(fb[index].env.data);

	//free cache
	for (i = 0; i < MAX_CACHE_ENTRY; i++) {
		FREE(fb[index].cache[i].name);
		FREE(fb[index].cache[i].value);
	}

	fb[index].valid = 0;
}

/*
 * return idx (0 ~ iMAX_CACHE_ENTRY)
 * return -1 if no such value or empty cache
 */
static int cache_idx(int index, char *name)
{
	int i;

	for (i = 0; i < MAX_CACHE_ENTRY; i++) {
		if (!fb[index].cache[i].name)
			return -1;
		if (!strcmp(name, fb[index].cache[i].name))
			return i;
	}
	return -1;
}

char *nvram_get(int index, char *name)
{
	/* Initial value should be NULL */
	char *recv = NULL;

#ifndef CONFIG_KERNEL_NVRAM
	/* Get the fresh value from Kernel NVRAM moduel,
	 * so there is no need to do nvram_close() and nvram_init() again
	 */
	//LIBNV_PRINT("--> nvram_get\n");
	if ( nvram_init(index) == -1 )
	{
		return "";
	}
#endif
	recv = nvram_bufget(index, name);

	//btw, we don't return NULL anymore!
	if (!recv)
	    recv = "";

#ifndef CONFIG_KERNEL_NVRAM
	//Always need close nvram
	nvram_close(index);
#endif

    return recv;
}

int nvram_set(int index, char *name, char *value)
{
	int rc;

#ifndef CONFIG_KERNEL_NVRAM
	/* Get the fresh value from Kernel NVRAM moduel,
	 * so there is no need to do nvram_close() and nvram_init() again
	 */
	if ( nvram_init(index) == -1 )
		return -1;
#endif
	if (nvram_bufset(index, name, value) == -1 ) 
		rc = -1;
	else
	rc = nvram_commit(index);
#ifndef CONFIG_KERNEL_NVRAM
	//Always need close nvram
	nvram_close(index);
#endif
	return rc;
}

char *nvram_bufget(int index, char *name)
{
	int idx;
	/* Initial value should be NULL */
	static char *ret = NULL;
#ifdef CONFIG_KERNEL_NVRAM
	int fd;
	nvram_ioctl_t nvr;
#endif
	//LIBNV_PRINT("--> nvram_bufget %d\n", index);
	LIBNV_CHECK_INDEX("");
	LIBNV_CHECK_VALID("");

#ifdef CONFIG_KERNEL_NVRAM
	nvr.index = index;
	nvr.name = name;
	nvr.value = malloc(MAX_VALUE_LEN);
	if ( !nvr.value )
	{
		perror("malloc");
		return "";
	}

	fd = open(NV_DEV, O_RDONLY);
	if ( fd < 0 )
	{
		perror(NV_DEV);
		free(nvr.value);
		return "";
	}
	if ( ioctl(fd, RALINK_NVRAM_IOCTL_GET, &nvr) < 0 )
	{
		perror("ioctl");
		free(nvr.value);
		close(fd);
		return "";
	}
	close(fd);
#endif

	idx = cache_idx(index, name);

	if (-1 != idx) {
		if (fb[index].cache[idx].value) {
			//duplicate the value in case caller modify it
#ifdef CONFIG_KERNEL_NVRAM
			FREE(fb[index].cache[idx].value);
			fb[index].cache[idx].value = strdup(nvr.value);
			FREE(nvr.value);
#endif
			//Tom.Hung 2010-5-7, strdup() will cause memory leakage
			//but if we return value directly, it will cause many other crash or delete value to nvram error.
#ifdef NVRAM_LIB_LIBNVRAM_SSTRDUP
			ret = sstrdup(fb[index].cache[idx].value);
#else
			ret = strdup(fb[index].cache[idx].value);
#endif
			LIBNV_PRINT("bufget %d '%s'->'%s'\n", index, name, ret);

			//btw, we don't return NULL anymore!
			if (!ret)
			    ret = "";

		    return ret;
		}
	}

	//no default value set?
	//btw, we don't return NULL anymore!
	LIBNV_PRINT("bufget %d '%s'->''(empty) Warning!\n", index, name);

	return "";
}

int nvram_bufset(int index, char *name, char *value)
{
	int idx;
#ifdef CONFIG_KERNEL_NVRAM
	int fd;
	nvram_ioctl_t nvr;
#endif
	//LIBNV_PRINT("--> nvram_bufset\n");
	LIBNV_CHECK_INDEX(-1);
	LIBNV_CHECK_VALID(-1);

#ifdef CONFIG_KERNEL_NVRAM
	nvr.index = index;
	nvr.name = name;
	nvr.value = value;

	fd = open(NV_DEV, O_RDONLY);
	if (fd < 0) {
		perror(NV_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_NVRAM_IOCTL_SET, &nvr) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
#endif
	idx = cache_idx(index, name);

	if (-1 == idx) {
		//find the first empty room
		for (idx = 0; idx < MAX_CACHE_ENTRY; idx++) {
			if (!fb[index].cache[idx].name)
				break;
		}
		//no any empty room
		if (idx == MAX_CACHE_ENTRY) {
			LIBNV_ERROR("run out of env cache, please increase MAX_CACHE_ENTRY\n");
			return -1;
		}
		fb[index].cache[idx].name = strdup(name);
		fb[index].cache[idx].value = strdup(value);
	}
	else {
		//abandon the previous value
		FREE(fb[index].cache[idx].value);
		fb[index].cache[idx].value = strdup(value);
	}
	LIBNV_PRINT("bufset %d '%s'->'%s'\n", index, name, value);
	fb[index].dirty = 1;
	return 0;
}

void nvram_buflist(int index)
{
	int i;

	//LIBNV_PRINT("--> nvram_buflist %d\n", index);
	LIBNV_CHECK_INDEX();
	LIBNV_CHECK_VALID();

	for (i = 0; i < MAX_CACHE_ENTRY; i++) {
		if (!fb[index].cache[i].name)
			break;
		printf("  '%s'='%s'\n", fb[index].cache[i].name, fb[index].cache[i].value);
	}
}

/*
 * write flash from cache
 */
int nvram_commit(int index)
{

#ifdef CONFIG_KERNEL_NVRAM
	int fd;
	nvram_ioctl_t nvr;

	LIBNV_PRINT("--> nvram_commit %d\n", index);

	nvr.index = index;
	fd = open(NV_DEV, O_RDONLY);
	if (fd < 0) {
		perror(NV_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_NVRAM_IOCTL_COMMIT, &nvr) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
#else
	unsigned long to;
	int i, len;
	char *p;

	//LIBNV_PRINT("--> nvram_commit %d\n", index);
	LIBNV_CHECK_INDEX(-1);
	LIBNV_CHECK_VALID(-1);

	if (!fb[index].dirty) {
		LIBNV_PRINT("nothing to be committed\n");
		return 0;
	}

	//construct env block
	len = fb[index].flash_max_len - sizeof(fb[index].env.crc);
	fb[index].env.data = (char *)malloc(len);
	if (fb[index].env.data == NULL) {
		LIBNV_ERROR("nvram_commit(%d): not enough memory!", index);
		return 0;
	}
	bzero(fb[index].env.data, len);
	p = fb[index].env.data;
	for (i = 0; i < MAX_CACHE_ENTRY; i++) {
		int l;
		if (!fb[index].cache[i].name || !fb[index].cache[i].value)
			break;
		l = strlen(fb[index].cache[i].name) + strlen(fb[index].cache[i].value) + 2;
		if (p - fb[index].env.data + 2 >= fb[index].flash_max_len) {
			LIBNV_ERROR("ENV_BLK_SIZE 0x%x is not enough!", ENV_BLK_SIZE);
			FREE(fb[index].env.data);
			return -1;
		}
		snprintf(p, l, "%s=%s", fb[index].cache[i].name, fb[index].cache[i].value);
		p += l;
	}
	*p = '\0'; //ending null

	//calculate crc
	fb[index].env.crc = (unsigned long)crc32(0, (unsigned char *)fb[index].env.data, len);
	LIBNV_PRINT("Commit crc = %x\n", (unsigned int)fb[index].env.crc);

	//write crc to flash
	to = fb[index].flash_offset;
	len = sizeof(fb[index].env.crc);
	flash_write((char *)&fb[index].env.crc, to, len);

	//write data to flash
	to = to + len;
	len = fb[index].flash_max_len - len;
	flash_write(fb[index].env.data, to, len);
	FREE(fb[index].env.data);
#endif
	fb[index].dirty = 0;
	return 0;
}

/*
 * clear flash by writing all 1's value
 */
int nvram_clear(int index)
{
#ifdef CONFIG_KERNEL_NVRAM
	int fd;
	nvram_ioctl_t nvr;
	nvr.index = index;
	fd = open(NV_DEV, O_RDONLY);
	if (fd < 0) {
		perror(NV_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_NVRAM_IOCTL_CLEAR, &nvr) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
#else
	unsigned long to;
	int len;

	///LIBNV_PRINT("--> nvram_clear %d\n", index);
	LIBNV_CHECK_INDEX(-1);
	nvram_close(index);

	//construct all 1s env block
	len = fb[index].flash_max_len - sizeof(fb[index].env.crc);
	fb[index].env.data = (char *)malloc(len);
	if (fb[index].env.data == NULL) {
		LIBNV_ERROR("nvram_clear(%d): not enough memory!", index);
		return 0;
	}
	memset(fb[index].env.data, 0xFF, len);

	//calculate and write crc
	fb[index].env.crc = (unsigned long)crc32(0, (unsigned char *)fb[index].env.data, len);
	to = fb[index].flash_offset;
	len = sizeof(fb[index].env.crc);
	flash_write((char *)&fb[index].env.crc, to, len);

	//write all 1s data to flash
	to = to + len;
	len = fb[index].flash_max_len - len;
	flash_write(fb[index].env.data, to, len);
	FREE(fb[index].env.data);
	LIBNV_PRINT("clear flash from 0x%x for 0x%x bytes\n", (unsigned int *)to, len);
#endif
	fb[index].dirty = 0;
	return 0;
}

#if 0
//WARNING: this fuunction is dangerous because it erases all other data in the same sector
int nvram_erase(int index)
{
	int s, e;

	///LIBNV_PRINT("--> nvram_erase %d\n", index);
	LIBNV_CHECK_INDEX(-1);
	nvram_close(index);

	s = fb[index].flash_offset;
	e = fb[index].flash_offset + fb[index].flash_max_len - 1;
	LIBNV_PRINT("erase flash from 0x%x to 0x%x\n", s, e);
	FlashErase(s, e);
	return 0;
}
#endif

int getNvramNum(void)
{
	return FLASH_BLOCK_NUM;
}

unsigned int getNvramOffset(int index)
{
	LIBNV_CHECK_INDEX(0);
	return fb[index].flash_offset;
}

char *getNvramName(int index)
{
	LIBNV_CHECK_INDEX(NULL);
	return fb[index].name;
}

unsigned int getNvramBlockSize(int index)
{
	LIBNV_CHECK_INDEX(0);
	return fb[index].flash_max_len;
}

unsigned int getNvramIndex(char *name)
{
	int i;
	for (i = 0; i < FLASH_BLOCK_NUM; i++) {
		if (!strcmp(fb[i].name, name)) {
			return i;
		}
	}
	return -1;
}

void toggleNvramDebug()
{
#ifndef DEBUG
	if (libnvram_debug) {
		libnvram_debug = 0;
		printf("%s: turn off debugging\n", __FILE__);
	}
	else {
		libnvram_debug = 1;
		printf("%s: turn ON debugging\n", __FILE__);
	}
#else
		libnvram_debug = 1;
#endif
}

int renew_nvram(int mode, char *fname)
{
	FILE *fp;
	char buf[BUFSZ], *p;
	int found = 0, need_commit = 0;

	fp = fopen(fname, "ro");
	if ( !fp )
	{
		perror("fopen");
		return -1;
	}

	//find "Default" first
	while ( fgets(buf, BUFSZ, fp) )
	{
		if (buf[0] == '\n' || buf[0] == '#')
			continue;
		if ( !strncmp(buf, "Default\n", 8) )
		{
			found = 1;
			break;
		}
	}
	if ( !found )
	{
		printf("file format error!\n");
		fclose(fp);
		return -1;
	}

	if ( nvram_init(mode) == -1 )
	{
		fclose(fp);
		return -1;
	}

	while ( fgets(buf, BUFSZ, fp) )
	{
		if (buf[0] == '\n' || buf[0] == '#')
			continue;
		if ( !(p = strchr(buf, '=')) )
		{
			if ( need_commit )
			{
				nvram_commit(mode);
				need_commit = 0;
			}
			printf("%s file format error!\n", fname);
			goto out;
		}
		buf[strlen(buf) - 1] = '\0'; //remove carriage return
		*p++ = '\0'; //seperate the string
		//printf("bufset %d '%s'='%s'\n", mode, buf, p);
		nvram_bufset(mode, buf, p);
		need_commit = 1;
	}

	if (need_commit)
	    nvram_commit(mode);
out:
	fclose(fp);
	nvram_close(mode);
	return 0;
}

int nvram_show(int mode)
{
	char *buffer, *p;
	int crc;
	unsigned int len = 0x4000;

	if ( nvram_init(mode) == -1 )
	{
		return -1;
	}
	len = getNvramBlockSize(mode);
	buffer = malloc(len);
	if ( !buffer )
	{
		fprintf(stderr, "nvram_show: Can not allocate memory!\n");
		return -1;
	}

	flash_read(buffer, getNvramOffset(mode), len);
	memcpy(&crc, buffer, 4);

	fprintf(stderr, "crc = %x\n", crc);
	for ( p = buffer + 4; *p; p += strlen(p) + 1 )
	{
		printf("%s\n", p);
	}

	FREE(buffer);
	nvram_close(mode);
	return 0;
}

int isMacValid(char *str)
{
	int i, len = strlen(str);
	if(len != 17)
		return 0;

	for ( i = 0; i < 5; i++ )
	{
		if( (!isxdigit( str[i*3])) || (!isxdigit( str[i*3+1])) || (str[i*3+2] != ':') )
			return 0;
	}
	return (isxdigit(str[15]) && isxdigit(str[16])) ? 1: 0;
}

int nvram_load_default(void)
{
	//default macs is OK
	int mac_ok=1;

	LIBNV_PRINT("Store macs...");
	char *WLAN_MAC_ADDR	= nvram_get(RT2860_NVRAM, "WLAN_MAC_ADDR");
        char *WAN_MAC_ADDR	= nvram_get(RT2860_NVRAM, "WAN_MAC_ADDR");
        char *LAN_MAC_ADDR	= nvram_get(RT2860_NVRAM, "LAN_MAC_ADDR");
        char *CHECKMAC		= nvram_get(RT2860_NVRAM, "CHECKMAC");

	LIBNV_PRINT("Clear nvram...");
	nvram_clear(RT2860_NVRAM);
	LIBNV_PRINT("Load defaults nvram...");
	renew_nvram(RT2860_NVRAM, "/etc/default/nvram_default");

	LIBNV_PRINT("Renew nvram...");
	//reinit nvram before commit
	if ( nvram_init(RT2860_NVRAM) == -1 )
		return -1;

	//set default chip type
#if defined(CONFIG_RALINK_RT3050_1T1R)
        nvram_bufset(RT2860_NVRAM, "RFICType", "5");
#elif defined(CONFIG_RALINK_RT3051_1T2R)
        nvram_bufset(RT2860_NVRAM, "RFICType", "7");
#elif defined(CONFIG_RALINK_RT3052_2T2R)
        nvram_bufset(RT2860_NVRAM, "RFICType", "8");
#else
        nvram_bufset(RT2860_NVRAM, "RFICType", "5");
#endif
	LIBNV_PRINT("Restore old macs...");
	if ((strlen(WLAN_MAC_ADDR) > 0) && isMacValid(WLAN_MAC_ADDR))
	    nvram_bufset(RT2860_NVRAM, "WLAN_MAC_ADDR", WLAN_MAC_ADDR);
	else
	    mac_ok=0;
	if ((strlen(WAN_MAC_ADDR) > 0) && isMacValid(WAN_MAC_ADDR))
    	    nvram_bufset(RT2860_NVRAM, "WAN_MAC_ADDR",  WAN_MAC_ADDR);
	else
	    mac_ok=0;
	if ((strlen(LAN_MAC_ADDR) > 0) && isMacValid(LAN_MAC_ADDR))
    	    nvram_bufset(RT2860_NVRAM, "LAN_MAC_ADDR",  LAN_MAC_ADDR);
	else
	    mac_ok=0;

	//all restore ok ?
	if ( mac_ok == 1 )
	{
	    LIBNV_PRINT("Restore checkmac atribute.");
    	    nvram_bufset(RT2860_NVRAM, "CHECKMAC", CHECKMAC);
	}
	else
	{
	    LIBNV_PRINT("Set checkmac atribute.");
    	    nvram_bufset(RT2860_NVRAM, "CHECKMAC", "YES");
	}

	//set wive flag
        nvram_bufset(RT2860_NVRAM, "IS_WIVE", "YES");
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

    return 0;
}

int gen_wifi_config(int mode)
{
	FILE *fp;
	int  i, ssid_num = 1;
	char tx_rate[16], wmm_enable[16];
	char temp[2], buf[4];

	if ( mode == RT2860_NVRAM )
	{
	    //Read radio config
	    flash_read_NicConf(buf);

		if ( nvram_init(mode) == -1 )
			return -1;

	    //chiptype
	    sprintf(temp, "%x", buf[1]);
	    if (!atoi(temp))
		sprintf(temp, "%x", "5");
	    nvram_bufset(mode, "RFICType", temp);

	    //TxStream for select HT mode
	    sprintf(temp, "%x", buf[0]&0xf0>>4);
	    if (atoi(temp) < atoi(nvram_bufget(mode, "HT_TxStream")))
		nvram_bufset(mode, "HT_TxStream", temp);

	    //RxStream for select HT mode
	    nvram_bufset(mode, "TXPath", temp);
	    sprintf(temp, "%x", buf[0]&0x0f);
	    if (atoi(temp) < atoi(nvram_bufget(mode, "HT_RxStream")))
		nvram_bufset(mode, "HT_RxStream", temp);
	    nvram_bufset(mode, "RXPath", temp);
	    nvram_commit(mode);

	    //reinit nvram
	    nvram_close(mode);
	}
	if ( nvram_init(mode) == -1 )
		return -1;

	system("mkdir -p /etc/Wireless/RT2860");
	if ( mode == RT2860_NVRAM )
	{
		fp = fopen("/etc/Wireless/RT2860/RT2860.dat", "w+");
	}
	else
		return 0;

	fprintf(fp, "#The word of \"Default\" must not be removed\n");
	fprintf(fp, "Default\n");

#define FPRINT_NUM(x) fprintf(fp, #x"=%d\n", atoi(nvram_bufget(mode, #x)));
#define FPRINT_STR(x) fprintf(fp, #x"=%s\n", nvram_bufget(mode, #x));

	if (RT2860_NVRAM == mode) {
		FPRINT_STR(WLAN_MAC_ADDR);
		FPRINT_NUM(CountryRegion);
		FPRINT_NUM(CountryRegionABand);
		FPRINT_STR(CountryCode);
#ifdef CONFIG_RT2860V2_EXT_CHANNEL_LIST
		FPRINT_NUM(ChannelGeography);
#endif
		FPRINT_NUM(Channel);
		FPRINT_NUM(AutoChannelSelect);

#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
		FPRINT_NUM(BssidNum);
		ssid_num = atoi(nvram_bufget(mode, "BssidNum"));

		FPRINT_STR(SSID1);
		FPRINT_STR(SSID2);
		FPRINT_STR(SSID3);
		FPRINT_STR(SSID4);
		FPRINT_STR(SSID5);
		FPRINT_STR(SSID6);
		FPRINT_STR(SSID7);
		FPRINT_STR(SSID8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(SSID9);
		FPRINT_STR(SSID10);
		FPRINT_STR(SSID11);
		FPRINT_STR(SSID12);
		FPRINT_STR(SSID13);
		FPRINT_STR(SSID14);
		FPRINT_STR(SSID15);
		FPRINT_STR(SSID16);
#endif
#endif

		FPRINT_NUM(WirelessMode);
		FPRINT_NUM(AutoConnect);
		FPRINT_NUM(FastConnect);
		FPRINT_NUM(HiPower);
		FPRINT_NUM(AutoRoaming);
		FPRINT_NUM(FixedTxMode);

		//TxRate(FixedRate)
		bzero(tx_rate, sizeof(char)*16);
		for (i = 0; i < ssid_num; i++)
		{
			sprintf(tx_rate+strlen(tx_rate), "%d",
					atoi(nvram_bufget(mode, "TxRate")));
			sprintf(tx_rate+strlen(tx_rate), "%c", ';');
		}
		tx_rate[strlen(tx_rate) - 1] = '\0';
		fprintf(fp, "TxRate=%s\n", tx_rate);

		FPRINT_NUM(BasicRate);
		FPRINT_NUM(BeaconPeriod);
		FPRINT_NUM(DtimPeriod);
		FPRINT_NUM(TxPower);
		FPRINT_NUM(DisableOLBC);
		FPRINT_NUM(BGProtection);
		FPRINT_NUM(TxPreamble);
		FPRINT_NUM(RTSThreshold);
		FPRINT_NUM(FragThreshold);
		FPRINT_NUM(TxBurst);
		FPRINT_NUM(PktAggregate);
		FPRINT_NUM(FreqDelta);

#if defined (CONFIG_RT2860V2_AP_VIDEO_TURBINE) || defined (CONFIG_RT2860V2_STA_VIDEO_TURBINE)
		FPRINT_NUM(VideoTurbine);
#endif

		//WmmCapable
		bzero(wmm_enable, sizeof(char)*16);
		for (i = 0; i < ssid_num; i++)
		{
			sprintf(wmm_enable+strlen(wmm_enable), "%d",
					atoi(nvram_bufget(mode, "WmmCapable")));
			sprintf(wmm_enable+strlen(wmm_enable), "%c", ';');
		}
		wmm_enable[strlen(wmm_enable) - 1] = '\0';
		fprintf(fp, "WmmCapable=%s\n", wmm_enable);

		FPRINT_NUM(McastPhyMode);
		FPRINT_NUM(McastMcs);

		FPRINT_STR(APAifsn);
		FPRINT_STR(APCwmin);
		FPRINT_STR(APCwmax);
		FPRINT_STR(APTxop);
		FPRINT_STR(APACM);
		FPRINT_STR(BSSAifsn);
		FPRINT_STR(BSSCwmin);
		FPRINT_STR(BSSCwmax);
		FPRINT_STR(BSSTxop);
		FPRINT_STR(BSSACM);
		FPRINT_STR(AckPolicy);
		FPRINT_STR(APSDCapable);
		FPRINT_STR(DLSCapable);
		FPRINT_STR(NoForwarding);
		FPRINT_NUM(NoForwardingBTNBSSID);
		FPRINT_STR(HideSSID);
		FPRINT_NUM(ShortSlot);

		FPRINT_STR(IEEE8021X);
		FPRINT_NUM(IEEE80211H);
		FPRINT_NUM(DebugFlags);
		FPRINT_NUM(CSPeriod);
#if defined (CONFIG_RT2860V2_AP_CARRIER) || defined (CONFIG_RT2860V2_STA_CARRIER)
		FPRINT_NUM(CarrierDetect);
#endif
		FPRINT_STR(RDRegion);
		FPRINT_NUM(StationKeepAlive);

#if defined (CONFIG_RT2860V2_AP_DFS) || defined (CONFIG_RT2860V2_STA_DFS)
		FPRINT_NUM(DfsLowerLimit);
		FPRINT_NUM(DfsUpperLimit);
		FPRINT_NUM(DfsIndoor);
		FPRINT_NUM(DFSParamFromConfig);
		FPRINT_STR(FCCParamCh0);
		FPRINT_STR(FCCParamCh1);
		FPRINT_STR(FCCParamCh2);
		FPRINT_STR(FCCParamCh3);
		FPRINT_STR(CEParamCh0);
		FPRINT_STR(CEParamCh1);
		FPRINT_STR(CEParamCh2);
		FPRINT_STR(CEParamCh3);
		FPRINT_STR(JAPParamCh0);
		FPRINT_STR(JAPParamCh1);
		FPRINT_STR(JAPParamCh2);
		FPRINT_STR(JAPParamCh3);
		FPRINT_STR(JAPW53ParamCh0);
		FPRINT_STR(JAPW53ParamCh1);
		FPRINT_STR(JAPW53ParamCh2);
		FPRINT_STR(JAPW53ParamCh3);
		FPRINT_NUM(FixDfsLimit);
		FPRINT_NUM(LongPulseRadarTh);
		FPRINT_NUM(AvgRssiReq);
		FPRINT_NUM(DFS_R66);
		FPRINT_STR(blockch);
#endif

		FPRINT_STR(PreAuth);
		FPRINT_STR(AuthMode);
		FPRINT_STR(EncrypType);

    		FPRINT_STR(RekeyMethod);
		FPRINT_NUM(RekeyInterval);
		FPRINT_STR(PMKCachePeriod);

#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
#if defined(CONFIG_RT2860V2_STA_WAPI) || defined(CONFIG_RT2860V2_AP_WAPI)
		/*kurtis: WAPI*/
		FPRINT_STR(WapiPsk1);
		FPRINT_STR(WapiPsk2);
		FPRINT_STR(WapiPsk3);
		FPRINT_STR(WapiPsk4);
		FPRINT_STR(WapiPsk5);
		FPRINT_STR(WapiPsk6);
		FPRINT_STR(WapiPsk7);
		FPRINT_STR(WapiPsk8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(WapiPsk9);
		FPRINT_STR(WapiPsk10);
		FPRINT_STR(WapiPsk11);
		FPRINT_STR(WapiPsk12);
		FPRINT_STR(WapiPsk13);
		FPRINT_STR(WapiPsk14);
		FPRINT_STR(WapiPsk15);
		FPRINT_STR(WapiPsk16);
#endif
#endif
		FPRINT_STR(WapiPskType);
		FPRINT_STR(Wapiifname);
		FPRINT_STR(WapiAsCertPath);
		FPRINT_STR(WapiUserCertPath);
		FPRINT_STR(WapiAsIpAddr);
		FPRINT_STR(WapiAsPort);
#endif
#if defined(CONFIG_RT2860V2_STA_MESH) || defined(CONFIG_RT2860V2_AP_MESH)
		FPRINT_NUM(MeshAutoLink);
		FPRINT_STR(MeshAuthMode);
		FPRINT_STR(MeshEncrypType);
		FPRINT_NUM(MeshDefaultkey);
		FPRINT_STR(MeshWEPKEY);
		FPRINT_STR(MeshWPAKEY);
		FPRINT_STR(MeshId);
#endif
#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
		//WPAPSK
		FPRINT_STR(WPAPSK1);
		FPRINT_STR(WPAPSK2);
		FPRINT_STR(WPAPSK3);
		FPRINT_STR(WPAPSK4);
		FPRINT_STR(WPAPSK5);
		FPRINT_STR(WPAPSK6);
		FPRINT_STR(WPAPSK7);
		FPRINT_STR(WPAPSK8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(WPAPSK9);
		FPRINT_STR(WPAPSK10);
		FPRINT_STR(WPAPSK11);
		FPRINT_STR(WPAPSK12);
		FPRINT_STR(WPAPSK13);
		FPRINT_STR(WPAPSK14);
		FPRINT_STR(WPAPSK15);
		FPRINT_STR(WPAPSK16);
#endif
#endif
		FPRINT_STR(DefaultKeyID);
#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
		FPRINT_STR(Key1Type);
		FPRINT_STR(Key1Str1);
		FPRINT_STR(Key1Str2);
		FPRINT_STR(Key1Str3);
		FPRINT_STR(Key1Str4);
		FPRINT_STR(Key1Str5);
		FPRINT_STR(Key1Str6);
		FPRINT_STR(Key1Str7);
		FPRINT_STR(Key1Str8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(Key1Str9);
		FPRINT_STR(Key1Str10);
		FPRINT_STR(Key1Str11);
		FPRINT_STR(Key1Str12);
		FPRINT_STR(Key1Str13);
		FPRINT_STR(Key1Str14);
		FPRINT_STR(Key1Str15);
		FPRINT_STR(Key1Str16);
#endif

		FPRINT_STR(Key2Type);
		FPRINT_STR(Key2Str1);
		FPRINT_STR(Key2Str2);
		FPRINT_STR(Key2Str3);
		FPRINT_STR(Key2Str4);
		FPRINT_STR(Key2Str5);
		FPRINT_STR(Key2Str6);
		FPRINT_STR(Key2Str7);
		FPRINT_STR(Key2Str8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(Key2Str9);
		FPRINT_STR(Key2Str10);
		FPRINT_STR(Key2Str11);
		FPRINT_STR(Key2Str12);
		FPRINT_STR(Key2Str13);
		FPRINT_STR(Key2Str14);
		FPRINT_STR(Key2Str15);
		FPRINT_STR(Key2Str16);
#endif
		FPRINT_STR(Key3Type);
		FPRINT_STR(Key3Str1);
		FPRINT_STR(Key3Str2);
		FPRINT_STR(Key3Str3);
		FPRINT_STR(Key3Str4);
		FPRINT_STR(Key3Str5);
		FPRINT_STR(Key3Str6);
		FPRINT_STR(Key3Str7);
		FPRINT_STR(Key3Str8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(Key3Str9);
		FPRINT_STR(Key3Str10);
		FPRINT_STR(Key3Str11);
		FPRINT_STR(Key3Str12);
		FPRINT_STR(Key3Str13);
		FPRINT_STR(Key3Str14);
		FPRINT_STR(Key3Str15);
		FPRINT_STR(Key3Str16);
#endif
		FPRINT_STR(Key4Type);
		FPRINT_STR(Key4Str1);
		FPRINT_STR(Key4Str2);
		FPRINT_STR(Key4Str3);
		FPRINT_STR(Key4Str4);
		FPRINT_STR(Key4Str5);
		FPRINT_STR(Key4Str6);
		FPRINT_STR(Key4Str7);
		FPRINT_STR(Key4Str8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(Key4Str9);
		FPRINT_STR(Key4Str10);
		FPRINT_STR(Key4Str11);
		FPRINT_STR(Key4Str12);
		FPRINT_STR(Key4Str13);
		FPRINT_STR(Key4Str14);
		FPRINT_STR(Key4Str15);
		FPRINT_STR(Key4Str16);
#endif
#endif
		//MIMO
		FPRINT_NUM(HT_HTC);
		FPRINT_NUM(HT_RDG);
		FPRINT_NUM(HT_OpMode);
		FPRINT_NUM(HT_MpduDensity);
		FPRINT_NUM(HT_EXTCHA);
		FPRINT_NUM(HT_BW);
		FPRINT_NUM(HT_AutoBA);
		FPRINT_NUM(HT_BADecline);
		FPRINT_NUM(HT_AMSDU);
		FPRINT_NUM(HT_BAWinSize);
		FPRINT_NUM(HT_GI);
		FPRINT_NUM(HT_STBC);
		FPRINT_STR(HT_MCS);
		FPRINT_NUM(HT_TxStream);
		FPRINT_NUM(HT_RxStream);
		FPRINT_NUM(HT_PROTECT);
		FPRINT_NUM(HT_DisallowTKIP);
		FPRINT_NUM(HT_40MHZ_INTOLERANT);
		FPRINT_NUM(HT_MIMOPSMode);
		FPRINT_NUM(HT_BSSCoexistence);
		FPRINT_NUM(HT_BSSCoexApCntThr);
#ifdef CONFIG_RT2860V2_AP_GREENAP
		FPRINT_NUM(GreenAP);
#endif

		FPRINT_NUM(WscConfMode);

		//WscConfStatus
		if (atoi(nvram_bufget(mode, "WscConfigured")) == 0)
			fprintf(fp, "WscConfStatus=%d\n", 1);
		else
			fprintf(fp, "WscConfStatus=%d\n", 2);
		if (strcmp(nvram_bufget(mode, "WscVendorPinCode"), "") != 0)
			FPRINT_STR(WscVendorPinCode);

		FPRINT_NUM(WCNTest);

#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
		FPRINT_NUM(AccessPolicy0);
		FPRINT_STR(AccessControlList0);
		FPRINT_NUM(AccessPolicy1);
		FPRINT_STR(AccessControlList1);
		FPRINT_NUM(AccessPolicy2);
		FPRINT_STR(AccessControlList2);
		FPRINT_NUM(AccessPolicy3);
		FPRINT_STR(AccessControlList3);
		FPRINT_NUM(AccessPolicy4);
		FPRINT_STR(AccessControlList4);
		FPRINT_NUM(AccessPolicy5);
		FPRINT_STR(AccessControlList5);
		FPRINT_NUM(AccessPolicy6);
		FPRINT_STR(AccessControlList6);
		FPRINT_NUM(AccessPolicy7);
		FPRINT_STR(AccessControlList7);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_NUM(AccessPolicy8);
		FPRINT_STR(AccessControlList8);
		FPRINT_NUM(AccessPolicy9);
		FPRINT_STR(AccessControlList9);
		FPRINT_NUM(AccessPolicy10);
		FPRINT_STR(AccessControlList10);
		FPRINT_NUM(AccessPolicy11);
		FPRINT_STR(AccessControlList11);
		FPRINT_NUM(AccessPolicy12);
		FPRINT_STR(AccessControlList12);
		FPRINT_NUM(AccessPolicy13);
		FPRINT_STR(AccessControlList13);
		FPRINT_NUM(AccessPolicy14);
		FPRINT_STR(AccessControlList14);
		FPRINT_NUM(AccessPolicy15);
		FPRINT_STR(AccessControlList15);
#endif
#endif

		FPRINT_NUM(WdsEnable);
		FPRINT_STR(WdsPhyMode);
		FPRINT_STR(WdsTxMcs);
		FPRINT_STR(WdsEncrypType);
		FPRINT_STR(WdsList);
		FPRINT_STR(Wds0Key);
		FPRINT_STR(Wds1Key);
		FPRINT_STR(Wds2Key);
		FPRINT_STR(Wds3Key);
		FPRINT_STR(RADIUS_Server);
		FPRINT_STR(RADIUS_Port);
#if defined (CONFIG_RT2860V2_AP_MBSS) || defined (CONFIG_RT2860V2_STA_MBSS)
		FPRINT_STR(RADIUS_Key1);
		FPRINT_STR(RADIUS_Key2);
		FPRINT_STR(RADIUS_Key3);
		FPRINT_STR(RADIUS_Key4);
		FPRINT_STR(RADIUS_Key5);
		FPRINT_STR(RADIUS_Key6);
		FPRINT_STR(RADIUS_Key7);
		FPRINT_STR(RADIUS_Key8);
#if (defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT3883) || defined (CONFIG_RALINK_RT6855)) && defined (CONFIG_16MBSSID_MODE)
		FPRINT_STR(RADIUS_Key9);
		FPRINT_STR(RADIUS_Key10);
		FPRINT_STR(RADIUS_Key11);
		FPRINT_STR(RADIUS_Key12);
		FPRINT_STR(RADIUS_Key13);
		FPRINT_STR(RADIUS_Key14);
		FPRINT_STR(RADIUS_Key15);
		FPRINT_STR(RADIUS_Key16);
#endif
#endif

		FPRINT_STR(own_ip_addr);
		FPRINT_STR(EAPifname);
		FPRINT_STR(PreAuthifname);
		FPRINT_STR(PSMode);
		FPRINT_NUM(session_timeout_interval);
		FPRINT_NUM(quiet_interval);
		FPRINT_NUM(TGnWifiTest);

#ifdef CONFIG_RT2860V2_AP_APCLI
		//AP Client parameters
		FPRINT_NUM(ApCliEnable);
		FPRINT_STR(ApCliSsid);
		FPRINT_STR(ApCliBssid);
		FPRINT_STR(ApCliAuthMode);
		FPRINT_STR(ApCliEncrypType);
		FPRINT_STR(ApCliDefaultKeyID);
		FPRINT_STR(ApCliWPAPSK);
		FPRINT_NUM(ApCliKey1Type);
		FPRINT_STR(ApCliKey1Str);
		FPRINT_NUM(ApCliKey2Type);
		FPRINT_STR(ApCliKey2Str);
		FPRINT_NUM(ApCliKey3Type);
		FPRINT_STR(ApCliKey3Str);
		FPRINT_NUM(ApCliKey4Type);
		FPRINT_STR(ApCliKey4Str);
		FPRINT_NUM(ApCliTxMode);
		FPRINT_NUM(ApCliTxMcs);
		FPRINT_NUM(ApCliAutoConnect);
#endif

		//Radio On/Off
		if (atoi(nvram_bufget(mode, "RadioOff")) == 1)
			fprintf(fp, "RadioOn=0\n");
		else
			fprintf(fp, "RadioOn=1\n");

		/*
		 * There are no SSID/WPAPSK/Key1Str/Key2Str/Key3Str/Key4Str anymore since driver1.5 , but 
		 * STA WPS still need these entries to show the WPS result(That is the only way i know to get WPAPSK key) and
		 * so we create empty entries here.   --YY
		 */
		fprintf(fp, "SSID=\nWPAPSK=\nKey1Str=\nKey2Str=\nKey3Str=\nKey4Str=\n");
	}

    nvram_close(mode);
    fclose(fp);
    return 0;
}
