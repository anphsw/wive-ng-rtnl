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

#include "clockstuff.h"
#include "ntp_stdlib.h"

const char *	set_tod_using = "SetSystemTime";

int
ntp_set_tod(
	struct timeval *tv,
	void *tzp
	)
{
	SYSTEMTIME st;
	struct tm *gmtm;
	long x = tv->tv_sec;
	long y = tv->tv_usec;
	(void) tzp;

	gmtm = gmtime((const time_t *) &x);
	st.wSecond		= (WORD) gmtm->tm_sec;
	st.wMinute		= (WORD) gmtm->tm_min;
	st.wHour		= (WORD) gmtm->tm_hour;
	st.wDay 		= (WORD) gmtm->tm_mday;
	st.wMonth		= (WORD) (gmtm->tm_mon	+ 1);
	st.wYear		= (WORD) (gmtm->tm_year + 1900);
	st.wDayOfWeek		= (WORD) gmtm->tm_wday;
	st.wMilliseconds	= (WORD) (y / 1000);

	if (!SetSystemTime(&st)) {
		msyslog(LOG_ERR, "SetSystemTime failed: %m\n");
		return -1;
	}
	return 0;
}
