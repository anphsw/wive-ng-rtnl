/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1998, 1999, 2003 by Ralf Baechle
 */
#ifndef _ASM_TIMEX_H
#define _ASM_TIMEX_H

#ifdef __KERNEL__

#include <asm/cpu-features.h>
#include <asm/mipsregs.h>
#include <asm/cpu.h>

/*
 * This is the frequency of the timer used for Linux's timer interrupt.
 * The value should be defined as accurate as possible or under certain
 * circumstances Linux timekeeping might become inaccurate or fail.
 *
 * For many system the exact clockrate of the timer isn't known but due to
 * the way this value is used we can get away with a wrong value as long
 * as this value is:
 *
 *  - a multiple of HZ
 *  - a divisor of the actual rate
 *
 * 500000 is a good such cheat value.
 *
 * The obscure number 1193182 is the same as used by the original i8254
 * time in legacy PC hardware; the chip unfortunately also found in a
 * bunch of MIPS systems.  The last remaining user of the i8254 for the
 * timer interrupt is the RM200; it's a very standard system so there is
 * no reason to make this a separate architecture.
 */

#include <timex.h>

/*
 * Standard way to access the cycle counter.
 * Currently only used on SMP for scheduling.
 *
 * Only the low 32 bits are available as a continuously counting entity.
 * But this only means we'll force a reschedule every 8 seconds or so,
 * which isn't an evil thing.
 *
 * We know that all SMP capable CPUs have cycle counters.
 */

typedef unsigned int cycles_t;

/*
 * On R4000/R4400 before version 5.0 an erratum exists such that if the
 * cycle counter is read in the exact moment that it is matching the
 * compare register, no interrupt will be generated.
 *
 * There is a suggested workaround and also the erratum can't strike if
 * the compare interrupt isn't being used as the clock source device.
 * However for now the implementaton of this function doesn't get these
 * fine details right.
 */
static inline cycles_t get_cycles(void)
{
	switch (boot_cpu_type()) {
	case CPU_R4400PC:
	case CPU_R4400SC:
	case CPU_R4400MC:
		if ((read_c0_prid() & 0xff) >= 0x0050)
			return read_c0_count();
		break;

        case CPU_R4000PC:
        case CPU_R4000SC:
        case CPU_R4000MC:
		break;

	default:
		if (cpu_has_counter)
			return read_c0_count();
		break;
	}

	return 0;	/* no usable counter */
}

#endif /* __KERNEL__ */

#endif /*  _ASM_TIMEX_H */
