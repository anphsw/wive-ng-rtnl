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
 * Copyright (C) 2002-2005 Roman Zippel <zippel@linux-m68k.org>
 * Copyright (C) 2002-2005 Sam Ravnborg <sam@ravnborg.org>
 *
 * Released under the terms of the GNU GPL v2.0.
 */

#include <string.h>
#include "lkc.h"

/* file already present in list? If not add it */
struct file *file_lookup(const char *name)
{
	struct file *file;

	for (file = file_list; file; file = file->next) {
		if (!strcmp(name, file->name))
			return file;
	}

	file = malloc(sizeof(*file));
	memset(file, 0, sizeof(*file));
	file->name = strdup(name);
	file->next = file_list;
	file_list = file;
	return file;
}

/* write a dependency file as used by kbuild to track dependencies */
int file_write_dep(const char *name)
{
	struct file *file;
	FILE *out;

	if (!name)
		name = ".kconfig.d";
	out = fopen("..config.tmp", "w");
	if (!out)
		return 1;
	fprintf(out, "deps_config := \\\n");
	for (file = file_list; file; file = file->next) {
		if (file->next)
			fprintf(out, "\t%s \\\n", file->name);
		else
			fprintf(out, "\t%s\n", file->name);
	}
	fprintf(out, "\n.config include/autoconf.h: $(deps_config)\n\n$(deps_config):\n");
	fclose(out);
	rename("..config.tmp", name);
	return 0;
}


/* Allocate initial growable sting */
struct gstr str_new(void)
{
	struct gstr gs;
	gs.s = malloc(sizeof(char) * 64);
	gs.len = 16;
	strcpy(gs.s, "\0");
	return gs;
}

/* Allocate and assign growable string */
struct gstr str_assign(const char *s)
{
	struct gstr gs;
	gs.s = strdup(s);
	gs.len = strlen(s) + 1;
	return gs;
}

/* Free storage for growable string */
void str_free(struct gstr *gs)
{
	if (gs->s)
		free(gs->s);
	gs->s = NULL;
	gs->len = 0;
}

/* Append to growable string */
void str_append(struct gstr *gs, const char *s)
{
	size_t l = strlen(gs->s) + strlen(s) + 1;
	if (l > gs->len) {
		gs->s   = realloc(gs->s, l);
		gs->len = l;
	}
	strcat(gs->s, s);
}

/* Append printf formatted string to growable string */
void str_printf(struct gstr *gs, const char *fmt, ...)
{
	va_list ap;
	char s[10000]; /* big enough... */
	va_start(ap, fmt);
	vsnprintf(s, sizeof(s), fmt, ap);
	str_append(gs, s);
	va_end(ap);
}

/* Retrieve value of growable string */
const char *str_get(struct gstr *gs)
{
	return gs->s;
}

