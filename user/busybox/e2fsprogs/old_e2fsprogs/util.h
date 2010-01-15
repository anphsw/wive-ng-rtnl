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
 * util.h --- header file defining prototypes for helper functions
 * used by tune2fs and mke2fs
 *
 * Copyright 2000 by Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Public
 * License.
 * %End-Header%
 */

extern void proceed_question(void);
extern void check_plausibility(const char *device, int force);
extern void parse_journal_opts(char **, int *, int *, const char *opts);
extern void check_mount(const char *device, int force, const char *type);
extern int figure_journal_size(int size, ext2_filsys fs);
extern void print_check_message(ext2_filsys fs);
extern void make_journal_device(char *journal_device, ext2_filsys fs, int quiet, int force);
extern void make_journal_blocks(ext2_filsys fs, int journal_size, int journal_flags, int quiet);
extern char *e2fs_set_sbin_path(void);
