#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/config.h>

#include "nvram.h"

#define DEFAULT_FLASH_ZONE_NAME "2860"

int set_usage(char *aout)
{
#ifndef CONFIG_RT2860V2_USER_MEMORY_OPTIMIZATION
	int i;

	printf("Usage example: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr 1.2.3.4\n");
	}
#endif
	return -1;
}

int get_usage(char *aout)
{
#ifndef CONFIG_RT2860V2_USER_MEMORY_OPTIMIZATION
	int i;

	printf("Usage: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr\n");
	}
#endif
	return -1;
}

int buf_get_usage(char *aout)
{
#ifndef CONFIG_RT2860V2_USER_MEMORY_OPTIMIZATION
	int i;

	printf("Usage: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr wan_ipaddr ...\n");
	}
#endif
	return -1;
}

int ra_nv_set(int argc,char **argv)
{
	int index, rc;
	char *fz = "", *key = "", *value = "";

	if (argc == 1 || argc > 5)
		return set_usage(argv[0]);

	if (argc == 2) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
		value = "";
	} else if (argc == 3) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
		value = argv[2];
	} else if (argc == 4) {
		fz = argv[1];
		key = argv[2];
		value = argv[3];
	}

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return set_usage(argv[0]);
	}

	rc = nvram_set(index, key, value);
    return rc;
}

int ra_nv_get(int argc, char *argv[])
{
	char *fz;
	char *key;
	char *rc;

	int index, ret;

	if (argc != 3 && argc != 2)
		return get_usage(argv[0]);

	if (argc == 2) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
	} else {
		fz = argv[1];
		key = argv[2];
	}

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return get_usage(argv[0]);
	}

	rc = nvram_get(index, key);
	if (rc) {
	    printf("%s\n", rc);
	    ret = 0;
	} else {
	    fprintf(stderr, "nvram_get return error or not return data!\n");
	    ret = -1;
	}

    return (ret);
}

int ra_nv_buf_get(int argc, char *argv[])
{
	char *fz, *key, *rc;
	int i, index, ret=0;

	if (argc < 3)
		return buf_get_usage(argv[0]);

	fz = argv[1];
	key = argv[2];

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return get_usage(argv[0]);
	}

#ifndef CONFIG_KERNEL_NVRAM
	if ( nvram_init(index) == -1 )
	{
		return "";
	}
#endif
	for (i = 2; i < argc; i++) {
	    rc = nvram_bufget(index, argv[i]);
	    if (rc) {
		printf("%s=%s\n",argv[i], rc);
	    } else {
		fprintf(stderr, "nvram_buf_get return error or not return data!\n");
		ret = -1;
		break;
	    }
	}

#ifndef CONFIG_KERNEL_NVRAM
	//Always need close nvram
	nvram_close(index);
#endif
    return (ret);
}

void usage(char *cmd)
{
#ifndef CONFIG_RT2860V2_USER_MEMORY_OPTIMIZATION
	printf("Usage:\n");
	printf("  %s <command> [<platform>] [<file>]\n\n", cmd);
	printf("command:\n");
	printf("  rt2860_nvram_show - display rt2860 values in nvram\n");
#ifdef CONFIG_DUAL_IMAGE
	printf("  uboot_nvram_show - display uboot parameter values\n");
#endif
	printf("  show    - display values in nvram for <platform>\n");
	printf("  gen     - generate config file from nvram for <platform>\n");
	printf("  renew   - replace nvram values for <platform> with <file>\n");
	printf("  clear	  - clear all entries in nvram for <platform>\n");
	printf("  default - load default for <platform>\n");
	printf("platform:\n");
	printf("  2860    - rt2860\n");
#ifdef CONFIG_DUAL_IMAGE
	printf("  uboot    - uboot parameter\n");
#endif
	printf("file:\n");
	printf("          - file name for renew command\n");
#endif
	exit(0);
}

int main(int argc, char *argv[])
{
	char *cmd = "";
	int index;

	if (argc < 2)
		usage(argv[0]);

	//call nvram_get, nvram_set or nvram_show
	if (cmd == strrchr(argv[0], '/'))
		cmd++;
	else
		cmd = argv[0];

	if (!strncmp(cmd, "nvram_get", 10))
		return ra_nv_get(argc, argv);
	if (!strncmp(cmd, "nvram_buf_get", 14))
		return ra_nv_buf_get(argc, argv);
	else if (!strncmp(cmd, "nvram_set", 10))
		return ra_nv_set(argc, argv);
	else if (!strncmp(cmd, "nvram_show", 11))
		return nvram_show(RT2860_NVRAM);
	else if (!strncmp(cmd, "nvram_default", 14))
		return nvram_load_default();

	if (argc == 2) {
		if (!strncmp(argv[1], "rt2860_nvram_show", 18))
			nvram_show(RT2860_NVRAM);
#ifdef CONFIG_DUAL_IMAGE
		else if (!strncmp(argv[1], "uboot_nvram_show", 17))
			nvram_show(UBOOT_NVRAM);
#endif
		else
			usage(argv[0]);
	} else if (argc == 3) {
		/* TODO: <cmd> gen 2860ap */
		if (!strncasecmp(argv[1], "gen", 4) ||
		    !strncasecmp(argv[1], "make_wireless_config", 21)) {
			if (!strncmp(argv[2], "2860", 5) ||
			    !strncasecmp(argv[2], "rt2860", 7)) //b-compatible
				gen_wifi_config(RT2860_NVRAM);
#ifdef CONFIG_DUAL_IMAGE
			else if (!strncasecmp(argv[2], "uboot", 6))
				fprintf(stderr,"No support of gen command of uboot parameter.\n");
#endif
			else
				usage(argv[0]);
		} else if (!strncasecmp(argv[1], "show", 5)) {
			if (!strncmp(argv[2], "2860", 5) ||
			    !strncasecmp(argv[2], "rt2860", 7)) //b-compatible
				nvram_show(RT2860_NVRAM);
#ifdef CONFIG_DUAL_IMAGE
			else if (!strncasecmp(argv[2], "uboot", 6))
				nvram_show(UBOOT_NVRAM);
#endif
			else {
				if ((index = getNvramIndex(argv[2])) == -1) {
					fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], argv[2]);
					usage(argv[0]);
				} else
					nvram_show(index);
			}

		} else if(!strncasecmp(argv[1], "clear", 6)) {
			if (!strncmp(argv[2], "2860", 5) || 
			    !strncasecmp(argv[2], "rt2860", 7)) //b-compatible
				nvram_clear(RT2860_NVRAM);
#ifdef CONFIG_DUAL_IMAGE
			else if (!strncasecmp(argv[2], "uboot", 6))
				nvram_clear(UBOOT_NVRAM);
#endif
			else
				usage(argv[0]);
		} else
			usage(argv[0]);
	} else if (argc == 4) {
		if (!strncasecmp(argv[1], "renew", 6)) {
			if (!strncmp(argv[2], "2860", 5) ||
			    !strncasecmp(argv[2], "rt2860", 7)) //b-compatible
				renew_nvram(RT2860_NVRAM, argv[3]);
#ifdef CONFIG_DUAL_IMAGE
			else if (!strncasecmp(argv[2], "uboot", 6))
				fprintf(stderr,"No support of renew command of uboot parameter.\n");
#endif
		} else
			usage(argv[0]);
	} else
		usage(argv[0]);
	return 0;
}
