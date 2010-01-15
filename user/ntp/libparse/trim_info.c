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
 * /src/NTP/ntp4-dev/libparse/trim_info.c,v 4.5 2005/04/16 17:32:10 kardel RELEASE_20050508_A
 *
 * trim_info.c,v 4.5 2005/04/16 17:32:10 kardel RELEASE_20050508_A
 *
 * $Created: Sun Aug  2 20:20:34 1998 $
 *
 * Copyright (c) 1995-2005 by Frank Kardel <kardel <AT> ntp.org>
 * Copyright (c) 1989-1994 by Frank Kardel, Friedrich-Alexander Universität Erlangen-Nürnberg, Germany
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
#include "ntp_types.h"
#include "trimble.h"

cmd_info_t *
trimble_convert(
		unsigned int cmd,
		cmd_info_t   *tbl
		)
{
  int i;

  for (i = 0; tbl[i].cmd != 0xFF; i++)
    {
      if (tbl[i].cmd == cmd)
	return &tbl[i];
    }
  return 0;
}

/*
 * trim_info.c,v
 * Revision 4.5  2005/04/16 17:32:10  kardel
 * update copyright
 *
 * Revision 4.4  2004/11/14 15:29:41  kardel
 * support PPSAPI, upgrade Copyright to Berkeley style
 *
 * Revision 4.2  1998/12/20 23:45:31  kardel
 * fix types and warnings
 *
 * Revision 4.1  1998/08/09 22:27:48  kardel
 * Trimble TSIP support
 *
 */
