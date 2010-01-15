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
 * getenforce
 *
 * Based on libselinux 1.33.1
 * Port to BusyBox  Hiroshi Shinji <shiroshi@my.email.ne.jp>
 *
 */

#include "libbb.h"

int getenforce_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int getenforce_main(int argc UNUSED_PARAM, char **argv UNUSED_PARAM)
{
	int rc;

	rc = is_selinux_enabled();
	if (rc < 0)
		bb_error_msg_and_die("is_selinux_enabled() failed");

	if (rc == 1) {
		rc = security_getenforce();
		if (rc < 0)
			bb_error_msg_and_die("getenforce() failed");

		if (rc)
			puts("Enforcing");
		else
			puts("Permissive");
	} else {
		puts("Disabled");
	}

	return 0;
}
