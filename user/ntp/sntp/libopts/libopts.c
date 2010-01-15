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
#define AUTOOPTS_INTERNAL
#include "compat/compat.h"
#define LOCAL static
#include "autoopts/options.h"
#include "autoopts/usage-txt.h"
#include "genshell.h"
#include "autoopts.h"
#include "proto.h"
#include "autoopts.c"
#include "boolean.c"
#include "configfile.c"
#include "cook.c"
#include "enumeration.c"
#include "environment.c"
#include "genshell.c"
#include "load.c"
#include "makeshell.c"
#include "nested.c"
#include "numeric.c"
#include "pgusage.c"
#include "putshell.c"
#include "restore.c"
#include "save.c"
#include "sort.c"
#include "stack.c"
#include "streqvcmp.c"
#include "text_mmap.c"
#include "tokenize.c"
#include "usage.c"
#include "version.c"
