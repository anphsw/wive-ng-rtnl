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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <nvram/bcmnvram.h>
#include <shutils.h>
typedef unsigned char   bool;
#include <wlutils.h>
#include <syslog.h>
#include "iboxcom.h"

#define MAXARRAY 64
#define MAXARRSIZE  64

#ifdef NOPARSER
int
parse_array(char *file, char *func, char *strarr)
{
	FILE *fp;
	char line[1024], *s, *l;
	int flag=0, len;
	int i;

	if(!(fp=fopen(file, "r+"))) return 0;
	
	while(fgets(line, sizeof(line), fp))
	{
		if(strstr(line, func)) flag=1;
		if(flag && strstr(line, "return"))
		{
			flag=2;
			break;
		}	
	}
	fclose(fp);

	if (flag!=2) return 0;

	l = strchr(line, ']');
	if(l) *l=0;
	l = strchr(line, '[');
	l++;
	s=l;
	len = strlen(s);
	while (strsep(&s, ","));

	i=0;
	/* Set token values */
	for (s = l; s < &l[len] && *s; s+=strlen(s)+1) 
	{
		strcpy(&strarr[i*MAXARRSIZE], s);
		i++;

		if(i==MAXARRAY) break;
	}
	return i;	
}

int isClaimedIDE(char *model)
{
	char tmparray[MAXARRAY][MAXARRSIZE];
	int tmpidx;
	unsigned long size=0;
	int i;

	tmpidx=parse_array("/tmp/diskstatus", "claimed_disk_interface_names", tmparray);

	for(i=0;i<tmpidx;i++)
	{
		printf("%d:%s\n", i, &tmparray[i][0]);
		if (strstr(&tmparray[i][0], "IDE")) break;
	}

	if (i==tmpidx) return 0;

	tmpidx=parse_array("/tmp/diskstatus", "claimed_disk_total_size", tmparray);
	if (i>=tmpidx) return 0;
	size = atoi(&tmparray[i][0]);
	
	tmpidx=parse_array("/tmp/diskstatus", "claimed_disk_model_info", tmparray);
	if (i>=tmpidx) return 0;
	strcpy(model, &tmparray[i][0]);
	return size;
}

int isBlankIDE(char *model)
{
	char tmparray[MAXARRAY][MAXARRSIZE];
	int tmpidx;
	unsigned long size=0;
	int i;

	tmpidx=parse_array("/tmp/diskstatus", "blank_disk_interface_names", tmparray);

	for(i=0;i<tmpidx;i++)
	{
		printf("%d:%s\n", i, &tmparray[i][0]);
		if (strstr(&tmparray[i][0], "IDE")) break;
	}

	if (i==tmpidx) return 0;

	tmpidx=parse_array("/tmp/diskstatus", "blank_disk_total_size", tmparray);
	if (i>=tmpidx) return 0;
	size = atoi(&tmparray[i][0]);
	
	tmpidx=parse_array("/tmp/diskstatus", "blank_disk_model_info", tmparray);
	if (i>=tmpidx) return 0;
	strcpy(model, &tmparray[i][0]);
	return size;
}


int isPrinter(char *model1, char *model2)
{
	char tmparray[MAXARRAY][MAXARRSIZE];
	int tmpidx;
	unsigned long size=0;
	int i;

	tmpidx=parse_array("/tmp/diskstatus", "printer_models", tmparray);
	if (tmpidx<=0) return 0;

	if (tmpidx>=1)
		strcpy(model1, &tmparray[0][0]);
	if (tmpidx>=2)
		strcpy(model2, &tmparray[1][0]);
	
	return tmpidx;
}
#endif

