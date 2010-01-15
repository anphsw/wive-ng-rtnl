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
/* This header declares the necessary definitions for using the exponentiation
 * acceleration capabilities of Atalla cards. The only cryptographic operation
 * is performed by "ASI_RSAPrivateKeyOpFn" and this takes a structure that
 * defines an "RSA private key". However, it is really only performing a
 * regular mod_exp using the supplied modulus and exponent - no CRT form is
 * being used. Hence, it is a generic mod_exp function in disguise, and we use
 * it as such.
 *
 * Thanks to the people at Atalla for letting me know these definitions are
 * fine and that they can be reproduced here.
 *
 * Geoff.
 */

typedef struct ItemStr
	{
	unsigned char *data;
	int len;
	} Item;

typedef struct RSAPrivateKeyStr
	{
	void *reserved;
	Item version;
	Item modulus;
	Item publicExponent;
	Item privateExponent;
	Item prime[2];
	Item exponent[2];
	Item coefficient;
	} RSAPrivateKey;

/* Predeclare the function pointer types that we dynamically load from the DSO.
 * These use the same names and form that Ben's original support code had (in
 * crypto/bn/bn_exp.c) unless of course I've inadvertently changed the style
 * somewhere along the way!
 */

typedef int tfnASI_GetPerformanceStatistics(int reset_flag,
					unsigned int *ret_buf);

typedef int tfnASI_GetHardwareConfig(long card_num, unsigned int *ret_buf);

typedef int tfnASI_RSAPrivateKeyOpFn(RSAPrivateKey * rsaKey,
					unsigned char *output,
					unsigned char *input,
					unsigned int modulus_len);

