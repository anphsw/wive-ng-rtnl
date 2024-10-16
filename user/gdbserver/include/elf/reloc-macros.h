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
/* Generic relocation support for BFD.
   Copyright (C) 1998 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* These macros are used by the various *.h target specific header
   files to either generate an enum containing all the known relocations
   for that target, or if RELOC_MACROS_GEN_FUNC is defined, a recognition
   function is generated instead.  (This is used by binutils/readelf.c)

   Given a header file like this:

   	START_RELOC_NUMBERS (foo)
   	    RELOC_NUMBER (R_foo_NONE,    0)
   	    RELOC_NUMBER (R_foo_32,      1)
   	    FAKE_RELOC   (R_foo_illegal, 2)
   	    EMPTY_RELOC  (R_foo_max)
   	END_RELOC_NUMBERS

   Then the following will be produced by default (ie if
   RELOC_MACROS_GEN_FUNC is *not* defined).

   	enum foo {
   	  R_foo_NONE = 0,
   	  R_foo_32 = 1,
   	  R_foo_illegal = 2,
   	  R_foo_max
   	};

   If RELOC_MACROS_GEN_FUNC *is* defined, then instead the
   following function will be generated:

   	static char * foo PARAMS ((unsigned long rtype));
   	static char *
   	foo (rtype)
   	    unsigned long rtype;
   	{
   	   switch (rtype)
   	   {
   	   case 0: return "R_foo_NONE";
   	   case 1: return "R_foo_32";
   	   default: return NULL;
   	   }
   	}
   */
   
#ifndef _RELOC_MACROS_H
#define _RELOC_MACROS_H

#ifdef RELOC_MACROS_GEN_FUNC

/* This function takes the relocation number and returns the
   string version name of the name of that relocation.  If
   the relocation is not recognised, NULL is returned.  */

#define START_RELOC_NUMBERS(name)   				\
static const char * name    PARAMS ((unsigned long rtype)); 	\
static const char *						\
name (rtype)							\
	unsigned long rtype;					\
{								\
  switch (rtype)						\
  {

#ifdef __STDC__					  
#define RELOC_NUMBER(name, number)  case number : return #name ;
#else
#define RELOC_NUMBER(name, number)  case number : return "name" ;
#endif

#define FAKE_RELOC(name, number)    
#define EMPTY_RELOC(name)
					  
#define END_RELOC_NUMBERS		default: return NULL;	\
					}			\
				    }


#else /* default to generating enum */

/* Some compilers cannot cope with an enum that ends with a trailing
   comma, so START_RELOC_NUMBERS creates a fake reloc entry, (initialised
   to -1 so that the first real entry will still default to 0).  Further
   entries then prepend a comma to their definitions, creating a list
   of enumerator entries that will satisfy these compilers.  */
#define START_RELOC_NUMBERS(name)   enum name { name = -1
  
#define RELOC_NUMBER(name, number)  , name = number
#define FAKE_RELOC(name, number)    , name = number 
#define EMPTY_RELOC(name)           , name 
#define END_RELOC_NUMBERS           };

#endif

#endif /* RELOC_MACROS_H */
