/*
 * Codes at here are heavily taken from upload.cgi.c which is for large file uploading , but 
 * in fact "upload_settings" only need few memory(~16k) so it is not necessary to follow 
 * upload.cgi.c at all.
 * 
 * YYHuang@Ralink TODO: code size.
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <linux/reboot.h>
#include <errno.h>

#include "upload.cgi.h"
#include "../options.h"

void *memmem(const void *buf, size_t buf_len, const void *byte_line, size_t byte_line_len)
{
	unsigned char *bl = (unsigned char *)byte_line;
	unsigned char *bf = (unsigned char *)buf;
	unsigned char *p  = bf;

	while (byte_line_len <= (buf_len - (p - bf)))
	{
		unsigned int b = *bl & 0xff;
		if ((p = (unsigned char *) memchr(p, b, buf_len - (p - bf))) != NULL)
		{
			if ((memcmp(p, byte_line, byte_line_len)) == 0)
				return p;
			else
				p++;
		}
		else
			break;
	}
	return NULL;
}

int findStrInFile(char *filename, int offset, unsigned char *str, int str_len)
{
	int pos = 0, rc;
	FILE *fp;
	unsigned char mem[MEM_SIZE];

	if(str_len > MEM_HALF)
		return -1;
	if(offset <0)
		return -1;

	fp = fopen(filename, "rb");
	if(!fp)
		return -1;

	rewind(fp);
	fseek(fp, offset + pos, SEEK_SET);
	rc = fread(mem, 1, MEM_SIZE, fp);
	while(rc){
		unsigned char *mem_offset;
		mem_offset = (unsigned char*)memmem(mem, rc, str, str_len);
		if(mem_offset){
			fclose(fp);	//found it
			return (mem_offset - mem) + pos + offset;
		}

		if(rc == MEM_SIZE){
			pos += MEM_HALF;	// 8
		}else
			break;
		
		rewind(fp);
		fseek(fp, offset+pos, SEEK_SET);
		rc = fread(mem, 1, MEM_SIZE, fp);
	}

	fclose(fp);
	return -1;
}

/*
 *  ps. callee must free memory...
 */
void *getMemInFile(char *filename, int offset, int len)
{
	void *result;
	FILE *fp;
	if ((fp = fopen(filename, "r")) == NULL )
		return NULL;

	fseek(fp, offset, SEEK_SET);
	result = malloc(sizeof(unsigned char) * len );
	
	if(!result)
		return NULL;

	if (fread(result, 1, len, fp) != len)
	{
		free(result);
		return NULL;
	}

	return result;
}

//use libnvram
#if 0
char *nvram_get(char *buf, const char *param)
{
	char *nl;
	char *result = NULL;
	char cmdline[64];
	FILE *fp;
	
	sprintf(cmdline, "nvram_get 2860 %s", param);
	memset(buf, 0, sizeof(buf));

	if ((fp = popen(cmdline, "r")) != NULL)
	{
		if (fgets(buf, sizeof(buf), fp))
		{
			if (strlen(buf)>0)
			{
				if ((nl = strchr(buf, '\n')) != NULL)
					*nl = '\0';
				result = buf;
			}
		}

		pclose(fp);
	}
	
	return result;
}
#endif

/*
 * description: return LAN interface name
 */
const char* getLanIfName(void)
{
	const char *if_name = "br0";
	char *mode = nvram_get(RT2860_NVRAM, "OperationMode");

	if (mode == NULL)
		return if_name;
	if (!strncmp(mode, "0", 2))
		if_name = "br0";
	else if (!strncmp(mode, "1", 2))
	{
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
		if_name = "br0";
#elif defined  CONFIG_ICPLUS_PHY && CONFIG_RT2860V2_AP_MBSS
		char *num_s = nvram_get(buf, "BssidNum");
		if (num_s == NULL)
			return if_name;
		else if (atoi(num_s) > 1)	// multiple ssid
			if_name = "br0";
		else
			if_name = "ra0";
#else
		if_name = "ra0";
#endif
	}
	else if (!strncmp(mode, "2", 2))
		if_name = "eth2";
	else if (!strncmp(mode, "3", 2))
		if_name = "br0";
	return if_name;
}

/*
 * arguments: ifname  - interface name
 *            if_addr - a 64-byte buffer to store ip address
 * description: fetch ip address, netmask associated to given interface name
 */
int getIfIp(const char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("getIfIp: open socket error");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0)
	{
		printf("getIfIp: ioctl SIOCGIFADDR error for %s", ifname);
		return -1;
	}

	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}

char *getLanIP(char *buf)
{
	if (getIfIp(getLanIfName(), buf) == 0)
		return buf;
	return NULL;
}

void html_header()
{
	printf
	(
		"Server: %s\n"
		"Pragma: no-cache\n"
		"Content-type: text/html\n",
		getenv("SERVER_SOFTWARE")
	);

	printf
	(
		"\n<html>\n<head>\n"
		"<title>Import Settings</title>\n"
		"<link rel=\"stylesheet\" href=\"/style/normal_ws.css\" type=\"text/css\">\n"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
		"</head>\n"
		"<body>\n"
		"<h1>Import Settings</h1>\n"
	);
}

void html_success(const char *timeout)
{
	char buf[64];
	
	printf
	(
		"<p>Done</p>\n"
		"<script language=\"JavaScript\" type=\"text/javascript\">\n"
		"function refresh_all()\n"
		"{\n"
		"	top.location.href = \"http://%s\";\n"
		"}\n\n"
		"function update()\n"
		"{\n"
		"	self.setTimeout(\"refresh_all()\", %s);\n"
		"}\n\n"
		"update();\n"
		"</script>"
		"</body></html>\n\n",
		getLanIP(buf), timeout
	);
}

void html_error(const char *s)
{
	char buf[64];
	
	printf
	(
		"<p>%s</p>\n"
		"<script language=\"JavaScript\" type=\"text/javascript\">\n"
		"alert('%s');\n"
		"top.location.href = \"http://%s\";\n"
		"</script>\n"
		"</body></html>\n\n",
		s, s,
		getLanIP(buf)
	);
}
