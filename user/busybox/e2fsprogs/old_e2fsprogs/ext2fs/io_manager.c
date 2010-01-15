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
 * io_manager.c --- the I/O manager abstraction
 */

#include <stdio.h>
#include <string.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <time.h>
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include "ext2_fs.h"
#include "ext2fs.h"

errcode_t io_channel_set_options(io_channel channel, const char *opts)
{
	errcode_t retval = 0;
	char *next, *ptr, *options, *arg;

	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);

	if (!opts)
		return 0;

	if (!channel->manager->set_option)
		return EXT2_ET_INVALID_ARGUMENT;

	options = malloc(strlen(opts)+1);
	if (!options)
		return EXT2_ET_NO_MEMORY;
	strcpy(options, opts);
	ptr = options;

	while (ptr && *ptr) {
		next = strchr(ptr, '&');
		if (next)
			*next++ = 0;

		arg = strchr(ptr, '=');
		if (arg)
			*arg++ = 0;

		retval = (channel->manager->set_option)(channel, ptr, arg);
		if (retval)
			break;
		ptr = next;
	}
	free(options);
	return retval;
}

errcode_t io_channel_write_byte(io_channel channel, unsigned long offset,
				int count, const void *data)
{
	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);

	if (channel->manager->write_byte)
		return channel->manager->write_byte(channel, offset,
						    count, data);

	return EXT2_ET_UNIMPLEMENTED;
}
