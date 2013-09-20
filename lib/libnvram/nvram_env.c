#include <linux/config.h>
#include "nvram.h"
#include "flash_api.h"
#include <errno.h>

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
	int i, len;
	char *p, *q;
#ifdef CONFIG_KERNEL_NVRAM
	int fd;
	nvram_ioctl_t nvr;

	/*
	 * read data from flash
	 * skip crc checking which is done by Kernel NVRAM module 
	 */
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
	unsigned long from;

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
	nvr.size  = MAX_VALUE_LEN;
	nvr.index = index;
	nvr.name  = name;

	fd = open(NV_DEV, O_RDONLY);
	if ( fd < 0 )
	{
		perror(NV_DEV);
		free(nvr.value);
		return "";
	}

	while (1)
	{
		nvr.value = malloc(nvr.size);

		if (nvr.value == NULL)
		{
			perror("malloc");
			return "";
		}

		// Perform request
		int result = ioctl(fd, RALINK_NVRAM_IOCTL_GET, &nvr);
		if (result >= 0)
			break;

		if (errno != EOVERFLOW) // Error is not caused by not-enough-space?
		{
			perror("ioctl");
			free(nvr.value);
			close(fd);
			return "";
		}

		// Calculate new buffer size
		free(nvr.value);
		nvr.size += ((MAX_VALUE_LEN - nvr.size % MAX_VALUE_LEN) % MAX_VALUE_LEN);
		if (nvr.size >= MAX_PERMITTED_VALUE_LEN)
		{
			perror("nvram_bufget overflow");
			free(nvr.value);
			close(fd);
			return "";
		}
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

#ifdef CONFIG_KERNEL_NVRAM
	FREE(nvr.value);
#endif

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
	nvr.name  = name;
	nvr.value = value;
	nvr.size  = strlen(value) + 1;

	fd = open(NV_DEV, O_RDONLY);
	if (fd < 0) {
		perror(NV_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_NVRAM_IOCTL_SET, &nvr) < 0)
	{
		if (errno == EOVERFLOW)
			perror("nvram_bufset overflow");
		else
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
