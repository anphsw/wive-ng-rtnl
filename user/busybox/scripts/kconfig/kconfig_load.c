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
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "lkc.h"

#define P(name,type,arg)	type (*name ## _p) arg
#include "lkc_proto.h"
#undef P

void kconfig_load(void)
{
	void *handle;
	char *error;

	handle = dlopen("./libkconfig.so", RTLD_LAZY);
	if (!handle) {
		handle = dlopen("./scripts/kconfig/libkconfig.so", RTLD_LAZY);
		if (!handle) {
			fprintf(stderr, "%s\n", dlerror());
			exit(1);
		}
	}

#define P(name,type,arg)			\
{						\
	name ## _p = dlsym(handle, #name);	\
	if ((error = dlerror()))  {		\
		fprintf(stderr, "%s\n", error);	\
		exit(1);			\
	}					\
}
#include "lkc_proto.h"
#undef P
}
