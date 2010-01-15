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
 * compact speed_t <-> speed functions for busybox
 *
 * Copyright (C) 2003  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include <termios.h>
#include "libbb.h"

struct speed_map {
	unsigned short speed;
	unsigned short value;
};

static const struct speed_map speeds[] = {
	{B0, 0},
	{B50, 50},
	{B75, 75},
	{B110, 110},
	{B134, 134},
	{B150, 150},
	{B200, 200},
	{B300, 300},
	{B600, 600},
	{B1200, 1200},
	{B1800, 1800},
	{B2400, 2400},
	{B4800, 4800},
	{B9600, 9600},
#ifdef	B19200
	{B19200, 19200},
#elif defined(EXTA)
	{EXTA, 19200},
#endif
#ifdef	B38400
	{B38400, 38400/256 + 0x8000U},
#elif defined(EXTB)
	{EXTB, 38400/256 + 0x8000U},
#endif
#ifdef B57600
	{B57600, 57600/256 + 0x8000U},
#endif
#ifdef B115200
	{B115200, 115200/256 + 0x8000U},
#endif
#ifdef B230400
	{B230400, 230400/256 + 0x8000U},
#endif
#ifdef B460800
	{B460800, 460800/256 + 0x8000U},
#endif
};

enum { NUM_SPEEDS = ARRAY_SIZE(speeds) };

unsigned FAST_FUNC tty_baud_to_value(speed_t speed)
{
	int i = 0;

	do {
		if (speed == speeds[i].speed) {
			if (speeds[i].value & 0x8000U) {
				return ((unsigned long) (speeds[i].value) & 0x7fffU) * 256;
			}
			return speeds[i].value;
		}
	} while (++i < NUM_SPEEDS);

	return 0;
}

speed_t FAST_FUNC tty_value_to_baud(unsigned int value)
{
	int i = 0;

	do {
		if (value == tty_baud_to_value(speeds[i].speed)) {
			return speeds[i].speed;
		}
	} while (++i < NUM_SPEEDS);

	return (speed_t) - 1;
}

#if 0
/* testing code */
#include <stdio.h>

int main(void)
{
	unsigned long v;
	speed_t s;

	for (v = 0 ; v < 500000; v++) {
		s = tty_value_to_baud(v);
		if (s == (speed_t) -1) {
			continue;
		}
		printf("v = %lu -- s = %0lo\n", v, (unsigned long) s);
	}

	printf("-------------------------------\n");

	for (s = 0 ; s < 010017+1; s++) {
		v = tty_baud_to_value(s);
		if (!v) {
			continue;
		}
		printf("v = %lu -- s = %0lo\n", v, (unsigned long) s);
	}

	return 0;
}
#endif
