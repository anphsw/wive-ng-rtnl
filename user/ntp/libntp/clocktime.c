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
 * clocktime - compute the NTP date from a day of year, hour, minute
 *	       and second.
 */
#include "ntp_fp.h"
#include "ntp_unixtime.h"
#include "ntp_stdlib.h"

/*
 * Hacks to avoid excercising the multiplier.  I have no pride.
 */
#define	MULBY10(x)	(((x)<<3) + ((x)<<1))
#define	MULBY60(x)	(((x)<<6) - ((x)<<2))	/* watch overflow */
#define	MULBY24(x)	(((x)<<4) + ((x)<<3))

/*
 * Two days, in seconds.
 */
#define	TWODAYS		(2*24*60*60)

/*
 * We demand that the time be within CLOSETIME seconds of the receive
 * time stamp.  This is about 4 hours, which hopefully should be
 * wide enough to collect most data, while close enough to keep things
 * from getting confused.
 */
#define	CLOSETIME	(4*60*60)


int
clocktime(
	int yday,
	int hour,
	int minute,
	int second,
	int tzoff,
	u_long rec_ui,
	u_long *yearstart,
	u_int32 *ts_ui
	)
{
	register long tmp;
	register u_long date;
	register u_long yst;

	/*
	 * Compute the offset into the year in seconds.  Note that
	 * this could come out to be a negative number.
	 */
	tmp = (long)(MULBY24((yday-1)) + hour + tzoff);
	tmp = MULBY60(tmp) + (long)minute;
	tmp = MULBY60(tmp) + (long)second;

	/*
	 * Initialize yearstart, if necessary.
	 */
	yst = *yearstart;
	if (yst == 0) {
		yst = calyearstart(rec_ui);
		*yearstart = yst;
	}

	/*
	 * Now the fun begins.  We demand that the received clock time
	 * be within CLOSETIME of the receive timestamp, but
	 * there is uncertainty about the year the timestamp is in.
	 * Use the current year start for the first check, this should
	 * work most of the time.
	 */
	date = (u_long)(tmp + (long)yst);
	if (date < (rec_ui + CLOSETIME) &&
	    date > (rec_ui - CLOSETIME)) {
		*ts_ui = date;
		return 1;
	}

	/*
	 * Trouble.  Next check is to see if the year rolled over and, if
	 * so, try again with the new year's start.
	 */
	yst = calyearstart(rec_ui);
	if (yst != *yearstart) {
		date = (u_long)((long)yst + tmp);
		*ts_ui = date;
		if (date < (rec_ui + CLOSETIME) &&
		    date > (rec_ui - CLOSETIME)) {
			*yearstart = yst;
			return 1;
		}
	}

	/*
	 * Here we know the year start matches the current system
	 * time.  One remaining possibility is that the time code
	 * is in the year previous to that of the system time.  This
	 * is only worth checking if the receive timestamp is less
	 * than a couple of days into the new year.
	 */
	if ((rec_ui - yst) < TWODAYS) {
		yst = calyearstart(yst - TWODAYS);
		if (yst != *yearstart) {
			date = (u_long)(tmp + (long)yst);
			if (date < (rec_ui + CLOSETIME) &&
			    date > (rec_ui - CLOSETIME)) {
				*yearstart = yst;
				*ts_ui = date;
				return 1;
			}
		}
	}

	/*
	 * One last possibility is that the time stamp is in the year
	 * following the year the system is in.  Try this one before
	 * giving up.
	 */
	yst = calyearstart(rec_ui + TWODAYS);
	if (yst != *yearstart) {
		date = (u_long)((long)yst + tmp);
		if (date < (rec_ui + CLOSETIME) &&
		    date > (rec_ui - CLOSETIME)) {
			*yearstart = yst;
			*ts_ui = date;
			return 1;
		}
	}

	/*
	 * Give it up.
	 */
	return 0;
}
