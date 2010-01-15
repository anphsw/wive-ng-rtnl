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
 * Copyright (C) 2008 by Denys Vlasenko <vda.linux@googlemail.com>
 *
 * Licensed under GPLv2, see file LICENSE in this tarball for details.
 */

#include <errno.h>

struct globals;

#ifndef GCC_COMBINE

/* We cheat here. It is declared as const ptr in libbb.h,
 * but here we make it live in R/W memory */
struct globals *ptr_to_globals;

#ifdef __GLIBC__
int *bb_errno;
#endif


#else


/* gcc -combine will see through and complain */
/* Using alternative method which is more likely to break
 * on weird architectures, compilers, linkers and so on */
struct globals *const ptr_to_globals __attribute__ ((section (".data")));

#ifdef __GLIBC__
int *const bb_errno __attribute__ ((section (".data")));
#endif

#endif
