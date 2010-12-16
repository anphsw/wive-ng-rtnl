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
	buf = nvram_get(RT2860_NVRAM, "lan_ipaddr");
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
