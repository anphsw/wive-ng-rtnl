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
 * Utility routines.
 *
 * Copyright (C) 2007 Denys Vlasenko
 *
 * Licensed under GPL version 2, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

#if ENABLE_MONOTONIC_SYSCALL

#include <sys/syscall.h>
/* Old glibc (< 2.3.4) does not provide this constant. We use syscall
 * directly so this definition is safe. */
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

/* libc has incredibly messy way of doing this,
 * typically requiring -lrt. We just skip all this mess */
static void get_mono(struct timespec *ts)
{
	if (syscall(__NR_clock_gettime, CLOCK_MONOTONIC, ts))
		bb_error_msg_and_die("clock_gettime(MONOTONIC) failed");
}
unsigned long long FAST_FUNC monotonic_ns(void)
{
	struct timespec ts;
	get_mono(&ts);
	return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}
unsigned long long FAST_FUNC monotonic_us(void)
{
	struct timespec ts;
	get_mono(&ts);
	return ts.tv_sec * 1000000ULL + ts.tv_nsec/1000;
}
unsigned FAST_FUNC monotonic_sec(void)
{
	struct timespec ts;
	get_mono(&ts);
	return ts.tv_sec;
}

#else

unsigned long long FAST_FUNC monotonic_ns(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000000ULL + tv.tv_usec * 1000;
}
unsigned long long FAST_FUNC monotonic_us(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000ULL + tv.tv_usec;
}
unsigned FAST_FUNC monotonic_sec(void)
{
	return time(NULL);
}

#endif
