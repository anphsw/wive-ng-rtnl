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

struct globals_misc;
struct globals_memstack;
struct globals_var;

#ifndef GCC_COMBINE

/* We cheat here. They are declared as const ptr in ash.c,
 * but here we make them live in R/W memory */
struct globals_misc     *ash_ptr_to_globals_misc;
struct globals_memstack *ash_ptr_to_globals_memstack;
struct globals_var      *ash_ptr_to_globals_var;

#else

/* gcc -combine will see through and complain */
/* Using alternative method which is more likely to break
 * on weird architectures, compilers, linkers and so on */
struct globals_misc     *const ash_ptr_to_globals_misc __attribute__ ((section (".data")));
struct globals_memstack *const ash_ptr_to_globals_memstack __attribute__ ((section (".data")));
struct globals_var      *const ash_ptr_to_globals_var __attribute__ ((section (".data")));

#endif
