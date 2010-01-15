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
/*
 * linked list helper functions.
 *
 * Copyright (C) 2003 Glenn McGrath
 * Copyright (C) 2005 Vladimir Oleynik
 * Copyright (C) 2005 Bernhard Fischer
 * Copyright (C) 2006 Rob Landley <rob@landley.net>
 *
 * Licensed under the GPL v2 or later, see the file LICENSE in this tarball.
 */

#include "libbb.h"

/* Add data to the start of the linked list.  */
void FAST_FUNC llist_add_to(llist_t **old_head, void *data)
{
	llist_t *new_head = xmalloc(sizeof(llist_t));

	new_head->data = data;
	new_head->link = *old_head;
	*old_head = new_head;
}

/* Add data to the end of the linked list.  */
void FAST_FUNC llist_add_to_end(llist_t **list_head, void *data)
{
	llist_t *new_item = xmalloc(sizeof(llist_t));

	new_item->data = data;
	new_item->link = NULL;

	if (!*list_head)
		*list_head = new_item;
	else {
		llist_t *tail = *list_head;

		while (tail->link)
			tail = tail->link;
		tail->link = new_item;
	}
}

/* Remove first element from the list and return it */
void* FAST_FUNC llist_pop(llist_t **head)
{
	void *data, *next;

	if (!*head)
		return NULL;

	data = (*head)->data;
	next = (*head)->link;
	free(*head);
	*head = next;

	return data;
}

/* Unlink arbitrary given element from the list */
void FAST_FUNC llist_unlink(llist_t **head, llist_t *elm)
{
	llist_t *crt;

	if (!(elm && *head))
		return;

	if (elm == *head) {
		*head = (*head)->link;
		return;
	}

	for (crt = *head; crt; crt = crt->link) {
		if (crt->link == elm) {
			crt->link = elm->link;
			return;
		}
	}
}

/* Recursively free all elements in the linked list.  If freeit != NULL
 * call it on each datum in the list */
void FAST_FUNC llist_free(llist_t *elm, void (*freeit) (void *data))
{
	while (elm) {
		void *data = llist_pop(&elm);

		if (freeit)
			freeit(data);
	}
}

#ifdef UNUSED
/* Reverse list order. */
llist_t* FAST_FUNC llist_rev(llist_t *list)
{
	llist_t *rev = NULL;

	while (list) {
		llist_t *next = list->link;

		list->link = rev;
		rev = list;
		list = next;
	}
	return rev;
}
#endif
