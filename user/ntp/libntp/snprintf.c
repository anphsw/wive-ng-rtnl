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
#include <config.h>

#if !HAVE_SNPRINTF
#include <sys/types.h>

#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include <stdio.h>

#include "l_stdlib.h"

#ifdef __STDC__
int snprintf(char *str, size_t n, const char *fmt, ...)
#else
int snprintf(str, n, fmt, va_alist)
	char *str;
	size_t n;
	const char *fmt;
	va_dcl
#endif
{
	va_list ap;
	int rval;
#ifdef VSPRINTF_CHARSTAR
	char *rp;
#endif
#ifdef __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
#endif
#ifdef VSPRINTF_CHARSTAR
	rp = vsprintf(str, fmt, ap);
	va_end(ap);
	return (strlen(rp));
#else
	rval = vsprintf(str, fmt, ap);
	va_end(ap);
	return (rval);
#endif
}

int
vsnprintf(
	char *str,
	size_t n,
	const char *fmt,
	va_list ap
	)
{
#ifdef VSPRINTF_CHARSTAR
	return (strlen(vsprintf(str, fmt, ap)));
#else
	return (vsprintf(str, fmt, ap));
#endif
}
#else
int snprintf_bs;
#endif
