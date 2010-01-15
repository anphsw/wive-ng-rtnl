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
 * modetoa - return an asciized mode
 */
#include <stdio.h>

#include "lib_strbuf.h"
#include "ntp_stdlib.h"

const char *
modetoa(
	int mode
	)
{
	char *bp;
	static const char *modestrings[] = {
		"unspec",
		"sym_active",
		"sym_passive",
		"client",
		"server",
		"broadcast",
		"control",
		"private",
		"bclient",
	};

	if (mode < 0 || mode >= (sizeof modestrings)/sizeof(char *)) {
		LIB_GETBUF(bp);
		(void)sprintf(bp, "mode#%d", mode);
		return bp;
	}

	return modestrings[mode];
}
