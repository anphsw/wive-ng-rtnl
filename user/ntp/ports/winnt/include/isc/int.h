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
 * Copyright (C) 1999-2001  Internet Software Consortium.
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

/* $Id: int.h,v 1.1.1.1 2007-04-02 09:43:49 yy Exp $ */

#ifndef ISC_INT_H
#define ISC_INT_H 1

#define _INTEGRAL_MAX_BITS 64
#include <limits.h>

typedef __int8				isc_int8_t;
typedef unsigned __int8			isc_uint8_t;
typedef __int16				isc_int16_t;
typedef unsigned __int16		isc_uint16_t;
typedef __int32				isc_int32_t;
typedef unsigned __int32		isc_uint32_t;
typedef __int64				isc_int64_t;
typedef unsigned __int64		isc_uint64_t;

#define ISC_INT8_MIN	-128
#define ISC_INT8_MAX	127
#define ISC_UINT8_MAX	255

#define ISC_INT16_MIN	-32768
#define ISC_INT16_MAX	32767
#define ISC_UINT16_MAX	65535

/*
 * Note that "int" is 32 bits on all currently supported Unix-like operating
 * systems, but "long" can be either 32 bits or 64 bits, thus the 32 bit
 * constants are not qualified with "L".
 */
#define ISC_INT32_MIN	_I32_MIN
#define ISC_INT32_MAX	_I32_MAX
#define ISC_UINT32_MAX	_UI32_MAX

#define ISC_INT64_MIN	_I64_MIN
#define ISC_INT64_MAX	_I64_MAX
#define ISC_UINT64_MAX	_UI64_MAX

#endif /* ISC_INT_H */
