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
 * Mini getpty implementation for busybox
 * Bjorn Wesen, Axis Communications AB (bjornw@axis.com)
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

#define DEBUG 0

int FAST_FUNC xgetpty(char *line)
{
	int p;

#if ENABLE_FEATURE_DEVPTS
	p = open("/dev/ptmx", O_RDWR);
	if (p > 0) {
		grantpt(p); /* chmod+chown corresponding slave pty */
		unlockpt(p); /* (what does this do?) */
#if 0 /* if ptsname_r is not available... */
		const char *name;
		name = ptsname(p); /* find out the name of slave pty */
		if (!name) {
			bb_perror_msg_and_die("ptsname error (is /dev/pts mounted?)");
		}
		safe_strncpy(line, name, GETPTY_BUFSIZE);
#else
		/* find out the name of slave pty */
		if (ptsname_r(p, line, GETPTY_BUFSIZE-1) != 0) {
			bb_perror_msg_and_die("ptsname error (is /dev/pts mounted?)");
		}
		line[GETPTY_BUFSIZE-1] = '\0';
#endif
		return p;
	}
#else
	struct stat stb;
	int i;
	int j;

	strcpy(line, "/dev/ptyXX");

	for (i = 0; i < 16; i++) {
		line[8] = "pqrstuvwxyzabcde"[i];
		line[9] = '0';
		if (stat(line, &stb) < 0) {
			continue;
		}
		for (j = 0; j < 16; j++) {
			line[9] = j < 10 ? j + '0' : j - 10 + 'a';
			if (DEBUG)
				fprintf(stderr, "Trying to open device: %s\n", line);
			p = open(line, O_RDWR | O_NOCTTY);
			if (p >= 0) {
				line[5] = 't';
				return p;
			}
		}
	}
#endif /* FEATURE_DEVPTS */
	bb_error_msg_and_die("can't find free pty");
}
