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
    Copyright 2004, Broadcom Corporation      
    All Rights Reserved.      
          
    THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY      
    KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM      
    SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS      
    FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.      
*/

#ifndef _uio_h_
#define _uio_h_

struct _ufile;
typedef struct _ufile UFILE;

UFILE *usopen(char *str, int maxlen);
UFILE *ufopen(const char *fname);
UFILE *udopen(int fd);
int utell(UFILE *up);
char *ubuffer(UFILE *up);
void uclose(UFILE *up);
int uprintf(UFILE *up, char *fmt, ...);
int ufileno(UFILE *up);
void uflush(UFILE *up);

#endif /* uio_h_ */
