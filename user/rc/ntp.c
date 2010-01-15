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
/*
 * Copyright 2004, ASUSTek Inc.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND ASUS GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: ntp.c,v 1.1.1.1 2007/01/25 12:52:21 jiahao_jhou Exp $
 */
 
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include <nvram/bcmnvram.h>
#include <shutils.h>
#include <rc.h>
#include <stdarg.h>

static void catch_sig(int sig)
{
	if (sig == SIGUSR1)
	{
		//printf("SIGUSR1 catched\n");
	}
}

int ntp_main(void)
{
	char servers[32]; 
	//char *ntpclient_argv[] = {"ntpclient", "-h", servers, "-i", "3", "-l", "-s", NULL};
	pid_t pid;
	FILE *fp;
	int ret;
	
	strcpy(servers, nvram_safe_get("ntp_servers"));

	if (!strlen(servers)) return 0;
	
	fp=fopen("/var/run/ntp.pid","w");
	if (fp==NULL) exit(0);
	fprintf(fp, "%d", getpid());
	fclose(fp);

	signal(SIGUSR1, catch_sig);

	while(1)
	{
		ret=eval("/usr/sbin/ntpclient", "-h", servers, "-i", "3", "-l", "-s");
		pause();
	}	
}
