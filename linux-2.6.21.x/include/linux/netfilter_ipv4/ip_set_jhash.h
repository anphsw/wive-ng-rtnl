#ifndef _LINUX_IPSET_JHASH_H
#define _LINUX_IPSET_JHASH_H

/* This is a copy of linux/jhash.h but the types u32/u8 are changed
 * to __u32/__u8 so that the header file can be included into
 * userspace code as well. Jozsef Kadlecsik (kadlec@blackhole.kfki.hu)
 */

/* jhash.h: Jenkins hash support.
 *
 * Copyright (C) 1996 Bob Jenkins (bob_jenkins@burtleburtle.net)
 *
 * http://burtleburtle.net/bob/hash/
 *
 * These are the credits from Bob's sources:
 *
 * lookup2.c, by Bob Jenkins, December 1996, Public Domain.
 * hash(), hash2(), hash3, and mix() are externally useful functions.
 * Routines to test the hash are included if SELF_TEST is defined.
 * You can use this free for any purpose.  It has no warranty.
 *
 * Copyright (C) 2003 David S. Miller (davem@redhat.com)
 *
 * I've modified Bob's hash to be useful in the Linux kernel, and
 * any bugs present are surely my fault.  -DaveM
 */

#include <linux/jhash.h>
#include <linux/sfhash.h>

/* NOTE: Arguments are modified. */
#define __jhash_mix(a, b, c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

/* The golden ration: an arbitrary value */
#define JHASH_GOLDEN_RATIO	0x9e3779b9

/* The most generic version, hashes an arbitrary sequence
 * of bytes.  No alignment or length assumptions are made about
 * the input key.
 */
static inline __u32 jhash(void *key, __u32 length, __u32 initval)
{
	__u32 a, b, c, len;
	__u8 *k = key;

	len = length;
	a = b = JHASH_GOLDEN_RATIO;
	c = initval;

	while (len >= 12) {
		a += (k[0] +((__u32)k[1]<<8) +((__u32)k[2]<<16) +((__u32)k[3]<<24));
		b += (k[4] +((__u32)k[5]<<8) +((__u32)k[6]<<16) +((__u32)k[7]<<24));
		c += (k[8] +((__u32)k[9]<<8) +((__u32)k[10]<<16)+((__u32)k[11]<<24));

		__jhash_mix(a,b,c);

		k += 12;
		len -= 12;
	}

	c += length;
	switch (len) {
	case 11: c += ((__u32)k[10]<<24);
	case 10: c += ((__u32)k[9]<<16);
	case 9 : c += ((__u32)k[8]<<8);
	case 8 : b += ((__u32)k[7]<<24);
	case 7 : b += ((__u32)k[6]<<16);
	case 6 : b += ((__u32)k[5]<<8);
	case 5 : b += k[4];
	case 4 : a += ((__u32)k[3]<<24);
	case 3 : a += ((__u32)k[2]<<16);
	case 2 : a += ((__u32)k[1]<<8);
	case 1 : a += k[0];
	};

	__jhash_mix(a,b,c);

	return c;
}

/* A special optimized version that handles 1 or more of __u32s.
 * The length parameter here is the number of __u32s in the key.
 */
static inline __u32 jhash2(__u32 *k, __u32 length, __u32 initval)
{
	__u32 a, b, c, len;

	a = b = JHASH_GOLDEN_RATIO;
	c = initval;
	len = length;

	while (len >= 3) {
		a += k[0];
		b += k[1];
		c += k[2];
		__jhash_mix(a, b, c);
		k += 3; len -= 3;
	}

	c += length * 4;

	switch (len) {
	case 2 : b += k[1];
	case 1 : a += k[0];
	};

	__jhash_mix(a,b,c);

	return c;
}


/* A special ultra-optimized versions that knows they are hashing exactly
 * 3, 2 or 1 word(s).
 *
 * NOTE: In partilar the "c += length; __jhash_mix(a,b,c);" normally
 *       done at the end is not done here.
 */
static inline __u32 jhash_3words(__u32 a, __u32 b, __u32 c, __u32 initval)
{
	a += JHASH_GOLDEN_RATIO;
	b += JHASH_GOLDEN_RATIO;
	c += initval;

	__jhash_mix(a, b, c);

	return c;
}

static inline __u32 jhash_2words(__u32 a, __u32 b, __u32 initval)
{
	return HASH_3WORDS(a, b, 0, initval);
}

static inline __u32 jhash_1word(__u32 a, __u32 initval)
{
	return HASH_3WORDS(a, 0, 0, initval);
}

#endif /* _LINUX_IPSET_JHASH_H */
