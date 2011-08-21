#ifndef _system_iconv_h
#define _system_iconv_h
/* 
   Unix SMB/CIFS implementation.

   iconv memory system include wrappers

   Copyright (C) Andrew Tridgell 2004
   
     ** NOTE! The following LGPL license applies to the replace
     ** library. This does NOT imply that all of Samba is released
     ** under the LGPL
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, see <http://www.gnu.org/licenses/>.

*/

#ifndef HAVE_NATIVE_ICONV
#define HAVE_NATIVE_ICONV
#endif

#define HAVE_ICONV

#ifdef HAVE_NATIVE_ICONV
#if defined(HAVE_ICONV)
#include <iconv-compat.h>
#elif defined(HAVE_GICONV)
#include <giconv.h>
#elif defined(HAVE_BICONV)
#include <biconv.h>
#endif
#endif /* HAVE_NATIVE_ICONV */

/* needed for some systems without iconv. Doesn't really matter
   what error code we use */
#ifndef EILSEQ
#define EILSEQ EIO
#endif

#endif