int
getStorageStatus(STORAGE_INFO_T *st)
{
	char tmparray[MAXARRAY][MAXARRSIZE];
	char *username, *password;

	st->Capability=0;

	//printf("chk getStorage Status:\n");	// tmp test
	unsigned long apps_status=0;

	// count apps_status
	if (nvram_match("swap_on", "1"))
	{
		apps_status|=APPS_STATUS_SWAP;
		//printf("chk 1: swap on\n");	// tmp test
	}
	if (nvram_match("apps_comp", "1"))
	{
		apps_status|=APPS_STATUS_FILECOMPLETENESS;
		//printf("chk 2: apps comp\n");	// tmp test
	}
	if (nvram_match("apps_disk_free", "1"))
	{
		apps_status|=APPS_STATUS_FREESPACE;
		//printf("chk 3: apps disk free\n");	// tmp test
	}
	//if (nvram_match("ftp_running", "1") && nvram_match("st_samba_mode_x", "1"))
	if (nvram_match("ftp_running", "1"))
	{
		apps_status|=APPS_STATUS_SAMBAMODE;
		//printf("chk 4: ftp/samba\n");	// tmp test
	}
	if (nvram_match("apps_running", "1") && nvram_match("apps_dl_x", "1"))
	{
		apps_status|=APPS_STATUS_RUNNING;
		//printf("chk 5: apps running/apps/dl_x\n");	// tmp test
	}
	//if (nvram_invmatch("usb_disc0_dev", "") || nvram_invmatch("usb_disc1_dev", ""))	
	if (nvram_invmatch("usb_disc0_dev", ""))	
	{
		apps_status|=APPS_STATUS_DISCONPORT;
		//printf("chk 6: usb0/1 disc dev\n");	// tmp test
	}
/*
	if (nvram_match("apps_dms_usb_port_x", "2"))
	{
		apps_status|=APPS_STATUS_DMPORT;
		//printf("chk 7: usb port\n");	// tmp test
	}
*/
	if (nvram_match("apps_dlx", "1"))
	{
		apps_status|=APPS_STATUS_DMMODE;
		//printf("chk 8: dlx\n");	// tmp test
	}
	if (nvram_match("apps_status_checked", "1"))
	{
		apps_status|=APPS_STATUS_CHECKED;
		//printf("chk 9: status checked\n");	// tmp test
	}
	//if ( (nvram_match("usb_disc0_port", "1") || nvram_match("usb_disc0_port", "2")) &&
	//     (strcmp(nvram_safe_get("usb_disc0_port"), nvram_safe_get("usb_disc1_port"))!=0)
	//   )
	if (nvram_match("usb_disc0_port", "1"))
	{
		apps_status|=APPS_STATUS_USBPORT1;
		//printf("chk 10: usb disk0 port\n");	// tmp test
	}
/*
	if ( (nvram_match("usb_disc1_port", "1") || nvram_match("usb_disc1_port", "2")) &&
	     (strcmp(nvram_safe_get("usb_disc0_port"), nvram_safe_get("usb_disc1_port"))!=0)
	   )
	{
		apps_status|=APPS_STATUS_USBPORT2;
		//printf("chk 11: usb disk1 port\n");	// tmp test
	}
*/
	st->AppsStatus=apps_status;	// disable for tmp
	//st->AppsStatus=0x3bf;	// conn success. ## 0x03ac: empty disk;  0x02ac: ##: after install, before run apps

	printf("apps status : %x\n", apps_status);	// tmp test

	if(nvram_invmatch("apps_pool", "") && nvram_invmatch("apps_share", ""))
	{		
		st->Capability = (unsigned long) atoi(nvram_safe_get("apps_caps"));
		//strcpy(st->AppsPool, nvram_safe_get("apps_pool"));
		strcpy(st->AppsPool, nvram_safe_get("usb_mnt_first_path"));
		strcpy(st->AppsShare, nvram_safe_get("apps_share"));
		//printf("chk 12-1: apps pool/share\n");	// tmp test
	}
	else
	{
		st->Capability=0;
		st->AppsPool[0]=0;
		st->AppsShare[0]=0;
		//printf("chk 12-2: no pool/share\n");	// tmp test
	}

	st->DiskSize=0;
	st->DiskStatus=DISK_STATUS_NONE;

	st->DiskModel[0]=0;	// disable for tmp
	//st->DiskModel[0]=0x32;	// tmp test
	//st->DiskModel[1]=0x2e;	// tmp test
	//st->DiskModel[2]=0x31;	// tmp test

	//st->PrinterModel1[0]=0;
	//st->PrinterModel2[0]=0;
	memset(st->PrinterModel1, 0, 32);
	memset(st->PrinterModel2, 0, 32);
	if(strncmp(nvram_safe_get("st_ftp_modex"), "2", 1) == 0)
	{
		username = nvram_safe_get("acc_username0");
		password = nvram_safe_get("acc_password0");
		strncpy(st->PrinterModel1, username, strlen(username));
		strncpy(st->PrinterModel2, password, strlen(password));
	}
	else
	{
		strncpy(st->PrinterModel1, "anonymous", 9);
	}

#ifdef NOPARSER
		//printf("chk 13: noparser\n");	// tmp test
	// get from page
	eval("wget", "http://127.0.0.1/diskstatus.asp", "-O", "/tmp/diskstatus");
	// find claimed ide
	if ((st->DiskSize=isClaimedIDE(st->DiskModel)))
	{
		st->DiskStatus=DISK_STATUS_CLAIMED;
		printf("chk 14: is claimed IDE\n");	// tmp test
	}
	else if ((st->DiskSize=isBlankIDE(st->DiskModel)))
	{	
		st->DiskStatus=DISK_STATUS_BLANK;
		//printf("chk 15: disk blank\n");	// tmp test
	}
	isPrinter(st->PrinterModel1, st->PrinterModel2);
#endif

	printf("Storage: %d %s\n", st->DiskStatus, st->AppsShare);	// tmp test
	return 0;
}

