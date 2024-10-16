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
/* Java language support definitions for GDB, the GNU debugger.
   Copyright 1997, 1998, 1999 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

extern int
java_parse PARAMS ((void));	/* Defined in jv-exp.y */

extern void
java_error PARAMS ((char *));	/* Defined in jv-exp.y */

/* sizeof (struct Object) */
#define JAVA_OBJECT_SIZE (get_java_object_header_size ())

extern struct type *java_int_type;
extern struct type *java_byte_type;
extern struct type *java_short_type;
extern struct type *java_long_type;
extern struct type *java_boolean_type;
extern struct type *java_char_type;
extern struct type *java_float_type;
extern struct type *java_double_type;
extern struct type *java_void_type;

extern int
java_val_print PARAMS ((struct type *, char *, int, CORE_ADDR, GDB_FILE *, int, int,
			int, enum val_prettyprint));

extern int
java_value_print PARAMS ((struct value *, GDB_FILE *, int,
			  enum val_prettyprint));

extern value_ptr java_class_from_object PARAMS ((value_ptr));

extern struct type *type_from_class PARAMS ((struct value *));

extern struct type *java_primitive_type PARAMS ((int signature));

extern struct type *java_primitive_type_from_name PARAMS ((char*, int));

extern struct type *java_array_type PARAMS ((struct type *, int));

extern struct type *get_java_object_type PARAMS ((void));
extern int get_java_object_header_size PARAMS ((void));

extern struct type *java_lookup_class PARAMS((char *));

extern int is_object_type PARAMS ((struct type*));

extern void			/* Defined in jv-typeprint.c */
java_print_type PARAMS ((struct type *, char *, GDB_FILE *, int, int));

extern char *java_demangle_type_signature PARAMS ((char *));
