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
#ifndef COMMON_H
#define COMMON_H

#include <endian.h>
#include <byteswap.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define le_to_host16(n) (n)
#define host_to_le16(n) (n)
#define be_to_host16(n) bswap_16(n)
#define host_to_be16(n) bswap_16(n)
#else
#define le_to_host16(n) bswap_16(n)
#define host_to_le16(n) bswap_16(n)
#define be_to_host16(n) (n)
#define host_to_be16(n) (n)
#endif


#include <stdint.h>
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

void hostapd_hexdump(const char *title, u8 *buf, size_t len);
int hwaddr_aton(char *txt, u8 *addr);

static inline void print_char(char c)
{
	if (c >= 32 && c < 127)
		printf("%c", c);
	else
		printf("<%02x>", c);
}

static inline void fprint_char(FILE *f, char c)
{
	if (c >= 32 && c < 127)
		fprintf(f, "%c", c);
	else
		fprintf(f, "<%02x>", c);
}

#endif /* COMMON_H */
