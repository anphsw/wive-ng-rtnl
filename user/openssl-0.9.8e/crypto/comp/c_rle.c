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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/objects.h>
#include <openssl/comp.h>

static int rle_compress_block(COMP_CTX *ctx, unsigned char *out,
	unsigned int olen, unsigned char *in, unsigned int ilen);
static int rle_expand_block(COMP_CTX *ctx, unsigned char *out,
	unsigned int olen, unsigned char *in, unsigned int ilen);

static COMP_METHOD rle_method={
	NID_rle_compression,
	LN_rle_compression,
	NULL,
	NULL,
	rle_compress_block,
	rle_expand_block,
	NULL,
	NULL,
	};

COMP_METHOD *COMP_rle(void)
	{
	return(&rle_method);
	}

static int rle_compress_block(COMP_CTX *ctx, unsigned char *out,
	     unsigned int olen, unsigned char *in, unsigned int ilen)
	{
	/* int i; */

	if (olen < (ilen+1))
		{
		/* ZZZZZZZZZZZZZZZZZZZZZZ */
		return(-1);
		}

	*(out++)=0;
	memcpy(out,in,ilen);
	return(ilen+1);
	}

static int rle_expand_block(COMP_CTX *ctx, unsigned char *out,
	     unsigned int olen, unsigned char *in, unsigned int ilen)
	{
	int i;

	if (olen < (ilen-1))
		{
		/* ZZZZZZZZZZZZZZZZZZZZZZ */
		return(-1);
		}

	i= *(in++);
	if (i == 0)
		{
		memcpy(out,in,ilen-1);
		}
	return(ilen-1);
	}

