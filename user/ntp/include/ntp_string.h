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
 * Define string ops: strchr strrchr memcmp memmove memset 
 */

#ifndef  _ntp_string_h
#define  _ntp_string_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MEMORY_H
# include <memory.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#endif

#ifdef HAVE_BSTRING_H
# include <bstring.h>
#endif

#ifndef STDC_HEADERS
# ifndef HAVE_STRCHR
#  include <strings.h>
#  define strchr index
#  define strrchr rindex
# endif
# ifndef __GNUC__
char *strchr(), *strrchr();
# endif
# ifndef HAVE_MEMCPY
#  define NTP_NEED_BOPS
# endif
#endif /* STDC_HEADERS */

#ifdef NTP_NEED_BOPS
# define memcmp(a,b,c) bcmp(a,b,(int)c)
# define memmove(t,f,c) bcopy(f,t,(int)c)
# define memcpy(t,f,c) bcopy(f,t,(int)c)
# define memset(a,x,c) if (x == 0x00) bzero(a,(int)c); else ntp_memset((char*)a,x,c)

void ntp_memset P((char *, int, int));

#endif /*  NTP_NEED_BOPS */

#endif /* _ntp_string_h */
