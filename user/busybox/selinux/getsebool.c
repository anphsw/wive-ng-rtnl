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
 * getsebool
 *
 * Based on libselinux 1.33.1
 * Port to BusyBox  Hiroshi Shinji <shiroshi@my.email.ne.jp>
 *
 */

#include "libbb.h"

int getsebool_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int getsebool_main(int argc, char **argv)
{
	int i, rc = 0, active, pending, len = 0;
	char **names;
	unsigned opt;

	selinux_or_die();
	opt = getopt32(argv, "a");

	if (opt) { /* -a */
		if (argc > 2)
			bb_show_usage();

		rc = security_get_boolean_names(&names, &len);
		if (rc)
			bb_perror_msg_and_die("cannot get boolean names");

		if (!len) {
			puts("No booleans");
			return 0;
		}
	}

	if (!len) {
		if (argc < 2)
			bb_show_usage();
		len = argc - 1;
		names = xmalloc(sizeof(char *) * len);
		for (i = 0; i < len; i++)
			names[i] = xstrdup(argv[i + 1]);
	}

	for (i = 0; i < len; i++) {
		active = security_get_boolean_active(names[i]);
		if (active < 0) {
			bb_error_msg_and_die("error getting active value for %s", names[i]);
		}
		pending = security_get_boolean_pending(names[i]);
		if (pending < 0) {
			bb_error_msg_and_die("error getting pending value for %s", names[i]);
		}
		printf("%s --> %s", names[i], (active ? "on" : "off"));
		if (pending != active)
			printf(" pending: %s", (pending ? "on" : "off"));
		bb_putchar('\n');
	}

	if (ENABLE_FEATURE_CLEAN_UP) {
		for (i = 0; i < len; i++)
			free(names[i]);
		free(names);
	}

	return rc;
}
