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
 * load_policy
 * Author: Yuichi Nakamura <ynakam@hitachisoft.jp>
 */
#include "libbb.h"

int load_policy_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int load_policy_main(int argc, char **argv UNUSED_PARAM)
{
	int rc;

	if (argc != 1) {
		bb_show_usage();
	}

	rc = selinux_mkload_policy(1);
	if (rc < 0) {
		bb_perror_msg_and_die("can't load policy");
	}

	return 0;
}
