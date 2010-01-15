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

/* $Id: msgcat.h,v 1.1.1.1 2007-04-02 09:43:49 yy Exp $ */

#ifndef ISC_MSGCAT_H
#define ISC_MSGCAT_H 1

/*****
 ***** Module Info
 *****/

/*
 * ISC Message Catalog
 *
 * Message catalogs aid internationalization of applications by allowing
 * messages to be retrieved from locale-specific files instead of
 * hardwiring them into the application.  This allows translations of
 * messages appropriate to the locale to be supplied without recompiling
 * the application.
 *
 * Notes:
 *	It's very important that message catalogs work, even if only the
 *	default_text can be used.
 *
 * MP:
 *	The caller must ensure appropriate synchronization of
 *	isc_msgcat_open() and isc_msgcat_close().  isc_msgcat_get()
 *	ensures appropriate synchronization.
 *
 * Reliability:
 *	No anticipated impact.
 *
 * Resources:
 *	<TBS>
 *
 * Security:
 *	No anticipated impact.
 *
 * Standards:
 *	None.
 */

/*****
 ***** Imports
 *****/

#include <isc/lang.h>
#include <isc/types.h>

ISC_LANG_BEGINDECLS

/*****
 ***** Methods
 *****/

void
isc_msgcat_open(const char *name, isc_msgcat_t **msgcatp);
/*
 * Open a message catalog.
 *
 * Notes:
 *
 *	If memory cannot be allocated or other failures occur, *msgcatp
 *	will be set to NULL.  If a NULL msgcat is given to isc_msgcat_get(),
 *	the default_text will be returned, ensuring that some message text
 *	will be available, no matter what's going wrong.
 *
 * Requires:
 *
 *	'name' is a valid string.
 *
 *	msgcatp != NULL && *msgcatp == NULL
 */

void
isc_msgcat_close(isc_msgcat_t **msgcatp);
/*
 * Close a message catalog.
 *
 * Notes:
 *
 *	Any string pointers returned by prior calls to isc_msgcat_get() are
 *	invalid after isc_msgcat_close() has been called and must not be
 *	used.
 *
 * Requires:
 *
 *	*msgcatp is a valid message catalog or is NULL.
 *
 * Ensures:
 *
 *	All resources associated with the message catalog are released.
 *
 *	*msgcatp == NULL
 */

const char *
isc_msgcat_get(isc_msgcat_t *msgcat, int set, int message,
	       const char *default_text);
/*
 * Get message 'message' from message set 'set' in 'msgcat'.  If it
 * is not available, use 'default_text'.
 *
 * Requires:
 *
 *	'msgcat' is a valid message catalog or is NULL.
 *
 *	set > 0
 *
 *	message > 0
 *
 *	'default_text' is a valid string.
 */

ISC_LANG_ENDDECLS

#endif /* ISC_MSGCAT_H */
