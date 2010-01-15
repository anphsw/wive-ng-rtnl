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
/* vector.h ..... store a vector of PPTP_CALL information and search it
 *                efficiently.
 *                C. Scott Ananian <cananian@alumni.princeton.edu>
 *
 * $Id: vector.h,v 1.1.1.1 2008/07/21 09:19:28 james26_jang Exp $
 */

#ifndef INC_VECTOR_H
#define INC_VECTOR_H

#include "pptp_ctrl.h" /* for definition of PPTP_CALL */

typedef struct vector_struct VECTOR;

VECTOR *vector_create();
void vector_destroy(VECTOR *v);

int vector_size(VECTOR *v);

/* vector_insert and vector_search return TRUE on success, FALSE on failure. */
int  vector_insert(VECTOR *v, int key, PPTP_CALL * call);
int  vector_remove(VECTOR *v, int key);
int  vector_search(VECTOR *v, int key, PPTP_CALL ** call);
/* vector_contains returns FALSE if not found, TRUE if found. */
int  vector_contains(VECTOR *v, int key);
/* find first unused key. Returns TRUE on success, FALSE if no. */
int  vector_scan(VECTOR *v, int lo, int hi, int *key);
/* get a specific PPTP_CALL ... useful only when iterating. */
PPTP_CALL * vector_get_Nth(VECTOR *v, int n);

#endif /* INC_VECTOR_H */
