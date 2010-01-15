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
 * $Header: /home/cvsroot/RT288x_SDK/source/user/ntp/include/ntp_debug.h,v 1.1.1.1 2007-04-02 09:43:49 yy Exp $
 *
 * $Created: Sat Aug 20 14:23:01 2005 $
 *
 * Copyright (C) 2005 by Frank Kardel
 */
#ifndef NTP_DEBUG_H
#define NTP_DEBUG_H

/*
 * macros for debugging output - cut down on #ifdef pollution in the code
 */

#ifdef DEBUG
#define DPRINTF(_lvl_, _arg_)                   \
        if (debug >= (_lvl_))                   \
                printf _arg_;
#else
#define DPRINTF(_lvl_, _arg_)
#endif

#endif
/*
 * $Log: ntp_debug.h,v $
 * Revision 1.1.1.1  2007-04-02 09:43:49  yy
 * new NTP version
 *
 */
