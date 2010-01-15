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
 * Frontend command-line utility for Linux NVRAM layer
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: nvram.c,v 1.1 2007/06/08 10:22:42 arthur Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ASUS_NVRAM
#include <unistd.h>	// for sleep()
#include <nvram/typedefs.h>
#include <nvram/bcmnvram.h>
#else	// !ASUS_NVRAM
#include <typedefs.h>
#include <bcmnvram.h>
#endif	// ASUS_NVRAM

static void
usage(void)
{
	fprintf(stderr, "usage: nvram [get name] [set name=value] [unset name] [show]\n");
	exit(0);
}

#ifdef ASUS
#define PROFILE_HEADER 	"HDR1"

#if 0 //eric++
/* added by Jiahao */
static char *NLS_NVRAM_U2C="asusnlsu2c";
static char *NLS_NVRAM_C2U="asusnlsc2u";
#endif	// #if 0 //eric++

// save nvram to file
int nvram_save(char *file, char *buf)
{
	FILE *fp;
	char *name;
	unsigned long count, filelen, i;

   	if ((fp=fopen(file, "w"))==NULL) return -1;

	count = 0;
	for (name=buf;*name;name+=strlen(name)+1)
	{	
		puts(name);
		count = count+strlen(name)+1;
	}
   	
   	filelen = count + (1024 - count%1024);	
   	fwrite(PROFILE_HEADER, 1, 4, fp);
   	fwrite(&filelen, 1, 4, fp);
   	fwrite(buf, 1, count, fp);
   	for(i=count;i<filelen;i++) fwrite(name, 1, 1, fp);   	
   	fclose(fp);
	return 0;
}
int issyspara(char *p)
{
	struct nvram_tuple *t/*eric--, *u*/;

#if 0
//#warning When you apply this nvram library to an platform, please remove these code and provide an array called router_defaults.
	struct nvram_tuple router_defaults[]={{"","",NULL}};

	return 1;	// FIXME: All nvram variables are considered as system parameters.
#else	// !ASUS_NVRAM
	extern struct nvram_tuple router_defaults[];
#endif	// ASUS_NVRAM

	for (t = router_defaults; t->name; t++)
	{
		if (strstr(p, t->name))
			break;
	}

	if (t->name) return 1;
	else return 0;
}

// restore nvram from file
int nvram_restore(char *file, char *buf)
{
   	FILE *fp;
   	char header[8], *p, *v;
  	unsigned long count, *filelen;

   	if ((fp=fopen(file, "r+"))==NULL) return -1;
    	   
   	count = fread(header, 1, 8, fp);
   	if (count>=8 && strncmp(header, PROFILE_HEADER, 4)==0)
   	{  
	    filelen = (unsigned long *)(header + 4);
   	    count = fread(buf, 1, *filelen, fp);
   	}   
   	fclose(fp);

   	p = buf;       	           
   
   	while(*p)
   	{       
		//printf("nv:%s\n", p);     	     	        	     	
       		v = strchr(p, '=');

		if(v!=NULL)
		{	
			*v++ = '\0' /*eric--NULL*/;

			if (issyspara(p))
			{
				nvram_set(p, v);
			}

       			p = v + strlen(v) + 1;			
		}
		else 
		{
			nvram_unset(p);
			p = p + 1;
		}
   	}

	nvram_set("x_Setting", "1");
	return 0;
}
#endif

/* NVRAM utility */
int
main(int argc, char **argv)
{
#ifdef ASUS_NVRAM
	char *name, *value, *buf;
#else	// !ASUS_NVRAM
	char *name, *value, buf[NVRAM_SPACE];
#endif	// ASUS_NVRAM

	int size;

	/* Skip program name */
	--argc;
	++argv;

	if (!*argv) 
		usage();
	
#ifdef ASUS_NVRAM
		buf = malloc (NVRAM_SPACE);
		if (buf == NULL)	{
			perror ("Out of memory!\n");
			return -1;
		}
#endif	// ASUS_NVRAM

	/* Process the remaining arguments. */
	for (; *argv; argv++) {
		if (!strncmp(*argv, "get", 3)) {
			if (*++argv) {
/*
				value = nvram_get(*argv);
				if (value==NULL)
//				if (value==-1)
					fprintf(stderr, "CP C: NULL\n");
				else
					fprintf(stderr, "CP D: not NULL\n");
				fprintf(stderr, "value2:%X\n", value);
				fprintf(stderr, "%s\n", value);
				puts(value);
*/				

				if ((value = nvram_get(*argv)))
					puts(value);
			}
		}
		else if (!strncmp(*argv, "set", 3)) {
			if (*++argv) {
				#ifdef ASUS_NVRAM
                		strncpy(value = buf, *argv, NVRAM_SPACE);
				#else   // !ASUS_NVRAM
                		strncpy(value = buf, *argv, sizeof(buf));
				#endif  // ASUS_NVRAM
				name = strsep(&value, "=");
				nvram_set(name, value);
			}
		}
		else if (!strncmp(*argv, "unset", 5)) {
			if (*++argv)
				nvram_unset(*argv);
		}
		else if (!strncmp(*argv, "commit", 5)) {
			nvram_commit();
		}
#ifdef ASUS
		else if (!strncmp(*argv, "save", 4)) 
		{
			if (*++argv) 
			{
#ifdef ASUS_NVRAM
				nvram_getall(buf, NVRAM_SPACE);	
#else	// !ASUS_NVRAM
				nvram_getall(buf, sizeof(buf));	
#endif	// ASUS_NVRAM
				nvram_save(*argv, buf);
			}
			
		}
		else if (!strncmp(*argv, "restore", 7)) 
		{
			if (*++argv) 
			{
				nvram_restore(*argv, buf);
			}
			
		}
#if 0 //eric++
		else if (!strncmp(*argv, "xfr", 3)) 
		{
			if (*++argv) 
			{				
				if ((value = nvram_xfr(*argv)))
					puts(value);
			}
		}
#endif	// #if 0 //eric++
#endif
		else if (!strncmp(*argv, "show", 4) || !strncmp(*argv, "getall", 6)) {
#ifdef ASUS_NVRAM
			nvram_getall(buf, NVRAM_SPACE);
#else	// !ASUS_NVRAM
			nvram_getall(buf, sizeof(buf));
#endif	// ASUS_NVRAM
			for (name = buf; *name; name += strlen(name) + 1)
				puts(name);
			size = sizeof(struct nvram_header) + (int) name - (int) buf;
			fprintf(stderr, "size: %d bytes (%d left)\n", size, NVRAM_SPACE - size);
		}
		if (!*argv)
			break;
	}

#ifdef ASUS_NVRAM
	if (buf != NULL)
		free (buf);
#endif	// ASUS_NVRAM

	return 0;
}	
