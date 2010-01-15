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
 * prioritynames[] and facilitynames[]
 *
 * Copyright (C) 2008 by Denys Vlasenko <vda.linux@gmail.com>
 *
 * Licensed under GPLv2, see file LICENSE in this tarball for details.
 */

#include "libbb.h"
#define SYSLOG_NAMES
#define SYSLOG_NAMES_CONST
#include <syslog.h>

#if 0
/* For the record: with SYSLOG_NAMES <syslog.h> defines
 * (not declares) the following:
 */
typedef struct _code {
	/*const*/ char *c_name;
	int c_val;
} CODE;
/*const*/ CODE prioritynames[] = {
    { "alert", LOG_ALERT },
...
    { NULL, -1 }
};
/* same for facilitynames[] */

/* This MUST occur only once per entire executable,
 * therefore we can't just do it in syslogd.c and logger.c -
 * there will be two copies of it.
 *
 * We cannot even do it in separate file and then just reference
 * prioritynames[] from syslogd.c and logger.c - bare <syslog.h>
 * will not emit extern decls for prioritynames[]! Attempts to
 * emit "matching" struct _code declaration defeat the whole purpose
 * of <syslog.h>.
 *
 * For now, syslogd.c and logger.c are simply compiled into
 * one object file.
 */
#endif

#if ENABLE_SYSLOGD
#include "syslogd.c"
#endif

#if ENABLE_LOGGER
#include "logger.c"
#endif
