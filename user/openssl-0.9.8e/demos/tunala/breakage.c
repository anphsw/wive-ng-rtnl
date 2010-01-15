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
#include "tunala.h"

int int_strtoul(const char *str, unsigned long *val)
{
#ifdef HAVE_STRTOUL
	char *tmp;
	unsigned long ret = strtoul(str, &tmp, 10);
	if((str == tmp) || (*tmp != '\0'))
		/* The value didn't parse cleanly */
		return 0;
	if(ret == ULONG_MAX)
		/* We hit a limit */
		return 0;
	*val = ret;
	return 1;
#else
	char buf[2];
	unsigned long ret = 0;
	buf[1] = '\0';
	if(str == '\0')
		/* An empty string ... */
		return 0;
	while(*str != '\0') {
		/* We have to multiply 'ret' by 10 before absorbing the next
		 * digit. If this will overflow, catch it now. */
		if(ret && (((ULONG_MAX + 10) / ret) < 10))
			return 0;
		ret *= 10;
		if(!isdigit(*str))
			return 0;
		buf[0] = *str;
		ret += atoi(buf);
		str++;
	}
	*val = ret;
	return 1;
#endif
}

#ifndef HAVE_STRSTR
char *int_strstr(const char *haystack, const char *needle)
{
	const char *sub_haystack = haystack, *sub_needle = needle;
	unsigned int offset = 0;
	if(!needle)
		return haystack;
	if(!haystack)
		return NULL;
	while((*sub_haystack != '\0') && (*sub_needle != '\0')) {
		if(sub_haystack[offset] == sub_needle) {
			/* sub_haystack is still a candidate */
			offset++;
			sub_needle++;
		} else {
			/* sub_haystack is no longer a possibility */
			sub_haystack++;
			offset = 0;
			sub_needle = needle;
		}
	}
	if(*sub_haystack == '\0')
		/* Found nothing */
		return NULL;
	return sub_haystack;
}
#endif
