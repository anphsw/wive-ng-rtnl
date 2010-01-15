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
 * Utility routines.
 *
 * Copyright (C) 2006 Denys Vlasenko
 *
 * Licensed under GPL version 2, see file LICENSE in this tarball for details.
 */

/* We do not include libbb.h - #define makedev() is there! */
#include "platform.h"
#include <features.h>
#include <sys/sysmacros.h>

#ifdef __GLIBC__
/* At least glibc has horrendously large inline for this, so wrap it */
/* uclibc people please check - do we need "&& !__UCLIBC__" above? */

/* suppress gcc "no previous prototype" warning */
unsigned long long FAST_FUNC bb_makedev(unsigned int major, unsigned int minor);
unsigned long long FAST_FUNC bb_makedev(unsigned int major, unsigned int minor)
{
	return makedev(major, minor);
}
#endif
