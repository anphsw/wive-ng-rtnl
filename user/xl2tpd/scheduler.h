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
/*
 * Layer Two Tunnelling Protocol Daemon
 * Copyright (C) 1998 Adtran, Inc.
 * Copyright (C) 2002 Jeff McAdams
 *
 * Mark Spencer
 *
 * This software is distributed under the terms
 * of the GPL, which you should have received
 * along with this source.
 *
 * Scheduler structures and functions
 *
 */

#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <sys/time.h>

/*
 * The idea is to provide a general scheduler which can schedule
 * events to be run periodically
 */

struct schedule_entry
{
    struct timeval tv;          /* Scheduled time to execute */
    void (*func) (void *);      /* Function to execute */
    void *data;                 /* Data to be passed to func */
    struct schedule_entry *next;        /* Next entry in queue */
};

extern struct schedule_entry *events;

/* Schedule func to be executed with argument data sometime
   tv in the future. */

struct schedule_entry *schedule (struct timeval tv, void (*func) (void *),
                                 void *data);

/* Like schedule() but tv represents an absolute time in the future */

struct schedule_entry *aschedule (struct timeval tv, void (*func) (void *),
                                  void *data);

/* Remove a scheduled event from the queue */

void deschedule (struct schedule_entry *);

/* The alarm handler */

void alarm_handler (int);

/* Initialization function */
void init_scheduler (void);

/* Prevent the scheduler from running */
void schedule_lock ();

/* Restore normal scheduling functions */
void schedule_unlock ();

/* Compare two timeval functions and see if a <= b */

#define TVLESS(a,b) ((a).tv_sec == (b).tv_sec ? \
				((a).tv_usec < (b).tv_usec) : \
				((a).tv_sec < (b).tv_sec))
#define TVLESSEQ(a,b) ((a).tv_sec == (b).tv_sec ? \
				((a).tv_usec <= (b).tv_usec) : \
				((a).tv_sec <= (b).tv_sec))
#define TVGT(a,b) ((a).tv_sec == (b).tv_sec ? \
				((a).tv_usec > (b).tv_usec) : \
				((a).tv_sec > (b).tv_sec))
#endif
