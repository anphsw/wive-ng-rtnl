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
 * libbb/selinux_common.c
 *   -- common SELinux utility functions
 *
 * Copyright 2007 KaiGai Kohei <kaigai@kaigai.gr.jp>
 */
#include "libbb.h"
#include <selinux/context.h>

context_t FAST_FUNC set_security_context_component(security_context_t cur_context,
					 char *user, char *role, char *type, char *range)
{
	context_t con = context_new(cur_context);
	if (!con)
		return NULL;

	if (user && context_user_set(con, user))
		goto error;
	if (type && context_type_set(con, type))
		goto error;
	if (range && context_range_set(con, range))
		goto error;
	if (role && context_role_set(con, role))
		goto error;
	return con;

error:
	context_free(con);
	return NULL;
}

void FAST_FUNC setfscreatecon_or_die(security_context_t scontext)
{
	if (setfscreatecon(scontext) < 0) {
		/* Can be NULL. All known printf implementations
		 * display "(null)", "<null>" etc */
		bb_perror_msg_and_die("cannot set default "
				"file creation context to %s", scontext);
	}
}

void FAST_FUNC selinux_preserve_fcontext(int fdesc)
{
	security_context_t context;

	if (fgetfilecon(fdesc, &context) < 0) {
		if (errno == ENODATA || errno == ENOTSUP)
			return;
		bb_perror_msg_and_die("fgetfilecon failed");
	}
	setfscreatecon_or_die(context);
	freecon(context);
}

