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
/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright (C) 2007 Denys Vlasenko
 *
 * Licensed under GPL version 2, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

void FAST_FUNC fputc_printable(int ch, FILE *file)
{
	if ((ch & (0x80 + PRINTABLE_META)) == (0x80 + PRINTABLE_META)) {
		fputs("M-", file);
		ch &= 0x7f;
	}
	ch = (unsigned char) ch;
	if (ch == 0x9b) {
		/* VT100's CSI, aka Meta-ESC, is not printable on vt-100 */
		ch = '{';
		goto print_caret;
	}
	if (ch < ' ') {
		ch += '@';
		goto print_caret;
	}
	if (ch == 0x7f) {
		ch = '?';
 print_caret:
		fputc('^', file);
	}
	fputc(ch, file);
}
