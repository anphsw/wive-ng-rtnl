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
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: linux_osl.h,v 1.1.1.1 2008/07/21 09:20:38 james26_jang Exp $
 */

#include <arpa/inet.h>	    
#include <assert.h>	    
#include <ctype.h>	    
#include <errno.h>
#include <fcntl.h>	    
#include <netdb.h>	    
#include <netinet/tcp.h>    
#include <stdio.h>	    
#include <stdlib.h>	    
#include <string.h>	    
#include <sys/socket.h>	    
#include <sys/stat.h>	    
#include <sys/uio.h>
#include <time.h>	    
#include <unistd.h>	    
#include <stdarg.h>	    // for va_list, etc.


/*  damn - the unix and vxworks version of inet_aton return different error codes. */
#define UPNP_INET_ATON(a,b)   (inet_aton(a,b)!=0)

#define OSL_NULL_FILE "/dev/null"
