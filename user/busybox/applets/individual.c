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
/* Minimal wrapper to build an individual busybox applet.
 *
 * Copyright 2005 Rob Landley <rob@landley.net
 *
 * Licensed under GPL version 2, see file LICENSE in this tarball for details
 */

const char *applet_name;

#include <stdio.h>
#include <stdlib.h>
#include "usage.h"

int main(int argc, char **argv)
{
	applet_name = argv[0];
	return APPLET_main(argc,argv);
}

void bb_show_usage(void)
{
	fputs(APPLET_full_usage "\n", stdout);
	exit(EXIT_FAILURE);
}
