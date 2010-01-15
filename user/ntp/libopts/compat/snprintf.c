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

#ifndef HAVE_VPRINTF
#include "choke-me: no vprintf and no snprintf"
#endif

#if defined(HAVE_STDARG_H)
#  include <stdarg.h>
#  ifndef   VA_START
#    define VA_START(a, f)  va_start(a, f)
#    define VA_END(a)       va_end(a)
#  endif /* VA_START */
#  define SNV_USING_STDARG_H

#elif defined(HAVE_VARARGS_H)
#  include <varargs.h>
#  ifndef   VA_START
#    define VA_START(a, f) va_start(a)
#    define VA_END(a)    va_end(a)
#  endif /* VA_START */
#  undef  SNV_USING_STDARG_H

#else
#  include "must-have-stdarg-or-varargs"
#endif

static int
snprintf(char *str, size_t n, char const *fmt, ...)
{
    va_list ap;
    int rval;

#ifdef VSPRINTF_CHARSTAR
    char *rp;
    VA_START(ap, fmt);
    rp = vsprintf(str, fmt, ap);
    VA_END(ap);
    rval = strlen(rp);

#else
    VA_START(ap, fmt);
    rval = vsprintf(str, fmt, ap);
    VA_END(ap);
#endif

    if (rval > n) {
        fprintf(stderr, "snprintf buffer overrun %d > %d\n", rval, n);
        abort();
    }
    return rval;
}

static int
vsnprintf( char *str, size_t n, char const *fmt, va_list ap )
{
#ifdef VSPRINTF_CHARSTAR
    return (strlen(vsprintf(str, fmt, ap)));
#else
    return (vsprintf(str, fmt, ap));
#endif
}
