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
 * sleep implementation for busybox
 *
 * Copyright (C) 2003  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* BB_AUDIT SUSv3 compliant */
/* BB_AUDIT GNU issues -- fancy version matches except args must be ints. */
/* http://www.opengroup.org/onlinepubs/007904975/utilities/sleep.html */

/* Mar 16, 2003      Manuel Novoa III   (mjn3@codepoet.org)
 *
 * Rewritten to do proper arg and error checking.
 * Also, added a 'fancy' configuration to accept multiple args with
 * time suffixes for seconds, minutes, hours, and days.
 */

#include "libbb.h"

/* This is a NOFORK applet. Be very careful! */


#if ENABLE_FEATURE_FANCY_SLEEP || ENABLE_FEATURE_FLOAT_SLEEP
static const struct suffix_mult sfx[] = {
	{ "s", 1 },
	{ "m", 60 },
	{ "h", 60*60 },
	{ "d", 24*60*60 },
	{ }
};
#endif

int sleep_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int sleep_main(int argc UNUSED_PARAM, char **argv)
{
#if ENABLE_FEATURE_FLOAT_SLEEP
	double duration;
	struct timespec ts;
#else
	unsigned duration;
#endif

	++argv;
	if (!*argv)
		bb_show_usage();

#if ENABLE_FEATURE_FLOAT_SLEEP

	duration = 0;
	do {
		char *arg = *argv;
		if (strchr(arg, '.')) {
			double d;
			int len = strspn(arg, "0123456789.");
			char sv = arg[len];
			arg[len] = '\0';
			d = bb_strtod(arg, NULL);
			if (errno)
				bb_show_usage();
			arg[len] = sv;
			len--;
			sv = arg[len];
			arg[len] = '1';
			duration += d * xatoul_sfx(&arg[len], sfx);
			arg[len] = sv;
		} else
			duration += xatoul_sfx(arg, sfx);
	} while (*++argv);

	ts.tv_sec = MAXINT(typeof(ts.tv_sec));
	ts.tv_nsec = 0;
	if (duration >= 0 && duration < ts.tv_sec) {
		ts.tv_sec = duration;
		ts.tv_nsec = (duration - ts.tv_sec) * 1000000000;
	}
	do {
		errno = 0;
		nanosleep(&ts, &ts);
	} while (errno == EINTR);

#elif ENABLE_FEATURE_FANCY_SLEEP

	duration = 0;
	do {
		duration += xatou_range_sfx(*argv, 0, UINT_MAX - duration, sfx);
	} while (*++argv);
	sleep(duration);

#else /* simple */

	duration = xatou(*argv);
	sleep(duration);
	// Off. If it's really needed, provide example why
	//if (sleep(duration)) {
	//	bb_perror_nomsg_and_die();
	//}

#endif

	return EXIT_SUCCESS;
}
