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
 * setsebool
 * Simple setsebool
 * NOTE: -P option requires libsemanage, so this feature is
 * omitted in this version
 * Yuichi Nakamura <ynakam@hitachisoft.jp>
 */

#include "libbb.h"

int setsebool_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int setsebool_main(int argc, char **argv)
{
	char *p;
	int value;

	if (argc != 3)
		bb_show_usage();

	p = argv[2];

	if (LONE_CHAR(p, '1') || strcasecmp(p, "true") == 0 || strcasecmp(p, "on") == 0) {
		value = 1;
	} else if (LONE_CHAR(p, '0') || strcasecmp(p, "false") == 0 || strcasecmp(p, "off") == 0) {
		value = 0;
	} else {
		bb_show_usage();
	}

	if (security_set_boolean(argv[1], value) < 0)
		bb_error_msg_and_die("can't set boolean");

	return 0;
}
