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
#include <unistd.h>

/* Just #include "autoconf.h" doesn't work for builds in separate
 * object directory */
#include "../include/autoconf.h"

/* Since we can't use platform.h, have to do this again by hand: */
#if ENABLE_NOMMU
#define BB_MMU 0
#define USE_FOR_NOMMU(...) __VA_ARGS__
#define USE_FOR_MMU(...)
#else
#define BB_MMU 1
#define USE_FOR_NOMMU(...)
#define USE_FOR_MMU(...) __VA_ARGS__
#endif

static const char usage_messages[] = ""
#define MAKE_USAGE
#include "usage.h"
#include "applets.h"
;

int main(void)
{
	write(STDOUT_FILENO, usage_messages, sizeof(usage_messages));
	return 0;
}
