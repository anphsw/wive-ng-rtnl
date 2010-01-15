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
/* Based on ipsvd utilities written by Gerrit Pape <pape@smarden.org>
 * which are released into public domain by the author.
 * Homepage: http://smarden.sunsite.dk/ipsvd/
 *
 * Copyright (C) 2007 Denys Vlasenko.
 *
 * Licensed under GPLv2, see file LICENSE in this tarball for details.
 */

#if __GNUC_PREREQ(4,1)
# pragma GCC visibility push(hidden)
#endif

struct hcc {
	char *ip;
	int pid;
};

void ipsvd_perhost_init(unsigned);

/* Returns number of already opened connects to this ips, including this one.
 * ip should be a malloc'ed ptr.
 * If return value is <= maxconn, ip is inserted into the table
 * and pointer to table entry if stored in *hccpp
 * (useful for storing pid later).
 * Else ip is NOT inserted (you must take care of it - free() etc) */
unsigned ipsvd_perhost_add(char *ip, unsigned maxconn, struct hcc **hccpp);

/* Finds and frees element with pid */
void ipsvd_perhost_remove(int pid);

//unsigned ipsvd_perhost_setpid(int pid);
//void ipsvd_perhost_free(void);

#if __GNUC_PREREQ(4,1)
# pragma GCC visibility pop
#endif
