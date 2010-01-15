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
 * pid file routines
 *
 * Copyright (C) 2007 by Stephane Billiart <stephane.billiart@gmail.com>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* Override ENABLE_FEATURE_PIDFILE */
#define WANT_PIDFILE 1
#include "libbb.h"

smallint wrote_pidfile;

void FAST_FUNC write_pidfile(const char *path)
{
	int pid_fd;
	char *end;
	char buf[sizeof(int)*3 + 2];
	struct stat sb;

	if (!path)
		return;
	/* we will overwrite stale pidfile */
	pid_fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (pid_fd < 0)
		return;

	/* path can be "/dev/null"! Test for such cases */
	wrote_pidfile = (fstat(pid_fd, &sb) == 0) && S_ISREG(sb.st_mode);

	if (wrote_pidfile) {
		/* few bytes larger, but doesn't use stdio */
		end = utoa_to_buf(getpid(), buf, sizeof(buf));
		*end = '\n';
		full_write(pid_fd, buf, end - buf + 1);
	}
	close(pid_fd);
}
