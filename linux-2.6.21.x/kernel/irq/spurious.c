/*
 * linux/kernel/irq/spurious.c
 *
 * Copyright (C) 1992, 1998-2004 Linus Torvalds, Ingo Molnar
 *
 * This file contains spurious interrupt handling.
 */

#include <linux/jiffies.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/interrupt.h>
#include <linux/timer.h>

static int irqfixup __read_mostly;

#define POLL_SPURIOUS_IRQ_INTERVAL (HZ/10)
static void poll_spurious_irqs(unsigned long dummy);
static DEFINE_TIMER(poll_spurious_irq_timer, poll_spurious_irqs, 0, 0);

/*
 * Recovery handler for misrouted interrupts.
 */
static int try_one_irq(int irq, struct irq_desc *desc)
{
	struct irqaction *action;
	int ok = 0;
	int work = 0;	/* Did we do work for a real IRQ */

	spin_lock(&desc->lock);
	/* Already running on another processor */
	if (desc->status & IRQ_INPROGRESS) {
		/*
		 * Already running: If it is shared get the other
		 * CPU to go looking for our mystery interrupt too
		 */
		if (desc->action && (desc->action->flags & IRQF_SHARED))
			desc->status |= IRQ_PENDING;
		spin_unlock(&desc->lock);
		return ok;
	}
	/* Honour the normal IRQ locking */
	desc->status |= IRQ_INPROGRESS;
	action = desc->action;
	spin_unlock(&desc->lock);

	while (action) {
		/* Only shared IRQ handlers are safe to call */
		if (action->flags & IRQF_SHARED) {
			if (action->handler(irq, action->dev_id) ==
				IRQ_HANDLED)
				ok = 1;
		}
		action = action->next;
	}
	local_irq_disable();
	/* Now clean up the flags */
	spin_lock(&desc->lock);
	action = desc->action;

	/*
	 * While we were looking for a fixup someone queued a real
	 * IRQ clashing with our walk:
	 */
	while ((desc->status & IRQ_PENDING) && action) {
		/*
		 * Perform real IRQ processing for the IRQ we deferred
		 */
		work = 1;
		spin_unlock(&desc->lock);
		handle_IRQ_event(irq, action);
		spin_lock(&desc->lock);
		desc->status &= ~IRQ_PENDING;
	}
	desc->status &= ~IRQ_INPROGRESS;
	/*
	 * If we did actual work for the real IRQ line we must let the
	 * IRQ controller clean up too
	 */
	if (work && desc->chip && desc->chip->end)
		desc->chip->end(irq);
	spin_unlock(&desc->lock);

	return ok;
}

static int misrouted_irq(int irq)
{
	int i;
	int ok = 0;

	for (i = 1; i < NR_IRQS; i++) {
		struct irq_desc *desc = irq_desc + i;

		if (i == irq)	/* Already tried */
			continue;

		if (try_one_irq(i, desc))
			ok = 1;
	}
	/* So the caller can adjust the irq error counts */
	return ok;
}

static void poll_spurious_irqs(unsigned long dummy)
{
	int i;
	for (i = 1; i < NR_IRQS; i++) {
		struct irq_desc *desc = irq_desc + i;
		unsigned int status;

		/* Racy but it doesn't matter */
		status = desc->status;
		barrier();
		if (!(status & IRQ_SPURIOUS_DISABLED))
			continue;

		try_one_irq(i, desc);
	}

	mod_timer(&poll_spurious_irq_timer, jiffies + POLL_SPURIOUS_IRQ_INTERVAL);
}

/*
 * If 99,900 of the previous 100,000 interrupts have not been handled
 * then assume that the IRQ is stuck in some manner. Drop a diagnostic
 * and try to turn the IRQ off.
 *
 * (The other 100-of-100,000 interrupts may have been a correctly
 *  functioning device sharing an IRQ with the failing one)
 *
 * Called under desc->lock
 */

static void
__report_bad_irq(unsigned int irq, struct irq_desc *desc,
		 irqreturn_t action_ret)
{
	struct irqaction *action;

	if (action_ret != IRQ_HANDLED && action_ret != IRQ_NONE) {
		printk(KERN_ERR "irq event %d: bogus return value %x\n",
				irq, action_ret);
	} else {
		printk(KERN_ERR "irq %d: nobody cared (try booting with "
				"the \"irqpoll\" option)\n", irq);
	}
	dump_stack();
	printk(KERN_ERR "handlers:\n");

	action = desc->action;
	while (action) {
		printk(KERN_ERR "[<%p>]", action->handler);
		print_symbol(" (%s)",
			(unsigned long)action->handler);
		printk("\n");
		action = action->next;
	}
}

static void
report_bad_irq(unsigned int irq, struct irq_desc *desc, irqreturn_t action_ret)
{
	static int count = 100;

	if (count > 0) {
		count--;
		__report_bad_irq(irq, desc, action_ret);
	}
}

void note_interrupt(unsigned int irq, struct irq_desc *desc,
		    irqreturn_t action_ret)
{
	if (unlikely(action_ret != IRQ_HANDLED)) {
		/*
		 * If we are seeing only the odd spurious IRQ caused by
		 * bus asynchronicity then don't eventually trigger an error,
		 * otherwise the couter becomes a doomsday timer for otherwise
		 * working systems
		 */
		if (time_after(jiffies, desc->last_unhandled + HZ/10))
			desc->irqs_unhandled = 1;
		else
			desc->irqs_unhandled++;
		desc->last_unhandled = jiffies;
		if (unlikely(action_ret != IRQ_NONE))
			report_bad_irq(irq, desc, action_ret);
	}

	if (unlikely(irqfixup)) {
		/* Don't punish working computers */
		if ((irqfixup == 2 && irq == 0) || action_ret == IRQ_NONE) {
			int ok = misrouted_irq(irq);
			if (action_ret == IRQ_NONE)
				desc->irqs_unhandled -= ok;
		}
	}

	desc->irq_count++;
	if (likely(desc->irq_count < 100000))
		return;

	desc->irq_count = 0;
	if (unlikely(desc->irqs_unhandled > 99900)) {
		/*
		 * The interrupt is stuck
		 */
		__report_bad_irq(irq, desc, action_ret);
		/*
		 * Now kill the IRQ
		 */
		printk(KERN_EMERG "Disabling IRQ #%d\n", irq);
		desc->status |= IRQ_DISABLED | IRQ_SPURIOUS_DISABLED;
		desc->depth++;
		desc->chip->disable(irq);

		mod_timer(&poll_spurious_irq_timer, jiffies + POLL_SPURIOUS_IRQ_INTERVAL);
	}
	desc->irqs_unhandled = 0;
}

int noirqdebug __read_mostly;

int noirqdebug_setup(char *str)
{
	noirqdebug = 1;
	printk(KERN_INFO "IRQ lockup detection disabled\n");

	return 1;
}

__setup("noirqdebug", noirqdebug_setup);

static int __init irqfixup_setup(char *str)
{
	irqfixup = 1;
	printk(KERN_WARNING "Misrouted IRQ fixup support enabled.\n");
	printk(KERN_WARNING "This may impact system performance.\n");

	return 1;
}

__setup("irqfixup", irqfixup_setup);

static int __init irqpoll_setup(char *str)
{
	irqfixup = 2;
	printk(KERN_WARNING "Misrouted IRQ fixup and polling support "
				"enabled\n");
	printk(KERN_WARNING "This may significantly impact system "
				"performance\n");
	return 1;
}

__setup("irqpoll", irqpoll_setup);
