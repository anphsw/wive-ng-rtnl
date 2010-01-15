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
 * Copyright (C) 1998-2001  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: error.h,v 1.1.1.1 2007-04-02 09:43:49 yy Exp $ */

#ifndef ISC_ERROR_H
#define ISC_ERROR_H 1

#include <stdarg.h>

#include <isc/formatcheck.h>
#include <isc/lang.h>

ISC_LANG_BEGINDECLS

typedef void (*isc_errorcallback_t)(const char *, int, const char *, va_list);

void
isc_error_setunexpected(isc_errorcallback_t);

void
isc_error_setfatal(isc_errorcallback_t);

void
isc_error_unexpected(const char *, int, const char *, ...)
     ISC_FORMAT_PRINTF(3, 4);

void
isc_error_fatal(const char *, int, const char *, ...)
     ISC_FORMAT_PRINTF(3, 4);

void
isc_error_runtimecheck(const char *, int, const char *);

#define ISC_ERROR_RUNTIMECHECK(cond) \
	((void) ((cond) || \
		 ((isc_error_runtimecheck)(__FILE__, __LINE__, #cond), 0)))

ISC_LANG_ENDDECLS

#endif /* ISC_ERROR_H */
