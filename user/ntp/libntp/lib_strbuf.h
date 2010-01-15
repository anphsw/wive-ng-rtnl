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
 * lib_strbuf.h - definitions for routines which use the common string buffers
 */

#include <ntp_types.h>

/*
 * Sizes of things
 */
#define	LIB_NUMBUFS	200
#define	LIB_BUFLENGTH	80

/*
 * Macro to get a pointer to the next buffer
 */
#define	LIB_GETBUF(buf) \
	do { \
		if (!lib_inited) \
			init_lib(); \
		buf = &lib_stringbuf[lib_nextbuf][0]; \
		if (++lib_nextbuf >= LIB_NUMBUFS) \
			lib_nextbuf = 0; \
		memset(buf, 0, LIB_BUFLENGTH); \
	} while (0)

extern char lib_stringbuf[LIB_NUMBUFS][LIB_BUFLENGTH];
extern int lib_nextbuf;
extern int lib_inited;
