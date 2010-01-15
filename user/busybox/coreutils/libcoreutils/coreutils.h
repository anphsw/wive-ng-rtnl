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
 * Licensed under the GPL v2 or later, see the file LICENSE in this tarball.
 */

#ifndef COREUTILS_H
#define COREUTILS_H		1

#if __GNUC_PREREQ(4,1)
# pragma GCC visibility push(hidden)
#endif

typedef int (*stat_func)(const char *fn, struct stat *ps);

int cp_mv_stat2(const char *fn, struct stat *fn_stat, stat_func sf) FAST_FUNC;
int cp_mv_stat(const char *fn, struct stat *fn_stat) FAST_FUNC;

mode_t getopt_mk_fifo_nod(char **argv) FAST_FUNC;

#if __GNUC_PREREQ(4,1)
# pragma GCC visibility pop
#endif

#endif
