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
/*  Copyright (C) 2003     Manuel Novoa III
 *
 *  Licensed under GPL v2, or later.  See file LICENSE in this tarball.
 */

/*  Nov 6, 2003  Initial version.
 *
 *  NOTE: This implementation is quite strict about requiring all
 *    field seperators.  It also does not allow leading whitespace
 *    except when processing the numeric fields.  glibc is more
 *    lenient.  See the various glibc difference comments below.
 *
 *  TODO:
 *    Move to dynamic allocation of (currently statically allocated)
 *      buffers; especially for the group-related functions since
 *      large group member lists will cause error returns.
 *
 */

#ifndef GETXXKEY_R_FUNC
#error GETXXKEY_R_FUNC is not defined!
#endif

int GETXXKEY_R_FUNC(GETXXKEY_R_KEYTYPE key,
				GETXXKEY_R_ENTTYPE *__restrict resultbuf,
				char *__restrict buffer, size_t buflen,
				GETXXKEY_R_ENTTYPE **__restrict result)
{
	FILE *stream;
	int rv;

	*result = NULL;

	stream = fopen_for_read(GETXXKEY_R_PATHNAME);
	if (!stream)
		return errno;
	while (1) {
		rv = bb__pgsreader(GETXXKEY_R_PARSER, resultbuf, buffer, buflen, stream);
		if (!rv) {
			if (GETXXKEY_R_TEST(resultbuf)) { /* Found key? */
				*result = resultbuf;
				break;
			}
		} else {
			if (rv == ENOENT) {	/* end-of-file encountered. */
				rv = 0;
			}
			break;
		}
	}
	fclose(stream);

	return rv;
}

#undef GETXXKEY_R_FUNC
#undef GETXXKEY_R_PARSER
#undef GETXXKEY_R_ENTTYPE
#undef GETXXKEY_R_TEST
#undef GETXXKEY_R_KEYTYPE
#undef GETXXKEY_R_PATHNAME
