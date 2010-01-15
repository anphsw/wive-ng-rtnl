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
 * /src/NTP/ntp4-dev/include/ieee754io.h,v 4.3 2005/04/16 17:32:10 kardel RELEASE_20050508_A
 *
 * ieee754io.h,v 4.3 2005/04/16 17:32:10 kardel RELEASE_20050508_A
 *
 * $Created: Sun Jul 13 12:22:11 1997 $
 *
 * Copyright (c) 1997-2005 by Frank Kardel <kardel <AT> ntp.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#ifndef IEEE754IO_H
#define IEEE754IO_H

#define IEEE_SINGLE 1
#define IEEE_DOUBLE 2

#define IEEE_MSB 1
#define IEEE_LSB 2

#define IEEE_OK          0	/* conversion ok */
#define IEEE_BADCALL     1	/* bad call parameters */
#define IEEE_NAN         2	/* found an NaN */
#define IEEE_POSINFINITY 3	/* positive infinity */
#define IEEE_NEGINFINITY 4	/* negative infinity */
#define IEEE_POSOVERFLOW 5	/* positive overflow */
#define IEEE_NEGOVERFLOW 6	/* negative overflow */

#define IEEE_OFFSETS     8	/* number of byte positions */
typedef unsigned char offsets_t[IEEE_OFFSETS];

int fetch_ieee754 P((unsigned char **bufp, int size, l_fp *lfpp, offsets_t offsets));
int put_ieee754 P((unsigned char **bufpp, int size, l_fp *lfpp, offsets_t offsets));

#endif
/*
 * History:
 *
 * ieee754io.h,v
 * Revision 4.3  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.2  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.0  1998/04/10 19:50:40  kardel
 * Start 4.0 release version numbering
 *
 * Revision 1.1  1998/04/10 19:27:33  kardel
 * initial NTP VERSION 4 integration of PARSE with GPS166 binary support
 *
 * Revision 1.1  1997/10/06 20:55:37  kardel
 * new parse structure
 *
 */
