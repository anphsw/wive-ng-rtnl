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
/* PPC ELF support for BFD.
   Copyright (C) 1995, 1998 Free Software Foundation, Inc.

   By Michael Meissner, Cygnus Support, <meissner@cygnus.com>, from information
   in the System V Application Binary Interface, PowerPC Processor Supplement
   and the PowerPC Embedded Application Binary Interface (eabi).

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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This file holds definitions specific to the PPC ELF ABI.  Note
   that most of this is not actually implemented by BFD.  */

#ifndef _ELF_PPC_H
#define _ELF_PPC_H

#include "elf/reloc-macros.h"

/* Relocations.  */
START_RELOC_NUMBERS (elf_ppc_reloc_type)
  RELOC_NUMBER (R_PPC_NONE, 0)
  RELOC_NUMBER (R_PPC_ADDR32, 1)
  RELOC_NUMBER (R_PPC_ADDR24, 2)
  RELOC_NUMBER (R_PPC_ADDR16, 3)
  RELOC_NUMBER (R_PPC_ADDR16_LO, 4)
  RELOC_NUMBER (R_PPC_ADDR16_HI, 5)
  RELOC_NUMBER (R_PPC_ADDR16_HA, 6)
  RELOC_NUMBER (R_PPC_ADDR14, 7)
  RELOC_NUMBER (R_PPC_ADDR14_BRTAKEN, 8)
  RELOC_NUMBER (R_PPC_ADDR14_BRNTAKEN, 9)
  RELOC_NUMBER (R_PPC_REL24, 10)
  RELOC_NUMBER (R_PPC_REL14, 11)
  RELOC_NUMBER (R_PPC_REL14_BRTAKEN, 12)
  RELOC_NUMBER (R_PPC_REL14_BRNTAKEN, 13)
  RELOC_NUMBER (R_PPC_GOT16, 14)
  RELOC_NUMBER (R_PPC_GOT16_LO, 15)
  RELOC_NUMBER (R_PPC_GOT16_HI, 16)
  RELOC_NUMBER (R_PPC_GOT16_HA, 17)
  RELOC_NUMBER (R_PPC_PLTREL24, 18)
  RELOC_NUMBER (R_PPC_COPY, 19)
  RELOC_NUMBER (R_PPC_GLOB_DAT, 20)
  RELOC_NUMBER (R_PPC_JMP_SLOT, 21)
  RELOC_NUMBER (R_PPC_RELATIVE, 22)
  RELOC_NUMBER (R_PPC_LOCAL24PC, 23)
  RELOC_NUMBER (R_PPC_UADDR32, 24)
  RELOC_NUMBER (R_PPC_UADDR16, 25)
  RELOC_NUMBER (R_PPC_REL32, 26)
  RELOC_NUMBER (R_PPC_PLT32, 27)
  RELOC_NUMBER (R_PPC_PLTREL32, 28)
  RELOC_NUMBER (R_PPC_PLT16_LO, 29)
  RELOC_NUMBER (R_PPC_PLT16_HI, 30)
  RELOC_NUMBER (R_PPC_PLT16_HA, 31)
  RELOC_NUMBER (R_PPC_SDAREL16, 32)
  RELOC_NUMBER (R_PPC_SECTOFF, 33)
  RELOC_NUMBER (R_PPC_SECTOFF_LO, 34)
  RELOC_NUMBER (R_PPC_SECTOFF_HI, 35)
  RELOC_NUMBER (R_PPC_SECTOFF_HA, 36)

/* The remaining relocs are from the Embedded ELF ABI, and are not
   in the SVR4 ELF ABI.  */
  RELOC_NUMBER (R_PPC_EMB_NADDR32, 101)
  RELOC_NUMBER (R_PPC_EMB_NADDR16, 102)
  RELOC_NUMBER (R_PPC_EMB_NADDR16_LO, 103)
  RELOC_NUMBER (R_PPC_EMB_NADDR16_HI, 104)
  RELOC_NUMBER (R_PPC_EMB_NADDR16_HA, 105)
  RELOC_NUMBER (R_PPC_EMB_SDAI16, 106)
  RELOC_NUMBER (R_PPC_EMB_SDA2I16, 107)
  RELOC_NUMBER (R_PPC_EMB_SDA2REL, 108)
  RELOC_NUMBER (R_PPC_EMB_SDA21, 109)
  RELOC_NUMBER (R_PPC_EMB_MRKREF, 110)
  RELOC_NUMBER (R_PPC_EMB_RELSEC16, 111)
  RELOC_NUMBER (R_PPC_EMB_RELST_LO, 112)
  RELOC_NUMBER (R_PPC_EMB_RELST_HI, 113)
  RELOC_NUMBER (R_PPC_EMB_RELST_HA, 114)
  RELOC_NUMBER (R_PPC_EMB_BIT_FLD, 115)
  RELOC_NUMBER (R_PPC_EMB_RELSDA, 116)

  /* These are GNU extensions to enable C++ vtable garbage collection.  */
  RELOC_NUMBER (R_PPC_GNU_VTINHERIT, 253)
  RELOC_NUMBER (R_PPC_GNU_VTENTRY, 254)

/* This is a phony reloc to handle any old fashioned TOC16 references
   that may still be in object files.  */
  RELOC_NUMBER (R_PPC_TOC16, 255)

  EMPTY_RELOC (R_PPC_max)
END_RELOC_NUMBERS


/* Processor specific flags for the ELF header e_flags field.  */

#define	EF_PPC_EMB		0x80000000	/* PowerPC embedded flag  */

						/* CYGNUS local bits below */
#define	EF_PPC_RELOCATABLE	0x00010000	/* PowerPC -mrelocatable flag */
#define	EF_PPC_RELOCATABLE_LIB	0x00008000	/* PowerPC -mrelocatable-lib flag */

/* Processor specific section headers, sh_type field */

#define SHT_ORDERED		SHT_HIPROC	/* Link editor is to sort the \
						   entries in this section \
						   based on the address \
						   specified in the associated \
						   symbol table entry.  */

/* Processor specific section flags, sh_flags field */

#define SHF_EXCLUDE		0x80000000	/* Link editor is to exclude \
						   this section from executable \
						   and shared objects that it \
						   builds when those objects \
						   are not to be furhter \
						   relocated.  */
#endif /* _ELF_PPC_H */
