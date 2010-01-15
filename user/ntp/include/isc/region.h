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
 * Copyright (C) 2004  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2002  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: region.h,v 1.1.1.1 2007-04-02 09:43:49 yy Exp $ */

#ifndef ISC_REGION_H
#define ISC_REGION_H 1

#include <isc/types.h>

struct isc_region {
	unsigned char *	base;
	unsigned int	length;
};

struct isc_textregion {
	char *		base;
	unsigned int	length;
};

/* XXXDCL questionable ... bears discussion.  we have been putting off
 * discussing the region api.
 */
struct isc_constregion {
	const void *	base;
	unsigned int	length;
};

struct isc_consttextregion {
	const char *	base;
	unsigned int	length;
};

/*
 * The region structure is not opaque, and is usually directly manipulated.
 * Some macros are defined below for convenience.
 */

#define isc_region_consume(r,l) \
	do { \
		isc_region_t *_r = (r); \
		unsigned int _l = (l); \
		INSIST(_r->length >= _l); \
		_r->base += _l; \
		_r->length -= _l; \
	} while (0)

#define isc_textregion_consume(r,l) \
	do { \
		isc_textregion_t *_r = (r); \
		unsigned int _l = (l); \
		INSIST(_r->length >= _l); \
		_r->base += _l; \
		_r->length -= _l; \
	} while (0)

#define isc_constregion_consume(r,l) \
	do { \
		isc_constregion_t *_r = (r); \
		unsigned int _l = (l); \
		INSIST(_r->length >= _l); \
		_r->base += _l; \
		_r->length -= _l; \
	} while (0)

int
isc_region_compare(isc_region_t *r1, isc_region_t *r2);
/*
 * Compares the contents of two regions 
 *
 * Requires: 
 *	'r1' is a valid region
 *	'r2' is a valid region
 *
 * Returns:
 *	 < 0 if r1 is lexicographically less than r2
 *	 = 0 if r1 is lexicographically identical to r2
 *	 > 0 if r1 is lexicographically greater than r2
 */

#endif /* ISC_REGION_H */
