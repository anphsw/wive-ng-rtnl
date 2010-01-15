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
 * Copyright (C) 2002 Internet Software Consortium.
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

/* $Id: win32os.c,v 1.1.1.1 2007-04-02 09:43:49 yy Exp $ */

#include <windows.h>

#include <isc/win32os.h>

static BOOL bInit = FALSE;
static OSVERSIONINFOEX osVer;

static void
initialize_action(void) {
	BOOL bSuccess;

	if (bInit)
		return;	
	/*
	 * NOTE: VC++ 6.0 gets this function declaration wrong
	 * so we compensate by casting the argument
	 */
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bSuccess = GetVersionEx((OSVERSIONINFO *) &osVer);

	/*
	 * Versions of NT before NT4.0 SP6 did not return the
	 * extra info that the EX structure provides and returns
	 * a failure so we need to retry with the old structure.
	 */
	if(!bSuccess) {
		osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		bSuccess = GetVersionEx((OSVERSIONINFO *) &osVer);
	}
	bInit = TRUE;
}

unsigned int
isc_win32os_majorversion(void) {
	initialize_action();
	return ((unsigned int)osVer.dwMajorVersion);
}

unsigned int
isc_win32os_minorversion(void) {
	initialize_action();
	return ((unsigned int)osVer.dwMinorVersion);
}

unsigned int
isc_win32os_servicepackmajor(void) {
	initialize_action();
	return ((unsigned int)osVer.wServicePackMajor);
}

unsigned int
isc_win32os_servicepackminor(void) {
	initialize_action();
	return ((unsigned int)osVer.wServicePackMinor);
}

int
isc_win32os_versioncheck(unsigned int major, unsigned int minor,
		     unsigned int spmajor, unsigned int spminor) {

	initialize_action();

	if (major < isc_win32os_majorversion())
		return (1);
	if (major > isc_win32os_majorversion())
		return (-1);
	if (minor < isc_win32os_minorversion())
		return (1);
	if (minor > isc_win32os_minorversion())
		return (-1);
	if (spmajor < isc_win32os_servicepackmajor())
		return (1);
	if (spmajor > isc_win32os_servicepackmajor())
		return (-1);
	if (spminor < isc_win32os_servicepackminor())
		return (1);
	if (spminor > isc_win32os_servicepackminor())
		return (-1);

	/* Exact */
	return (0);
}