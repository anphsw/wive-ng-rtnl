/* internal.h: mm/ internal definitions
 *
 * Copyright (C) 2004 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#ifndef __MM_INTERNAL_H
#define __MM_INTERNAL_H

#include <linux/mm.h>
#include <linux/hardirq.h>

static inline void set_page_count(struct page *page, int v)
{
	atomic_set(&page->_count, v);
}

/*
 * Turn a non-refcounted page (->_count == 0) into refcounted with
 * a count of one.
 */
static inline void set_page_refcounted(struct page *page)
{
	VM_BUG_ON(PageCompound(page) && page_private(page) != (unsigned long)page);
	VM_BUG_ON(atomic_read(&page->_count));
	set_page_count(page, 1);
}

static inline void __put_page(struct page *page)
{
	atomic_dec(&page->_count);
}

extern void fastcall __init __free_pages_bootmem(struct page *page,
						unsigned int order);

/*
 * FLATMEM and DISCONTIGMEM configurations use alloc_bootmem_node,
 * so all functions starting at paging_init should be marked __init
 * in those cases. SPARSEMEM, however, allows for memory hotplug,
 * and alloc_bootmem_node is not used.
 */
#ifdef CONFIG_SPARSEMEM
#define __paginginit __meminit
#else
#define __paginginit __init
#endif

#define ALLOC_HARDER		0x01 /* try to alloc harder */
#define ALLOC_HIGH		0x02 /* __GFP_HIGH set */
#define ALLOC_WMARK_MIN		0x04 /* use pages_min watermark */
#define ALLOC_WMARK_LOW		0x08 /* use pages_low watermark */
#define ALLOC_WMARK_HIGH	0x10 /* use pages_high watermark */
#define ALLOC_NO_WATERMARKS	0x20 /* don't check watermarks at all */
#define ALLOC_CPUSET		0x40 /* check for correct cpuset */

/*
 * get the deepest reaching allocation flags for the given gfp_mask
 */
static int inline gfp_to_alloc_flags(gfp_t gfp_mask)
{
	struct task_struct *p = current;
	int alloc_flags = ALLOC_WMARK_MIN | ALLOC_CPUSET;
	const gfp_t wait = gfp_mask & __GFP_WAIT;

	/*
	 * The caller may dip into page reserves a bit more if the caller
	 * cannot run direct reclaim, or if the caller has realtime scheduling
	 * policy or is asking for __GFP_HIGH memory.  GFP_ATOMIC requests will
	 * set both ALLOC_HARDER (!wait) and ALLOC_HIGH (__GFP_HIGH).
	 */
	if (gfp_mask & __GFP_HIGH)
		alloc_flags |= ALLOC_HIGH;

	if (!wait) {
		alloc_flags |= ALLOC_HARDER;
		/*
		 * Ignore cpuset if GFP_ATOMIC (!wait) rather than fail alloc.
		 * See also cpuset_zone_allowed() comment in kernel/cpuset.c.
		 */
		alloc_flags &= ~ALLOC_CPUSET;
	} else if (unlikely(rt_task(p)) && !in_interrupt())
		alloc_flags |= ALLOC_HARDER;

	if (likely(!(gfp_mask & __GFP_NOMEMALLOC))) {
		if (gfp_mask & __GFP_EMERGENCY)
			alloc_flags |= ALLOC_NO_WATERMARKS;
		else if (!in_irq() && (p->flags & PF_MEMALLOC))
			alloc_flags |= ALLOC_NO_WATERMARKS;
		else if (!in_interrupt() &&
				unlikely(test_thread_flag(TIF_MEMDIE)))
			alloc_flags |= ALLOC_NO_WATERMARKS;
	}

	return alloc_flags;
}

#define MAX_ALLOC_RANK	16

/*
 * classify the allocation: 0 is hardest, 16 is easiest.
 */
static inline int alloc_flags_to_rank(int alloc_flags)
{
	int rank;

	if (alloc_flags & ALLOC_NO_WATERMARKS)
		return 0;

	rank = alloc_flags & (ALLOC_WMARK_MIN|ALLOC_WMARK_LOW|ALLOC_WMARK_HIGH);
	rank -= alloc_flags & (ALLOC_HARDER|ALLOC_HIGH);

	return rank;
}

static __always_inline int gfp_to_rank(gfp_t gfp_mask)
{
	/*
	 * Although correct this full version takes a ~3% performance hit
	 * on the network test in aim9.
	 *
	 * return alloc_flags_to_rank(gfp_to_alloc_flags(gfp_mask));
	 *
	 * So we go cheat a little. We'll only focus on the correctness of
	 * rank 0.
	 */

	if (likely(!(gfp_mask & __GFP_NOMEMALLOC))) {
		if (gfp_mask & __GFP_EMERGENCY)
			return 0;
		else if (!in_irq() && (current->flags & PF_MEMALLOC))
			return 0;
		/*
		 * We skip the TIF_MEMDIE test:
		 *
		 * if (!in_interrupt() && unlikely(test_thread_flag(TIF_MEMDIE)))
		 * 	return 0;
		 *
		 * this will force an alloc but since we are allowed the memory
		 * that will succeed. This will make this very rare occurence
		 * very expensive when under severe memory pressure, but it
		 * seems a valid tradeoff.
		 */
	}

	/* Cheat by lumping everybody else in rank 1. */
	return 1;
}

#endif
