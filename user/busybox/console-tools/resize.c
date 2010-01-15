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
 * resize - set terminal width and height.
 *
 * Copyright 2006 Bernhard Fischer
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */
/* no options, no getopt */
#include "libbb.h"

#define ESC "\033"

#define old_termios (*(struct termios*)&bb_common_bufsiz1)

static void
onintr(int sig UNUSED_PARAM)
{
	tcsetattr(STDERR_FILENO, TCSANOW, &old_termios);
	exit(EXIT_FAILURE);
}

int resize_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int resize_main(int argc UNUSED_PARAM, char **argv UNUSED_PARAM)
{
	struct termios new;
	struct winsize w = { 0, 0, 0, 0 };
	int ret;

	/* We use _stderr_ in order to make resize usable
	 * in shell backticks (those redirect stdout away from tty).
	 * NB: other versions of resize open "/dev/tty"
	 * and operate on it - should we do the same?
	 */

	tcgetattr(STDERR_FILENO, &old_termios); /* fiddle echo */
	new = old_termios;
	new.c_cflag |= (CLOCAL | CREAD);
	new.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	bb_signals(0
		+ (1 << SIGINT)
		+ (1 << SIGQUIT)
		+ (1 << SIGTERM)
		+ (1 << SIGALRM)
		, onintr);
	tcsetattr(STDERR_FILENO, TCSANOW, &new);

	/* save_cursor_pos 7
	 * scroll_whole_screen [r
	 * put_cursor_waaaay_off [$x;$yH
	 * get_cursor_pos [6n
	 * restore_cursor_pos 8
	 */
	fprintf(stderr, ESC"7" ESC"[r" ESC"[999;999H" ESC"[6n");
	alarm(3); /* Just in case terminal won't answer */
	scanf(ESC"[%hu;%huR", &w.ws_row, &w.ws_col);
	fprintf(stderr, ESC"8");

	/* BTW, other versions of resize recalculate w.ws_xpixel, ws.ws_ypixel
	 * by calculating character cell HxW from old values
	 * (gotten via TIOCGWINSZ) and recomputing *pixel values */
	ret = ioctl(STDERR_FILENO, TIOCSWINSZ, &w);

	tcsetattr(STDERR_FILENO, TCSANOW, &old_termios);

	if (ENABLE_FEATURE_RESIZE_PRINT)
		printf("COLUMNS=%d;LINES=%d;export COLUMNS LINES;\n",
			w.ws_col, w.ws_row);

	return ret;
}
