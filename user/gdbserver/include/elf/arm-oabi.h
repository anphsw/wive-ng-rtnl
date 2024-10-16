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
/* ARM ELF support for BFD.
   Copyright (C) 1998, 1999 Free Software Foundation, Inc.

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

#ifndef _ELF_ARM_H
#define _ELF_ARM_H

#include "elf/reloc-macros.h"

/* Processor specific flags for the ELF header e_flags field.  */
#define EF_ARM_RELEXEC     0x01
#define EF_ARM_HASENTRY    0x02
#define EF_INTERWORK       0x04
#define EF_APCS_26         0x08
#define EF_APCS_FLOAT      0x10
#define EF_PIC             0x20
#define EF_ALIGN8	   0x40		/* 8-bit structure alignment is in use */
#define EF_NEW_ABI         0x80
#define EF_OLD_ABI         0x100

/* Local aliases for some flags to match names used by COFF port.  */
#define F_INTERWORK	   EF_INTERWORK
#define F_APCS26	   EF_APCS_26
#define F_APCS_FLOAT	   EF_APCS_FLOAT
#define F_PIC              EF_PIC

/* Additional symbol types for Thumb */
#define STT_ARM_TFUNC     0xd

/* ARM-specific values for sh_flags */
#define SHF_ENTRYSECT      0x10000000   /* Section contains an entry point */
#define SHF_COMDEF         0x80000000   /* Section may be multiply defined in the input to a link step */

/* ARM-specific program header flags */
#define PF_ARM_SB          0x10000000   /* Segment contains the location addressed by the static base */

/* Relocation types.  */
START_RELOC_NUMBERS (elf_arm_reloc_type)
  RELOC_NUMBER (R_ARM_NONE,         0)
  RELOC_NUMBER (R_ARM_PC24,         1)
  RELOC_NUMBER (R_ARM_ABS32,        2)
  RELOC_NUMBER (R_ARM_REL32,        3)
  RELOC_NUMBER (R_ARM_ABS8,         4)
  RELOC_NUMBER (R_ARM_ABS16,        5)
  RELOC_NUMBER (R_ARM_ABS12,        6)
  RELOC_NUMBER (R_ARM_THM_ABS5,     7)
  RELOC_NUMBER (R_ARM_THM_PC22,     8)
  RELOC_NUMBER (R_ARM_SBREL32,      9)
  RELOC_NUMBER (R_ARM_AMP_VCALL9,  10)
  RELOC_NUMBER (R_ARM_THM_PC11,    11)       /* cygnus extension to abi: thumb unconditional branch */
  RELOC_NUMBER (R_ARM_THM_PC9,     12)       /* cygnus extension to abi: thumb conditional branch */
  RELOC_NUMBER (R_ARM_GNU_VTINHERIT, 13)
  RELOC_NUMBER (R_ARM_GNU_VTENTRY, 14)
  RELOC_NUMBER (R_ARM_COPY,        20)       /* copy symbol at runtime */
  RELOC_NUMBER (R_ARM_GLOB_DAT,    21)       /* create GOT entry */
  RELOC_NUMBER (R_ARM_JUMP_SLOT,   22)       /* create PLT entry */
  RELOC_NUMBER (R_ARM_RELATIVE,    23)       /* adjust by program base */
  RELOC_NUMBER (R_ARM_GOTOFF,      24)       /* 32 bit offset to GOT */
  RELOC_NUMBER (R_ARM_GOTPC,       25)       /* 32 bit PC relative offset to GOT */
  RELOC_NUMBER (R_ARM_GOT32,       26)       /* 32 bit GOT entry */
  RELOC_NUMBER (R_ARM_PLT32,       27)       /* 32 bit PLT address */
  FAKE_RELOC   (FIRST_INVALID_RELOC, 28)
  FAKE_RELOC   (LAST_INVALID_RELOC,  249)
  RELOC_NUMBER (R_ARM_RSBREL32,   250)
  RELOC_NUMBER (R_ARM_THM_RPC22,  251)
  RELOC_NUMBER (R_ARM_RREL32,     252)
  RELOC_NUMBER (R_ARM_RABS32,     253)
  RELOC_NUMBER (R_ARM_RPC24,      254)
  RELOC_NUMBER (R_ARM_RBASE,      255)
END_RELOC_NUMBERS

#endif
