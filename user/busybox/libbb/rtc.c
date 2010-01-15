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
/*
 * Common RTC functions
 */

#include "libbb.h"
#include "rtc_.h"

#if ENABLE_FEATURE_HWCLOCK_ADJTIME_FHS
# define ADJTIME_PATH "/var/lib/hwclock/adjtime"
#else
# define ADJTIME_PATH "/etc/adjtime"
#endif

int FAST_FUNC rtc_adjtime_is_utc(void)
{
	int utc = 0;
	FILE *f = fopen_for_read(ADJTIME_PATH);

	if (f) {
		RESERVE_CONFIG_BUFFER(buffer, 128);

		while (fgets(buffer, sizeof(buffer), f)) {
			int len = strlen(buffer);

			while (len && isspace(buffer[len - 1]))
				len--;

			buffer[len] = 0;

			if (strncmp(buffer, "UTC", 3) == 0) {
				utc = 1;
				break;
			}
		}
		fclose(f);

		RELEASE_CONFIG_BUFFER(buffer);
	}

	return utc;
}

int FAST_FUNC rtc_xopen(const char **default_rtc, int flags)
{
	int rtc;

	if (!*default_rtc) {
		*default_rtc = "/dev/rtc";
		rtc = open(*default_rtc, flags);
		if (rtc >= 0)
			return rtc;
		*default_rtc = "/dev/rtc0";
		rtc = open(*default_rtc, flags);
		if (rtc >= 0)
			return rtc;
		*default_rtc = "/dev/misc/rtc";
	}

	return xopen(*default_rtc, flags);
}

time_t FAST_FUNC rtc_read_time(int fd, int utc)
{
	struct tm tm;
	char *oldtz = 0;
	time_t t = 0;

	memset(&tm, 0, sizeof(struct tm));
	xioctl(fd, RTC_RD_TIME, &tm);
	tm.tm_isdst = -1; /* not known */

	if (utc) {
		oldtz = getenv("TZ");
		putenv((char*)"TZ=UTC0");
		tzset();
	}

	t = mktime(&tm);

	if (utc) {
		unsetenv("TZ");
		if (oldtz)
			putenv(oldtz - 3);
		tzset();
	}

	return t;
}
