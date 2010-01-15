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
#ifndef _CLOCKSTUFF_H
#define _CLOCKSTUFF_H

#include "ntp_machine.h"
#include <time.h>
#include <sys\timeb.h>

#include "ntp_syslog.h"

/* Windows NT versions of gettimeofday and settimeofday
 *
 * ftime() has internal DayLightSavings related BUGS
 * therefore switched to GetSystemTimeAsFileTime()
 */

void init_winnt_time(void);
void shutdown_winnt_time(void);
void reset_winnt_time(void);

/* 100ns intervals between 1/1/1601 and 1/1/1970 as reported by
 * SystemTimeToFileTime()
 */

#define FILETIME_1970     0x019db1ded53e8000
#define HECTONANOSECONDS  10000000ui64

/*
 * Multimedia Timer
 */

void set_mm_timer(int);

enum {
	MM_TIMER_LORES,
	MM_TIMER_HIRES
};
#endif
