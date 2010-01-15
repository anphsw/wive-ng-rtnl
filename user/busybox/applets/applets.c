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
 * Stub for linking busybox binary against libbusybox.
 *
 * Copyright (C) 2007 Denys Vlasenko <vda.linux@googlemail.com>
 *
 * Licensed under GPLv2, see file License in this tarball for details.
 */

#include <assert.h>
#include "busybox.h"

#if ENABLE_BUILD_LIBBUSYBOX
int main(int argc UNUSED_PARAM, char **argv)
{
	return lbb_main(argv);
}
#endif
